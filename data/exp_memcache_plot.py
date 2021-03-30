#!/usr/bin/env python3
import matplotlib.pyplot as plt
import pandas as pd

#from math import comb #XXX python 3.8
from mpl_toolkits import mplot3d

import operator as op
from functools import reduce

f = "memcache.csv"
df = pd.read_csv(f)

cold = df[df.Temp == "Cold"]
fifo = cold[cold.Cache == "mru"]
flat = cold[cold.Cache == "flat"]
none = cold[cold.Cache == "none"]

none100   = sum(none[none.M == 100].Time) / len((none[none.M == 100].Time))
none1000  = sum(none[none.M == 1000].Time) / len((none[none.M == 1000].Time))
none10000 = sum(none[none.M == 10000].Time) / len((none[none.M == 10000].Time))

fifoS1 = fifo[fifo.Size == 10].Time
fifoS2 = fifo[fifo.Size == 50].Time
fifoS3 = fifo[fifo.Size == 100].Time
flatS1 = flat[flat.Size == 10].Time
flatS2 = flat[flat.Size == 50].Time
flatS3 = flat[flat.Size == 100].Time

#plt.ion()

ms = pd.Series.unique(flat.M)

plt.cla()
plt.clf()
plt.plot(ms, [none100, none1000, none10000], color="grey", marker=".", linestyle='-', label="no cache")
plt.plot(ms, flatS1, color="blue", marker=".", linestyle='--', label="Flat 10%" )
plt.plot(ms, flatS2, color="blue", marker="+", linestyle='-.', label="Flat 50%" )
plt.plot(ms, flatS3, color="blue", marker="*", linestyle='-' , label="Flat 100%")
plt.plot(ms, fifoS1, color="red",  marker=".", linestyle='--', label="Fifo 10%" )
plt.plot(ms, fifoS2, color="red",  marker="+", linestyle='-.', label="Fifo 50%" )
plt.plot(ms, fifoS3, color="red",  marker="*", linestyle='-',  label="Fifo 100%")
plt.xlabel("M")
plt.ylabel("milliseconds")
plt.title("Flat vs Fifo")
plt.legend()

plt.show()
