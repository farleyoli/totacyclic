import statistics 

width = []
size = []

for _ in range(8):
    array = list(map(int, input().split()))
    width.append(max(array))
    size.append(len(array))

print(statistics.mean(width))
print(statistics.stdev(width))
print(statistics.mean(size))
print(statistics.stdev(size))
