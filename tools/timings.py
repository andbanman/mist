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
    return timeit.timeit("search.start()", setup="from __main__ import search", number=n) / float(n)

search = lm.Search()
search.cache_enabled = False
search.probability_algorithm = "bitset"
#search.probability_algorithm = "vector"
search.measure = "symmetricdelta"
search.cache_enabled = False
search.ranks = 1

print("%s,%s,%s,%s,%s,%s,%s" %('n','d','m','b','q','vector','bitset'))
#ms = [100, 1000, 5000, 10000, 15000, 20000, 250000]
ms = list(range(1000,10000,1000))
for m in ms:
    b = 6
    d = 3
    n = 20
    q = 50
    ts = lm.TupleSpace()
    vs = list(range(0,n))
    ts.addVariableGroup("all", list(range(0,n)))
    ts.addVariableGroupTuple([0]*d)
    search.tuple_space = ts
    T = ts.count_tuples()
    data = matrix(n=n, m=m, b=b)
    search.probability_algorithm = "bitset"
    tb = run_times(data, search, q) / float(T)
    search.probability_algorithm = "vector"
    tv = run_times(data, search, q) / float(T)
    print("%d,%d,%d,%d,%d,%f,%f" %(n,d,m,b,q,tv,tb))
