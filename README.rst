Mist
====

MIST is a **M**\ ultivariable **I**\ nformation Theory-based dependence **S**\ earch **T**\ ool. The Mist library API computes entropy-based measures that detect functional dependencies between variables. Mist provides the **libmist** library and **mistcli** Linux command line tool.

- Mist source is hosted on `Github <https://github.com/andbanman/mist/>`_.
- Mist for Python is available on `PyPi <https://pypi.org/project/libmist/>`_.
- Mist documentation is hosted on `ReadTheDocs <https://libsearch.readthedocs.io>`_.

Background
----------

A biological system is intrinsically complex and can be viewed as a large set of components, variables, and attributes that store and transmit information from one another. This information depends on how each component interacts with, and is related to, other components of the system. Handling the problem of representing and measuring the information is the goal of Mist.

A central question of this problem is: How can we fully describe the joint probability density of the *N* variables that define the system? Characterization of the joint probability distribution is at the heart of describing the mathematical dependency among the variables. Mist provides a number of tools that are useful in the pursuit for the description and quantitation of dependences in complex biological systems.

A function between variables defines a deterministic relationship between them, a dependency; it can be as simple as *if X then Y* or something more complicated involving many variables. Thus, a functional dependency among variables implies the existence of a function. See [Galas2014]_. Here we focus on the task of finding a functional dependency without concerning ourselves with the nature of the underlying function.

Mist is designed to quickly find functional dependencies among many variables without knowing the exact function. It uses model-free Information Theory measures based on entropy to compute the strength of the dependence. Mist allows us to detect functional dependencies for any function, involving any number of variables, limited only by processing capabilities and statistical power. This makes Mist a great tool for paring down a large set of variables into an interesting subset of dependencies, which may then be studied by other methods. This may be seen as compression of data by identifying redundant variables.

Quick Start
-----------

The easiest way to run Mist is by using the **libmist** Python module. The following minimal example sets up an exhaustive search for dependencies between two variables, estimated with the default measurement

::

    import libmist
    search = libsearch.Search()
    search.load_file('/path/to/data.csv')
    search.outfile = '/dev/stdout'
    search.start()

There are numerous functions to configure Mist -- below are some of the most important. For a full list see `Mist documentation <api.html#_CPPv2N4mist4MistE>`_ and consult the `User Guide <userguide.html>`_.

::

    search.load_ndarray()     # load data from a Python.Numpy.ndarray (see docs for restrictions)
    search.tuple_size         # set the number of variables in each tuple
    search.measure            # set the Information Theory Measure
    search.threads            # set the number of computration threads

This Python syntax is virtually identical to the C++ code you would write for a program using the Mist library, as you can see in the examples directory.

Installation
------------

Docker
^^^^^^

Mist can be built into a Docker image with the included docker file

::

  cd /path/to/mist
  docker image build . -t mist
  docker run --rm -v ./:/mist mist

The default command builds the Mist python module, which can then be run in an interactive session or with python script, e.g.

::

  docker run --it --rm -v ./:/mist mist python3

mist
^^^^

These packages are required to build libmist and mistcli:

- CMake (minimum version 3.5)
- Boost (minimum version 1.58.0)

Run *cmake* in out-of-tree build directory:

::

    mkdir /path/to/build
    cd /path/to/build
    cmake /path/to/mist
    make install


mist python library
^^^^^^^^^^^^^^^^^^^

Use pip package manager to install libmist:

::

    pip install libmist


Or build and install from source.

Additional build requirements:

- Python development packages (python3-dev or python-dev).
- Boost Python and Numpy components. For Boost newer than 1.63 use the integrated Boost.Numpy (libboost-numpy) package. For earlier versions install `ndarray/Boost.Numpy <https://github.com/ndarray/Boost.NumPy>`_.

Run *cmake* with *BuildPython* set to *ON*:

::

    mkdir /path/to/build
    cd /path/to/build
    cmake -DBuildPython:BOOL=ON /path/to/mist
    make install

Note: both the mist and ndarray/Boost.numpy builds use the default python version installed on the system. To use a different python version, change the FindPythonInterp, FindPythonLibs, and FindNumpy invocations in both packages to use the same python version.

Documentation (optional)
^^^^^^^^^^^^^^^^^^^^^^^^

Additional Requirements

- `Doxygen <http://www.doxygen.nl/download.html>`_
- `Sphinx <https://www.sphinx-doc.org/en/master/usage/installation.html>`_
- `Breathe <https://pypi.org/project/breathe/>`_
- `sphinx_rtd_theme <https://github.com/rtfd/sphinx_rtd_theme>`_


Run *cmake* with *BuildDoc* set to *ON*:

::

    mkdir /path/to/build
    cd /path/to/build
    cmake -DBuildDoc:BOOL=ON /path/to/mist
    make Sphinx

And then run the build as above.

For Developers
--------------

This project follows the `Pitchfork Layout <https://github.com/vector-of-bool/pitchfork>`_.  Namespaces are encapsulated in separate directories. Any physical unit must only include headers within its namespace, the root namespace (core), or interface headers in other namespaces.  The build system discourages violations by making it awkward to link objects across namespaces.

Documentation for this project is dynamically generated with Doxygen and Sphinx. Comments in the source following Javadoc style are included in the docs. Non-documented comments, e.g. implementation notes, developer advice, etc. follow standard C++ comment style.

Credits
-------

Mist is written by Andrew Banman. It is based on software written by Nikita Sakhanenko. The ideas behind entropy-based functional dependency come from Information Theory research by David Galas, Nikita Sakhanenko, and James Kunert.

For copyright information see the LICENSE.txt file included with the source.

References
----------

.. [Galas2014] Galas, David J et al. “Describing the complexity of systems: multivariable "set complexity" and the information basis of systems biology.” Journal of computational biology : a journal of computational molecular cell biology vol. 21,2 (2014): 118-40. doi:10.1089/cmb.2013.0039 `PMC <https://www.ncbi.nlm.nih.gov/pmc/articles/PMC3904535/>`_

.. [Shannon1949] Shannon, Claude Elwood, and Warren Weaver. The Mathematical Theory of Communicaton. University of Illinois Press, 1949.
