import timings
import matplotlib.pyplot as plt
import pandas as pd
import numpy as np

# plotting params
props = dict(boxstyle='round', facecolor='wheat', alpha=0.5)

#
# 1. Cache vs. NoCache
#
ranks = range(1,13,1)
ms = [100]
bs = [2]
cs = [True,False]
ns = [200]
algs = ['vector']
df = timings.time_mist(ms=ms, ns=ns, bs=bs, rs=ranks, cs=cs, q=200, algs=algs)
r = df[df['cache'] == True]['r']
cache = df[df['cache'] == True]['time'] * 1e6
nocache = df[df['cache'] == False]['time'] * 1e6
fig,ax = plt.subplots()
ax.plot(r,cache,label="cache")
ax.plot(r,nocache,label="nocache")
ax.text(0.20, 0.90, "m=%s,b=%s,n=%s,\nalg=%s,q=200" %(str(ms),str(bs),str(ns), str(algs)), transform=ax.transAxes, fontsize=10, verticalalignment='top', bbox=props)
ax.legend()
ax.set_ylabel('us')
ax.set_xlabel('threads')
plt.savefig("scaling1.tif")

#
# 2. Thread scaling, nocache
#
ranks = range(1,13,1)
ms = [10000]
bs = [2]
cs = [False]
ns = [100]
algs = ['vector']
df = timings.time_mist(ms=ms, ns=ns, bs=bs, rs=ranks, cs=cs, q=200, algs=algs)
r = df['r']
t = df['time'] * 1e3
fig,ax = plt.subplots()
ax.plot(r,t)
ax.text(0.20, 0.90, "m=%s,b=%s,n=%s,cache=%s\nalg=%s,q=200" %(str(ms),str(bs),str(ns), str(cs), str(algs)), transform=ax.transAxes, fontsize=10, verticalalignment='top', bbox=props)
ax.legend()
ax.set_ylabel('ms')
ax.set_xlabel('threads')
plt.savefig("scaling2.tif")

#
# 3. Thread scaling d3, cache
#
df = timings.time_mist(ms=[1000], ns=[100], bs=[2], ds=[3], rs=range(1,21,2), cs=[True], q=10, algs=['vector'], progress=True)
ranks = range(1,13,1)
ms = [1000]
bs = [2]
ds = [3]
cs = [True]
ns = [100]
algs = ['vector']
df = timings.time_mist(ms=ms, ns=ns, bs=bs, ds=ds, rs=ranks, cs=cs, q=200, algs=algs)
r = df['r']
t = df['time']
fig,ax = plt.subplots()
ax.plot(r,t)
ax.text(0.20, 0.90, "m=%s,b=%s,n=%s,d=%s\ncache=%s,alg=%s,q=200" %(str(ms),str(bs),str(ns), str(ds), str(cs), str(algs)), transform=ax.transAxes, fontsize=10, verticalalignment='top', bbox=props)
ax.legend()
ax.set_ylabel('s')
ax.set_xlabel('threads')
plt.savefig("scaling3.tif")



#
# 4. N scaling at different thread counts
#
ranks = [2,4,6]
ms=[100]
bs=[2]
cs=[False]
ns=[50,100,150,200,250,300]
algs=['vector']
df = timings.time_mist(ms=ms, ns=ns, bs=bs, rs=ranks, cs=cs, q=100, algs=algs)
n = ns
fig,ax = plt.subplots()
for rank in ranks:
    t = df[df['r'] == rank]['time'] * 1e6
    ax.plot(n,t,label="%d threads" % rank)
ax.text(0.40, 0.90, "m=%s,b=%s,cache=%s,\nalg=%s" %(str(ms),str(bs),str(cs), str(algs)),
        transform=ax.transAxes, fontsize=10, verticalalignment='top', bbox=props)
ax.legend()
ax.set_ylabel('us')
ax.set_xlabel('n')
plt.savefig("scaling4.tif")

#
# 5. Thread scaling at different N counts
#
ranks = range(10,130,10)
ms = [1000]
bs = [2]
cs = [False]
ns = range(100,1000,200)
algs = ['vector']
df = timings.time_mist(ms=ms, ns=ns, bs=bs, rs=ranks, cs=cs, q=200, algs=algs)
fig,ax = plt.subplots()
for n in ns:
    t = df[df['n'] == n]['time'] * 1e6
    ax.plot(ranks,t,label="n=%d" % n)
ax.text(0.20, 0.90, "m=%s,b=%s,cache=%s,\nalg=%s,q=200" %(str(ms),str(bs),str(cs), str(algs)), transform=ax.transAxes, fontsize=10, verticalalignment='top', bbox=props)
ax.legend()
ax.set_ylabel('us')
ax.set_xlabel('threads')
plt.savefig("scaling5.tif")

#
# 6. Thread scaling at different M counts
#
ranks = range(1,121,20) # salyut
ranks = range(1,13,1)   # galas155335
ms = range(100,2000,200)
bs = [2]
cs = [False]
ns = [300]
algs = ['vector']
df = timings.time_mist(ms=ms, ns=ns, bs=bs, rs=ranks, cs=cs, q=200, algs=algs)
fig,ax = plt.subplots()
for m in ms:
    t = df[df['m'] == m]['time'] * 1e6
    ax.plot(ranks,t,label="m=%d" % m)
ax.text(0.20, 0.90, "n=%s,b=%s,cache=%s,\nalg=%s,q=200" %(str(ns),str(bs),str(cs), str(algs)), transform=ax.transAxes, fontsize=10, verticalalignment='top', bbox=props)
ax.legend()
ax.set_ylabel('us')
ax.set_xlabel('threads')
plt.savefig("scaling6.tif")

#
# 7. Demonstrate Amdahl's Law
#
ranks = range(1,121,10)
ms = [1000]
bs = [2]
cs = [False]
ns = [300]
algs = ['vector']
df = timings.time_mist(ms=ms, ns=ns, bs=bs, rs=ranks, cs=cs, q=200, algs=algs)
fig,ax = plt.subplots()
t = df['time'] * 1e6
speedup = t[0] / t
ax.plot(ranks, speedup)
props = dict(boxstyle = 'round', facecolor = 'wheat', alpha = 0.5)
ax.text(0.20, 0.10, "m=%s,b=%s,n=%s,cache=%s\nalg=%s,q=200" %(str(ms),str(bs),str(ns), str(cs), str(algs)), transform=ax.transAxes, fontsize=10, verticalalignment='top', bbox=props)
ax.set_ylabel('speedup')
ax.set_xlabel('threads')
plt.savefig("scaling7.tif")
