#!/usr/bin/env python3
import numpy as np
import pandas as pd
import libmist as lm
import time
import datetime

global search

# n variables
# m samples
# b bins
def matrix(n,m,b):
    return np.random.randint(0,b,size=(n,m)).astype('int8')

def run_times(search, n):
    search.start() # to prime caches...
    times = []
    for i in range(0,n):
        t1 = time.time()
        search.start()
        t2 = time.time()
        times = times + [t2 - t1]
    return np.average(times) / float(n)

def time_mist(q = 50,
              progress = False,
              rs = [1],
              ms = [10,30,60,90,120],
              ns = [10,20,30],
              bs = [2,3,4],
              ds = [2],
              cs = [False,True],
              algs = ['vector','bitset']):
    search = lm.Search()
    search.measure = "symmetricdelta"
    df = pd.DataFrame(columns=['n','d','m','b','r','q','cache','alg','time'])
    if (progress):
        print("%s" %(','.join(df.columns)))
    jobs = len(ns)*len(bs)*len(ms)*len(ds)*len(rs)*len(cs)*len(algs)
    i = 1
    for n in ns:
        for m in ms:
            for b in bs:
                data = matrix(n=n, m=m, b=b)
                search.load_ndarray(data)
                for d in ds:
                    ts = lm.TupleSpace()
                    vs = list(range(0,n))
                    ts.addVariableGroup("all", list(range(0,n)))
                    ts.addVariableGroupTuple([0]*d)
                    search.tuple_space = ts
                    for r in rs:
                        search.ranks = r
                        for alg in algs:
                            search.probability_algorithm = alg
                            for c in cs:
                                search.cache_enabled = c
                                t = run_times(search, q)
                                row = pd.DataFrame(
                                        {'n':[n],'m':[m],'d':[d],'b':[b],'r':[r],'q':[q],'alg':[alg],'cache':[c],'time':[t]}
                                        )
                                if (progress):
                                    print("[%02d/%02d : %s]\n%s" %(i,jobs,datetime.datetime.now(),str(row)))
                                df = df.append(row, ignore_index = True)
                                i = i + 1
    return df

