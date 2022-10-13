import pandas as pd
import matplotlib.pyplot as plt

data = pd.read_csv("/Users/ctrave/Downloads/Research/ISU REU 2022/complexities4.txt", sep=" ", header=None, names=["Input Length", "Time(ms)", "Output Length"])
print(data)
#print(data.nlargest(2, "Time(ms)"))
#print(data.nlargest(3, "Output Length"))
#data.plot.scatter(x="Input Length", y="Output Length", color="Blue", marker="x")
data.plot.scatter(x="Input Length", y="Time(ms)", color="Red", marker="x")
plt.title("1 Iteration, 5 Propositional Variables, Delta = 10, Interval Max = 10", y=1.05)
plt.show()