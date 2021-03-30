#!/usr/bin/env python3
import matplotlib.pyplot as plt
import pandas as pd

#from math import comb #XXX python 3.8
from mpl_toolkits import mplot3d

import operator as op
from functools import reduce

f = "iocache.csv"
df = pd.read_csv(f)
cold = df[df.Temp == "Cold"]
hott = df[df.Temp == "Hot"]
none = cold[cold.Cache == "none"]
iocold = cold[cold.Cache == "io"]
iohott = hott[hott.Cache == "io"]

#plt.ion()

ms = iohott.M

plt.cla()
plt.clf()
plt.plot(ms, none.Time,   color="grey", marker=".", linestyle='-', label="no cache")
plt.plot(ms, iocold.Time, color="blue", marker=".", linestyle='-', label="Small files - Cold")
plt.plot(ms, iohott.Time, color="red",  marker=".", linestyle='-', label="Small files - Hot")
plt.xlabel("M")
plt.ylabel("milliseconds")
plt.title("IO Cache")
plt.legend()

plt.show()
