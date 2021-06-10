#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdbool.h>

#define N (57)  // number of lines in the input file
typedef struct comp {
    int a, b, val;
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
            comp[i++] = a <= b ? (Comp){a, b, a + b, false} : (Comp){b, a, a + b, false};
        }
        fclose(f);
    }
    return i;
}

static int bridge(int count, int span, int port, int nmin, int nmax)
{
    int i = nmin, b;
    while (i < nmax && comp[i].a < port) {
        ++i;
    }
    if (i == nmax) {
        return span;
    }
    while (i < nmax && comp[i].a == port) {
        if (!comp[i].used) {
            comp[i].used = true;
            b += bridge(count + 1, span + comp[i].val, comp[i].b, i + 1, nmax);
            comp[i].used = false;
        }
        ++i;
    }
    return bmax;
}

int main(void)
{
    int n = load();
    qsort(comp, N, sizeof *comp, sort);
    for (int i = 0; i < n; ++i) {
        printf("%2d: %2d %2d\n", i, comp[i].a, comp[i].b);
    }
    printf("%d\n", bridge(0, 0, 0, n));  // 743 = too low

    return 0;
}
