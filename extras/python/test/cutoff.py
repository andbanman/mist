import libmist as pld
import numpy as np
import pytest

def test_cutoff_mean():
    filename = "sample_data.csv"

    # all results
    mist = pld.Search()
    mist.load_file_column_major(filename)
    mist.tuple_size = 3
    res = mist.start()

    # cutoff
    cutoff = np.mean(res[:,-1])
    mist.cutoff = cutoff
    res2 = mist.start()
    assert(sum(res2[:,-1] < cutoff) == 0)
