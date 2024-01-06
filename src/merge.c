#include <merge.h>
#include <stdio.h>
#include <stdbool.h>

void merge(int input_FileDesc, int chunkSize, int bWay, int output_FileDesc) {
    int totalBlocks = HP_GetIdOfLastBlock(input_FileDesc);
    int numChunks = totalBlocks / chunkSize; // Calculate the total number of chunks
    
    // Loop through chunks to perform merging
    for (int i = 0; i < numChunks; i += bWay) {
        // Calculate the number of chunks to merge in this iteration
        int remainingChunks = numChunks - i;
        int chunksToMerge = remainingChunks < bWay ? remainingChunks : bWay;
        
        // Merge 'chunksToMerge' chunks from input_FileDesc to output_FileDesc
        for (int j = 0; j < chunksToMerge; ++j) {
            // Determine the current chunk's starting and ending block IDs
            int startBlockId = i * chunkSize + j * chunkSize; // Adjusted start block calculation
            int endBlockId = startBlockId + chunkSize - 1;
            
            // Make sure 'endBlockId' does not exceed the total blocks
            if (endBlockId >= totalBlocks) {
                endBlockId = totalBlocks - 1; // Adjust 'endBlockId' to stay within bounds
            }
            
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
            while (iterator.current <= endBlockId) {
                Record record;
                if (CHUNK_GetIthRecordInChunk(&chunk, iterator.current, &record) == 0) {
                    CHUNK_UpdateIthRecord(&chunk, iterator.current, record); // Update records in the output chunk
                } else {
                    break; // No more records in this chunk
                }
                iterator.current++; // Move to the next block within the chunk
            }
        }
    }
}

