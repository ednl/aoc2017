#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdbool.h>

#define N (57)  // number of lines in the input file
typedef struct comp {
    int a, b;
    bool used;
} Comp;
static Comp comp[N] = {0};

static int sort(const void *a, const void *b)
{
    const Comp *p = (const Comp *)a;
    const Comp *q = (const Comp *)b;
    int d = p->a - q->a;
    if (d == 0) {
        return p->b - q->b;
    }
    return d;
}

static int load(void)
{
    int i = 0;
    FILE *f = fopen("24.txt", "r");
    if (f != NULL) {
        int a, b;
        while (i < N && fscanf(f, "%d/%d ", &a, &b) == 2) {
            comp[i++] = a <= b ? (Comp){ a, b, false } : (Comp){ b, a, false };
        }
        fclose(f);
    }
    return i;
}

static int bridge(int count, int prev, int port)
{
    static int bmax = 0;
    int i = 0, next = prev;
    while (i < N) {
        if (!comp[i].used && (comp[i].a == port || comp[i].b == port)) {
            comp[i].used = true;
            next = prev + comp[i].a + comp[i].b;
            // printf("%d: %d-%d (%d) | ", count, comp[i].a, comp[i].b, next);
            next += bridge(count + 1, next, port == comp[i].a ? comp[i].b : comp[i].a);
            comp[i].used = false;
        }
        ++i;
    }
    if (next >= bmax) {
        bmax = next;
        printf("count %2d sum %4d\n", count, next);
    }
    return next;
}

int main(void)
{
    int n = load();
    qsort(comp, N, sizeof *comp, sort);
    for (int i = 0; i < n; ++i) {
        printf("%2d: %2d %2d\n", i, comp[i].a, comp[i].b);
    }
    printf("%d\n", bridge(0, 0, 0));  // 743 = too low, 23028 = too high

    return 0;
}
