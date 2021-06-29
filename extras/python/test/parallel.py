import libmist as pld
import numpy as np
import pytest
from multiprocessing import cpu_count

def test_parallel_2():
    filename = "sample_data.csv"
    cpus = cpu_count()
    total_ranks = cpus * 2
    ranks_per = cpus

    # non-parallel
    mist = pld.Search()
    mist.load_file(filename)
    mist.tuple_size = 3
    res = mist.start()

    # first ranks
    mist = pld.Search()
    mist.load_file(filename)
    mist.tuple_size = 3
    mist.ranks = ranks_per
    mist.total_ranks = total_ranks
    res1 = mist.start()

    # second ranks
    mist = pld.Search()
    mist.load_file(filename)
    mist.tuple_size = 3
    mist.ranks = ranks_per
    mist.total_ranks = total_ranks
    mist.start_rank = ranks_per
    res2 = mist.start()

    res_sort = np.sort(res, axis=0)
    res_parallel = np.concatenate([res1, res2])
    res_parallel_sort = np.sort(res_parallel, axis=0)
    assert(res.shape[0] == res_parallel_sort.shape[0])
    assert(res.shape[1] == res_parallel_sort.shape[1])
    np.testing.assert_array_equal(res_sort, res_parallel_sort)

def test_parallel_3_same_obj():
    filename = "sample_data.csv"
    cpus = cpu_count()
    total_ranks = cpus * 3
    ranks_per = cpus

    # non-parallel
    mist = pld.Search()
    mist.load_file(filename)
    mist.tuple_size = 3
    res = mist.start()

    # first ranks
    mist.ranks = ranks_per
    mist.total_ranks = total_ranks
    res1 = mist.start()

    # second ranks
    mist.ranks = ranks_per
    mist.total_ranks = total_ranks
    mist.start_rank = ranks_per
    res2 = mist.start()

    # third ranks
    mist.ranks = ranks_per
    mist.total_ranks = total_ranks
    mist.start_rank = ranks_per * 2
    res3 = mist.start()

    res_sort = np.sort(res, axis=0)
    res12 = np.concatenate([res1, res2])
    res_parallel = np.concatenate([res12, res3])
    res_parallel_sort = np.sort(res_parallel, axis=0)

    assert(res.shape[0] == res_parallel_sort.shape[0])
    assert(res.shape[1] == res_parallel_sort.shape[1])
    np.testing.assert_array_equal(res_sort, res_parallel_sort)
