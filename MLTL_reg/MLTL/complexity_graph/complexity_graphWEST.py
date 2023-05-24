import pandas as pd
import matplotlib.pyplot as plt
import sys

# Run python3 ./complexity_graph.py {experiment number} {plot_length}
# Set plot_length = 1 to plot length vs length
# Set plot_length = 0 to plot length vs time

experiment = int(sys.argv[1])
plot_length = int(sys.argv[2])
x_font = 10
y_font = 10
title_font = 10

data = pd.read_csv("./complexities"+str(experiment)+".txt", sep=" ", header=None, names=["Input Length", "Time(s)", "Output Length"])

if plot_length:
    data.plot.scatter(x="Input Length", y="Output Length", color="Blue", marker="x")
    plt.ylabel("Output Length(# of chars)", fontsize = y_font)
else: # plot time
    data.plot.scatter(x="Input Length", y="Time(s)", color="Red", marker="x")
    plt.ylabel("Time(s)", fontsize=y_font)

plt.xlabel('Input Length(# of chars)', fontsize=x_font)

if experiment == 1:
    if plot_length:
        title = "WEST Space Simulation 1"
    else:
        title = "WEST Time Simulation 1"
elif experiment == 2:
    if plot_length:
        title = "WEST Space Simulation 2"
    else:
        title = "WEST Time Simulation 2"
elif experiment == 3:
    if plot_length:
        title = "WEST Space Simulation 3"
    else:
        title = "WEST Time Simulation 3"
elif experiment == 4:
    if plot_length:
        title = "WEST Space Simulation 4"
    else:
        title = "WEST Time Simulation 4"

plt.title(title, y=1, fontsize=title_font)

fig = plt.gcf()
fig.set_size_inches(4.5, 3)
plt.tight_layout()
plt.show()