import pandas as pd
import matplotlib.pyplot as plt
import sys

x_font = 10
y_font = 10
title_font = 10

data = pd.read_csv("./RESTcomplexities.txt", sep=" ", header=None, names=["Input Length", "Time(s)"])


data.plot.scatter(x="Input Length", y="Time(s)", color="Red", marker="x")
plt.ylabel("Time(s)", fontsize=y_font)
plt.xlabel('Input Length(# of chars)', fontsize=x_font)
title = "REST Time Simulation"


plt.title(title, y=1.05, fontsize=title_font)

fig = plt.gcf()
fig.set_size_inches(4.5, 3)
plt.tight_layout()
plt.show()
