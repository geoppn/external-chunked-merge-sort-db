#include <merge.h>
#include <stdio.h>
#include <stdbool.h>

void merge(int input_FileDesc, int chunkSize, int bWay, int output_FileDesc) {
    int totalBlocks = HP_GetIdOfLastBlock(input_FileDesc);
    int numChunks = totalBlocks / chunkSize;
    int remainingBlocks = totalBlocks % chunkSize;

    printf("Total Blocks: %d\n", totalBlocks);
    printf("Number of Chunks: %d\n", numChunks);
    printf("Remaining Blocks: %d\n", remainingBlocks);

    for (int i = 0; i < numChunks; i += bWay) {
        int remainingChunks = numChunks - i;
        int chunksToMerge = remainingChunks < bWay ? remainingChunks : bWay;

        printf("Processing Chunk: %d\n", i);
        printf("Remaining Chunks: %d\n", remainingChunks);
        printf("Chunks to Merge: %d\n", chunksToMerge);

        // Merge 'chunksToMerge' chunks from input_FileDesc to output_FileDesc
        for (int j = 0; j < chunksToMerge; ++j) {
            CHUNK_Iterator iterator = CHUNK_CreateIterator(input_FileDesc, chunkSize);
            iterator.current = i + j;

            printf("Processing Sub-Chunk: %d\n", j);

            CHUNK chunk;
            chunk.file_desc = output_FileDesc;
            chunk.from_BlockId = i * chunkSize;
            chunk.to_BlockId = chunk.from_BlockId + chunkSize - 1;
            chunk.blocksInChunk = chunkSize;
            chunk.recordsInChunk = chunkSize * HP_GetMaxRecordsInBlock(input_FileDesc);

            printf("Processing Block: %d to %d\n", chunk.from_BlockId, chunk.to_BlockId);

            for (int k = 0; k < chunk.recordsInChunk; ++k) {
                printf("Processing Record: %d\n", k);

                Record record;
                if (CHUNK_GetIthRecordInChunk(&chunk, k, &record) == 0) {
                    CHUNK_UpdateIthRecord(&chunk, k, record);
                } else {
                    printf("Failed to get Record: %d\n", k);
                    break;
                }
            }
        }
    }

    if (remainingBlocks > 0) {
        printf("Processing Remaining Blocks\n");

        CHUNK_Iterator iterator = CHUNK_CreateIterator(input_FileDesc, remainingBlocks);
        iterator.current = numChunks;

        CHUNK chunk;
        chunk.file_desc = output_FileDesc;
        chunk.from_BlockId = numChunks * chunkSize;
        chunk.to_BlockId = chunk.from_BlockId + remainingBlocks - 1;
        chunk.blocksInChunk = remainingBlocks;
        chunk.recordsInChunk = remainingBlocks * HP_GetMaxRecordsInBlock(input_FileDesc);

        printf("Processing Block: %d to %d\n", chunk.from_BlockId, chunk.to_BlockId);

        for (int k = 0; k < chunk.recordsInChunk; ++k) {
            printf("Processing Record: %d\n", k);

            Record record;
            if (CHUNK_GetIthRecordInChunk(&chunk, k, &record) == 0) {
                CHUNK_UpdateIthRecord(&chunk, k, record);
            } else {
                printf("Failed to get Record: %d\n", k);
                break;
            }
        }
    }
}