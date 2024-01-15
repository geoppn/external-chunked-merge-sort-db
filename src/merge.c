#include <merge.h>
#include <stdio.h>
#include <stdbool.h>
#include <chunk.h>

void merge(int input_FileDesc, int chunkSize, int bWay, int output_FileDesc) {
    int totalBlocks = HP_GetIdOfLastBlock(input_FileDesc);
    int numChunks = totalBlocks / chunkSize;
    int remainingBlocks = totalBlocks; // TEAM EDIT: ADDED TO HANDLE THE REMAINING BLOCKS FROM NON PERFECT DIVISION

    // Initialize arrays for chunks and record iterators
    CHUNK chunks[bWay];
    CHUNK_RecordIterator recordIterators[bWay];
    bool hasMoreRecords[bWay];
    Record currentRecords[bWay];

    // Merging rounds
    for (int i = 0; i < numChunks; i += bWay) {
        // Calculate the number of chunks to merge in this iteration
        int chunksToMerge = (i + bWay) <= numChunks ? bWay : (numChunks - i);

        // Initialize and open 'chunksToMerge' chunks for this round
        for (int j = 0; j < chunksToMerge; ++j) {
            int chunkIndex = i + j;
            chunks[j].file_desc = input_FileDesc;
            chunks[j].from_BlockId = chunkIndex * chunkSize + 1; // TEAM EDIT: ADDED +1
            chunks[j].to_BlockId = (chunkIndex + 1) * chunkSize; // TEAM EDIT: REMOVED -1
            chunks[j].blocksInChunk = chunkSize;
            chunks[j].recordsInChunk = chunkSize * HP_GetMaxRecordsInBlock(input_FileDesc);

            recordIterators[j] = CHUNK_CreateRecordIterator(&chunks[j]);
            hasMoreRecords[j] = CHUNK_GetNextRecord(&recordIterators[j], &currentRecords[j]) == 0;
        }

        // Merge 'chunksToMerge' chunks from input_FileDesc to output_FileDesc
        while (true) {
            int minChunkIndex = -1;

            // Find the chunk with the minimum record
            for (int j = 0; j < chunksToMerge; ++j) {
                if (hasMoreRecords[j]) {
                    if (minChunkIndex == -1 || shouldSwap(&currentRecords[j], &currentRecords[minChunkIndex]) > 0) {
                        minChunkIndex = j;
                    }
                }
            }

            // No more records to merge
            if (minChunkIndex == -1) {
                break;
            }

            // Insert the minimum record into the output file
            HP_InsertEntry(output_FileDesc, currentRecords[minChunkIndex]); // TEAM EDIT: FIXED INSERTRECORD TO INSERTENTRY

            // Get the next record from the same chunk
            hasMoreRecords[minChunkIndex] = CHUNK_GetNextRecord(&recordIterators[minChunkIndex], &currentRecords[minChunkIndex]) == 0;
        }
    }
    if (remainingBlocks > 0) {
        CHUNK remainingChunk;
        remainingChunk.file_desc = input_FileDesc;
        remainingChunk.from_BlockId = numChunks * chunkSize + 1;
        remainingChunk.to_BlockId = totalBlocks;
        remainingChunk.blocksInChunk = remainingBlocks;
        remainingChunk.recordsInChunk = remainingBlocks * HP_GetMaxRecordsInBlock(input_FileDesc);

        CHUNK_RecordIterator remainingRecordIterator = CHUNK_CreateRecordIterator(&remainingChunk);
        Record remainingRecord;
        bool hasMoreRemainingRecords = CHUNK_GetNextRecord(&remainingRecordIterator, &remainingRecord) == 0;

        while (hasMoreRemainingRecords) {
            HP_InsertEntry(output_FileDesc, remainingRecord);
            hasMoreRemainingRecords = CHUNK_GetNextRecord(&remainingRecordIterator, &remainingRecord) == 0;
        }
    }

}