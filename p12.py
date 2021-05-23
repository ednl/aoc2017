import numpy as np
pipe = np.zeros((2000, 2000), dtype=np.uint32)
with open("input12.txt") as f:
    for line in f:
        a = line.strip().split(' <-> ')
        i = int(a[0])
        b = list(map(int, a[1].split(', ')))
        for j in b:
            pipe[i, j] = pipe[j, i] = 1

def connected(i):
    return np.arange(2000)[pipe[i, :] != 0]
