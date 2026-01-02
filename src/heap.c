#include "../include/heap.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/mman.h>

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
static chunk_t *pool_start = NULL; // Memory pool
static size_t pool_total = 0;

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

static void coalesce() {
    chunk_t *curr = heap.start;
    while (curr && curr->next) {
        if (!curr->inuse && !curr->next->inuse) {
            curr->size += sizeof(chunk_t) + curr->next->size;
            curr->next = curr->next->next;
        } else {
            curr = curr->next;
        }
    }
}

void hfree(void *ptr) {
    if (!ptr) {
        errno = EINVAL;
        return;
    }

    chunk_t *chunk = (chunk_t *)((char *)ptr - sizeof(chunk_t));
    if (!chunk->inuse) {
        errno = EINVAL;
        return;
    }

    chunk->inuse = 0;
    coalesce();
}

void heap_gc() {
    chunk_t *curr = heap.start;
    while (curr) {
        if (curr->inuse && curr->size == 0) {
            curr->inuse = 0;
        }
        curr = curr->next;
    }
    coalesce();
}

int detect_heap_spray(void *ptr) {
    if (!ptr) return 0;
    chunk_t *chunk = (chunk_t *)((char *)ptr - sizeof(chunk_t));
    if (chunk->size > heap.avail / 2) {
        return 1;
    }
    return 0;
}

void memory_pool_init(size_t pool_size) {
    pool_total = pool_size;
    pool_start = (chunk_t *)malloc(pool_size);
    if (!pool_start) {
        fprintf(stderr, "Memory pool allocation failed\n");
        exit(1);
    }
    pool_start->size = pool_size - sizeof(chunk_t);
    pool_start->inuse = 0;
    pool_start->next = NULL;
}

void *pool_alloc(size_t size) {
    if (!pool_start) return NULL;
    uint32_t aligned_size = ALIGN8(size);

    chunk_t *curr = pool_start;
    while (curr) {
        if (!curr->inuse && curr->size >= aligned_size) {
            // split
            if (curr->size >= aligned_size + sizeof(chunk_t) + 8) {
                chunk_t *new_chunk = (chunk_t *)((char *)curr + sizeof(chunk_t) + aligned_size);
                new_chunk->size = curr->size - aligned_size - sizeof(chunk_t);
                new_chunk->inuse = 0;
                new_chunk->next = curr->next;
                curr->size = aligned_size;
                curr->next = new_chunk;
            }
            curr->inuse = 1;
            return (void *)((char *)curr + sizeof(chunk_t));
        }
        curr = curr->next;
    }
    return NULL;
}

void pool_free(void *ptr) {
    if (!ptr) return;
    chunk_t *chunk = (chunk_t *)((char *)ptr - sizeof(chunk_t));
    chunk->inuse = 0;
}
