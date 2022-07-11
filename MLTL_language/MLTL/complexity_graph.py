import pandas as pd
import matplotlib.pyplot as plt

data = pd.read_csv("complexities.txt", sep=" ", header=None, names=["Input Length", "Output Length"])
print(data)
data.plot.scatter(x="Input Length", y="Output Length")
plt.title("2 Iterations, 5 Propositonal Variables, delta=5, Mission end=10")
plt.show()