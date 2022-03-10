#!/usr/bin/env python3
import numpy as np
import pandas ad pd
import libmist as lm import timeit

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

# parameters
q = 50                                  # number of samples per timing
rs = [1]                                # number of threads
ms = [100,200,300]                      # samples
ns = [10,20,30]                         # variables
bs = [3,4]
ds = [2]

# header
print("%s,%s,%s,%s,%s,%s,%s" %('n','d','m','b','q','vector','bitset'))

for n in ns:
    for m in ms:
        for b in bs:
            data = matrix(n=n, m=m, b=b)
            for d in ds:
                ts = lm.TupleSpace()
                vs = list(range(0,n))
                ts.addVariableGroup("all", list(range(0,n)))
                ts.addVariableGroupTuple([0]*d)
                search.tuple_space = ts
                for r in rs:
                    search.ranks = r
                    search.probability_algorithm = "bitset"
                    tb = run_times(data, search, q)
                    search.probability_algorithm = "vector"
                    tv = run_times(data, search, q)
                    print("%d,%d,%d,%d,%d,%f,%f" %(n,d,m,b,q,tv,tb))
