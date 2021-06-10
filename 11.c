#include <stdio.h>
#include <stdlib.h>

static inline int dist(int q, int r)
{
    return (abs(q) + abs(q + r) + abs(r)) / 2;
}

int main(void)
{
    int q = 0, r = 0, d = 0, dmax = 0;
    FILE *f = fopen("11.txt", "r");
    if (f != NULL) {
        int c, p = 0;
        while ((c = fgetc(f)) != EOF) {
            switch (c)
            {
                case 'n': --r; break;
                case 's': ++r; break;
                case 'e': ++q; if (p == 's') --r; break;
                case 'w': --q; if (p == 'n') ++r; break;
            }
            p = c;
            d = dist(q, r);
            if (d > dmax) {
                dmax = d;
            }
        }
        fclose(f);
    }
    printf("Part 1: %d\n", d);
    printf("Part 2: %d\n", dmax);
    return 0;
}
