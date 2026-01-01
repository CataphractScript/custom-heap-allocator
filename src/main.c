#include <stdio.h>
#include <string.h>
#include "../include/heap.h"

int main(void) {
    if (hinit(4096) != 0) {
        perror("hinit failed");
        return 1;
    }

    int *a = (int *)halloc(sizeof(int));
    if (!a) {
        perror("halloc a failed");
        return 1;
    }
    *a = 42;

    int *b = (int *)halloc(10 * sizeof(int));
    if (!b) {
        perror("halloc b failed");
        return 1;
    }

    for (int i = 0; i < 10; i++)
        b[i] = i * i;

    char *c = (char *)halloc(32);
    if (!c) {
        perror("halloc c failed");
        return 1;
    }
    strcpy(c, "custom heap allocator");

    printf("a = %d\n", *a);
    printf("b: ");
    for (int i = 0; i < 10; i++)
        printf("%d ", b[i]);
    printf("\n");
    printf("c = %s\n", c);

    hfree(a);
    hfree(b);
    hfree(c);

    void *d = halloc(128);
    if (!d) {
        perror("halloc d failed");
        return 1;
    }

    printf("re-allocation successful\n");

    hfree(d);

    return 0;
}