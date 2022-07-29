import random

file = open('current_test.txt', 'w')
answ_file = open('current_test_answer.txt', 'w')

random.seed()
map_x = random.randint(5, 50)
map_y = random.randint(5, 50)
kernel_x = random.randint(2, 6)
kernel_y = random.randint(2, 6)
map = [['0' for d in range(map_x)] for c in range(map_y)]
res_map = [['0' for d in range(map_x)] for c in range(map_y)]
kernel = [['0' for d in range(kernel_x)] for c in range(kernel_y)]
for y in range(map_y):
    for x in range(map_x):
        rand = random.randint(0, 1)
        if rand == 0:
            map[y][x] = '0'
            res_map[y][x] = '0'
        else:
            map[y][x] = '1'
            res_map[y][x] = '1'

for ky in range(kernel_y):
    for kx in range(kernel_x):
        rand = random.randint(0, 1)
        if rand == 0:
            kernel[ky][kx] = '0'
        else:
            kernel[ky][kx] = '1'

for y in range(map_y - kernel_y + 1):
    for x in range(map_x - kernel_x + 1):
        recognized = 1
        for ky in range(kernel_y):
            if recognized == 1:
                for kx in range(kernel_x):
                    if kernel[ky][kx] != res_map[y + ky][x + kx]:
                        recognized = 0
                        break
            else:
                break
        if recognized == 1:
            for ky2 in range(kernel_y):
                for kx2 in range(kernel_x):
                    if res_map[y + ky2][x + kx2] == '0':
                        res_map[y + ky2][x + kx2] = '*'
                    else:
                        res_map[y + ky2][x + kx2] = '2'

file.write(" ".join([str(kernel_x), str(kernel_y)]) + '\n')
for ky in range(kernel_y):
    for kx in range(kernel_x):
        file.write(kernel[ky][kx] + ' ')
    file.write('\n')

file.write(" ".join([str(map_x), str(map_y)]) + '\n')
for y in range(map_y):
    for x in range(map_x):
        file.write(map[y][x] + ' ')
    file.write('\n')

for y in range(map_y):
    for x in range(map_x):
        answ_file.write(res_map[y][x] + ' ')
    answ_file.write('\n')

answ_file.close()
file.close()