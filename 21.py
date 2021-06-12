import numpy as np

image = '.#./..#/###'

def str2arr(s):
    return (np.array([list(row) for row in s.strip().split('/')]) == '#').astype(np.uint8)

def arr2val(a):
    return int(''.join(map(str, a.flatten())), 2)

def str2val(s):
    return int(s.strip().replace('/', '').replace('.', '0').replace('#', '1'), 2)

def val2arr(dim, i):
    s = bin(i)[2:]
    t = '0' * (dim * dim - len(s)) + s
    return np.array(list(map(int, list(t)))).reshape((dim, dim))

def perm(a):
    p = set()
    b = np.rot90(a, 1)
    p.add(arr2val(a))
    p.add(arr2val(b))
    p.add(arr2val(np.rot90(a, 2)))
    p.add(arr2val(np.rot90(a, 3)))
    p.add(arr2val(np.fliplr(a)))
    p.add(arr2val(np.flipud(a)))
    p.add(arr2val(np.fliplr(b)))
    p.add(arr2val(np.flipud(b)))
    return list(p)

def divide(a):
    p = []
    d = a.shape[0]
    if d % 2 == 0:
        for i in range(0, d, 2):
            for j in range(0, d, 2):
                p.append(arr2val(a[i:i+2,j:j+2]))
        return (2, p)
    elif d % 3 == 0:
        for i in range(0, d, 3):
            for j in range(0, d, 3):
                p.append(arr2val(a[i:i+3,j:j+3]))
        return (3, p)
    return (0, p)

rule = {}
with open('21.txt') as f:
    for line in f:
        a, b = line.split(' => ')
        p = perm(str2arr(a))  # pattern
        r = str2val(b)        # replacement
        for i in p:
            rule[(2 if len(a) == 5 else 3, i)] = r

print(str2val(image))
print(rule)

# org: ab
#      cd
# rot1: ca  rot2: dc  rot3: bd
#       db        ba        ac
# flp1: ba  flp2: cd
#       dc        ab
# r+f1: ac  r+f2: db
#       bd        ca
#
# abcd, acbd, dcba, dbca
# badc, bdac, cdab, cadb

# org: abc
#      def
#      ghi
# rot1: gda  rot2: ihg  rot3: cfi
#       heb        fed        beh
#       ifc        cba        adg
# flp1: cba  flp2: ghi
#       fed        def
#       ihg        abc
# r+f1: adg  r+f2: ifc
#       beh        heb
#       cfi        gda
#
# abcdefghi, adgbehcfi, ihgfedcba, ifchebgda
# cbafedihg, cfibehadg, ghidefabc, gdahebifc
