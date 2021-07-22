set(PYTHON_TESTS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/test)

macro(add_pytest testfile)
    set(name pytest_${testfile})
    add_test(NAME ${name} COMMAND ${PYTHON_EXECUTABLE} -m pytest ${testfile} WORKING_DIRECTORY ${PYTHON_TESTS_DIR})
    set_tests_properties(${name} PROPERTIES ENVIRONMENT PYTHONPATH=${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
endmacro()

add_pytest(library_load.py)
add_pytest(DataMatrix.py)
add_pytest(TupleSpace.py)
add_pytest(Numpy.py)
add_pytest(parallel.py)
