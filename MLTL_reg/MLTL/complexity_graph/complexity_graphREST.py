import pandas as pd
import matplotlib.pyplot as plt
import sys

x_font = 16
y_font = 16
title_font = 16

data = pd.read_csv("./RESTcomplexities.txt", sep=" ", header=None, names=["Input Length", "Time(s)"])


data.plot.scatter(x="Input Length", y="Time(s)", color="Red", marker="x")
plt.ylabel("Time(s)", fontsize=y_font)
plt.xlabel('Input Length(# of chars)', fontsize=x_font)
title = "REST Experiment"


plt.title(title, y=1.05, fontsize=title_font)
plt.show()
