a = list(open("09.txt").read())
i = 0
group = 0
score = 0
garbage = False
count = 0
while i < len(a):
    if garbage:
        if a[i] == '!':
            i += 1
        elif a[i] == '>':
            garbage = False
        else:
            count += 1
    else:
        if a[i] == '{':
            group += 1
        elif a[i] == '}':
            score += group
            group -= 1
        elif a[i] == '<':
            garbage = True
    i += 1
print(score, count)
