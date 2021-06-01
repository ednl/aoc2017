#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DIM (201)
#define LEN (50)

static char diag[DIM][DIM + 1] = {0};

static void load(void)
{
    FILE *f = fopen("19.txt", "r");
    if (f == NULL) {
        fprintf(stderr, "File not found");
        exit(1);
    }
    char *buf = NULL;
    size_t buflen = 0, line = 0;
    ssize_t len;
    while ((len = getline(&buf, &buflen, f)) != -1 && line < DIM) {
        memcpy(diag[line++], buf, len - 1);
    }
    fclose(f);
    free(buf);
}

int main(void)
{
    char part1[LEN] = {0};
    size_t part1len = 0;
    int part2 = 1;

    load();
    int i = 0, j = 0;
    while (diag[i][j] && diag[i][j] != '|') {
        ++j;
    }

    int d;
    while(1) {
        d = i == 0 || diag[i - 1][j] == ' ' ? 1 : -1;
        if (diag[i + d][j] == ' ') {
            break;
        }
        while (diag[i + d][j] != ' ') {
            i += d;
            if (diag[i][j] >= 'A' && diag[i][j] <= 'Z') {
                part1[part1len++] = diag[i][j];
            }
            ++part2;
        }

        d = j == 0 || diag[i][j - 1] == ' ' ? 1 : -1;
        if (diag[i][j + d] == ' ') {
            break;
        }
        while (diag[i][j + d] != ' ') {
            j += d;
            if (diag[i][j] >= 'A' && diag[i][j] <= 'Z') {
                part1[part1len++] = diag[i][j];
            }
            ++part2;
        }
    }

    printf("Part 1: %s\n", part1);
    printf("Part 2: %i\n", part2);
    return 0;
}
