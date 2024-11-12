import os
import matplotlib.pyplot as plt
import numpy as np
import matplotlib
import matplotlib.colors as mcolors
from matplotlib.markers import MarkerStyle


# matplotlib.rcParams['font.family'] = "CMU Serif"
# matplotlib.rcParams['text.latex.preamble'] = r"\usepackage{amssymb,amsfonts,amsmath,stix}"
# matplotlib.rcParams['text.usetex'] = True
plt.style.use(['presentation.mplstyle'])
restypes = ["prop","propS","smt2"]
outsT = dict()
outsS = dict()
timeouts = dict()
outsTot = dict()

outsTr = dict()
outsSr = dict()
timeoutsr = dict()
outsTotr = dict()

def fileparser(filename,tp1,tp):
    try:
        with open(filename + tp1,'r') as lsfile:
            lsf1 = lsfile.read()
            lsf1 = lsf1.split("\n")[:-1]
            outT = []
            outS = []
            timeout = []
            for line in lsf1:
                parts = line.split("\t\t")
                outT.append(int(parts[1]))
                outS.append(int(parts[2]))
                if ( (parts[0] == "timeout") or (parts[0] == "unknown") ):
                    timeout.append(1)
            outsT[tp] = outsT[tp] + outT
            outsS[tp] = outsS[tp] + outS
            timeouts[tp] = timeouts[tp] + timeout
    except:
        raise FileExistsError



for tp in restypes:
    outsT[tp] = []
    outsS[tp] = []
    timeouts[tp] = []
    fileparser("../BenchmarkResults/airspace_properties.",tp,tp)
    fileparser("../BenchmarkResults/contract_properties.",tp,tp)
    fileparser("../BenchmarkResults/extended_properties.",tp,tp)
    fileparser("../BenchmarkResults/nominal_properties.",tp,tp)
    fileparser("../BenchmarkResults/verification_properties.",tp,tp)
    outsS[tp] = np.cumsum(outsS[tp])
    outsT[tp] = np.cumsum(outsT[tp])
    outsTot[tp] = [outsS[tp][i] + outsT[tp][i] for i in range(len(outsT[tp]))]
for tp1 in restypes:
    tp = tp1+"0"
    outsT[tp] = []
    outsS[tp] = []
    timeouts[tp] = []
    fileparser("../BenchmarkResults/airspace_properties0.",tp1,tp)
    fileparser("../BenchmarkResults/contract_properties0.",tp1,tp)
    fileparser("../BenchmarkResults/extended_properties0.", tp1,tp)
    fileparser("../BenchmarkResults/nominal_properties0.",tp1,tp)
    fileparser("../BenchmarkResults/verification_properties0.", tp1,tp)
    outsS[tp] = np.cumsum(outsS[tp])
    outsT[tp] = np.cumsum(outsT[tp])
    outsTot[tp] = [outsS[tp][i] + outsT[tp][i] for i in range(len(outsT[tp]))]

for tp1 in restypes:
    tp = tp1 + "00"
    outsT[tp] = []
    outsS[tp] = []
    timeouts[tp] = []
    fileparser("../BenchmarkResults/airspace_properties00.",tp1,tp)
    fileparser("../BenchmarkResults/contract_properties00.",tp1,tp)
    fileparser("../BenchmarkResults/extended_properties00.", tp1,tp)
    fileparser("../BenchmarkResults/nominal_properties00.",tp1,tp)
    fileparser("../BenchmarkResults/verification_properties00.", tp1,tp)
    outsS[tp] = np.cumsum(outsS[tp])
    outsT[tp] = np.cumsum(outsT[tp])
    outsTot[tp] = [outsS[tp][i] + outsT[tp][i] for i in range(len(outsT[tp]))]


fig, ax = plt.subplots()
ax.semilogy(outsT["smt2"], "-",color=mcolors.to_rgb('tab:blue'), fillstyle="none",linewidth=2.3)
ax.semilogy(outsT["prop"], "--",color=mcolors.to_rgb('tab:orange'), fillstyle="none",linewidth=2.3)
ax.semilogy(outsT["propS"], "-.",color=mcolors.to_rgb('tab:pink'), fillstyle="none",linewidth=2.3)
ax.semilogy(outsT["smt20"], "-",color=mcolors.to_rgb('tab:blue'), fillstyle="none",linewidth=2.3)
ax.semilogy(outsT["prop0"], "--",color=mcolors.to_rgb('tab:orange'), fillstyle="none",linewidth=2.3)
ax.semilogy(outsT["propS0"], "-.",color=mcolors.to_rgb('tab:pink'), fillstyle="none",linewidth=2.3)
ax.semilogy(outsT["smt200"], "-",color=mcolors.to_rgb('tab:blue'), fillstyle="none",linewidth=2.3)
ax.semilogy(outsT["prop00"], "--",color=mcolors.to_rgb('tab:orange'), fillstyle="none",linewidth=2.3)
ax.semilogy(outsT["propS00"], "-.",color=mcolors.to_rgb('tab:pink'), fillstyle="none",linewidth=2.3)
# outsT["propS"], "tab:orange-",outsT["prop"], "tab:pink--", fillstyle="none",linewidth=2.7)
# ax.semilogy(outsT["smt20"], "-.", outsT["propS0"], "-",outsT["prop0"], "--", fillstyle="none",linewidth=2.7)
ax.semilogy(600,outsT["smt2"][600],"d",color=mcolors.to_rgb('tab:blue'),markersize=12,fillstyle="none",markeredgewidth=2.5)
ax.semilogy(600,outsT["smt20"][600],"o",color=mcolors.to_rgb('tab:blue'),markersize=12,fillstyle="none",markeredgewidth=2.5)
ax.semilogy(600,outsT["smt200"][600],"s",color=mcolors.to_rgb('tab:blue'),markersize=12,fillstyle="none",markeredgewidth=2.5)
ax.semilogy(400,outsT["prop"][400],"d",color=mcolors.to_rgb('tab:orange'),markersize=12,fillstyle="none",markeredgewidth=2.5)
ax.semilogy(400,outsT["prop0"][400],"p",color=mcolors.to_rgb('tab:orange'),markersize=12,fillstyle="none",markeredgewidth=2.5)
ax.semilogy(400,outsT["prop00"][400],"s",color=mcolors.to_rgb('tab:orange'),markersize=12,fillstyle="none",markeredgewidth=2.5)
ax.semilogy(200,outsT["propS"][200],"d",color=mcolors.to_rgb('tab:pink'),markersize=12,fillstyle="none",markeredgewidth=2.5)
ax.semilogy(200,outsT["propS0"][200],"p",color=mcolors.to_rgb('tab:pink'),markersize=12,fillstyle="none",markeredgewidth=2.5)
ax.semilogy(200,outsT["propS00"][200],"s",color=mcolors.to_rgb('tab:pink'),markersize=12,fillstyle="none",markeredgewidth=2.5)
ax.spines['top'].set_visible(False)
ax.spines['right'].set_visible(False)
ax.axvline(x=200,color="k",linestyle=":")
ax.axvline(x=400,color="k",linestyle=":")
ax.axvline(x=600,color="k",linestyle=":")
plt.xlabel("Number of Formulas", fontsize=20)
plt.ylabel("Cum. Time (milliseconds)" , fontsize=20)
plt.xticks(fontsize=18)
plt.yticks(fontsize=18)
plt.legend(["SMT", "Bool-fast","Bool-slow"], fontsize=14)
# textstr = '\n'.join((
#     r'$\Diamond\quad m = 10^2$',
#     r'$\pentagon \quad m = 10^3 $' ,
#     '$\Box\quad m=10^4$' ))
# these are matplotlib.patch.Patch properties
props = dict(facecolor='white', alpha=0.5)

# place a text box in upper left in axes coords
# ax.text(0.3, 0.05, textstr, transform=ax.transAxes, fontsize=14,
#         verticalalignment='bottom', bbox=props)
plt.savefig("1a.png", bbox_inches='tight')

ax.clear()
ax.semilogy(outsS["smt2"], "-",color=mcolors.to_rgb('tab:blue'), fillstyle="none",linewidth=2.3)
ax.semilogy(outsS["prop"], "--",color=mcolors.to_rgb('tab:orange'), fillstyle="none",linewidth=2.3)
ax.semilogy(outsS["propS"], "-.",color=mcolors.to_rgb('tab:pink'), fillstyle="none",linewidth=2.3)
ax.semilogy(outsS["smt20"], "-",color=mcolors.to_rgb('tab:blue'), fillstyle="none",linewidth=2.3)
ax.semilogy(outsS["prop0"], "--",color=mcolors.to_rgb('tab:orange'), fillstyle="none",linewidth=2.3)
ax.semilogy(outsS["propS0"], "-.",color=mcolors.to_rgb('tab:pink'), fillstyle="none",linewidth=2.3)
ax.semilogy(outsS["smt200"], "-",color=mcolors.to_rgb('tab:blue'), fillstyle="none",linewidth=2.3)
ax.semilogy(outsS["prop00"], "--",color=mcolors.to_rgb('tab:orange'), fillstyle="none",linewidth=2.3)
ax.semilogy(outsS["propS00"], "-.",color=mcolors.to_rgb('tab:pink'), fillstyle="none",linewidth=2.3)
# outsT["propS"], "tab:orange-",outsT["prop"], "tab:pink--", fillstyle="none",linewidth=2.7)
# ax.semilogy(outsT["smt20"], "-.", outsT["propS0"], "-",outsT["prop0"], "--", fillstyle="none",linewidth=2.7)
ax.semilogy(600,outsS["smt2"][600],"d",color=mcolors.to_rgb('tab:blue'),markersize=12,fillstyle="none",markeredgewidth=2.5)
ax.semilogy(600,outsS["smt20"][600],"p",color=mcolors.to_rgb('tab:blue'),markersize=12,fillstyle="none",markeredgewidth=2.5)
ax.semilogy(600,outsS["smt200"][600],"s",color=mcolors.to_rgb('tab:blue'),markersize=12,fillstyle="none",markeredgewidth=2.5)
ax.semilogy(400,outsS["prop"][400],"d",color=mcolors.to_rgb('tab:orange'),markersize=12,fillstyle="none",markeredgewidth=2.5)
ax.semilogy(400,outsS["prop0"][400],"p",color=mcolors.to_rgb('tab:orange'),markersize=12,fillstyle="none",markeredgewidth=2.5)
ax.semilogy(400,outsS["prop00"][400],"s",color=mcolors.to_rgb('tab:orange'),markersize=12,fillstyle="none",markeredgewidth=2.5)
ax.semilogy(200,outsS["propS"][200],"d",color=mcolors.to_rgb('tab:pink'),markersize=12,fillstyle="none",markeredgewidth=2.5)
ax.semilogy(200,outsS["propS0"][200],"p",color=mcolors.to_rgb('tab:pink'),markersize=12,fillstyle="none",markeredgewidth=2.5)
ax.semilogy(200,outsS["propS00"][200],"s",color=mcolors.to_rgb('tab:pink'),markersize=12,fillstyle="none",markeredgewidth=2.5)
ax.spines['top'].set_visible(False)
ax.spines['right'].set_visible(False)
ax.axvline(x=200,color="k",linestyle=":")
ax.axvline(x=400,color="k",linestyle=":")
ax.axvline(x=600,color="k",linestyle=":")
plt.xlabel("Number of Formulas", fontsize=20)
plt.ylabel("Cum. Time (milliseconds)" , fontsize=20)
plt.xticks(fontsize=18)
plt.yticks(fontsize=18)
plt.legend(["SMT", "Bool-fast","Bool-slow"], fontsize=14)
# textstr = '\n'.join((
#     r'$\Diamond\quad m = 10^2$',
#     r'$\pentagon\quad m = 10^3 $' ,
#     '$\Box\quad m=10^4$' ))
# these are matplotlib.patch.Patch properties
props = dict(facecolor='white', alpha=0.5)
# ax.text(0.3, 0.05, textstr, transform=ax.transAxes, fontsize=14,
        # verticalalignment='bottom', bbox=props)
plt.savefig("1b.png", bbox_inches='tight')


ax.clear()
ax.semilogy(outsTot["smt2"], "-",color=mcolors.to_rgb('tab:blue'), fillstyle="none",linewidth=2.3)
ax.semilogy(outsTot["prop"], "--",color=mcolors.to_rgb('tab:orange'), fillstyle="none",linewidth=2.3)
ax.semilogy(outsTot["propS"], "-.",color=mcolors.to_rgb('tab:pink'), fillstyle="none",linewidth=2.3)
ax.semilogy(outsTot["smt20"], "-",color=mcolors.to_rgb('tab:blue'), fillstyle="none",linewidth=2.3)
ax.semilogy(outsTot["prop0"], "--",color=mcolors.to_rgb('tab:orange'), fillstyle="none",linewidth=2.3)
ax.semilogy(outsTot["propS0"], "-.",color=mcolors.to_rgb('tab:pink'), fillstyle="none",linewidth=2.3)
ax.semilogy(outsTot["smt200"], "-",color=mcolors.to_rgb('tab:blue'), fillstyle="none",linewidth=2.3)
ax.semilogy(outsTot["prop00"], "--",color=mcolors.to_rgb('tab:orange'), fillstyle="none",linewidth=2.3)
ax.semilogy(outsTot["propS00"], "-.",color=mcolors.to_rgb('tab:pink'), fillstyle="none",linewidth=2.3)
# outsT["propS"], "tab:orange-",outsT["prop"], "tab:pink--", fillstyle="none",linewidth=2.7)
# ax.semilogy(outsT["smt20"], "-.", outsT["propS0"], "-",outsT["prop0"], "--", fillstyle="none",linewidth=2.7)
ax.semilogy(600,outsTot["smt2"][600],"d",color=mcolors.to_rgb('tab:blue'),markersize=12,fillstyle="none",markeredgewidth=2.5)
ax.semilogy(600,outsTot["smt20"][600],"p",color=mcolors.to_rgb('tab:blue'),markersize=12,fillstyle="none",markeredgewidth=2.5)
ax.semilogy(600,outsTot["smt200"][600],"s",color=mcolors.to_rgb('tab:blue'),markersize=12,fillstyle="none",markeredgewidth=2.5)
ax.semilogy(400,outsTot["prop"][400],"d",color=mcolors.to_rgb('tab:orange'),markersize=12,fillstyle="none",markeredgewidth=2.5)
ax.semilogy(400,outsTot["prop0"][400],"p",color=mcolors.to_rgb('tab:orange'),markersize=12,fillstyle="none",markeredgewidth=2.5)
ax.semilogy(400,outsTot["prop00"][400],"s",color=mcolors.to_rgb('tab:orange'),markersize=12,fillstyle="none",markeredgewidth=2.5)
ax.semilogy(200,outsTot["propS"][200],"d",color=mcolors.to_rgb('tab:pink'),markersize=12,fillstyle="none",markeredgewidth=2.5)
ax.semilogy(200,outsTot["propS0"][200],"p",color=mcolors.to_rgb('tab:pink'),markersize=12,fillstyle="none",markeredgewidth=2.5)
ax.semilogy(200,outsTot["propS00"][200],"s",color=mcolors.to_rgb('tab:pink'),markersize=12,fillstyle="none",markeredgewidth=2.5)
ax.spines['top'].set_visible(False)
ax.spines['right'].set_visible(False)
ax.axvline(x=200,color="k",linestyle=":")
ax.axvline(x=400,color="k",linestyle=":")
ax.axvline(x=600,color="k",linestyle=":")
plt.xlabel("Number of Formulas", fontsize=20)
plt.ylabel("Cum. Time (milliseconds)" , fontsize=20)
plt.xticks(fontsize=18)
plt.yticks(fontsize=18)
plt.legend(["SMT", "Bool-fast","Bool-slow"], fontsize=14)
# textstr = '\n'.join((
#     r'$\Diamond\quad m = 10^2$',
#     r'$\pentagon\quad m = 10^3 $' ,
#     '$\Box\quad m=10^4$' ))
# these are matplotlib.patch.Patch properties
# props = dict(facecolor='white', alpha=0.5)

# place a text box in upper left in axes coords
# ax.text(0.3, 0.05, textstr, transform=ax.transAxes, fontsize=14,
        # verticalalignment='bottom', bbox=props)
plt.savefig("1c.png", bbox_inches='tight')

plt.clf()
fig, axs = plt.subplots(3,figsize=(5,4.8))
fig.tight_layout()
# plt.figure(figsize=(5, 5))
Names = ["SMT", "slow", "fast"]
# Names = [1,1.5,2]
vals2 = [len(timeouts["smt2"]), len(timeouts["propS"]), len(timeouts["prop"])]
vals1 = [len(timeouts["smt20"]), len(timeouts["propS0"]), len(timeouts["prop0"])]
vals0 = [len(timeouts["smt200"]), len(timeouts["propS00"]), len(timeouts["prop00"])]
some1 = axs[0].bar(Names,vals0,width=0.3)
some2 = axs[1].bar(Names,vals1,width=0.3)
some3 = axs[2].bar(Names,vals2,width=0.3)
for i in range(3):
    axs[i].spines['top'].set_visible(False)
    axs[i].spines['right'].set_visible(False)
    axs[i].spines['left'].set_visible(False)

plt.xticks(fontsize=20)
plt.yticks(fontsize=20)
# The part below is mostly taken from 
# https://www.geeksforgeeks.org/how-to-annotate-bars-in-barplot-with-matplotlib-in-python/
for bar in some1.patches:
    axs[0].annotate(format(bar.get_height(), 'd'), (bar.get_x() + bar.get_width() / 2, bar.get_height()), ha='center', va='center', size=20, xytext=(0, 10), textcoords='offset points')
axs[0].get_yaxis().set_ticks(ticks= [5],labels=["$m = 10^4$"], fontsize=20)
axs[0].tick_params(
    axis='y', 
    which='both', 
    left=False)
axs[0].get_xaxis().set_ticks([])

for bar in some2.patches:
    axs[1].annotate(format(bar.get_height(), 'd'), (bar.get_x() + bar.get_width() / 2, bar.get_height()), ha='center', va='center', size=20, xytext=(0, 10), textcoords='offset points')
axs[1].get_yaxis().set_ticks([])
axs[1].get_xaxis().set_ticks([])
axs[1].get_yaxis().set_ticks(ticks= [5],labels=["$m = 10^3$"], fontsize=20)
axs[1].tick_params(
    axis='y',
    which='both',
    left=False)
axs[0].get_xaxis().set_ticks([])
for bar in some3.patches:
    axs[2].annotate(format(bar.get_height(), 'd'), (bar.get_x() + bar.get_width() / 2, bar.get_height()), ha='center', va='center', size=20, xytext=(0, 9), textcoords='offset points')
axs[2].get_yaxis().set_ticks([])
axs[2].get_yaxis().set_ticks(ticks= [5],labels=["$m = 10^2$"], fontsize=20)
axs[2].tick_params(
    axis='y',
    which='both', 
    left=False)         


plt.savefig("1d.png", bbox_inches='tight')


def fileparserRan(filename,tp):
    try:
        with open(filename + tp,'r') as lsfile:
            lsf1 = lsfile.read()
            lsf1 = lsf1.split("\n")[:-1]
            outT = []
            outS = []
            timeout = []
            for line in lsf1:
                parts = line.split("\t\t")
                outT.append(int(parts[1]))
                outS.append(int(parts[2]))
                if ( (parts[0] == "timeout") or (parts[0] == "unknown") ):
                    timeout.append(1)
            outsTr[tp] = outsTr[tp] + outT
            outsSr[tp] = outsSr[tp] + outS
            timeoutsr[tp] = timeoutsr[tp] + timeout
    except:
        raise FileExistsError

def fileparserRanSmv(filename,tp):
    try:
        with open(filename + tp,'r') as lsfile:
            lsf1 = lsfile.read()
            lsf1 = lsf1.split("\n")[:-1]
            outT = []
            outS = []
            timeout = []
            for line in lsf1:
                parts = line.split("\t\t")
                outT.append(int(parts[0]))
                outS.append(int(parts[1]))
                if ( (int(parts[1]) >= 180000) ):
                    timeout.append(1)
            outsTr[tp] = outsTr[tp] + outT
            outsSr[tp] = outsSr[tp] + outS
            timeoutsr[tp] = timeoutsr[tp] + timeout
    except:
        raise FileExistsError

for tp in restypes:
    outsTr[tp] = []
    outsSr[tp] = []
    timeoutsr[tp] = []
    fileparserRan("../BenchmarkResults/randomList.",tp)
    outsSr[tp] = np.cumsum(outsSr[tp])
    outsTr[tp] = np.cumsum(outsTr[tp])
    outsTotr[tp] = [outsSr[tp][i] + outsTr[tp][i] for i in range(len(outsTr[tp]))]

restypesSmv = ["ic3","bmc"]

for tp in restypesSmv:
    outsTr[tp] = []
    outsSr[tp] = []
    timeoutsr[tp] = []
    fileparserRanSmv("../BenchmarkResults/randomList.",tp)
    outsSr[tp] = np.cumsum(outsSr[tp])
    outsTr[tp] = np.cumsum(outsTr[tp])
    outsTotr[tp] = [outsSr[tp][i] + outsTr[tp][i] for i in range(len(outsTr[tp]))]

fig, ax = plt.subplots()
ax.clear()
ax.semilogy(outsTr["smt2"], "-", outsTr["ic3"], "--", outsTr["propS"], "-.",outsTr["prop"], ":", fillstyle="none",linewidth=2.7)
plt.xlabel("Number of Formulas", fontsize=20)
plt.ylabel("Cum. Time (milliseconds)" , fontsize=20)
plt.xticks(fontsize=18)
plt.yticks(fontsize=18)
plt.legend(["SMT","SMV", "Bool-slow","Bool-fast"], fontsize=14)
plt.savefig("2a.png", bbox_inches='tight')

ax.clear()
ax.semilogy(outsSr["smt2"], "-", outsSr["ic3"], "--", outsSr["bmc"], "--", outsSr["propS"], "-.",outsSr["prop"], ":", fillstyle="none",linewidth=2.7)
plt.xlabel("Number of Formulas", fontsize=20)
plt.ylabel("Cum. Time (milliseconds)" , fontsize=20)
plt.xticks(fontsize=18)
plt.yticks(fontsize=18)
plt.legend(["SMT","IC3","BMC", "Bool-slow","Bool-fast"], fontsize=14)
plt.savefig("2b.png", bbox_inches='tight')

ax.clear()
ax.semilogy(outsTotr["smt2"], "-", outsTotr["ic3"], "--", outsTotr["bmc"], "--", outsTotr["propS"], "-.",outsTotr["prop"], ":", fillstyle="none",linewidth=2.7)
plt.xlabel("Number of Formulas", fontsize=20)
plt.ylabel("Cum. Time (milliseconds)" , fontsize=20)
plt.xticks(fontsize=18)
plt.yticks(fontsize=18)
plt.legend(["SMT","IC3","BMC", "Bool-slow","Bool-fast"], fontsize=14)
plt.savefig("2c.png", bbox_inches='tight')

ax.clear()
# plt.figure(figsize=(5, 5))
Names = ["SMT","IC3","BMC", "slow", "fast"]
vals = [len(timeoutsr["smt2"]),len(timeoutsr["ic3"]),len(timeoutsr["bmc"]), len(timeoutsr["propS"]), len(timeoutsr["prop"])]
some = plt.bar(Names,vals,width=0.3)
ax = plt.gca()
ax.spines['top'].set_visible(False)
ax.spines['right'].set_visible(False)
ax.spines['left'].set_visible(False)
plt.xticks(fontsize=32)
plt.yticks(fontsize=32)
# The part below is mostly taken from 
# https://www.geeksforgeeks.org/how-to-annotate-bars-in-barplot-with-matplotlib-in-python/
for bar in some.patches:
    plt.annotate(format(bar.get_height(), 'd'), (bar.get_x() + bar.get_width() / 2, bar.get_height()), ha='center', va='center', size=32, xytext=(0, 12), textcoords='offset points')
ax.get_yaxis().set_ticks([])
plt.savefig("2d.png", bbox_inches='tight')