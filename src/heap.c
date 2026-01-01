#include "../include/heap.h"
#include <sys/mman.h>
#include <errno.h>
#include <stdint.h>


typedef struct chunk_t {
    uint32_t size;
    uint8_t inuse; // boolean
    struct chunk_t *next;
} chunk_t;

typedef struct heap_t {
    chunk_t *start;
    uint32_t avail; // available memory
};

static struct heap_t heap;

#define ALIGN8(x) (((x) + 7) & ~7)

int hinit(size_t size) {
    size = ALIGN8(size);

    void *mem = mmap(NULL, size, PROT_READ | PROT_WRITE,
                     MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (mem == MAP_FAILED) {
        errno = ENOMEM;
        return -1;
    }

    heap.start = (chunk_t *)mem;
    heap.avail = size;

    heap.start->size = size - sizeof(chunk_t);
    heap.start->inuse = 0;
    heap.start->next = NULL;

    return 0;
}

static chunk_t *find_free_chunk(uint32_t size) {
    chunk_t *curr = heap.start;
    while (curr) {
        if (!curr->inuse && curr->size >= size)
            return curr;
        curr = curr->next;
    }
    return NULL;
}
