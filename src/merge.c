#include <merge.h>
#include <stdio.h>
#include <stdbool.h>
#include <chunk.h>

void merge(int input_FileDesc, int chunkSize, int bWay, int output_FileDesc) {
    int totalBlocks = HP_GetIdOfLastBlock(input_FileDesc);
    int numChunks = totalBlocks / chunkSize;
    int remainingBlocks = totalBlocks % chunkSize;

    // Initialize CHUNK iterators for each chunk
    CHUNK_Iterator iterators[numChunks];
    for (int i = 0; i < numChunks; ++i) {
        iterators[i] = CHUNK_CreateIterator(input_FileDesc, chunkSize);
    }

    // Iterate through chunks to perform merging
    for (int i = 0; i < numChunks; i += bWay) {
        // Calculate the number of chunks to merge in this iteration
        int remainingChunks = numChunks - i;
        int chunksToMerge = remainingChunks < bWay ? remainingChunks : bWay;

        // Merge 'chunksToMerge' chunks from input_FileDesc to output_FileDesc
        for (int j = 0; j < chunksToMerge; ++j) {
            CHUNK_Iterator iterator = iterators[i + j];

            CHUNK chunk;
            if (CHUNK_GetNext(&iterator, &chunk) != 0) {
                // No more chunks to read
                break;
            }

            // Your merging logic here
            Record minRecord;

            // Initialize minRecord with a placeholder
            if (CHUNK_GetNextRecord(&iterator, &minRecord) == 0) {
                // Loop through the rest of the chunks and find the minimum record
                for (int k = 1; k < chunk.recordsInChunk; ++k) {
                    Record currentRecord;
                    if (CHUNK_GetNextRecord(&iterator, &currentRecord) == 0) {
                        if (shouldSwap(&currentRecord, &minRecord) < 0) {
                            minRecord = currentRecord;
                        }
                    } else {
                        // No more records in this chunk
                        break;
                    }
                }

                // Write the minimum record to the output
                if (HP_InsertEntry(output_FileDesc, minRecord) != 1) {
                    // Handle error
                    fprintf(stderr, "Error: Failed to insert record into the output file.\n");
                    return;
                }
            }

            // Update the iterator for the next chunk
            iterators[i + j] = iterator;
        }
    }

    // Handle remaining blocks if any
    if (remainingBlocks > 0) {
        // Merge remaining blocks
        for (int i = 0; i < remainingBlocks; ++i) {
            // Your merging logic here
            Record minRecord;

            // Similar logic as above for the remaining blocks
            if (CHUNK_GetNextRecord(&iterators[i], &minRecord) == 0) {
                for (int j = 1; j < remainingBlocks; ++j) {
                    Record currentRecord;
                    if (CHUNK_GetNextRecord(&iterators[j], &currentRecord) == 0) {
                        if (shouldSwap(&currentRecord, &minRecord) < 0) {
                            minRecord = currentRecord;
                        }
                    } else {
                        // No more records in this block
                        break;
                    }
                }

                // Write the minimum record to the output
                if (HP_InsertEntry(output_FileDesc, minRecord) != 1) {
                    // Handle error
                    fprintf(stderr, "Error: Failed to insert record into the output file.\n");
                    return;
                }
            }
        }
    }
}
