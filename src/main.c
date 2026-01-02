#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "heap.h"

int main(void) {
    if (hinit(4096) != 0) {
        perror("hinit failed");
        return 1;
    }

    int *a = (int *)halloc(sizeof(int));
    if (!a) {
        fprintf(stderr, "halloc failed for a: %s\n", strerror(errno));
        return 1;
    }
    *a = 42;

    int *b = (int *)halloc(10 * sizeof(int));
    if (!b) {
        fprintf(stderr, "halloc failed for b: %s\n", strerror(errno));
        hfree(a); 
        return 1;
    }

    for (size_t i = 0; i < 10; i++)
        b[i] = i * i;

    char *c = (char *)halloc(32);
    if (!c) {
        fprintf(stderr, "halloc failed for c: %s\n", strerror(errno));
        hfree(a); hfree(b);
        return 1;
    }
    strcpy(c, "custom heap allocator");

    printf("a = %d\n", *a);
    printf("b: ");
    for (size_t i = 0; i < 10; i++)
        printf("%d ", b[i]);
    printf("\n");
    printf("c = %s\n", c);

    hfree(a); a = NULL;
    hfree(b); b = NULL;
    hfree(c); c = NULL;

    void *d = halloc(128);
    if (!d) {
        fprintf(stderr, "halloc failed for d: %s\n", strerror(errno));
        return 1;
    }
    printf("Re-allocation successful\n");
    hfree(d); d = NULL;

    memory_pool_init(1024);
    char *p1 = (char *)pool_alloc(100);
    char *p2 = (char *)pool_alloc(50);
    if (!p1 || !p2) {
        fprintf(stderr, "Memory pool allocation failed\n");
        return 1;
    }
    pool_free(p1); pool_free(p2);

    if (detect_heap_spray(p2)) {
        printf("Heap spraying detected!\n");
    }

    heap_gc();

    printf("All tests completed successfully!\n");
    return 0;
}
