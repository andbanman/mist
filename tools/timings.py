#!/usr/bin/env python3
import numpy as np
import libmist as lm
import timeit

global search

# n variables
# m samples
# b bins
def matrix(n,m,b):
    return np.random.randint(0,b,size=(n,m)).astype('int8')

def run_times(data, search, n):
    search.load_ndarray(data)
    search.start() # to prime caches...
    return timeit.timeit("search.start()", setup="from __main__ import search", number=n)


search = lm.Search()
search.cache_enabled = False
search.probability_algorithm = "vector"
search.measure = "symmetricdelta"

for m in range(100, 1100, 100):
    n = 100
    b = 2
    n = 100
    q = 1000
    data = matrix(n=n, m=m, b=b)
    t = run_times(data, search, q) / float(q)
    print("%d,%d,%d,%f,q" %(n,m,b,t,q))
