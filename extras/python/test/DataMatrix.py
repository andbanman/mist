import numpy as np
import filecmp
import pytest
import numpy_helper as nh

from libmist import DataMatrix
from datetime import datetime
from os import remove as rm

def test_constructor_random():
    dm = DataMatrix(10, 5, 2)
    assert(dm.n == 10)
    assert(dm.m == 5)

def test_constructor_file_rowmajor():
    dm = DataMatrix("sample_data.csv")
    assert(dm.n == 5)
    assert(dm.m == 10)

    dm = DataMatrix("sample_data.csv", True)
    assert(dm.n == 5)
    assert(dm.m == 10)

def test_constructor_file_colmajor():
    dm = DataMatrix("sample_data.csv", False)
    assert(dm.n == 10)
    assert(dm.m == 5)

def test_constructor_numpy_rowmajor():
    x = nh.load_row_major_numpy("sample_data.csv")
    dm = DataMatrix(x)
    assert(dm.n == 5)
    assert(dm.m == 10)

def test_constructor_numpy_colmajor():
    x = nh.load_column_major_numpy("sample_data.csv")
    dm = DataMatrix(x)
    assert(dm.n == 10)
    assert(dm.m == 5)

def test_constructor_numpy_wrong_dtype():
    x = np.array([[0,1],[2,4]], dtype="int64", order='C');
    print(x.flags)
    with pytest.raises(Exception) as exception:
        dm = DataMatrix(x)

#def test_constructor_numpy_wrong_memory_layout():
#    c = np.array([[0,1],[2,4]], dtype="int32", order='C');
#    f = np.array([[0,1],[2,4]], dtype="int32", order='F');
#    print(f.flags)
#    print(c.flags)
#    with pytest.raises(Exception) as exception:
#        dm = DataMatrix(f)
#    dm = DataMatrix(c)
#    assert(True)

def test_write_file():
    inputfile = "sample_data.csv"
    testfile = "/tmp/test_write_file_" + datetime.now().strftime("%y%m%d-%H%M%S") + inputfile
    dm = DataMatrix(inputfile)
    dm.write_file(testfile)
    assert(filecmp.cmp(inputfile, testfile, shallow=False))
    rm(testfile)

def test_write_file_numpy_rowmajor():
    inputfile = "sample_data.csv"
    testfile = "/tmp/test_write_file_numpy_" + datetime.now().strftime("%y%m%d-%H%M%S") + inputfile
    x = nh.load_row_major_numpy("sample_data.csv")
    dm = DataMatrix(x)
    dm.write_file(testfile)
    assert(filecmp.cmp(inputfile, testfile, shallow=False))
    rm(testfile)
