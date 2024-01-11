#include <merge.h>
#include <stdio.h>
#include <stdbool.h>
#include <chunk.h>

void merge(int input_FileDesc, int chunkSize, int bWay, int output_FileDesc) {
    int totalBlocks = HP_GetIdOfLastBlock(input_FileDesc);
    int numChunks = totalBlocks / chunkSize;
    int remainingBlocks = totalBlocks % chunkSize;

    // Initialize CHUNK iterators and RECORD iterators for each chunk
    CHUNK_Iterator chunkIterators[numChunks];
    CHUNK_RecordIterator recordIterators[numChunks];

    for (int i = 0; i < numChunks; ++i) {
        chunkIterators[i] = CHUNK_CreateIterator(input_FileDesc, chunkSize);
        recordIterators[i] = CHUNK_CreateRecordIterator(&chunkIterators[i]);
    }

    // Iterate through chunks to perform merging
    for (int i = 0; i < numChunks; i += bWay) {
        // Calculate the number of chunks to merge in this iteration
        int remainingChunks = numChunks - i;
        int chunksToMerge = remainingChunks < bWay ? remainingChunks : bWay;

        // Merge 'chunksToMerge' chunks from input_FileDesc to output_FileDesc
        for (int j = 0; j < chunksToMerge; ++j) {
            CHUNK_Iterator chunkIterator = chunkIterators[i + j];
            CHUNK chunk;

            if (CHUNK_GetNext(&chunkIterator, &chunk) != 0) {
                // No more chunks to read
                break;
            }

            // Your merging logic here
            Record minRecord;

            // Initialize minRecord with a placeholder
            if (CHUNK_GetNextRecord(&recordIterators[i + j], &minRecord) == 0) {
                // Loop through the rest of the records in the chunk and find the minimum record
                for (int k = 1; k < chunk.recordsInChunk; ++k) {
                    Record currentRecord;
                    if (CHUNK_GetNextRecord(&recordIterators[i + j], &currentRecord) == 0) {
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

            // Update the iterators for the next chunk
            chunkIterators[i + j] = chunkIterator;
            recordIterators[i + j] = CHUNK_CreateRecordIterator(&chunkIterator);
        }
    }

    // Handle remaining blocks if any
    if (remainingBlocks > 0) {
        CHUNK_Iterator remainingChunkIterator = chunkIterators[numChunks - 1];
        CHUNK_RecordIterator remainingRecordIterator = CHUNK_CreateRecordIterator(&remainingChunkIterator);

        // Merge remaining blocks
        for (int i = 0; i < remainingBlocks; ++i) {
            Record minRecord;

            // Initialize minRecord with a placeholder
            if (CHUNK_GetNextRecord(&remainingRecordIterator, &minRecord) == 0) {
                // Loop through the rest of the records in the block and find the minimum record
                for (int k = 1; k < HP_GetMaxRecordsInBlock(input_FileDesc); ++k) {
                    Record currentRecord;
                    if (CHUNK_GetNextRecord(&remainingRecordIterator, &currentRecord) == 0) {
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