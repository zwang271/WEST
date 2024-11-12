# Plots the results of running benchmark.py
# Author : Zili Wang
# Creates and saves a plot for each parameter (m, n, d) with the following information:
#       - Average time taken to run WEST on each value of the parameter, with std deviation
#       - Average output length for each value of the parameter, with std deviation

import matplotlib.pyplot as plt
import sys
import numpy as np
import os

description = {"m": "m: Maximum value of interval bounds", 
               "n": "n: Number of Atomic Propositions", 
               "d": "d: Max Nesting Depth of the Formula"
               }
FONTSIZE = 16
TITLE_FONTSIZE = 16

for dir in ["m", "n", "d"]:
    parameter = str(dir)
    values = []
    for file in os.listdir(dir):
        if file.endswith(".result"):
            value = file.replace(".result", "")
            times, output_lengths = [], []
            with open(f"{dir}/{file}", "r") as f:
                lines = f.readlines()
                for line in lines: # formula: time  output length
                    line = line.strip().split(" : ")
                    times.append(float(line[1]))
                    output_lengths.append(int(line[2]))
            values.append((value, times, output_lengths))
    values.sort(key=lambda x: int(x[0]))
    avg_times = [np.average(x[1]) for x in values]
    std_times = [np.std(x[1]) for x in values]
    avg_output_lengths = [np.average(x[2]) for x in values]
    std_output_lengths = [np.std(x[2]) for x in values]
    
    x = [int(x[0]) for x in values]
    y_times = avg_times
    y_output_lengths = avg_output_lengths

    # plot y_times and y_output_lengths vs x 
    fig = plt.figure()
    ax = fig.add_subplot(111)
    # plot dots as well as lines
    ax.plot(x, y_times, 'b--', label='Time (s)')
    ax.scatter(x, y_times, color='blue')
    ax2 = ax.twinx()
    ax2.plot(x, y_output_lengths, 'r-', label='Output Length')
    ax2.scatter(x, y_output_lengths, color='red')
    # combine the two legends
    lines, labels = ax.get_legend_handles_labels()
    lines2, labels2 = ax2.get_legend_handles_labels()
    ax2.legend(lines + lines2, labels + labels2, loc='upper left', fontsize=14)

    ax.grid()
    ax.set_xlim([min(x)-0.1, max(x)])
    ax.set_ylim([-0.1, 5])
    ax2.set_ylim([-(0.1/5)*90, 90])
    ax.set_xlabel(description[parameter], fontsize=FONTSIZE)
    ax.set_ylabel('Time (s)', color='b', fontsize=FONTSIZE)
    ax2.set_ylabel('Number of Regular Expressions', color='r', fontsize=FONTSIZE)
    ax.tick_params(axis='both', which='major', labelsize=FONTSIZE)
    ax2.tick_params(axis='both', which='major', labelsize=FONTSIZE)
    plt.title(f"Average Time and Output Length vs {parameter}", fontsize=TITLE_FONTSIZE)
    plt.savefig(f"{parameter}.png")
    plt.show()
    print("Plot saved as", f"{parameter}.png")
