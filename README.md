# Custom Heap Allocator

## Overview

This project implements a **custom heap allocator** in C, designed for educational purposes in the context of Operating Systems courses. The allocator allows dynamic memory allocation and deallocation on a manually managed heap using a **chunk-based linked list** approach.

The project demonstrates key concepts of memory management including:

* Heap initialization with `mmap`
* Dynamic allocation (`halloc`)
* Freeing memory (`hfree`)
* Chunk splitting and coalescing
* Error handling using `errno`
* Memory alignment for safe access

This allocator is a simplified version of the standard `malloc` and `free` functions, providing insight into low-level heap management and operating system memory mechanisms.

---

## Features

* **Heap Initialization**: Allocates a contiguous memory region using `mmap` with `MAP_PRIVATE | MAP_ANONYMOUS`.
* **Aligned Allocations**: Memory allocations are aligned to 8 bytes to ensure proper alignment for any data type.
* **Chunk Management**: Heap is divided into chunks, each storing metadata including size, allocation status, and pointer to the next chunk.
* **Chunk Splitting**: Large free chunks are split to minimize wasted memory.
* **Freeing and Coalescing**: Freed chunks are merged with adjacent free chunks to reduce fragmentation.
* **Error Handling**: Returns `NULL` and sets `errno` on allocation errors (`ENOMEM`, `EINVAL`, etc.).
* **Encapsulation**: Heap internals are private to the implementation file (`heap.c`); users interact only with the public API.

---

## File Structure

```text
project/
│
├── include/
│   └── heap.h           # Public API
│
└── src/
    ├── heap.c           # Implementation
    │
    └── main.c               # Test program
```

---

## Public API

```c
int hinit(size_t size);      // Initialize the heap with a given size
void *halloc(size_t size);   // Allocate memory from the heap
void hfree(void *ptr);       // Free a previously allocated memory block
```

---

## Usage

1. **Include the header** in your source files:

```c
#include "heap.h"
```

2. **Initialize the heap** before any allocation:

```c
if (hinit(4096) != 0) {
    perror("hinit failed");
    return 1;
}
```

3. **Allocate memory**:

```c
int *x = (int *)halloc(sizeof(int));
```

4. **Free memory**:

```c
hfree(x);
```

---

## Compilation Instructions

Assuming the following directory structure:

```
project/
├── include/heap.h
├── src/heap.c
└── main.c
```

Compile using GCC:

```bash
gcc -o heap_test main.c src/heap.c -I include
```

Run the test program:

```bash
./heap_test
```

Expected output for the provided `main.c`:

```
a = 42
b: 0 1 4 9 16 25 36 49 64 81
c = custom heap allocator
re-allocation successful
```

---

## Implementation Details

### Chunk Structure

```c
typedef struct chunk_t {
    uint32_t size;          // Size of user data
    uint8_t inuse;          // 1 = allocated, 0 = free
    struct chunk_t *next;   // Pointer to the next chunk
} chunk_t;
```

* Metadata is stored **at the start of each chunk**.
* `halloc` returns a pointer **after the metadata** to the user.
* The heap is managed as a **linked list of chunks**.

### Memory Alignment

All allocations are aligned to 8 bytes:

```c
#define ALIGN8(x) (((x) + 7) & ~7)
```

Ensures safe access for any data type.

### Heap Initialization

`hinit` uses `mmap` to request memory from the kernel:

```c
void *mem = mmap(NULL, size, PROT_READ | PROT_WRITE,
                 MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
```

* `MAP_ANONYMOUS` → memory not backed by any file
* `MAP_PRIVATE` → memory private to this process
* Memory is **page-aligned** and zero-initialized

### Allocation (`halloc`)

1. Align requested size
2. Search for a free chunk using `find_free_chunk`
3. Split large chunks to reduce waste
4. Mark chunk as `inuse` and return user pointer

### Freeing (`hfree`)

1. Convert pointer to chunk metadata
2. Set `inuse = 0`
3. Coalesce adjacent free chunks to reduce fragmentation

---

## Error Handling

* `halloc(0)` → returns `NULL` and sets `errno = EINVAL`
* Allocation failure (no free chunk) → returns `NULL`, `errno = ENOMEM`
* `hfree` with invalid pointer → sets `errno = EINVAL`
* Double free → sets `errno = EALREADY`

---

## Notes

* This is a **single-heap implementation**. Only one heap is managed at a time.
* Designed for **educational purposes**, not production.
* Thread-safety is **not implemented**.
* Cleanup function can be added to call `munmap` at the end of the program.

---

## References

* [Memory management - Wikipedia](https://en.wikipedia.org/wiki/Memory_management)
* [mmap man page](https://man7.org/linux/man-pages/man2/mmap.2.html)
* [C dynamic memory allocation](https://en.wikipedia.org/wiki/C_dynamic_memory_allocation)
* [Chunking (computing)](https://en.wikipedia.org/wiki/Chunking_%28computing%29)
