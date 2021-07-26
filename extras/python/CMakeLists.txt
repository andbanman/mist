set(PYTHON_TESTS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/test)

macro(add_pytest testfile)
    set(name pytest_${testfile})
    add_test(NAME ${name} COMMAND ${PYTHON_EXECUTABLE} -m pytest ${testfile} WORKING_DIRECTORY ${PYTHON_TESTS_DIR})
    set_tests_properties(${name} PROPERTIES ENVIRONMENT PYTHONPATH=${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
endmacro()

if (${BuildPyPi})
    set(tmp_install /tmp/.${PROJECT_NAME}-pypi-tmp-install)
    set(pypi_src_dir ${CMAKE_CURRENT_SOURCE_DIR}/package)
    set(pypi_build_dir ${CMAKE_CURRENT_BINARY_DIR}/package)
    add_custom_target(PyPi
        COMMAND rm -rf ${pypi_build_dir} ${tmp_install}
        COMMAND DESTDIR=${tmp_install} make -C ${CMAKE_BINARY_DIR} install
        COMMAND mkdir -p ${pypi_build_dir}
        COMMAND cp -R ${pypi_src_dir}/* ${pypi_build_dir}/
        COMMAND cp ${CMAKE_SOURCE_DIR}/README.rst ${pypi_src_dir}/
        COMMAND cp -R ${tmp_install}/usr/local/lib/python*/${PROJECT_NAME}/*
                      ${pypi_build_dir}/src/libmist/
        COMMAND cd ${pypi_build_dir} && ./build.sh
        COMMAND rm -rf ${tmp_install}
    )
endif()

add_pytest(library_load.py)
add_pytest(DataMatrix.py)
add_pytest(TupleSpace.py)
add_pytest(Numpy.py)
add_pytest(parallel.py)
add_pytest(cutoff.py)
