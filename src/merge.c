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
        
        for (int j = 0; j < chunksToMerge; ++j) {
            CHUNK_Iterator iterator = CHUNK_CreateIterator(input_FileDesc, chunkSize);
            iterator.current = i * bWay + j;
            
            CHUNK chunk;
            chunk.file_desc = output_FileDesc;
            chunk.from_BlockId = iterator.current * chunkSize;
            chunk.to_BlockId = chunk.from_BlockId + chunkSize - 1;
            chunk.blocksInChunk = chunkSize;
            chunk.recordsInChunk = chunkSize * HP_GetMaxRecordsInBlock(input_FileDesc);
            
            for (int k = 0; k < chunk.recordsInChunk; ++k) {
                Record record;
                if (CHUNK_GetIthRecordInChunk(&chunk, k, &record) == 0) {
                    CHUNK_UpdateIthRecord(&chunk, k, record);
                } else {
                    break;
                }
            }
        }
    }

    if (remainingBlocks > 0) {
        CHUNK_Iterator iterator = CHUNK_CreateIterator(input_FileDesc, remainingBlocks);
        iterator.current = numChunks;
        
        CHUNK chunk;
        chunk.file_desc = output_FileDesc;
        chunk.from_BlockId = iterator.current * chunkSize;
        chunk.to_BlockId = chunk.from_BlockId + remainingBlocks - 1;
        chunk.blocksInChunk = remainingBlocks;
        chunk.recordsInChunk = remainingBlocks * HP_GetMaxRecordsInBlock(input_FileDesc);
        
        for (int k = 0; k < chunk.recordsInChunk; ++k) {
            Record record;
            if (CHUNK_GetIthRecordInChunk(&chunk, k, &record) == 0) {
                CHUNK_UpdateIthRecord(&chunk, k, record);
            } else {
                break;
            }
        }
    }
}