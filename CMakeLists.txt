cmake_minimum_required(VERSION 3.5)

project("mist")

option(BuildDoc "Build dynamic documentation" OFF)
option(BuildTest "Build unit tests (default ON)" OFF)
option(BuildPython "Build python extension library" OFF)
option(BuildStatic "Build static binaries" OFF)
option(InstallExtern "Install third party libraries" ON)
set(PythonMinVersion "3.6" CACHE STRING "Minimum Python Version to build against")

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/extras/cmake" ${CMAKE_MODULE_PATH})

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    CACHE PATH "Output directory for static libraries.")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    CACHE PATH "Output directory for shared libraries.")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    CACHE PATH "Output directory for executables and DLL's.")

set(public_include_path "${PROJECT_SOURCE_DIR}/include")

if (${BuildStatic})
    if (${BuildPython})
        message(FATAL_ERROR "ERROR: BuildPython: cannot build Python library statically (unset BuildStatic)")
        return()
    endif()
    if (${InstallExtern})
        message(WARNING "BuildStatic: no need to install third party libraries for static build, continuing without InstallExtern")
        set(InstallExtern OFF)
    endif()
    set(Boost_USE_STATIC_LIBS ON)
    set(CMAKE_EXE_LINKER_FLAGS "-static")
endif()

if (${InstallExtern})
    set(CMAKE_BUILD_RPATH_USE_ORIGIN ON)
    include(ResolveSymlinks)
endif()

find_package(Boost REQUIRED COMPONENTS
    serialization
)
list(APPEND extern_libraries ${Boost_LIBRARIES})

# TODO Boost Min Version 1.58.0
set(BOOST_VERSION_STRING "${Boost_MAJOR_VERSION}.${Boost_MINOR_VERSION}.${Boost_SUBMINOR_VERSION}")

if(${BuildTest})
    find_package(Boost REQUIRED COMPONENTS
        unit_test_framework
        filesystem
    )
    enable_testing()
endif()

if(${BuildPython})
    # Find Python libraries, use default version of python if PythonMinVersion is not set
    find_package(PythonInterp ${PythonMinVersion} REQUIRED)
    find_package(PythonLibs ${PythonMinVersion} REQUIRED)
    list(APPEND extern_libraries ${PYTHON_LIBRARIES})
    find_package(NumPy ${PythonMinVersion} REQUIRED)
    set(PYTHON_VERSION ${PYTHON_VERSION_MAJOR}.${PYTHON_VERSION_MINOR} CACHE INTERNAL "")
    set(PYTHON_PACKAGE python${PYTHON_VERSION}/${PROJECT_NAME} CACHE INTERNAL "")
    set(INSTALL_LIB_PYTHON_PATH lib/${PYTHON_PACKAGE} CACHE INTERNAL "")

    # Different Boost.Python component naming conventions. See FindBoost documentation
    if (BOOST_VERSION_STRING VERSION_EQUAL "1.67.0" OR BOOST_VERSION_STRING VERSION_GREATER "1.67.0")
        set(BOOST_PYTHON_VERSION_SUFFIX "${PYTHON_VERSION_MAJOR}${PYTHON_VERSION_MINOR}" CACHE INTERNAL "")
    else()
        if (${PYTHON_VERSION_MAJOR} STREQUAL "3")
            set(BOOST_PYTHON_VERSION_SUFFIX "3" CACHE INTERNAL "")
        else()
            set(BOOST_PYTHON_VERSION_SUFFIX "" CACHE INTERNAL "")
        endif()
    endif()

    # Find Boost.Python Library
    find_package(Boost COMPONENTS REQUIRED python${BOOST_PYTHON_VERSION_SUFFIX})
    list(APPEND extern_libraries ${Boost_LIBRARIES})
    set(BOOST_PYTHON_LIBRARIES Boost::python${BOOST_PYTHON_VERSION_SUFFIX})

    # Find Numpy Library
    if (BOOST_VERSION_STRING VERSION_LESS "1.63.0")
        # Pre Numpy-Boost integration, see https://github.com/ndarray/Boost.NumPy
        set(BOOST_NUMPY_LIBRARIES boost_numpy)
        #list(APPEND extern_libraries ) # TODO path to boost_numpy
    else()
        find_package(Boost COMPONENTS REQUIRED numpy${BOOST_PYTHON_VERSION_SUFFIX})
        list(APPEND extern_libraries ${Boost_LIBRARIES})
        set(BOOST_NUMPY_LIBRARIES Boost::numpy${BOOST_PYTHON_VERSION_SUFFIX})
    endif()
endif()

add_subdirectory("src")
if(NOT ${BuildPython})
    # Cannot build examples against Python library
    add_subdirectory("examples")
endif()

if(${BuildDoc})
    add_subdirectory("extras/dynamicDocs")
endif()
if(${BuildPython})
    add_subdirectory("extras/python")
endif()
