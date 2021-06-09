#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>

#define N (57)
typedef struct comp {
    uint8_t id, a, b;
} Comp, *pComp;

static Comp comp[N] = {0};

static int sort(const void *a, const void *b)
{
    const Comp *p = (const Comp *)a;
    const Comp *q = (const Comp *)b;
    int d = p->a - q->a;
    if (d == 0) {
        d = p->b - q->b;
        if (d == 0) {
            return p->id - q->id;
        }
    }
    return d;
}

static uint8_t load(void)
{
    uint8_t i = 0;
    FILE *f = fopen("24.txt", "r");
    if (f != NULL) {
        uint8_t a, b;
        while (i < N && fscanf(f, "%"SCNd8"/%"SCNd8" ", &a, &b) == 2) {
            comp[i] = a <= b ? (Comp){i, a, b} : (Comp){i, b, a};
            ++i;
        }
        fclose(f);
    }
    return i;
}

int main(void)
{
    uint8_t n = load();
    qsort(comp, N, sizeof *comp, sort);
    for (int i = 0; i < n; ++i) {
        printf("%2d: %2d %2d\n", (comp[i].id = (uint8_t)i), comp[i].a, comp[i].b);
    }

    struct stackitem {
        int index;
        int bridge;
    };
    struct stackitem stack[100] = {0};
    int stacklen = 0;

    int i = 0, b = 0, maxspan = 0;
    stack[stacklen++] = (struct stackitem){ .index = i, .bridge = b };
    while (stacklen > 0) {
        --stacklen;
        i = stack[stacklen].index;
        b = stack[stacklen].bridge;
        b += comp[i].a + comp[i].b;
        if (b > maxspan) {
            maxspan = b;
        }
        int j = i + 1;
        while (comp[j].a < comp[i].b && j < n) {
            ++j;
        }
        while (comp[j].a == comp[i].b && comp[j].id && j < n && stacklen < 100) {
            stack[stacklen++] = (struct stackitem){ .index = j, .bridge = b };
            comp[j].id = 0;
        }
        if (stacklen == 100) {
            printf("%d\n", maxspan);
            fprintf(stderr, "Stack too small\n");
            exit(1);
        }
    }
    printf("%d\n", maxspan);  // 237 = too low

    // uint8_t port = 0;
    // int bridge = 0;
    // while (i < n) {
    //     while (i < n && comp[i].a < port) {
    //         ++i;
    //     }
    //     if (i < n) {
    //         printf("%d-%d ", comp[i].a, comp[i].b);
    //         port = comp[i].b;
    //         bridge += port;
    //         ++i;
    //     }
    // }
    // printf("= %d\n", bridge);
    return 0;
}
