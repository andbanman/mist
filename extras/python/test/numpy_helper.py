import numpy as np
def load_column_major_numpy(filename):
    x = np.genfromtxt(filename, delimiter=',', dtype='int8')
    x = np.copy(x, order='F')
    return x
def load_row_major_numpy(filename):
    x = np.genfromtxt(filename, delimiter=',', dtype='int8')
    return x
