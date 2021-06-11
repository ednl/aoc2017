import numpy as np

pat = []
rep = []
with open('21.txt') as f:
    for line in f:
        a, b = line.split(' => ')
        aa = [list(row) for row in a.split('/')]
        bb = [list(row) for row in b.strip().split('/')]
        pat.append((np.array(aa) == '#').astype(np.uint8))
        rep.append((np.array(bb) == '#').astype(np.uint8))
print(pat[3])
print(rep[3])

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
