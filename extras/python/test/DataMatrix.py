import numpy as np
import filecmp

from mist import DataMatrix
from datetime import datetime
from os import remove as rm

# DataMatrix works on files written with each column a variable,
# but python loads files in row-major order. TODO include an array
# validation function in the library
def load_column_order_numpy(filename):
    x = np.genfromtxt(filename, delimiter=',', dtype='int32')
    x = np.transpose(x)
    x = np.copy(x, order='C')
    return x

def test_constructor_random():
    dm = DataMatrix(10, 5, 2)
    assert(dm.n == 10)
    assert(dm.m == 5)

def test_constructor_file():
    dm = DataMatrix("sample_data.csv")
    assert(dm.n == 10)
    assert(dm.m == 5)

def test_constructor_numpy():
    x = load_column_order_numpy("sample_data.csv")
    dm = DataMatrix(x)
    assert(dm.n == 10)
    assert(dm.m == 5)

def test_constructor_numpy_wrong_dtype():
    x = np.array([0,1,2,4], dtype="int64", order='C');
    try:
        dm = DataMatrix(x)
        assert(False)
    except:
        assert(True);

def test_constructor_numpy_wrong_memory_layout():
    x = np.array([0,1,2,4], dtype="int32", order='C');
    y = np.array([0,1,2,4], dtype="int32", order='F');
    try:
        dm = DataMatrix(x)
        assert(False)
    except:
        assert(True);
    try:
        dm = DataMatrix(y)
        assert(True)
    except:
        assert(False);

def test_write_file():
    inputfile = "sample_data.csv"
    testfile = "/tmp/test_write_file_" + datetime.now().strftime("%y%m%d-%H%M%S") + inputfile
    dm = DataMatrix(inputfile)
    dm.write_file(testfile)
    assert(filecmp.cmp(inputfile, testfile, shallow=False))
    rm(testfile)

def test_write_file_numpy():
    inputfile = "sample_data.csv"
    testfile = "/tmp/test_write_file_numpy_" + datetime.now().strftime("%y%m%d-%H%M%S") + inputfile
    x = load_column_order_numpy("sample_data.csv")
    dm = DataMatrix(x)
    dm.write_file(testfile)
    assert(filecmp.cmp(inputfile, testfile, shallow=False))
    rm(testfile)
