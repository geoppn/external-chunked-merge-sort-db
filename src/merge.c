#include <merge.h>
#include <stdio.h>
#include <stdbool.h>
#include <chunk.h>

// Function to create a record iterator for a given chunk
CHUNK_RecordIterator CHUNK_CreateRecordIterator(CHUNK *chunk);

void merge(int input_FileDesc, int chunkSize, int bWay, int output_FileDesc) {
    int totalBlocks = HP_GetIdOfLastBlock(input_FileDesc);
    int numChunks = totalBlocks / chunkSize;

    // Initialize arrays for chunks and record iterators
    CHUNK chunks[bWay];
    CHUNK_RecordIterator recordIterators[bWay];
    bool hasMoreRecords[bWay];
    Record currentRecords[bWay];

    // Merging rounds
    while (numChunks > 1) {
        // Merge 'bWay' chunks in each round
        for (int i = 0; i < numChunks; i += bWay) {
            // Calculate the number of chunks to merge in this iteration
            int remainingChunks = numChunks - i;
            int chunksToMerge = remainingChunks < bWay ? remainingChunks : bWay;

            // Initialize and open 'chunksToMerge' chunks for this round
            for (int j = 0; j < chunksToMerge; ++j) {
                int chunkIndex = i + j;
                chunks[j].file_desc = input_FileDesc;
                chunks[j].from_BlockId = chunkIndex * chunkSize;
                chunks[j].to_BlockId = (chunkIndex + 1) * chunkSize - 1;
                chunks[j].blocksInChunk = chunkSize;
                chunks[j].recordsInChunk = chunkSize * HP_GetMaxRecordsInBlock(input_FileDesc);

                recordIterators[j] = CHUNK_CreateRecordIterator(&chunks[j]);
                hasMoreRecords[j] = CHUNK_GetNextRecord(&recordIterators[j], &currentRecords[j]) == 0;
            }

            // Merge 'chunksToMerge' chunks from input_FileDesc to output_FileDesc
            while (true) {
                int minChunkIndex = -1;

                // Find the chunk with the minimum record
                for (int j = 0; j < chunksToMerge; ++j) {
                    if (hasMoreRecords[j]) {
                        if (minChunkIndex == -1 || shouldSwap(&currentRecords[j], &currentRecords[minChunkIndex]) > 0) {
                            minChunkIndex = j;
                        }
                    }
                }

                // No more records to merge
                if (minChunkIndex == -1) {
                    break;
                }

                // Insert the minimum record into the output file
                HP_InsertRecord(output_FileDesc, currentRecords[minChunkIndex]);

                // Get the next record from the same chunk
                hasMoreRecords[minChunkIndex] = CHUNK_GetNextRecord(&recordIterators[minChunkIndex], &currentRecords[minChunkIndex]) == 0;
            }
        }

        // Update the number of chunks for the next round
        numChunks = (numChunks / bWay) + (numChunks % bWay != 0);
    }
}
