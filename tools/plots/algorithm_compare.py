import timings
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

#
# Demonstrate case when bitset > vector
#
df = timings.time_mist(ms=range(100,1100,100), ns=[50], bs=[2], cs=[False], q=50)
vec = df[df['alg'] == 'vector']
bit = df[df['alg'] == 'bitset']
m = vec['m']
v = vec['time'] * 1e6
b = bit['time'] * 1e6
coef_vec = np.polyfit(np.array(m,dtype='float'),np.array(v,dtype='float'),1)
coef_bit = np.polyfit(np.array(m,dtype='float'),np.array(b,dtype='float'),1)
fit_vec = np.poly1d(coef_vec)
fit_bit = np.poly1d(coef_bit)
fig,ax = plt.subplots()
ax.plot(m,v, 'bx', m, fit_vec(m), '--b',label='Vector')
ax.plot(m,b, 'r.', m, fit_bit(m), '--r',label='Bitset')
props = dict(boxstyle='round', facecolor='wheat', alpha=0.5)
ax.text(0.80, 0.10, "n=50,b=2", transform=ax.transAxes, fontsize=12,
                verticalalignment='top', bbox=props)
ax.legend()
ax.set_ylabel('us')
ax.set_xlabel('m')
plt.savefig("algorithm_compare2.tif")

#
# Demonstrate case when vector > bitset
#
df = timings.time_mist(ms=range(100,1100,100), ns=[50], bs=[6], cs=[False], q=50)
vec = df[df['alg'] == 'vector']
bit = df[df['alg'] == 'bitset']
m = vec['m']
v = vec['time'] * 1e6
b = bit['time'] * 1e6
coef_vec = np.polyfit(np.array(m,dtype='float'),np.array(v,dtype='float'),1)
coef_bit = np.polyfit(np.array(m,dtype='float'),np.array(b,dtype='float'),1)
fit_vec = np.poly1d(coef_vec)
fit_bit = np.poly1d(coef_bit)
fig,ax = plt.subplots()
ax.plot(m,v, 'bx', m, fit_vec(m), '--b',label='Vector')
ax.plot(m,b, 'r.', m, fit_bit(m), '--r',label='Bitset')
props = dict(boxstyle='round', facecolor='wheat', alpha=0.5)
ax.text(0.80, 0.10, "n=50,b=6", transform=ax.transAxes, fontsize=12,
                verticalalignment='top', bbox=props)
ax.legend()
ax.set_ylabel('us')
ax.set_xlabel('m')
plt.savefig("algorithm_compare4.tif")
