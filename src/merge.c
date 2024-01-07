#include <merge.h>
#include <stdio.h>
#include <stdbool.h>

void merge(int input_FileDesc, int chunkSize, int bWay, int output_FileDesc) {
    int totalBlocks = HP_GetIdOfLastBlock(input_FileDesc);
    int numChunks = totalBlocks / chunkSize;
    int remainingBlocks = totalBlocks % chunkSize;

    // Initialize output chunk and buffer
    CHUNK outputChunk;
    outputChunk.file_desc = output_FileDesc;
    outputChunk.from_BlockId = 1; // Start from the first block in the output database
    outputChunk.to_BlockId = 1; // Initialize output to the first block
    outputChunk.blocksInChunk = 1; // Output chunk contains one block
    outputChunk.recordsInChunk = chunkSize * HP_GetMaxRecordsInBlock(output_FileDesc);

    CHUNK buffer[bWay]; // Buffer to hold blocks from bWay chunks

    // Loop through chunks to perform merging
    for (int i = 0; i < numChunks; i += bWay) {
        int remainingChunks = numChunks - i;
        int chunksToMerge = remainingChunks < bWay ? remainingChunks : bWay;

        // Load one block from each chunk into the buffer
        for (int j = 0; j < chunksToMerge; ++j) {
            CHUNK_Iterator iterator = CHUNK_CreateIterator(input_FileDesc, chunkSize);
            if (CHUNK_GetNext(&iterator, &buffer[j]) != 0) {
                // Handle situation when no more blocks are available in the chunk
                // You can implement the required logic here, such as loading next chunks or terminating
            }
        }

        // Initialize the record cursor for each chunk in the buffer
        int recordCursors[bWay];
        for (int j = 0; j < chunksToMerge; ++j) {
            recordCursors[j] = buffer[j].from_BlockId; // Initialize cursor to the first block of each chunk
        }

        // Process records within the buffer
        while (true) {
            Record minRecord;
            minRecord.id = INT_MAX; // Set initial value to the maximum possible integer

            int minBlockIndex = -1;

            // Find the minimum record among the chunks in the buffer
            for (int j = 0; j < chunksToMerge; ++j) {
                Record currentRecord;
                if (recordCursors[j] <= buffer[j].to_BlockId) {
                    // Fetch record from the chunk
                    if (CHUNK_GetIthRecordInChunk(&buffer[j], recordCursors[j], &currentRecord) == 0) {
                        if (currentRecord.id < minRecord.id) {
                            minRecord = currentRecord;
                            minBlockIndex = j; // Keep track of the minimum block index
                        }
                    }
                }
            }

            if (minBlockIndex != -1) {
                // Write the minimum record to the output chunk
                CHUNK_UpdateIthRecord(&outputChunk, outputChunk.to_BlockId, minRecord);
                outputChunk.to_BlockId++;

                // Move to the next record in the chunk that provided the minimum record
                recordCursors[minBlockIndex]++;
            } else {
                // No more records to process in the chunks within the buffer
                break;
            }

            // Check if the output block is full
            if (outputChunk.to_BlockId - outputChunk.from_BlockId >= chunkSize) {
                // Write the filled output block to the output database
                // Replace outputChunk with a new empty block
                // Reset the cursor to the first block in the new outputChunk
                outputChunk.from_BlockId = outputChunk.to_BlockId;
            }
        }
    }

    // Process remaining blocks
    // You can follow a similar logic as above for the remaining blocks if necessary
    // Iterate through remainingBlocks or use a different approach based on your requirements
}