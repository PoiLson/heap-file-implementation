# Heap File Implementation

## About

This repository contains coursework developed for the *Database Systems Implementation* course at the Department of Informatics and Telecommunications, National and Kapodistrian University of Athens (NKUA).

The project focuses on implementing a heap file storage structure on top of a block-level memory manager.

## Main Idea

The goal of this project is to understand the internal storage mechanisms of database systems by implementing record management on top of a block-based file system.

Specifically, the project builds a heap file abstraction using a provided block file (BF) layer, which simulates disk storage with caching mechanisms (e.g., LRU/MRU replacement policies).

The implementation includes:

- creation and initialization of heap files
- insertion of records into blocks
- sequential search and retrieval of records
- management of metadata and block-level information

Each file is organized as a sequence of fixed-size blocks, where records are stored without any ordering or indexing, following the heap file model.

The project highlights the interaction between memory and disk, as well as the impact of block-level operations on system performance.

## Functionality

The following core functions are implemented:

- `HP_CreateFile` – creates and initializes a heap file
- `HP_OpenFile` – opens a heap file and loads metadata
- `HP_CloseFile` – closes the file and releases resources
- `HP_InsertEntry` – inserts a record into the file
- `HP_GetAllEntries` – retrieves all records matching a given id
- 
## Usage

The project includes multiple [`Makefile`](Makefile) targets for compiling and running the available examples.

### Block File example

To compile the executable that demonstrates the functionality of the BF library, run:

```make bf```

Then execute it with:

```./build/bf_main```

### Heap File example

To compile and run the heap file example, use:

```make hp```

This target removes any existing ```data.db``` file, compiles the heap file example, and executes it automatically.

### Extended test

To compile and run the extended test case, use:

```make test```

This target removes any existing ```data.db``` file, compiles the test program, and executes it with ```10000``` inserted records.

### Note

The same workflow applies to the available executables: first compile them through the corresponding ```make``` target, then run the generated executable from the ```build/```, [`build`](build), directory, if execution is not already included in the target.
