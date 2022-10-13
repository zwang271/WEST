import pandas as pd
import matplotlib.pyplot as plt
import sys

# Run python3 ./complexity_graph.py {experiment number} {plot_length}
# Set plot_length = 1 to plot length vs length
# Set plot_length = 0 to plot length vs time

experiment = int(sys.argv[1])
plot_length = int(sys.argv[2])
x_font = 16
y_font = 16
title_font = 16

data = pd.read_csv("./complexities"+str(experiment)+".txt", sep=" ", header=None, names=["Input Length", "Time(ms)", "Output Length"])
# print(data)
#print(data.nlargest(2, "Time(ms)"))
#print(data.nlargest(3, "Output Length"))

if plot_length:
    data.plot.scatter(x="Input Length", y="Output Length", color="Blue", marker="x")
    plt.ylabel("Output Length", fontsize = y_font)
else: # plot time
    data.plot.scatter(x="Input Length", y="Time(ms)", color="Red", marker="x")
    plt.ylabel("Time(ms)", fontsize=y_font)

plt.xlabel('Input Length', fontsize=x_font)

if experiment == 1:
    title = "iter=2, prop vars=5, delta=10, interval max=10"
elif experiment == 2:
    title = "iter=1, prop vars=10, delta=20, interval max=20"
elif experiment == 3:
    title = "iter=2, prop vars=10, delta=5, interval max=10"
elif experiment == 4:
    title = "iter=1, prop vars=5, delta=10, interval max=10"

plt.title(title, y=1.05, fontsize=title_font)
plt.show()