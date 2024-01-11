#include <merge.h>
#include <stdio.h>
#include <stdbool.h>
#include <chunk.h>

void merge(int input_FileDesc, int chunkSize, int bWay, int output_FileDesc) {
    int totalBlocks = HP_GetIdOfLastBlock(input_FileDesc);
    int numChunks = totalBlocks / chunkSize; 
    CHUNK chunks[bWay];
    CHUNK_RecordIterator recordIterators[bWay];
    Record currentRecords[bWay];
    bool hasMoreRecords[bWay];

    for (int round = 0; round < numChunks; round += bWay) {
        // Initialize chunks and get first record from each chunk
        for (int i = 0; i < bWay && round + i < numChunks; ++i) {
            chunks[i].file_desc = input_FileDesc;
            chunks[i].from_BlockId = (round + i) * chunkSize + 1;
            chunks[i].to_BlockId = (round + i + 1) * chunkSize;
            chunks[i].blocksInChunk = chunkSize;
            chunks[i].recordsInChunk = chunkSize * HP_GetMaxRecordsInBlock(input_FileDesc);

            recordIterators[i] = CHUNK_CreateRecordIterator(&chunks[i]);
            hasMoreRecords[i] = CHUNK_GetNextRecord(&recordIterators[i], &currentRecords[i]) == 0;
        }

        // Merge records
        while (true) {
            // Find smallest record
            int minIndex = -1;
            for (int i = 0; i < bWay && round + i < numChunks; ++i) {
                if (hasMoreRecords[i] && (minIndex == -1 || currentRecords[i].id < currentRecords[minIndex].id)) {
                    minIndex = i;
                }
            }

            // If no more records, break
            if (minIndex == -1) {
                break;
            }

            // Write smallest record to output file
            HP_InsertEntry(output_FileDesc, currentRecords[minIndex]);

            // Get next record from the chunk that provided the smallest record
            hasMoreRecords[minIndex] = CHUNK_GetNextRecord(&recordIterators[minIndex], &currentRecords[minIndex]) == 0;
        }
    }
    for (int blockId = 1; blockId <= 56; ++blockId) {
        HP_PrintBlockEntries(output_FileDesc, blockId);
    }
}