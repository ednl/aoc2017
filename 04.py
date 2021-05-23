valid1 = valid2 = 0
with open("04.txt") as f:
    for line in f:
        a = line.rstrip("\n").split(" ")
        valid1 += len(set(a)) == len(a)
        b = ["".join(sorted(word)) for word in a]
        valid2 += len(set(b)) == len(b)
print(valid1, valid2)
