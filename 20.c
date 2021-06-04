#include <stdio.h>     // printf, scanf
#include <stdlib.h>    // abs, exit
#include <math.h>      // fabs, sqrt

#define DIM       (3)
#define N      (1000)
#define COLMAX (2000)

typedef struct particle {
    int acc[DIM], vel[DIM], pos[DIM];
    int col;
    int ord;
} Particle;

static Particle cloud[N] = {0};

typedef struct colpair {
    int t;
    int n1, n2;
} ColPair;

static ColPair cols[COLMAX] = {0};

// Add vector to vector
static inline void addv(int *r, const int *a, const int *b)
{
    for (size_t i = 0; i < DIM; ++i) {
        r[i] = a[i] + b[i];
    }
}

// Multiply vector by scalar
static inline void muls(int *r, const int *a, const int n)
{
    for (size_t i = 0; i < DIM; ++i) {
        r[i] = a[i] * n;
    }
}

// Manhattan distance
static inline int manh(const int *a)
{
    return abs(a[0]) + abs(a[1]) + abs(a[2]);
}

// Sort particles by acceleration, then speed + half acc., then position
// (all in terms of Manhattan distance because that is the criterium)
//   p(t) = p + v.t + a.t.(t+1)/2
//        = p + (v + a/2).t + a.t.t/2
static int cmp1(const void *a, const void *b)
{
    const Particle *p = (const Particle *)a, *q = (const Particle *)b;
    int n = manh(p->acc);
    int m = manh(q->acc);
    if (n == m) {
        int t[DIM];
        muls(t, p->vel, 2);
        addv(t, p->acc, t);
        n = manh(t);
        muls(t, q->vel, 2);
        addv(t, q->acc, t);
        m = manh(t);
        if (n == m) {
            n = manh(p->pos);
            m = manh(q->pos);
            if (n == m) {
                return 0;
            }
        }
    }
    return n < m ? -1 : 1;
}

// Sort particles by ordinal
static int cmp2(const void *a, const void *b)
{
    return ((const Particle *)a)->ord - ((const Particle *)b)->ord;
}

static int load(void)
{
    FILE *f = fopen("20.txt", "r");
    if (f == NULL) {
        fprintf(stderr, "File not found");
        exit(1);
    }
    int n = 0;
    Particle p = {0};
    while (n < N && 9 == fscanf(f,
        "p=<%d,%d,%d>, "
        "v=<%d,%d,%d>, "
        "a=<%d,%d,%d> ",
        &p.pos[0], &p.pos[1], &p.pos[2],
        &p.vel[0], &p.vel[1], &p.vel[2],
        &p.acc[0], &p.acc[1], &p.acc[2])) {
        p.col = -1;  // never
        p.ord = n;
        cloud[n++] = p;
    }
    fclose(f);
    return n;
}

static inline int nonnegint(double a)
{
    double b = round(a);
    int i = fabs(a - b) < 0.000001 ? (int)b : -1;
    return i >= 0 ? i : -1;
}

static inline int check1(int a, int b, int t)
{
    return t >= 0 && (t == a || t == b || a == -2) ? t : -1;
}

static void check2(int *a, int *b, int t1, int t2)
{
    if (t1 < 0 && t2 < 0) {
        *a = *b = -1;
    } else if (t1 < 0) {
        *a = *b = check1(*a, *b, t2);
    } else if (t2 < 0 || t1 == t2) {
        *a = *b = check1(*a, *b, t1);
    } else if (*a == -2) {
        if (t1 < t2) {
            *a = t1;
            *b = t2;
        } else {
            *a = t2;
            *b = t1;
        }
    } else if (*a == t1 && *b != t2) {
        *b = t1;
    } else if (*a == t2 && *b != t1) {
        *b = t2;
    } else if (*b == t1 && *a != t2) {
        *a = t1;
    } else if (*b == t2 && *a != t1) {
        *a = t2;
    } else if (!((*a == t1 && *b == t2) || (*a == t2 && *b == t1))) {
        *a = *b = -1;
    }
}

// Returns earliest time of collision (<0 = no collision)
// p1(t) = p2(t)
// <=> p1 + v1.t + a1.t.(t+1)/2 = p2 + v2.t + a2.t.(t+1)/2
// <=> p1 + (v1 + a1/2).t + a1.t.t/2 = p2 + (v2 + a2/2).t + a2.t.t/2
// <=> (a1 - a2)/2.t.t + (v1 - v2 + (a1 - a2)/2).t + (p1 - p2) = 0
// <=> (a1 - a2).t.t + (2.(v1 - v2) + (a1 - a2)).t + 2.(p1 - p2) = 0
// <=> t.t + (2.(v1 - v2)/(a1 - a2) + 1).t + 2.(p1 - p2)/(a1 - a2) = 0

// (a1 - a2) == 0
//    => (v1 - v2).t + (p1 - p2) = 0
//   (v1 - v2) == 0
//      => (p1 - p2) == 0 ? all t : no t
//   (v1 - v2) != 0
//      => t = -(p1 - p2)/(v1 - v2)

// (a1 - a2) != 0
//    => "a" = (a1 - a2)
//       "b" = 2.(v1 - v2) + (a1 - a2)
//       "c" = 2.(p1 - p2)
//       "D" = b.b - 4.a.c = (2.(v1 - v2) + (a1 - a2))^2 - 8.(a1 - a2).(p1 - p2)
//   D < 0
//      => no t
//   D == 0
//      => t = -b/(2.a) = -(2.(v1 - v2) + (a1 - a2))/(2.(a1 - a2))
//   D > 0
//      => t0 = (-b - sqrt(D))/(2.a)
//         t1 = (-b + sqrt(D))/(2.a)
static int collide(const Particle *p1, const Particle *p2)
{
    int t0 = -2, t1 = -2;  // -2=all -1=none

    for (int i = 0; i < DIM; ++i) {
        int da = p1->acc[i] - p2->acc[i];
        int dv = p2->vel[i] - p1->vel[i];  // -(v1 - v2) = (v2 - v1)
        int dp = p1->pos[i] - p2->pos[i];
        if (da == 0) {
            if (dv == 0) {
                if (dp != 0) {
                    return -1;
                }
            } else {
                if (dp % dv) {
                    return -1;
                }
                t0 = t1 = check1(t0, t1, dp / dv);
            }
        } else {
            // double a = 1;
            double b = (double)dv / da - 0.5;  // = -b/2a
            double c = (double)(dp * 2) / da;  // = c/a
            double D = b * b - c;              // = D/4
            if (fabs(D) < 0.000001) {          // discriminant == 0
                t0 = t1 = check1(t0, t1, nonnegint(b));
            } else if (D > 0) {
                double root = sqrt(D);         // = sqrt(D)/2
                check2(&t0, &t1, nonnegint(b - root), nonnegint(b + root));
            } else {
                return -1;
            }
        }
        if (t0 == -1) {
            return -1;
        }
    }
    return t0 == -2 ? 0 : t0;  // but t0 is never -2 for my puzzle input
}

static int cmpcol(const void *a, const void *b)
{
    const ColPair *p = (const ColPair *)a, *q = (const ColPair *)b;
    int i = p->t - q->t;
    if (i == 0) {
        int j = p->n1 - q->n1;
        if (j == 0) {
            return p->n2 - q->n2;
        }
        return j;
    }
    return i < 0 ? -1 : 1;
}

int main(void)
{
    int particlecount = load();

    // Part 1
    qsort(cloud, (size_t)particlecount, sizeof(Particle), cmp1);
    printf("Part 1: %d\n", cloud[0].ord);

    // Reset particle order => cloud[i].n == i
    qsort(cloud, (size_t)particlecount, sizeof(Particle), cmp2);

    // Part 2
    int colcount = 0;
    for (int i = 0; i < particlecount - 1; ++i) {
        for (int j = i + 1; j < particlecount; ++j) {
            int t = collide(&cloud[i], &cloud[j]);
            if (colcount == COLMAX) {
                printf("cols[] too small\n");
                exit(2);
            }
            if (t >= 0 && colcount < COLMAX) {
                cols[colcount++] = (ColPair){
                    .t = t,
                    .n1 = cloud[i].ord,
                    .n2 = cloud[j].ord,
                };
            }
        }
    }

    // Sort collisions by ascending time, first ord, second ord
    qsort(cols, (size_t)colcount, sizeof(ColPair), cmpcol);

    // Strike out particles which collided
    for (int i = 0; i < colcount; ++i) {
        Particle *p1 = &cloud[cols[i].n1];
        Particle *p2 = &cloud[cols[i].n2];
        int t = cols[i].t;
        if ((p1->col < 0 || p1->col == t) && (p2->col < 0 || p2->col == t)) {
            p1->col = p2->col = t;
        }
    }

    int alive = 0;
    for (int i = 0; i < particlecount; ++i) {
        alive += (cloud[i].col < 0);
    }

    // Right answer = 438 for my puzzle input
    printf("Part 2: %d\n", alive);

    return 0;
}
