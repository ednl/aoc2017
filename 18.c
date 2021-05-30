#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define MEMSIZE (41)

typedef enum {
    INVALID = -1,
    SET,
    ADD,
    MUL,
    MOD,
    JGZ,
    SND,
    RCV,
} OpCode;

typedef struct {
    OpCode id;
    int    pc;
    char  *name;
} OpDict, *pOpDict;

static const OpDict cmd[] = {
    { .id = SET, .pc = 2, .name = "set" },
    { .id = ADD, .pc = 2, .name = "add" },
    { .id = MUL, .pc = 2, .name = "mul" },
    { .id = MOD, .pc = 2, .name = "mod" },
    { .id = JGZ, .pc = 2, .name = "jgz" },
    { .id = SND, .pc = 1, .name = "snd" },
    { .id = RCV, .pc = 1, .name = "rcv" },
};
static const int cmdsize = sizeof cmd / sizeof *cmd;

typedef struct {
    OpCode op;
    int r0, r1;
    int64_t val0, val1;
} Ins, *pIns;

static Ins mem[MEMSIZE] = {0};
static int64_t reg['z' - 'a' + 1] = {0}, sound = 0;

static Ins assemble(char *code)
{
    for (int i = 0; i < cmdsize; ++i) {
        int j = 0;
        while (cmd[i].name[j] == code[j]) {
            ++j;
        }
        if (j == 3) {
            int r0 = -1, r1 = -1;
            int64_t val0 = -1, val1 = -1;
            if (cmd[i].pc > 0) {
                if (code[4] >= 'a' && code[4] <= 'z') {
                    r0 = code[4] - 'a';
                } else {
                    sscanf(&code[4], "%lli", &val0);
                }
            }
            if (cmd[i].pc > 1) {
                if (code[6] >= 'a' && code[6] <= 'z') {
                    r1 = code[6] - 'a';
                } else {
                    sscanf(&code[6], "%lli", &val1);
                }
            }
            return (Ins){cmd[i].id, r0, r1, val0, val1};
        }
    }
    return (Ins){INVALID, -1, -1, -1, -1};
}

int main(void)
{
    FILE *fp = fopen("18.txt", "r");
    if (fp == NULL) {
        fprintf(stderr, "File not found");
        return 1;
    }

    char *buf = NULL;
    size_t buflen = 0, line = 0;
    ssize_t len;
    while ((len = getline(&buf, &buflen, fp) > 0) && line < MEMSIZE) {
        mem[line++] = assemble(buf);
    }
    fclose(fp);
    free(buf);

    for (size_t i = 0; i < line; ++i) {
        printf("%2zu: %s", i, cmd[mem[i].op].name);
        if (cmd[mem[i].op].pc > 0) {
            if (mem[i].r0 >= 0) {
                printf(" %c", 'a' + mem[i].r0);
            } else {
                printf(" %lli", mem[i].val0);
            }
        }
        if (cmd[mem[i].op].pc > 1) {
            if (mem[i].r1 >= 0) {
                printf(" %c", 'a' + mem[i].r1);
            } else {
                printf(" %lli", mem[i].val1);
            }
        }
        printf("\n");
    }

    int ip = 0, tick = 0;
    while (ip >= 0 && ip < MEMSIZE) {
        ++tick;
        if (mem[ip].r0 >= 0) {
            mem[ip].val0 = reg[mem[ip].r0];
        }
        if (mem[ip].r1 >= 0) {
            mem[ip].val1 = reg[mem[ip].r1];
        }
        switch (mem[ip].op) {
            case INVALID:
                break;
            case SET:
                reg[mem[ip].r0] = mem[ip].val1;
                break;
            case ADD:
                reg[mem[ip].r0] += mem[ip].val1;
                break;
            case MUL:
                reg[mem[ip].r0] *= mem[ip].val1;
                break;
            case MOD:
                reg[mem[ip].r0] %= mem[ip].val1;
                break;
            case JGZ:
                if (mem[ip].val0 > 0) {
                    ip += mem[ip].val1 - 1;
                }
                break;
            case SND:
                sound = mem[ip].val0;
                break;
            case RCV:
                if (mem[ip].val0 != 0) {
                    printf("freq: %lli\n", sound);
                    ip = MEMSIZE;
                }
                break;
        }
        ++ip;
    }
    printf("tick: %i\n", tick);

    return 0;
}
