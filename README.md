# External Chunked Merge Sort Database

This project implements an external merge sort algorithm for database records using chunked processing. It is designed to efficiently sort and merge large datasets that do not fit entirely in memory, leveraging custom heap file and buffer management.

## Features

- Chunked Sorting: Sorts records in manageable chunks for external memory efficiency.
- Merge Operations: Merges sorted chunks to produce a fully sorted dataset.
- Custom Heap File Management: Handles reading and writing of records using a custom heap file format.
- Buffer Management: Optimizes disk I/O using a buffer manager.

## Directory Structure

- `src/` — Core source files for chunking, sorting, merging, and record management.
- `include/` — Header files for all modules.
- `lib/` — Precompiled libraries for buffer and heap file operations.
- `examples/` — Example usage and test programs.
- `Makefile` — Build instructions.

## Dependencies

- Standard C libraries
- Provided precompiled libraries in `lib/`

## Authors

- Georgios Papaioannou
- Michail Aretakis
  
