#include <merge.h>
#include <stdio.h>
#include "chunk.h"


CHUNK_Iterator CHUNK_CreateIterator(int fileDesc, int blocksInChunk){
    CHUNK_Iterator iterator;
    iterator.file_desc = fileDesc; 
    iterator.current = 1;  // Start from the first block
    iterator.blocksInChunk = blocksInChunk;
    iterator.lastBlocksID = HP_GetIdOfLastBlock(fileDesc);  
    
    return iterator;
}

int CHUNK_GetNext(CHUNK_Iterator *iterator, CHUNK *chunk) {
    // Set CHUNK details
    chunk->file_desc = iterator->file_desc;
    chunk->from_BlockId = iterator->current;
    chunk->to_BlockId = iterator->current + iterator->blocksInChunk - 1;

    // Check if the calculated to_BlockId exceeds the lastBlocksID
    if (chunk->to_BlockId > iterator->lastBlocksID) {
        chunk->to_BlockId = iterator->lastBlocksID;
    }

    // Move to the next chunk
    iterator->current += iterator->blocksInChunk;

    if (iterator->current > iterator->lastBlocksID) {
        // No more chunks to read
        return -1;
    }

    return 0;
}



int CHUNK_GetIthRecordInChunk(CHUNK *chunk, int i, Record *record) {
    int blockId = chunk->from_BlockId + (i  / HP_GetMaxRecordsInBlock(chunk->file_desc));
    int cursor = i % HP_GetMaxRecordsInBlock(chunk->file_desc);
    
    if (HP_GetRecord(chunk->file_desc, blockId, cursor, record) == -1) {
        return -1;  // Failed to retrieve record
    }
    
    HP_Unpin(chunk->file_desc, blockId);  // Unpin the block

    return 0;
}

int CHUNK_UpdateIthRecord(CHUNK *chunk, int i, Record record) {
    int blockId = chunk->from_BlockId + (i / HP_GetMaxRecordsInBlock(chunk->file_desc));
    int cursor = i % HP_GetMaxRecordsInBlock(chunk->file_desc);
    if (HP_UpdateRecord(chunk->file_desc, blockId, cursor, record) == -1) {
        return -1;  // Failed to update record
    }
    HP_Unpin(chunk->file_desc, blockId);  // Unpin the block
    return 0;
}

void CHUNK_Print(CHUNK chunk) {
    Record record;
    int maxRecords = HP_GetMaxRecordsInBlock(chunk.file_desc); // WE MOVED THIS LINE OUT OF THE FOR LOOP
    for (int i = chunk.from_BlockId; i <= chunk.to_BlockId; ++i) {
        for (int j = 0; j < maxRecords; ++j) {
            if (CHUNK_GetIthRecordInChunk(&chunk, (i - chunk.from_BlockId) * maxRecords + j, &record) == 0) {
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

    HP_Unpin(iterator->chunk.file_desc, iterator->currentBlockId);  
    
    // Move to the next record
    iterator->cursor++;
    if (iterator->cursor > HP_GetMaxRecordsInBlock(iterator->chunk.file_desc) - 1) { // team flop: removed =
        iterator->currentBlockId++;
        iterator->cursor = 0;  // Move to the next block
    }
    
    return 0;
}
