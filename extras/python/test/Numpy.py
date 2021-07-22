import libmist as pld
import numpy as np
import numpy_helper as nh

filename = "sample_data.csv"

# TODO good tests
precision = 0.00001

def load_row_major_numpy(filename):
    x = np.genfromtxt(filename, delimiter=',', dtype='int32')
    #x = np.transpose(x)
    #x = np.copy(x, order='C')
    return x

def test_compute_in_file_out_numpy():
    mist = pld.Search()
    mist.load_file_column_major(filename)
    mist.threads = 4
    mist.tuple_size = 2
    res = mist.start()
    res2 = np.sort(res, axis=0)
    print(res2)
    assert(res.shape[0] == 45)
    assert(res.shape[1] == 3)
    assert(res2[0][0] == 0)
    assert(res2[0][1] == 1)
    assert(res2[0][2] == 0)
    assert(res2[-1][0] == 8)
    assert(res2[-1][1] == 9)
    assert(abs(res2[-1][2] - 0.72192809) < precision)

def test_compute_in_numpy_out_numpy():
    mist = pld.Search()
    x = nh.load_column_major_numpy(filename)
    mist.load_ndarray(x)
    mist.threads = 4
    mist.tuple_size = 2
    res = mist.start()
    res2 = np.sort(res, axis=0)
    print(res2)
    assert(res.shape[0] == 45)
    assert(res.shape[1] == 3)
    assert(res2[0][0] == 0)
    assert(res2[0][1] == 1)
    assert(res2[0][2] == 0)
    assert(res2[-1][0] == 8)
    assert(res2[-1][1] == 9)
    assert(abs(res2[-1][2] - 0.72192809) < precision)
