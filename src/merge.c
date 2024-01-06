#include <merge.h>
#include <stdio.h>
#include <stdbool.h>

void merge(int input_FileDesc, int chunkSize, int bWay, int output_FileDesc) {
    int totalBlocks = HP_GetIdOfLastBlock(input_FileDesc);
    int numChunks = totalBlocks / chunkSize; // Calculate the total number of full chunks
    int remainingBlocks = totalBlocks % chunkSize; // Calculate the number of remaining blocks

    // Loop through full chunks to perform merging
    for (int i = 0; i < numChunks; i += bWay) {
        // Calculate the number of chunks to merge in this iteration
        int remainingChunks = numChunks - i;
        int chunksToMerge = remainingChunks < bWay ? remainingChunks : bWay;
        
        // Merge 'chunksToMerge' chunks from input_FileDesc to output_FileDesc
        for (int j = 0; j < chunksToMerge; ++j) {
            // Determine the current chunk's starting and ending block IDs
            int startBlockId = i * bWay * chunkSize + j * chunkSize;
            int endBlockId = startBlockId + chunkSize;
            
            // Create CHUNK_Iterator for the current chunk
            CHUNK_Iterator iterator = CHUNK_CreateIterator(input_FileDesc, chunkSize);
            iterator.current = startBlockId; // Set the initial block for iteration
            
            // Initialize CHUNK for merging
            CHUNK chunk;
            chunk.file_desc = output_FileDesc;
            chunk.from_BlockId = startBlockId;
            chunk.to_BlockId = endBlockId;
            chunk.blocksInChunk = chunkSize;
            chunk.recordsInChunk = chunkSize * HP_GetMaxRecordsInBlock(input_FileDesc);
            
            // Merge the chunk to the output file
            for (int k = 0; k < chunk.recordsInChunk; ++k) {
                Record record;
                if (CHUNK_GetIthRecordInChunk(&chunk, k, &record) == 0) {
                    CHUNK_UpdateIthRecord(&chunk, k, record); // Update records in the output chunk
                } else {
                    break; // No more records in this chunk
                }
            }
        }
    }

    // If there are remaining blocks, merge them as well
    if (remainingBlocks > 0) {
        // Calculate the number of chunks to merge in this iteration
        int remainingChunks = 1;
        int chunksToMerge = remainingChunks < bWay ? remainingChunks : bWay;
        
        // Merge 'chunksToMerge' chunks from input_FileDesc to output_FileDesc
        for (int j = 0; j < chunksToMerge; ++j) {
            // Determine the current chunk's starting and ending block IDs
            int startBlockId = numChunks * bWay * chunkSize + j * remainingBlocks;
            int endBlockId = totalBlocks;
            
            // Create CHUNK_Iterator for the current chunk
            CHUNK_Iterator iterator = CHUNK_CreateIterator(input_FileDesc, remainingBlocks);
            iterator.current = startBlockId; // Set the initial block for iteration
            
            // Initialize CHUNK for merging
            CHUNK chunk;
            chunk.file_desc = output_FileDesc;
            chunk.from_BlockId = startBlockId;
            chunk.to_BlockId = endBlockId;
            chunk.blocksInChunk = remainingBlocks;
            chunk.recordsInChunk = remainingBlocks * HP_GetMaxRecordsInBlock(input_FileDesc);
            
            // Merge the chunk to the output file
            for (int k = 0; k < chunk.recordsInChunk; ++k) {
                Record record;
                if (CHUNK_GetIthRecordInChunk(&chunk, k, &record) == 0) {
                    CHUNK_UpdateIthRecord(&chunk, k, record); // Update records in the output chunk
                } else {
                    break; // No more records in this chunk
                }
            }
        }
    }
}