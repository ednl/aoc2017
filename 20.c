#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <limits.h>

#define DIM  (3)
#define N (1000)

typedef struct particle {
    int acc[DIM], vel[DIM], pos[DIM];
    int n;
    bool alive;
} Particle;

static Particle cloud[N] = {0};

static inline void addv(int *r, const int *a, const int *b)
{
    for (size_t i = 0; i < DIM; ++i) {
        r[i] = a[i] + b[i];
    }
}

static inline void subv(int *r, const int *a, const int *b)
{
    for (size_t i = 0; i < DIM; ++i) {
        r[i] = a[i] - b[i];
    }
}

static inline void muls(int *r, const int *a, const int n)
{
    for (size_t i = 0; i < DIM; ++i) {
        r[i] = a[i] * n;
    }
}

static inline void mulv(int *r, const int *a, const int *b)
{
    for (size_t i = 0; i < DIM; ++i) {
        r[i] = a[i] * b[i];
    }
}

// Divide vector by scalar
static inline void divs(int *r, const int *a, const int n)
{
    for (size_t i = 0; i < DIM; ++i) {
        r[i] = a[i] / n;
    }
}

// Divide vector by vector elementwise
static inline void divv(int *r, const int *a, const int *b)
{
    for (size_t i = 0; i < DIM; ++i) {
        r[i] = a[i] / b[i];
    }
}

// Modulus of vector by scalar
static inline void mods(int *r, const int *a, const int n)
{
    for (size_t i = 0; i < DIM; ++i) {
        r[i] = a[i] % n;
    }
}

// Modulus of vector by vector elementwise
static inline void modv(int *r, const int *a, const int *b)
{
    for (size_t i = 0; i < DIM; ++i) {
        r[i] = a[i] % b[i];
    }
}

// Manhattan distance
static inline int manh(const int *a)
{
    return abs(a[0]) + abs(a[1]) + abs(a[2]);
}

// Vector is zero?
static inline bool iszero(const int *a)
{
    return a[0] == 0 && a[1] == 0 && a[2] == 0;
}

// Vector is not negative?
static inline bool notneg(const int *a)
{
    return a[0] >= 0 && a[1] >= 0 && a[2] >= 0;
}

// All vector elements equal?
static inline bool isone(const int *a)
{
    return a[0] == a[1] && a[0] == a[2];
}

static int isqrt(int s)
{
    int x0 = s >> 1;
    if (x0) {
        int x1 = ((x0 + s / x0) >> 1);
        while (x1 < x0) {
            x0 = x1;
            x1 = ((x0 + s / x0) >> 1);
        }
        return x0;
    } else {
        return s;
    }
}

// p(t) = p + v.t + a.t.(t+1)/2
//      = p + (v + a/2).t + a.t.t/2
static int cmp(const void *a, const void *b)
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

static int load(void)
{
    FILE *f = fopen("20.txt", "r");
    if (f == NULL) {
        fprintf(stderr, "File not found");
        exit(1);
    }
    int n = 0;
    Particle p = {0};
    while (fscanf(f,
        "p=<%d,%d,%d>, "
        "v=<%d,%d,%d>, "
        "a=<%d,%d,%d> ",
        &p.pos[0], &p.pos[1], &p.pos[2],
        &p.vel[0], &p.vel[1], &p.vel[2],
        &p.acc[0], &p.acc[1], &p.acc[2]) == 9 && n < N) {
        p.n = n;
        p.alive = true;
        cloud[n++] = p;
    }
    fclose(f);
    return n;
}


// Returns time of collision (<0 = no collision)
//   p1 = p2
//   <=> p1 + (v1 + a1/2).t + a1.t.t/2 = p2 + (v2 + a2/2).t + a2.t.t/2
//   <=> (a1 - a2)/2.t.t + (v1 - v2 + (a1 - a2)/2).t + (p1 - p2) = 0
static int collide(const Particle *p, const Particle *q)
{
    int da[DIM], dv[DIM], dp[DIM], t0[DIM] = {-1, -1, -1}, t1[DIM] = {-1, -1, -1};

    subv(da, p->acc, q->acc);           // a1 - a2
    subv(dv, p->vel, q->vel);           // v1 - v2
    subv(dp, p->pos, q->pos);           // p1 - p2

    // Look at x, y, z separately
    for (int i = 0; i < DIM; ++i) {
        if (da[i] == 0) {               // (a1 - a2) == 0?
            // Linear equation
            //  => (v1 - v2).t + (p1 - p2) = 0
            // <=> t = -(p1 - p2)/(v1 - v2)
            if (dv[i] == 0) {           // same speed?
                if (dp[i] == 0) {       // same place?
                    t0[i] = t1[i] = 0;  // continuous collision from t=0
                }
            } else if (dp[i] % dv[i] == 0) {
                t0[i] = t1[i] = -dp[i] / dv[i];  // collision at t = -(p1 - p2)/(v1 - v2)
            }
        } else {
            // Quadratic equation
            //  (a1 - a2)/2.t.t + (v1 - v2 + (a1 - a2)/2).t + (p1 - p2) = 0
            //  => D = (v1 - v2 + (a1 - a2)/2)^2 - 2.(a1 - a2).(p1 - p2)

            // (a1 - a2) % 2 ? => no collision
            // D < 0 ? => no collision
            // D == 0 ? => t = (v2 - v1)/(a1 - a2) - 1/2
            // D not square ? => no collision

            if (da[i] % 2 == 0) {
                int b = dv[i] + da[i] / 2;
                int D = b * b - 2 * da[i] * dp[i];
                if (D == 0) {
                    if (b % da[i] == 0) {
                        t0[i] = t1[i] = -b / da[i];
                    }
                } else if (D > 0) {
                    int root = isqrt(D);
                    if (root * root == D) {
                        int n = -b - root;
                        int m = -b + root;
                        n = n % da[i] ? -1 : n / da[i];
                        m = m % da[i] ? -1 : m / da[i];
                        if (n < m) {
                            t0[i] = n;
                            t1[i] = m;
                        } else {
                            t0[i] = m;
                            t1[i] = n;
                        }
                    }
                }
            }
        }
    }
    // TODO: check t0 and t1, return collision time or not
    return -1;
}

int main(void)
{
    int n = load();

    // Part 1
    qsort(cloud, (size_t)n, sizeof(Particle), cmp);
    // Show first few sorted elements
    for (int i = 0; i < 5; ++i) {
        printf("%d : %3d : a(%2d,%2d,%2d) v(%3d,%3d,%3d) p(%5d,%5d,%5d)\n",
            i, cloud[i].n,
            cloud[i].acc[0], cloud[i].acc[1], cloud[i].acc[2],
            cloud[i].vel[0], cloud[i].vel[1], cloud[i].vel[2],
            cloud[i].pos[0], cloud[i].pos[1], cloud[i].pos[2]
        );
    }
    // Result
    printf("Part 1: %d\n", cloud[0].n);

    int i, j, t;
    for (i = 0; i < N - 1; ++i) {
        for (j = i + 1; j < N; ++j) {
            if ((t = collide(&cloud[i], &cloud[j])) >= 0) {
                printf("%5d : %3d %3d\n", t, cloud[i].n, cloud[j].n);
            }
        }
    }
    return 0;
}
