#include <merge.h>
#include <stdio.h>
#include "chunk.h"


CHUNK_Iterator CHUNK_CreateIterator(int fileDesc, int blocksInChunk){
    CHUNK_Iterator iterator;
    iterator.file_desc = fileDesc;
    iterator.current = 1;  // ??????????????????? TEAM EDIT: WE CHANGED THE VALUE OF .CURRENT FROM 1 TO 0 TO START FROM THE FIRST BLOCK
    iterator.blocksInChunk = blocksInChunk;
    iterator.lastBlocksID = HP_GetIdOfLastBlock(fileDesc);  // WE USE A HP_ FUNCTION TO GET THE ID OF THE LAST BLOCK
    
    return iterator;
}

int CHUNK_GetNext(CHUNK_Iterator *iterator, CHUNK *chunk) {
    if (iterator->current > iterator->lastBlocksID) {
        // No more chunks to read
        return -1;
    }
    
    // Set CHUNK details
    chunk->file_desc = iterator->file_desc;
    chunk->from_BlockId = iterator->current;
    chunk->to_BlockId = iterator->current + iterator->blocksInChunk - 1;
    
    // Update iterator for the next CHUNK
    iterator->current += iterator->blocksInChunk;
    
    return 0;
}


int CHUNK_UpdateIthRecord(CHUNK *chunk, int i, Record record) {
    int blockId = chunk->from_BlockId + (i - 1) / HP_GetMaxRecordsInBlock(chunk->file_desc);
    int cursor = (i - 1) % HP_GetMaxRecordsInBlock(chunk->file_desc);
    
    if (HP_UpdateRecord(chunk->file_desc, blockId, cursor, record) == -1) {
        return -1;  // Failed to update record
    }
    
    return 0;
}

void CHUNK_Print(CHUNK chunk) {
    Record record;
    for (int i = chunk.from_BlockId; i <= chunk.to_BlockId; ++i) {
        int maxRecords = HP_GetMaxRecordsInBlock(chunk.file_desc);
        for (int j = 0; j < maxRecords; ++j) {
            if (CHUNK_GetIthRecordInChunk(&chunk, (i - chunk.from_BlockId) * maxRecords + j + 1, &record) == 0) {
                // Print record details (adjust as per your Record structure)
                printf("Record: %s %s %s %d %s\n", record.name, record.surname, record.city, record.id, record.delimiter); // TEAM EDIT: WE EDITED THE PRINT TO CORRECTLY PRINT A RECORD BASED ON THE RECORD.H STRUCT
            } else {
                return;  // No more records in this chunk
            }
        }
    }
}



CHUNK_RecordIterator CHUNK_CreateRecordIterator(CHUNK *chunk) {
    CHUNK_RecordIterator recordIterator;
    recordIterator.chunk = *chunk;
    recordIterator.currentBlockId = chunk->from_BlockId;
    recordIterator.cursor = 0;
    
    return recordIterator;
}


int CHUNK_GetNextRecord(CHUNK_RecordIterator *iterator, Record *record) {
    if (iterator->currentBlockId > iterator->chunk.to_BlockId) {
        return -1;  // No more records
    }
    
    if (HP_GetRecord(iterator->chunk.file_desc, iterator->currentBlockId, iterator->cursor, record) == -1) {
        return -1;  // Failed to retrieve record
    }
    
    // Move to the next record
    iterator->cursor++;
    if (iterator->cursor >= HP_GetMaxRecordsInBlock(iterator->chunk.file_desc)) {
        iterator->currentBlockId++;
        iterator->cursor = 0;  // Move to the next block
    }
    
    return 0;
}
