#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bf.h"
#include "hp_file.h"
#include "record.h"
#include "sort.h"
#include "merge.h"
#include "chunk.h"

bool shouldSwap(Record* rec1, Record* rec2) {
    return strcmp(rec1->name, rec2->name) > 0;
}


void sort_FileInChunks(int file_desc, int numBlocksInChunk) {
    int totalBlocks = HP_GetIdOfLastBlock(file_desc);
    int numChunks = totalBlocks / numBlocksInChunk; // Calculate the total number of chunks
    
    // Sort records within each chunk
    for (int i = 1; i <= numChunks; ++i) {
        // Create CHUNK for the current chunk
        CHUNK chunk;
        chunk.file_desc = file_desc;
        chunk.from_BlockId = (i - 1) * numBlocksInChunk + 1;
        chunk.to_BlockId = i * numBlocksInChunk;
        chunk.blocksInChunk = numBlocksInChunk;
        chunk.recordsInChunk = numBlocksInChunk * HP_GetMaxRecordsInBlock(file_desc);
        
        // Sort the chunk
        sort_Chunk(&chunk);
    }
}


void sort_Chunk(CHUNK* chunk) {
    int numRecords = chunk->recordsInChunk;
    Record records[numRecords];
    
    // Read records from the chunk
    for (int i = 1; i <= numRecords; ++i) {
        CHUNK_GetIthRecordInChunk(chunk, i, &records[i - 1]);
    }
    
    // Sort the records based on the 'name' field
    for (int i = 0; i < numRecords - 1; ++i) {
        for (int j = 0; j < numRecords - i - 1; ++j) {
            if (shouldSwap(&records[j], &records[j + 1])) {
                // Swap records
                Record temp = records[j];
                records[j] = records[j + 1];
                records[j + 1] = temp;
            }
        }
    }
    
    // Write sorted records back to the chunk
    for (int i = 1; i <= numRecords; ++i) {
        CHUNK_UpdateIthRecord(chunk, i, records[i - 1]);
    }
}
