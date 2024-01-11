#include <merge.h>
#include <stdio.h>
#include <stdbool.h>

void merge(int input_FileDesc, int chunkSize, int bWay, int output_FileDesc) {
    // Create an array of iterators, one for each chunk
    CHUNK_Iterator iterators[bWay];
    for (int i = 0; i < bWay; ++i) {
        iterators[i] = CHUNK_CreateIterator(input_FileDesc, chunkSize);
    }

    // Create an array to hold the current record from each chunk
    Record currentRecords[bWay];
    int hasMoreRecords[bWay];

    // Initialize the array with the first record from each chunk
    for (int i = 0; i < bWay; ++i) {
        if (CHUNK_GetNextRecord(&iterators[i], &currentRecords[i]) == 0) {
            hasMoreRecords[i] = 1;
        } else {
            hasMoreRecords[i] = 0;
        }
    }

    // Merge the chunks
    while (1) {
        // Find the smallest record
        int minIndex = -1;
        for (int i = 0; i < bWay; ++i) {
            if (hasMoreRecords[i] && (minIndex == -1 || currentRecords[i].id < currentRecords[minIndex].id)) {
                minIndex = i;
            }
        }

        // If no more records, break the loop
        if (minIndex == -1) {
            break;
        }

        // Write the smallest record to the output file
        HP_InsertEntry(output_FileDesc, currentRecords[minIndex]);

        // Get the next record from the chunk that provided the smallest record
        if (CHUNK_GetNextRecord(&iterators[minIndex], &currentRecords[minIndex]) != 0) {
            hasMoreRecords[minIndex] = 0;
        }
    }
}