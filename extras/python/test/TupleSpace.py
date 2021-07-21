import libmist as lm
import numpy as np
import pytest

filename = "sample_data.csv"

def test_count_tuples_default():
    ts = lm.TupleSpace(100,2)
    assert(ts.count_tuples() == 4950)
    assert(lm.TupleSpace(10,1).count_tuples() == 10)
    assert(lm.TupleSpace(333,3).count_tuples() == 6099006)

def test_count_tuples_custom():
    ts = lm.TupleSpace()
    ts.addVariableGroup("A", list(range(0,10)))
    ts.addVariableGroup("B", [10])
    ts.addVariableGroupTuple(["A","B"])
    assert(ts.count_tuples() == 10)

    ts = lm.TupleSpace()
    ts.addVariableGroup("A", list(range(0,10)))
    ts.addVariableGroup("B", list(range(10,12)))
    ts.addVariableGroup("C", [12])
    ts.addVariableGroupTuple(["A","B","C"])
    assert(ts.count_tuples() == 20)

    ts = lm.TupleSpace()
    ts.addVariableGroup("A", list(range(0,100)))
    ts.addVariableGroup("B", [200])
    ts.addVariableGroupTuple(["A","B"])
    ts.addVariableGroupTuple(["A","A"])
    assert(ts.count_tuples() == (100 + 4950))

#TODO more test for exceptions, etc

def test_constructor_exception():
    with pytest.raises(Exception) as e_info:
        ts = lm.TupleSpace(10,0)
