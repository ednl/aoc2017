#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define MEMSIZE (41)
#define REGBASE ('a')
#define REGSTOP ('z')
#define REGSIZE (REGSTOP - REGBASE + 1)

typedef enum runstate {
    READY,
    RUN,
    WAIT,
} RunState;

typedef enum opcode {
    NOP = -1,
    SET =  0,
    ADD,
    MUL,
    MOD,
    JGZ,
    SND,
    RCV,
} OpCode;

typedef struct dict {
    OpCode id;
    int    param;
    char  *name;
} Dict, *pDict;

static const Dict cmd[] = {
    { .id = SET, .name = "set", .param = 2 },
    { .id = ADD, .name = "add", .param = 2 },
    { .id = MUL, .name = "mul", .param = 2 },
    { .id = MOD, .name = "mod", .param = 2 },
    { .id = JGZ, .name = "jgz", .param = 2 },
    { .id = SND, .name = "snd", .param = 1 },
    { .id = RCV, .name = "rcv", .param = 1 },
};
static const int cmdsize = sizeof cmd / sizeof *cmd;

typedef struct instr {
    pDict   op;
    int     r0, r1;
    int64_t v0, v1;
} Instr, *pInstr;

typedef struct list List, *pList;
struct list {
    int64_t val;
    pList   next;
};

typedef struct prog {
    int      pid, ip, tick, sent;
    RunState state;  // 0=ready, 1=running, 2=waiting
    int64_t  reg[REGSIZE];
    Instr    mem[MEMSIZE];
    pList    qhead, qtail;
} Prog, *pProg;

static Instr assemble(char *code)
{
    Instr ins = (Instr){NULL, -1, -1, -1, -1};
    int i, field = 0;
    char *s = strtok(code, " ");
    while (s) {
        if (field == 0) {
            for (i = 0; i < cmdsize; ++i) {
                if (strcmp(cmd[i].name, s) == 0) {
                    ins.op = &cmd[i];
                    break;
                }
            }
        } else if (field == 1) {
            if (*s >= 'a') {
                ins.r0 = *s - REGBASE;
            } else {
                ins.v0 = atoll(s);
            }
        } else if (field == 2) {
            if (*s >= 'a') {
                ins.r1 = *s - REGBASE;
            } else {
                ins.v1 = atoll(s);
            }
        }
        ++field;
        s = strtok(NULL, " ");
    }
    return ins;
}

static void load(pProg p, int id)
{
    p->pid   = id;
    p->ip    = 0;
    p->tick  = 0;
    p->sent  = 0;
    p->state = READY;
    p->qhead = NULL;
    p->qtail = NULL;

    for (int i = 0; i < REGSIZE; ++i) {
        p->reg[i] = 0;
    }
    p->reg['p' - REGBASE] = id;

    FILE *f = fopen("18.txt", "r");
    if (f == NULL) {
        fprintf(stderr, "File not found");
        exit(1);
    }
    char *buf = NULL;
    size_t buflen = 0, line = 0;
    ssize_t len;
    while ((len = getline(&buf, &buflen, f) > 0) && line < MEMSIZE) {
        p->mem[line++] = assemble(buf);
    }
    fclose(f);
    free(buf);
}

static void print(pProg p, int listing)
{
    int i;
    int64_t k;
    pList q;

    printf("Program ID : %i\n", p->pid);
    printf("Instr Pntr : %i\n", p->ip);
    printf("Clock Tick : %i\n", p->tick);
    printf("Send Count : %i\n", p->sent);
    printf("Run State  : %i (0=ready 1=running 2=waiting)\n", p->state);
    for (i = 0; i < REGSIZE; ++i) {
        if ((k = p->reg[i]) != 0) {
            printf("Register %c : %lli\n", REGBASE + i, k);
        }
    }
    printf("Message Q  :");
    i = 0;
    q = p->qhead;
    while (q) {
        printf(" %lli", q->val);
        q = q->next;
        ++i;
    }
    printf(" (len=%i)\n", i);
    if (listing) {
        for (i = 0; i < MEMSIZE; ++i) {
            if (p->mem[i].op != NULL && p->mem[i].op->id != NOP) {
                printf("  %2i: %s", i, p->mem[i].op->name);
                if (p->mem[i].op->param > 0) {
                    if (p->mem[i].r0 != -1) {
                        printf(" %c", REGBASE + p->mem[i].r0);
                    } else {
                        printf(" %lli", p->mem[i].v0);
                    }
                }
                if (p->mem[i].op->param > 1) {
                    if (p->mem[i].r1 != -1) {
                        printf(" %c", REGBASE + p->mem[i].r1);
                    } else {
                        printf(" %lli", p->mem[i].v1);
                    }
                }
                printf("\n");
            }
        }
    }
    printf("\n");
}

static int run(pProg p, pProg q, int part)
{
    pList pl;
    int clock = p->tick;
    if (p->state == READY) {
        p->state = RUN;
    }
    while (p->ip >= 0 && p->ip < MEMSIZE && (p->state != WAIT || (q != NULL && q->qhead != NULL))) {
        pInstr i = &(p->mem[p->ip]);
        if (i->op->param > 0 && i->r0 >= 0) {
            i->v0 = p->reg[i->r0];
        }
        if (i->op->param > 1 && i->r1 >= 0) {
            i->v1 = p->reg[i->r1];
        }
        switch (i->op->id) {
            case NOP:
                p->ip++;
                p->tick++;
                break;
            case SET:
                p->reg[i->r0] = i->v1;
                p->ip++;
                p->tick++;
                break;
            case ADD:
                p->reg[i->r0] += i->v1;
                p->ip++;
                p->tick++;
                break;
            case MUL:
                p->reg[i->r0] *= i->v1;
                p->ip++;
                p->tick++;
                break;
            case MOD:
                p->reg[i->r0] %= i->v1;
                p->ip++;
                p->tick++;
                break;
            case JGZ:
                if (i->v0 > 0) {
                    p->ip += i->v1;
                } else {
                    p->ip++;
                }
                p->tick++;
                break;
            case SND:
                pl = (pList)malloc(sizeof(List));
                if (pl) {
                    pl->val = i->v0;
                    pl->next = NULL;
                    if (p->qhead == NULL) {
                        p->qhead = pl;
                        p->qtail = pl;
                    } else {
                        p->qtail->next = pl;
                        p->qtail = pl;
                    }
                }
                p->ip++;
                p->tick++;
                p->sent++;
                break;
            case RCV:
                if (part == 1) {
                    if (i->v0 != 0) {
                        p->state = WAIT;
                    } else {
                        p->ip++;
                        p->tick++;
                    }
                } else if (part == 2) {
                    if (q != NULL && (pl = q->qhead) != NULL) {
                        p->reg[i->r0] = pl->val;
                        q->qhead = pl->next;
                        free(pl);
                        p->ip++;
                        p->tick++;
                    } else {
                        p->state = WAIT;
                    }
                }
                break;
        }
    }
    if (p->state == RUN) {
        p->state = READY;
    }
    return p->tick - clock;
}

int main(void)
{
    int t0 = 0, t1 = 0;
    Prog p, q;

    // Part 1
    load(&p, 0);
    t0 = run(&p, NULL, 1);
    print(&p, 0);  // debug
    printf("Part 1: %lli\n\n", p.qtail ? p.qtail->val : -1);

    // Part 2
    load(&q, 1);
    while (t0 || t1) {
        t1 = run(&q, &p, 2);
        t0 = run(&p, &q, 2);
    }
    print(&p, 0);  // debug
    print(&q, 0);  // debug
    printf("Part 2: %i\n\n", q.sent);

    // Clean up
    while (p.qhead) {
        pList t = p.qhead->next;
        free(p.qhead);
        p.qhead = t;
    }
    while (q.qhead) {
        pList t = q.qhead->next;
        free(q.qhead);
        q.qhead = t;
    }
    return 0;
}
