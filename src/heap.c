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

static void split_chunk(chunk_t *chunk, uint32_t size) {
    if (chunk->size >= size + sizeof(chunk_t) + 8) {
        chunk_t *new_chunk = (chunk_t *)((char *)chunk + sizeof(chunk_t) + size);
        new_chunk->size = chunk->size - size - sizeof(chunk_t);
        new_chunk->inuse = 0;
        new_chunk->next = chunk->next;

        chunk->size = size;
        chunk->next = new_chunk;
    }
}

void *halloc(size_t size) {
    if (size == 0) {
        errno = EINVAL;
        return NULL;
    }

    uint32_t aligned_size = ALIGN8(size);
    chunk_t *chunk = find_free_chunk(aligned_size);
    if (!chunk) {
        errno = ENOMEM;
        return NULL;
    }

    split_chunk(chunk, aligned_size);
    chunk->inuse = 1;

    return (void *)((char *)chunk + sizeof(chunk_t));
}
