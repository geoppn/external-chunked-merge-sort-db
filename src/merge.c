#include <merge.h>
#include <stdio.h>
#include <stdbool.h>
#include <chunk.h>

void merge(int input_FileDesc, int chunkSize, int bWay, int output_FileDesc) {
    // Calculate the total number of blocks in the input file
    int totalBlocks = HP_GetIdOfLastBlock(input_FileDesc);
    
    // Calculate the number of chunks in each phase
    int numChunks = totalBlocks / chunkSize;

    // Continue merging until we have a single chunk
    while (numChunks > 1) {
        // Initialize and open 'bWay' chunks for this round
        CHUNK chunks[bWay];
        CHUNK_RecordIterator recordIterators[bWay];
        bool hasMoreRecords[bWay];

        for (int j = 0; j < bWay; ++j) {
            int chunkIndex = numChunks - bWay + j;

            // Initialize the chunk
            chunks[j].file_desc = input_FileDesc;
            chunks[j].from_BlockId = chunkIndex * chunkSize;
            chunks[j].to_BlockId = (chunkIndex + 1) * chunkSize - 1;
            chunks[j].blocksInChunk = chunkSize;
            chunks[j].recordsInChunk = chunkSize * HP_GetMaxRecordsInBlock(input_FileDesc);

            // Create a record iterator for the chunk
            recordIterators[j] = CHUNK_CreateRecordIterator(&chunks[j]);
            hasMoreRecords[j] = true;
        }

        // Merge 'bWay' chunks from input_FileDesc to output_FileDesc
        while (true) {
            Record minRecord;
            int minChunkIndex = -1;

            // Find the chunk with the minimum record
            for (int j = 0; j < bWay; ++j) {
                if (hasMoreRecords[j] && CHUNK_GetNextRecord(&recordIterators[j], &minRecord) == 0) {
                    if (minChunkIndex == -1 || shouldSwap(&minRecord, &minRecord) > 0) {
                        minChunkIndex = j;
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
            if (HP_InsertEntry(output_FileDesc, minRecord) != 1) {
                fprintf(stderr, "Error: Failed to insert record into the output file.\n");
                return;
            }
        }

        // Update the number of chunks for the next round
        numChunks = (numChunks / bWay) + (numChunks % bWay != 0);
    }
}