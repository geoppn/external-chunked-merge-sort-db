#include <merge.h>
#include <stdio.h>
#include <stdbool.h>
#include <chunk.h>

void merge(int input_FileDesc, int chunkSize, int bWay, int output_FileDesc, int roundChunks) {
    // Initialize arrays for chunks and record iterators
    CHUNK chunks[bWay];
    CHUNK_RecordIterator recordIterators[bWay];
    bool hasMoreRecords[bWay];
    Record currentRecords[bWay];

    // Initialize and open 'roundChunks' chunks for this round
    for (int i = 0; i < roundChunks; ++i) {
        chunks[i].file_desc = input_FileDesc;
        chunks[i].from_BlockId = i * chunkSize;
        chunks[i].to_BlockId = (i + 1) * chunkSize - 1;
        chunks[i].blocksInChunk = chunkSize;
        chunks[i].recordsInChunk = chunkSize * HP_GetMaxRecordsInBlock(input_FileDesc);

        recordIterators[i] = CHUNK_CreateRecordIterator(&chunks[i]);
        hasMoreRecords[i] = true;

        // Initialize current record for each chunk
        if (CHUNK_GetNextRecord(&recordIterators[i], &currentRecords[i]) != 0) {
            hasMoreRecords[i] = false;
        }
    }

    // Merge 'roundChunks' chunks from input_FileDesc to output_FileDesc
    while (true) {
        Record minRecord;
        int minChunkIndex = -1;

        // Find the chunk with the minimum record
        for (int j = 0; j < roundChunks; ++j) {
            if (hasMoreRecords[j]) {
                if (minChunkIndex == -1 || shouldSwap(&currentRecords[j], &minRecord) > 0) {
                    minChunkIndex = j;
                    minRecord = currentRecords[j];
                }
            }
        }

        // No more records to merge
        if (minChunkIndex == -1) {
            break;
        }

        // Insert the minimum record into the output file
        if (HP_InsertRecord(output_FileDesc, minRecord) != 1) {
            fprintf(stderr, "Error: Failed to insert record into the output file.\n");
            return;
        }

        // Get the next record for the chosen chunk
        if (CHUNK_GetNextRecord(&recordIterators[minChunkIndex], &currentRecords[minChunkIndex]) != 0) {
            hasMoreRecords[minChunkIndex] = false;
        }
    }
}