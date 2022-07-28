import random
import matplotlib.pyplot as pyplot

file = open('current_test.txt', 'w')
answ_file = open('current_test_answer.txt', 'w')

random.seed()
n = random.randint(5, 15)
file.write(str(n) + '\n')
v = []
v.append([0, 0, 0, 100])
v.append([0, 100, 100, 100])
v.append([100, 100, 100, 0])
v.append([100, 0, 0, 0])
pyplot.plot([0, 0], [0, 100])
pyplot.plot([0, 100], [100, 100])
pyplot.plot([100, 100], [100, 0])
pyplot.plot([100, 0], [0, 0])
for i in range(4, n + 4):
    k1 = random.randint(0, 1)
    k2 = random.randint(0, 1)
    if k1:
        if k2:
            x1 = 0
        else:
            x1 = 100
        y1 = random.randint(0, 100)
    else:
        if k2:
            y1 = 0
        else:
            y1 = 100
        x1 = random.randint(0, 100)
    k1 = abs(k1 - 1)
    k2 = abs(k1 - 1)
    if k1:
        if k2:
            x2 = 0
        else:
            x2 = 100
        y2 = y1
        while y2 == y1:
            y2 = random.randint(0, 100)
    else:
        if k2:
            y2 = 0
        else:
            y2 = 100
        x2 = x1
        while x2 == x1:
            x2 = random.randint(0, 100)
    v.append([x1, y1, x2, y2])
    file.write(" ".join(str(v[i][c]) for c in range(4)) + '\n')
    pyplot.plot([x1, x2], [y1, y2])

pyplot.show()
file.close()