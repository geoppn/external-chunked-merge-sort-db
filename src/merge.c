#include <merge.h>
#include <stdio.h>
#include <stdbool.h>
#include <chunk.h>

void merge(int input_FileDesc, int chunkSize, int bWay, int output_FileDesc) {
    // Initialize arrays for chunks and record iterators
    CHUNK chunks[bWay];
    CHUNK_RecordIterator recordIterators[bWay];
    Record currentRecords[bWay];
    bool hasMoreRecords[bWay];

    // Initialize and open 'bWay' chunks for this round
    for (int i = 0; i < bWay; ++i) {
        chunks[i].file_desc = input_FileDesc;
        chunks[i].from_BlockId = i * chunkSize;
        chunks[i].to_BlockId = (i + 1) * chunkSize - 1;
        chunks[i].blocksInChunk = chunkSize;
        chunks[i].recordsInChunk = chunkSize * HP_GetMaxRecordsInBlock(input_FileDesc);

        recordIterators[i] = CHUNK_CreateRecordIterator(&chunks[i]);
        hasMoreRecords[i] = CHUNK_GetNextRecord(&recordIterators[i], &currentRecords[i]) == 0;
    }

    // Merge 'bWay' chunks from input_FileDesc to output_FileDesc
    while (true) {
        int minIndex = -1;

        // Find the chunk with the minimum record
        for (int i = 0; i < bWay; ++i) {
            if (hasMoreRecords[i] && (minIndex == -1 || shouldSwap(&currentRecords[minIndex], &currentRecords[i]))) {
                minIndex = i;
            }
        }

        // No more records to merge
        if (minIndex == -1) {
            break;
        }

        // Insert the minimum record into the output file
        HP_InsertEntry(output_FileDesc, currentRecords[minIndex]);

        // Get next record from the chunk that provided the smallest record
        hasMoreRecords[minIndex] = CHUNK_GetNextRecord(&recordIterators[minIndex], &currentRecords[minIndex]) == 0;
    }
}