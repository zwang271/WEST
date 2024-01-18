import sys
import numpy as np
import matplotlib.pyplot as plt


i = sys.argv[1]
# read in all rows in data, each row is 3 numbers separated by spaces
lengths, times, chars = np.loadtxt(f"./complexities{i}.txt", unpack=True)
# plot histogram of times and chars in same figure
fig, ax = plt.subplots(1, 2, figsize=(10, 5))
ax[0].hist(times, bins=10)
ax[0].set_title("Time Complexity")
ax[0].set_xlabel("Time (seconds)")
ax[0].set_ylabel("Frequency")
ax[1].hist(chars, bins=10)
ax[1].set_title("Space Complexity")
ax[1].set_xlabel("Space (characters)")
ax[1].set_ylabel("Frequency")
plt.savefig(f"./complexities{i}_historgram.png")

time_cutoff = 1
space_cutoff = 1000
# count how many times are less than 1s
print(f"Time less than {time_cutoff}s: {len(times[times < time_cutoff])}")
# count how many times are less than 100 characters
print(f"Space less than {space_cutoff} characters: {len(chars[chars < space_cutoff])}")
