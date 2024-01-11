#include <merge.h>
#include <stdio.h>
#include <stdbool.h>

void merge(int input_FileDesc, int chunkSize, int bWay, int output_FileDesc) {
    int totalBlocks = HP_GetIdOfLastBlock(input_FileDesc);
    int numChunks = totalBlocks / chunkSize;
    int remainingBlocks = totalBlocks % chunkSize;

    // Initialize output block
    int outputBlockId = 1; // Start from the first block in the output database

    // Initialize buffer to hold blocks
    int buffer[bWay]; // Buffer to hold blocks from bWay chunks

    // Loop through chunks to perform merging
    for (int i = 0; i < numChunks; i += bWay) {
        int remainingChunks = numChunks - i;
        int blocksToMerge = remainingChunks < bWay ? remainingChunks : bWay;

        // Load blocks from bWay chunks into the buffer
        for (int j = 0; j < blocksToMerge; ++j) {
            CHUNK_Iterator iterator = CHUNK_CreateIterator(input_FileDesc, chunkSize);
            if (CHUNK_GetNext(&iterator, &buffer[j]) != 0) {
                // Handle situation when no more blocks are available in the chunk
                // You can implement the required logic here, such as loading next chunks or terminating
            }
        }

        // Process records within the buffer
        int outputCursor = 0; // Cursor for the output block

        // Find the minimum record among the blocks in the buffer
        while (true) {
            Record minRecord;
            minRecord.id = INT_MAX; // Set initial value to the maximum possible integer

            int minBlockIndex = -1;

            // Find the minimum record among the blocks in the buffer
            for (int j = 0; j < blocksToMerge; ++j) {
                Record currentRecord;
                if (CHUNK_GetIthRecordInChunk(&buffer[j], outputCursor, &currentRecord) == 0) {
                    if (currentRecord.id < minRecord.id) {
                        minRecord = currentRecord;
                        minBlockIndex = j; // Keep track of the minimum block index
                    }
                }
            }

            if (minBlockIndex != -1) {
                // Write the minimum record to the output block
                HP_InsertEntry(output_FileDesc, minRecord);
                outputCursor++;

                // Move to the next record in the block that provided the minimum record
                CHUNK_RecordIterator recordIterator = CHUNK_CreateRecordIterator(&buffer[minBlockIndex]);
                if (CHUNK_GetNextRecord(&recordIterator, &minRecord) != 0) {
                    // No more records in this block, remove it from the buffer
                    // You can implement the required logic here, such as loading next blocks or removing the block from the buffer
                }
            } else {
                // No more records to process in the blocks within the buffer
                break;
            }

            // Check if the output block is full
            if (outputCursor >= HP_GetMaxRecordsInBlock(output_FileDesc)) {
                // Write the filled output block to the output database
                HP_CloseFile(output_FileDesc);
                HP_OpenFile("output.db", &output_FileDesc);
                outputBlockId++;

                // Reset the cursor for the new output block
                outputCursor = 0;
            }
        }
    }

    // Process remaining blocks
    // You can follow a similar logic as above for the remaining blocks if necessary
    // Iterate through remainingBlocks or use a different approach based on your requirements
}
