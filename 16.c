#include <stdio.h>
#include <stdint.h>

#define A ('a')
#define N (16)
#define M (N - 1)
#define L (N + 1)
static uint8_t line[L] = {0};

static void spin(uint8_t n)
{
    if (n > M) {
        n %= N;
    }
    if (n <= 0) {
        return;
    }
    while (n--) {
        uint8_t t = line[M];
        for (uint8_t i = M; i > 0; --i) {
            line[i] = line[i - 1];
        }
        line[0] = t;
    }
}

static void exchange(uint8_t i, uint8_t j)
{
    if (i > M || j > M) {
        return;
    }
    uint8_t t = line[i];
    line[i] = line[j];
    line[j] = t;
}

static void partner(uint8_t a, uint8_t b)
{
    if (a < A || a > A + M || b < A || b > A + M) {
        return;
    }
    for (uint8_t i = 0; i < N; ++i) {
        if (line[i] == a) {
            line[i] = b;
        } else if (line[i] == b) {
            line[i] = a;
        }
    }
}

int main(void)
{
    for (uint8_t i = 0; i < N; ++i) {
        line[i] = A + i;
    }

    // Part 1
    FILE *fp = fopen("16.txt", "r");
    if (fp != NULL) {
        int c;
        uint8_t i, j;
        while ((c = fgetc(fp)) != EOF) {
            if (c == 's') {
                if (fscanf(fp, "%hhu", &i) == 1) {
                    spin(i);
                }
            } else if (c == 'x') {
                if (fscanf(fp, "%hhu/%hhu", &i, &j) == 2) {
                    exchange(i, j);
                }
            } else if (c == 'p') {
                if (fscanf(fp, "%c/%c", &i, &j) == 2) {
                    partner(i, j);
                }
            }
        }
        fclose(fp);
    }
    printf("%s\n", line);

    // Part 2
    uint8_t cycle[N][L] = {0};
    for (uint8_t i = 0; i < N; ++i) {
        cycle[j]
    }
    const uint32_t n = 1000 * 1000 * 1000;
    // printf("%s\n", next);

    return 0;
}
