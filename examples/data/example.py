#!/usr/bin/env python3
import numpy as np
import libmist as lm
import matplotlib.pyplot as plt

# set the random seed
np.random.seed(42)

print("Generating sample data and results for Libmist v%s" % lm.__version__)

# params

def make_data(N,M,B):
	# generate the random data
	lowMI  = np.random.randint(low=0, high=B, size=(N//3,M), dtype='int8')
	highMI = np.random.randint(low=0, high=B, size=(N//3,M), dtype='int8')
	highD3 = np.random.randint(low=0, high=B, size=(N//3,M), dtype='int8')
	# create the low MI signal
	samples = np.random.choice(range(0,M), size=M//10, replace=False)
	lowMI[0:len(lowMI)//2,samples] = (lowMI[len(lowMI)//2:,samples] + 1) % B
	# create the high MI signal
	samples = np.random.choice(range(0,M), size=int(M//1.25), replace=False)
	highMI[0:len(highMI)//2,samples] = (highMI[len(highMI)//2:,samples] + 1) % B
	# create the D3 signal with XOR function
	samples = np.random.choice(range(0,M), size=int(M//1.25), replace=False)
	n = len(highD3)
	highD3[0:n//3,samples] = (highD3[n//3:2*n//3,samples] ^ highD3[2*n//3:n,samples]) % B
	data = np.concatenate((lowMI, highMI, highD3), axis=0)
	return data

def results_d2_mat(results, N):
	results_mat = np.zeros((N,N))
	for i in range(0,N):
	    for j in range(i+1,N):
	        ind = np.where((results[:,0] == i) & (results[:,1] == j))
	        results_mat[i,j] = results[ind][0][2]
	return results_mat

N=90
M=1000
B=4
data = make_data(N=N,M=M,B=B)

# full MI
search = lm.Search()
search.load_ndarray(data)
results_mi = search.start()
results_mat = results_d2_mat(results_mi, N)
plt.title("Mutual Information Heatmap")
hm=plt.imshow(results_mat, cmap='Blues',interpolation="nearest")
plt.colorbar(hm)
#plt.show()
plt.savefig("mi.png")
plt.close()

# limited MI
ts = lm.TupleSpace()
ts.addVariableGroup("lowMI", list(range(0,N//3)))
ts.addVariableGroupTuple([0,0])
search = lm.Search()
search.load_ndarray(data)
search.tuple_space = ts
results_mi = search.start()
results_mat = results_d2_mat(results_mi, N//3)
plt.title("Mutual Information Heatmap - limited variables")
hm=plt.imshow(results_mat, cmap='Blues',interpolation="nearest")
plt.colorbar(hm)
#plt.show()
plt.savefig("mi_limited.png")
plt.close()

# d3
search = lm.Search()
search.load_ndarray(data)
ts = lm.TupleSpace()
ts.addVariableGroup("all", list(range(0,N)))
ts.addVariableGroupTuple([0,0,0])
search.tuple_space = ts
results_d3 = search.start()
d3_signal = results_d3[np.where(results_d3[:,3] > 1)]
print("\nHistogram of D3 results:")
print(np.histogram(results_d3[:,3], bins=[0,1,2]))
print("\nD3 signal >=1:")
print(d3_signal)
np.savetxt("d3_signal.csv", d3_signal, delimiter=",", fmt="%d,%d,%d,%f")
