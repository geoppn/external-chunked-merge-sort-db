#include <merge.h>
#include <stdio.h>
#include <stdbool.h>
#include <chunk.h>

void merge(int input_FileDesc, int chunkSize, int bWay, int output_FileDesc, int roundChunks) {
    // Initialize arrays for chunks and record iterators
    CHUNK chunks[bWay];
    CHUNK_RecordIterator recordIterators[bWay];
    bool hasMoreRecords[bWay];

    // Initialize and open 'roundChunks' chunks for this round
    for (int i = 0; i < roundChunks; ++i) {
        chunks[i].file_desc = input_FileDesc;
        chunks[i].from_BlockId = i * chunkSize + roundChunks * chunkSize * (output_FileDesc - 1);
        chunks[i].to_BlockId = (i + 1) * chunkSize + roundChunks * chunkSize * (output_FileDesc - 1) - 1;
        chunks[i].blocksInChunk = chunkSize;
        chunks[i].recordsInChunk = chunkSize * HP_GetMaxRecordsInBlock(input_FileDesc);

        recordIterators[i] = CHUNK_CreateRecordIterator(&chunks[i]);
        hasMoreRecords[i] = true;
    }

    // Merge 'roundChunks' chunks from input_FileDesc to output_FileDesc
    Record minRecord;
    bool hasMinRecord = false;
    
    while (true) {
        int minChunkIndex = -1;

        // Find the chunk with the minimum record
        for (int j = 0; j < roundChunks; ++j) {
            if (hasMoreRecords[j] && CHUNK_GetNextRecord(&recordIterators[j], &minRecord) == 0) {
                if (!hasMinRecord || shouldSwap(&minRecord, &chunks[minChunkIndex].from_Record) > 0) {
                    minChunkIndex = j;
                    hasMinRecord = true;
                }
            } else {
                hasMoreRecords[j] = false;
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
    }
}