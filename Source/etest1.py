import random


with open("output.txt", "w") as f:
    for j in range(10):
        array = list(range(1, 1001))
        station = []
        output_array = []
        while len(array) != 0:
            that_time = random.randint(1, 100 if int(len(array) / 2) > 100 else len(array))
            station += array[0: that_time]
            array = array[that_time:]
            if random.randint(0, 1) == 0:
                that_time = random.randint(1, len(station))
                for i in " " * that_time:
                    output_array.append(station.pop())
        for i in range(len(station)):
            output_array.append(station.pop())
        f.write(" ".join(str(i) for i in output_array) + '\n')