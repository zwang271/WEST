from time import time

t0 = time()

n = 30
k = 0
for i in range(2**n):
    k += 1

print(k)
print(time() - t0)
    