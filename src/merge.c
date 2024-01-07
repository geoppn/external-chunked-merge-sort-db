#include <merge.h>
#include <stdio.h>
#include <stdbool.h>

void merge(int input_FileDesc, int chunkSize, int bWay, int output_FileDesc) {
    int totalBlocks = HP_GetIdOfLastBlock(input_FileDesc);
    int numChunks = totalBlocks / chunkSize;
    int remainingBlocks = totalBlocks % chunkSize;

    printf("Total blocks: %d, Chunks: %d, Remaining blocks: %d\n", totalBlocks, numChunks, remainingBlocks);

    for (int i = 0; i < numChunks; i += bWay) {
        int remainingChunks = numChunks - i;
        int chunksToMerge = remainingChunks < bWay ? remainingChunks : bWay;

        printf("Merging chunks %d to %d\n", i, i + chunksToMerge - 1);

        // Allocate buffer to hold blocks from bWay chunks
        CHUNK buffers[bWay];
        Record minRecord;
        int outputBufferIndex = 0;

        // Initialize buffers
        for (int j = 0; j < chunksToMerge; ++j) {
            CHUNK_Iterator iterator = CHUNK_CreateIterator(input_FileDesc, chunkSize);
            CHUNK_GetNext(&iterator, &buffers[j]);
        }

        while (outputBufferIndex < bWay) {
            int minBlockIndex = -1;
            minRecord.id = INT_MAX; // Initialize minRecord ID to maximum value

            // Find the smallest record among the current blocks in the buffers
            for (int j = 0; j < chunksToMerge; ++j) {
                Record currentRecord;
                int currentBlockId = buffers[j].from_BlockId;

                if (currentBlockId <= buffers[j].to_BlockId) {
                    CHUNK_GetIthRecordInChunk(&buffers[j], currentBlockId, &currentRecord);

                    if (currentRecord.id < minRecord.id) {
                        minRecord = currentRecord;
                        minBlockIndex = j;
                    }
                }
            }

            // Write the smallest record to the output block
            if (minBlockIndex != -1) {
                CHUNK_UpdateIthRecord(&buffers[minBlockIndex], buffers[minBlockIndex].from_BlockId, minRecord);
                buffers[minBlockIndex].from_BlockId++; // Move to the next block in the chunk
            } else {
                break; // No more records to merge
            }

            outputBufferIndex++;
        }

        printf("Finished merging chunks %d to %d\n", i, i + chunksToMerge - 1);
    }

    if (remainingBlocks > 0) {
        printf("Merging remaining blocks\n");

        // Perform similar operations as above for the remaining blocks
        CHUNK remainingBlocksChunk;
        CHUNK_Iterator iterator = CHUNK_CreateIterator(input_FileDesc, chunkSize);
        CHUNK_GetNext(&iterator, &remainingBlocksChunk);

        while (remainingBlocksChunk.from_BlockId <= remainingBlocksChunk.to_BlockId) {
            Record minRecord;
            int minBlockIndex = -1;
            minRecord.id = INT_MAX;

            // Find the smallest record among the current blocks in the remaining chunk
            CHUNK_GetIthRecordInChunk(&remainingBlocksChunk, remainingBlocksChunk.from_BlockId, &minRecord);

            // Write the smallest record to the output block
            CHUNK_UpdateIthRecord(&remainingBlocksChunk, remainingBlocksChunk.from_BlockId, minRecord);
            remainingBlocksChunk.from_BlockId++; // Move to the next block
        }

        printf("Finished merging remaining blocks\n");
    }
}