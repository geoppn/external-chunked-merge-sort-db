#include <merge.h>
#include <stdio.h>
#include <stdbool.h>

void merge(int input_FileDesc, int chunkSize, int bWay, int output_FileDesc) {
    int totalBlocks = HP_GetIdOfLastBlock(input_FileDesc);
    int numChunks = totalBlocks / chunkSize;
    int remainingBlocks = totalBlocks % chunkSize;

    for (int i = 0; i < numChunks; i += bWay) {
        int remainingChunks = numChunks - i;
        int chunksToMerge = remainingChunks < bWay ? remainingChunks : bWay;
        
        // Merge 'chunksToMerge' chunks from input_FileDesc to output_FileDesc
        for (int j = 0; j < chunksToMerge; ++j) {
            CHUNK_Iterator iterator = CHUNK_CreateIterator(input_FileDesc, chunkSize);
            iterator.current = i + j;
            
            CHUNK chunk;
            chunk.file_desc = output_FileDesc;
            chunk.from_BlockId = i * chunkSize;
            chunk.to_BlockId = chunk.from_BlockId + chunkSize - 1;
            chunk.blocksInChunk = chunkSize;
            chunk.recordsInChunk = chunkSize * HP_GetMaxRecordsInBlock(input_FileDesc);
            
            // Loop through blocks in the chunk
            for (int blockId = chunk.from_BlockId; blockId <= chunk.to_BlockId; ++blockId) {
                // Loop through records in each block
                for (int recordId = 0; recordId < HP_GetMaxRecordsInBlock(input_FileDesc); ++recordId) {
                    Record record;
                    if (HP_GetRecord(input_FileDesc, blockId, recordId, &record) == 0) {
                        CHUNK_UpdateIthRecord(&chunk, recordId, record);
                    } else {
                        break;
                    }
                }
            }
        }
    }

    // Remaining blocks handling
    if (remainingBlocks > 0) {
        CHUNK_Iterator iterator = CHUNK_CreateIterator(input_FileDesc, remainingBlocks);
        iterator.current = numChunks;
        
        CHUNK chunk;
        chunk.file_desc = output_FileDesc;
        chunk.from_BlockId = numChunks * chunkSize;
        chunk.to_BlockId = chunk.from_BlockId + remainingBlocks - 1;
        chunk.blocksInChunk = remainingBlocks;
        chunk.recordsInChunk = remainingBlocks * HP_GetMaxRecordsInBlock(input_FileDesc);
        
        // Similar record processing loop for remaining blocks
        for (int blockId = chunk.from_BlockId; blockId <= chunk.to_BlockId; ++blockId) {
            for (int recordId = 0; recordId < HP_GetMaxRecordsInBlock(input_FileDesc); ++recordId) {
                Record record;
                if (HP_GetRecord(input_FileDesc, blockId, recordId, &record) == 0) {
                    CHUNK_UpdateIthRecord(&chunk, recordId, record);
                } else {
                    break;
                }
            }
        }
    }
}
