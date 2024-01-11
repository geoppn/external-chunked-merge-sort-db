#include <merge.h>
#include <stdio.h>
#include <stdbool.h>
#include "chunk.h"  // Assume you have a header file for CHUNK functions
#include "hp_file.h"  // Assume you have a header file for HP functions

void merge(int input_FileDesc[], int numChunks, int output_FileDesc, int chunkSize) {
    // Create CHUNK Iterators for each input chunk
    CHUNK_Iterator chunkIterators[numChunks];
    for (int i = 0; i < numChunks; ++i) {
        chunkIterators[i] = CHUNK_CreateIterator(input_FileDesc[i], chunkSize);
    }

    // Initialize a buffer to hold one block from each input chunk
    CHUNK buffers[numChunks];
    for (int i = 0; i < numChunks; ++i) {
        CHUNK_GetNext(&chunkIterators[i], &buffers[i]);
    }

    // Initialize a buffer for the output block
    CHUNK outputBuffer;
    CHUNK_GetNext(&chunkIterators[0], &outputBuffer);

    while (true) {
        // Find the minimum record among the current records in the buffer
        Record minRecord;
        int minChunkIndex = -1;

        for (int i = 0; i < numChunks; ++i) {
            if (CHUNK_GetNextRecord(&buffers[i], &minRecord) == 0) {
                if (minChunkIndex == -1 || shouldSwap(&minRecord, &buffers[minChunkIndex]) > 0) {
                    minChunkIndex = i;
                }
            }
        }

        // Write the minimum record to the output file
        if (minChunkIndex != -1) {
            HP_InsertEntry(output_FileDesc, minRecord);
        }

        // Check if the output buffer is full
        if (CHUNK_GetNextRecord(&outputBuffer, &minRecord) != 0) {
            // Output buffer is full, write it to the output file
            // and load the next block from the corresponding input chunk
            HP_Unpin(output_FileDesc, outputBuffer.from_BlockId);
            CHUNK_GetNext(&chunkIterators[0], &outputBuffer);
        }
    }
}
