import pandas as pd
import matplotlib.pyplot as plt

data = pd.read_csv("complexities.txt", sep=" ", header=None, names=["Input Length", "Time(ms)", "Output Length"])
print(data)
data.plot.scatter(x="Input Length", y="Output Length", color="Blue")
#data.plot.scatter(x="Input Length", y="Time(ms)", color="Red")
plt.title("2 Iterations, 10 Propositional Variables, Delta = 10, Mission End = 10")
plt.show()