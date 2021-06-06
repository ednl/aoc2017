#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define INITSIZE  (25U)
#define CLEAN     (0)
#define WEAKENED  (1)
#define INFECTED  (2)
#define FLAGGED   (3)

static bool addrow_below(char **g, unsigned int *r, const unsigned int c)
{
    char *t = realloc(*g, (*r + 1) * c);
    if (t == NULL) {
        return false;
    }
    memset(t + *r * c, CLEAN, c);  // clear new last row (use old row count)
    *g = t;
    (*r)++;
    return true;
}

static bool addrow_above(char **g, unsigned int *r, const unsigned int c)
{
    char *t = realloc(*g, (*r + 1) * c);
    if (t == NULL) {
        return false;
    }
    memmove(t + c, t, *r * c);  // move grid down (use old row count)
    memset(t, CLEAN, c);        // clear new first row
    *g = t;
    (*r)++;
    return true;
}

static bool addcol(char **g, const unsigned int r, unsigned int *c, const unsigned int shift)
{
    const unsigned c1 = *c + 1;
    char *t = malloc(r * c1);
    if (t == NULL) {
        return false;
    }
    memset(t, CLEAN, r * c1);
    for (unsigned int i = 0; i < r; ++i) {
        memcpy(t + i * c1 + shift, *g + i * *c, *c);
    }
    free(*g);
    *g = t;
    *c = c1;
    return true;
}

static bool addcol_right(char **g, const unsigned int r, unsigned int *c)
{
    return addcol(g, r, c, 0);
}

static bool addcol_left(char **g, const unsigned int r, unsigned int *c)
{
    return addcol(g, r, c, 1);
}

static void print(const char *g, const unsigned int r, const unsigned int c)
{
    char ch = '?';
    for (unsigned int i = 0; i < r; ++i) {
        for (unsigned int j = 0; j < c; ++j) {
            switch (g[i * c + j]) {
                case CLEAN   : ch = '.'; break;
                case WEAKENED: ch = 'W'; break;
                case INFECTED: ch = '#'; break;
                case FLAGGED : ch = 'F'; break;
                default:       ch = '?'; break;
            }
            printf("%c", ch);
        }
        printf("\n");
    }
}

static void init(char **g, unsigned int *r, unsigned int *c)
{
    if (*g != NULL) {
        free(*g);
    }
    *g = calloc(INITSIZE * INITSIZE, sizeof(char));
    if (*g == NULL) {
        fprintf(stderr, "Out of memory");
        exit(2);
    }
    *r = *c = INITSIZE;

    FILE *f = fopen("22.txt", "r");
    if (f == NULL) {
        fprintf(stderr, "File not found");
        exit(1);
    }
    char *buf = NULL;
    size_t bufsize = 0;
    ssize_t len;
    unsigned int i = 0, j;
    while ((len = getline(&buf, &bufsize, f)) > 0 && i < INITSIZE) {
        char *pc = buf;
        j = 0;
        while (*pc && j < INITSIZE) {
            (*g)[i * INITSIZE + j++] = *pc++ == '#' ? INFECTED : CLEAN;
        }
        ++i;
    }
    free(buf);
    fclose(f);
}

static unsigned int part1(char **g, unsigned int *r, unsigned int *c, const unsigned int part)
{
    unsigned int infected = 0;
    unsigned int i = *r / 2, j = *c / 2;
    const char mask = 3;  // for grid and face
    char face = 0; // face 0=up, 1=right, 2=down, 3=left
    char step = part == 1 ? 2 : 1;
    int bursts = 10 * 1000;
    if (part == 2) {
        bursts *= 1000;
    }

    for (int burst = 0; burst < bursts; ++burst) {
        if (i == 0) {
            addrow_above(g, r, *c);
            i = 1;
        } else if (i == *r - 1) {
            addrow_below(g, r, *c);
        }
        if (j == 0) {
            addcol_left(g, *r, c);
            j = 1;
        } else if (j == *c - 1) {
            addcol_right(g, *r, c);
        }
        unsigned int index = i * *c + j;
        switch ((*g)[index]) {
            case CLEAN:
                face = (face + 3) & mask;  // turn left
                (*g)[index] = part == 1 ? INFECTED : WEAKENED;
                ++infected;
                break;
            case WEAKENED:
                (*g)[index] = INFECTED;
                break;
            case INFECTED:
                face = (face + 1) & mask;  // turn right
                (*g)[index] = CLEAN;
                break;
            case FLAGGED:
                (*g)[index] = CLEAN;
                break;
        }
        switch(face) {
            case 0: --i; break;
            case 1: ++j; break;
            case 2: ++i; break;
            case 3: --j; break;
        }
    }
    return infected;
}

int main(void)
{
    char *grid = NULL;
    unsigned int rows, cols;
    init(&grid, &rows, &cols);
    printf("Part 1: %u\n", part1(&grid, &rows, &cols)); // right answer = 5261
    print(grid, rows, cols);

    free(grid);
    return 0;
}
