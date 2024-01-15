#include <merge.h>
#include <stdio.h>
#include <stdbool.h>
#include <chunk.h>

void merge(int input_FileDesc, int chunkSize, int bWay, int output_FileDesc ){
    // Υποθέτουμε ότι τα chunkSize blocks σχηματίζουν ένα CHUNK.
    CHUNK_Iterator iterator = CHUNK_CreateIterator(input_FileDesc, chunkSize * bWay);

    // Αρχική δέσμευση μνήμης για τα ενδιάμεσα αρχεία.
    int numChunks = 0;
    CHUNK chunk;
    while (CHUNK_GetNext(&iterator, &chunk) == 0) {
        numChunks++;
    }

    // Εκκίνηση από την αρχή του αρχείου.
    iterator = CHUNK_CreateIterator(input_FileDesc, chunkSize * bWay);

    while (numChunks > 1) {
        // Ένωση των bWay συρμών στο νέο ενδιάμεσο αρχείο.
        for (int i = 0; i < numChunks; i += bWay) {
            CHUNK mergedChunk;
            CHUNK_GetNext(&iterator, &mergedChunk);

            // Ταξινόμηση του mergedChunk.
            sort_Chunk(&mergedChunk);
        }

        // Αύξηση του iterator για τον επόμενο κύκλο.
        iterator = CHUNK_CreateIterator(input_FileDesc, chunkSize * bWay);

        // Μείωση του αριθμού των συρμών.
        numChunks = (numChunks % bWay == 0) ? (numChunks / bWay) : (numChunks / bWay + 1);
    }
}