#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>

#define N (57)
static int comp[N] = {0};

static int sort(const void *a, const void *b)
{
    int d = (*(const int *)a & 0xff00) - (*(const int *)b & 0xff00);
    if (d == 0) {
        return (*(const int *)a & 0xff) - (*(const int *)b & 0xff);
    }
    return d;
}

int main(void)
{
    int a, b, n = 0;

    FILE *f = fopen("24.txt", "r");
    while (n < N && fscanf(f, "%d/%d ", &a, &b) == 2) {
        comp[n++] = a <= b ? (a << 8) | b : (b << 8) | a;
    }
    fclose(f);

    qsort(comp, N, sizeof *comp, sort);
    for (int i = 0; i < n; ++i) {
        a = (comp[i] >> 8) & 0xff;
        b = comp[i] & 0xff;
        printf("%2d: %2d %2d\n", i, a, b);
    }

    int i = 0, port = 0, bridge = 0;
    // while (i < n) {
    //     while (comp[i] < port && i < n) {
    //         ++i;
    //     }
    //     if (i < n) {
    //         bridge +=
    //     }
    // }
    return 0;
}
