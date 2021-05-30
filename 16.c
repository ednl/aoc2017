#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define A ('a')
#define N (16)
#define M (N - 1)
#define L (N + 1)
#define CACHESIZE (100)

static uint8_t line[L] = {0};

static void spin(uint8_t count)
{
    while (count--) {
        uint8_t t = line[M];
        for (uint8_t i = M; i > 0; --i) {
            line[i] = line[i - 1];
        }
        line[0] = t;
    }
}

static void exchange(uint8_t i, uint8_t j)
{
    uint8_t t = line[i];
    line[i] = line[j];
    line[j] = t;
}

static void partner(uint8_t a, uint8_t b)
{
    for (uint8_t i = 0; i < N; ++i) {
        if (line[i] == a) {
            line[i] = b;
        } else if (line[i] == b) {
            line[i] = a;
        }
    }
}

void part1(FILE *fp)
{
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
}

int main(void)
{
    uint8_t cache[CACHESIZE][L] = {0};
    uint32_t loop = 0;

    FILE *fp = fopen("16.txt", "r");
    if (fp == NULL) {
        fprintf(stderr, "File not found.");
        return 1;
    }

    // Init
    uint8_t i, j = A;
    for (i = 0; i < N; ++i) {
        line[i] = j++;
    }
    strcpy((char *)cache[loop++], (char *)line);

    // Part 1
    //   0: abcdefghijklmnop
    //   1: cgpfhdnambekjiol
    part1(fp);
    printf("%s\n", line);

    // Part 2
    //  60: abcdefghijklmnop
    //  1B: gjmiofcnaehpdlbk
    while (loop < CACHESIZE) {
        // Test if original configuration
        i = 0;
        j = A;
        while (line[i++] == j++)
            ;
        if (i == L) {
            // Found the loop
            break;
        } else {
            // Save & next permutation
            strcpy((char *)cache[loop++], (char *)line);
            rewind(fp);
            part1(fp);
        }
    }
    fclose(fp);

    if (loop == CACHESIZE) {
        fprintf(stderr, "Cache too small.");
        return 2;
    }
    printf("%s\n", cache[(1000 * 1000 * 1000) % loop]);
    return 0;
}
