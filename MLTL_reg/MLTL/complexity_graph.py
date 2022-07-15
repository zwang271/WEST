import pandas as pd
import matplotlib.pyplot as plt

data = pd.read_csv("/Users/ctrave/Downloads/Research/ISU REU 2022/complexities3.txt", sep=" ", header=None, names=["Input Length", "Time(ms)", "Output Length"])
print(data)
data.plot.scatter(x="Input Length", y="Output Length", color="Blue")
#data.plot.scatter(x="Input Length", y="Time(ms)", color="Red")
plt.title("1 Iteration, 5 Propositional Variables, Delta = 10, Interval Max = 10")
plt.show()