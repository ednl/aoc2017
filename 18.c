#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define MEMSIZE (41)
#define REGBASE ('a')
#define REGSIZE ('z' - REGBASE + 1)

typedef enum {
    NOP = -1,
    SET =  0,
    ADD,
    MUL,
    MOD,
    JGZ,
    SND,
    RCV,
} OpCode;

typedef struct {
    OpCode id;
    char  *name;
} Dict, *pDict;

static const Dict cmd[] = {
    { .id = SET, .name = "set" },
    { .id = ADD, .name = "add" },
    { .id = MUL, .name = "mul" },
    { .id = MOD, .name = "mod" },
    { .id = JGZ, .name = "jgz" },
    { .id = SND, .name = "snd" },
    { .id = RCV, .name = "rcv" },
};
static const int cmdsize = sizeof cmd / sizeof *cmd;

typedef struct {
    OpCode  op;
    int     mode;  // 0=rr 1=rv 2=vr 3=vv 4=r 5=v
    int     r0, r1;
    int64_t v0, v1;
} Instr, *pInstr;

typedef struct list List, *pList;
struct list {
    int64_t val;
    pList   next;
};

typedef struct {
    int     pid, ip;
    int     state;  // 0=paused 1=running 2=waiting
    Instr   mem[MEMSIZE];
    int64_t reg[REGSIZE];
    pList   firstmsg, lastmsg;
} Prog;

static Instr assemble(char *code)
{
    Instr ins = (Instr){NOP, -1, -1, -1, -1};
    int i, field = 0;
    char *s = strtok(code, " ");
    while (s) {
        if (field == 0) {
            for (i = 0; i < cmdsize; ++i) {
                if (strcmp(cmd[i].name, s) == 0) {
                    ins.op = cmd[i].id;
                    break;
                }
            }
        } else if (field == 1) {
            if (*s >= 'a') {
                ins.r0 = *s - 'a';
                ins.val0 = -1;
            } else {
                ins.r0 = -1;
                ins.val0 = atoll(s);
            }
        }
        s = strtok(NULL, " ");
        ++field;
    }
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
    return ins;
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
