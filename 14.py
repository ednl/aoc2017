import numpy as np
from functools import reduce

# From day 10
def knot(lenstr, rounds=64):
    hlen = 256
    hash = list(range(hlen))
    pos = skip = 0
    lengths = list(map(ord, lenstr)) + [17, 31, 73, 47, 23]
    for _ in range(rounds):
        for length in lengths:
            i = pos
            j = pos + length - 1
            while i < j:
                p = i % hlen
                q = j % hlen
                hash[p], hash[q] = hash[q], hash[p]
                i += 1
                j -= 1
            pos += length + skip
            skip += 1
    dense = ""
    for i in range(0, hlen, 16):
        dense += '{:08b}'.format(reduce(lambda x, y: x ^ y, hash[i:i + 16]))
    return np.array(list(dense)).astype(np.uint8)

disk = np.zeros((128, 128), dtype=np.uint8)
key = 'hxtvlmkl-'
for i in range(128):
    s = key + str(i)
    disk[i, :] = knot(s)
print(disk.sum())
