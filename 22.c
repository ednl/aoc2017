#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define INITSIZE (25U)

enum state {
    CLEAN,
    WEAKENED,
    INFECTED,
    FLAGGED,
};

static bool addrow_below(enum state **g, unsigned int *r, const unsigned int c)
{
    enum state *t = realloc(*g, (*r + 1) * c * sizeof **g);
    if (t == NULL) {
        return false;
    }
    memset(t + *r * c, CLEAN, c * sizeof *t);  // clear new last row (use old row count)
    *g = t;
    (*r)++;
    return true;
}

static bool addrow_above(enum state **g, unsigned int *r, const unsigned int c)
{
    enum state *t = realloc(*g, (*r + 1) * c * sizeof **g);
    if (t == NULL) {
        return false;
    }
    memmove(t + c, t, *r * c * sizeof *t);  // move grid down (use old row count)
    memset(t, CLEAN, c * sizeof *t);  // clear new first row
    *g = t;
    (*r)++;
    return true;
}

static bool addcol(enum state **g, const unsigned int r, unsigned int *c, const unsigned int shift)
{
    unsigned c1 = *c + 1;
    size_t newsize = r * c1 * sizeof **g;
    enum state *t = malloc(newsize);
    if (t == NULL) {
        return false;
    }
    memset(t, CLEAN, newsize);
    size_t oldrowsize = *c * sizeof **g;
    for (unsigned int i = 0; i < r; ++i) {
        memcpy(t + i * c1 + shift, *g + i * *c, oldrowsize);
    }
    free(*g);
    *g = t;
    *c = c1;
    return true;
}

static bool addcol_right(enum state **g, const unsigned int r, unsigned int *c)
{
    return addcol(g, r, c, 0);
}

static bool addcol_left(enum state **g, const unsigned int r, unsigned int *c)
{
    return addcol(g, r, c, 1);
}

static void print(const enum state *g, const unsigned int r, const unsigned int c)
{
    char ch;
    for (unsigned int i = 0; i < r; ++i) {
        for (unsigned int j = 0; j < c; ++j) {
            switch (g[i * c + j]) {
                case CLEAN   : ch = '.'; break;
                case WEAKENED: ch = 'o'; break;
                case INFECTED: ch = '#'; break;
                case FLAGGED : ch = '!'; break;
            }
            printf("%c", ch);
        }
        printf("\n");
    }
}

int main(void)
{
    // Make square array of arrays of bool
    unsigned int rows = INITSIZE, cols = INITSIZE;
    enum state *grid = malloc(rows * cols * sizeof *grid);
    if (grid == NULL) {
        fprintf(stderr, "Out of memory");
        exit(1);
    }
    // Read file into array
    FILE *f = fopen("22.txt", "r");
    if (f == NULL) {
        fprintf(stderr, "File not found");
        exit(2);
    }
    char *buf = NULL;
    size_t bufsize = 0;
    ssize_t len;
    unsigned int i = 0, j;
    while ((len = getline(&buf, &bufsize, f)) > 0 && i < rows) {
        char *c = buf;
        j = 0;
        while (*c && j < cols) {
            grid[i * cols + j++] = *c++ == '#' ? INFECTED : CLEAN;
        }
        ++i;
    }
    free(buf);
    fclose(f);

    unsigned int r = rows / 2, c = cols / 2;
    unsigned int infected = 0;
    char face = 0, mask = 3; // 0=up, 1=right, 2=down, 3=left
    for (int burst = 0; burst < 10000; ++burst) {
        if (r == 0) {
            addrow_above(&grid, &rows, cols);
            ++r;
        } else if (r == rows - 1) {
            addrow_below(&grid, &rows, cols);
        }
        if (c == 0) {
            addcol_left(&grid, rows, &cols);
            ++c;
        } else if (c == cols - 1) {
            addcol_right(&grid, rows, &cols);
        }
        unsigned int index = r * cols + c;
        if (grid[index] == INFECTED) {
            face = (face + 1) & mask;  // turn right
            grid[index] = CLEAN;
        } else if (grid[index] == CLEAN) {
            face = (face + 3) & mask;  // turn left
            grid[index] = INFECTED;
            ++infected;
        }
        switch(face) {
            case 0: --r; break;
            case 1: ++c; break;
            case 2: ++r; break;
            case 3: --c; break;
        }
    }
    print(grid, rows, cols);
    printf("Part 1: %u\n", infected); // right answer = 5261

    free(grid);
    return 0;
}
