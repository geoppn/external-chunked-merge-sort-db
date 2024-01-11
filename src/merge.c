#include <merge.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

void merge(int input_FileDesc, int chunkSize, int bWay, int output_FileDesc) {
    int numChunks = HP_GetIdOfLastBlock(input_FileDesc) / chunkSize;

    // Initialize CHUNK_Iterators for each chunk
    CHUNK_Iterator iterators[numChunks];
    Record buffers[numChunks];

    // Initialize iterators and buffers for each chunk
    for (int i = 0; i < numChunks; ++i) {
        iterators[i] = CHUNK_CreateIterator(input_FileDesc, chunkSize);
        CHUNK_GetNext(&iterators[i], NULL);  // Move to the first block in each chunk
        if (CHUNK_GetNextRecord(&iterators[i], &buffers[i]) != 0) {
            fprintf(stderr, "Error: Failed to read initial record from chunk %d\n", i);
            return;
        }
    }

    // Process chunks until all records are read
    while (true) {
        // Find the index of the minimum record
        int minChunkIndex = -1;
        Record minRecord;

        for (int i = 0; i < numChunks; ++i) {
            if (CHUNK_GetNextRecord(&iterators[i], &buffers[i]) == 0) {
                if (minChunkIndex == -1 || shouldSwap(&buffers[i], &minRecord)) {
                    minChunkIndex = i;
                    minRecord = buffers[i];
                }
            }
        }

        if (minChunkIndex == -1) {
            // No more records to process
            break;
        }

        // Write the smallest record to the output file
        if (HP_InsertEntry(output_FileDesc, minRecord) != 1) {
            fprintf(stderr, "Error: Failed to insert record into output file\n");
            return;
        }
    }
}
