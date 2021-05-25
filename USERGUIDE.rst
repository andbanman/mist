User Guide
==========

This guide explains how to prepare data for Mist, set up and execute a search, and tune the algorithm for best performance. The basic steps are:

1. Prepare the data
2. Select an appropriate IT Measure
3. Define the search space
4. Compute

Input data is represented by a matrix, where each column is a *variable* and each row is a measurement or subject. A *variable tuple* is a small combination of variables. Mist estimates the strength of the dependence between variables in the tuple, called the *signal*, by computing the *Information Theory measure*. The set of all variable tuples is the *search space*.

The procedure of defining a search space and computing the IT measure for each tuple in the space is simply called a *search*. Mist uses a parallel algorithm to divide the search among computing ranks. Each rank is a computing thread, and ranks can be placed on multiple nodes in a system.
Run modes
---------

There are three ways to run Mist. They all use the same `Mist C++ API <api.html>`_.

Python Module
^^^^^^^^^^^^^

The libmist Python module is the recommended way to run Mist searches. All of the examples in this guide use the Python module.

1. Download `libmist on PyPi <https://pypi.org/project/libmist/>`_ to use the python module.
2. Import the libmist module.

::

  import libmist

All of the classes needed to execute searches are extended to the Python module. For custom applications that need the full API, use the C++ library directly.


CLI Tool
^^^^^^^^

Mist provides a statically-compiled Linux command line tool called **mistcli**. It includes enough Mist features to run basic searches. This tool is a good option if Python is unavailable or the system is too old to run with standard libraries.

1. Download mistcli from the `release page <https://github.com/andbanman/mist/releases>`_.
2. Run on recent Linux system

Consult the help output for mistcli for instructions.

::

    mistcli -h

C++ Library
^^^^^^^^^^^

Application developers can develop new programs with the Mist API in C++. The mistcli program is the reference example.

1. Download libmist from the `release page <https://github.com/andbanman/mist/releases>`_ or compile from `source <https://github.com/andbanman/mist>`_.
2. Install development headers
3. Link program against libmist

A good procedure is to compile and install from source:

::

    git clone https://github.com/andbanman/mist
    mkdir mist/build
    cd mist/build
    cmake ../
    make install

And then add the appropriate linker flags, e.g.

::

    -std=c++14 -lmist

Prepare the Data
----------------

Data should be prepared to meet these requirements:

- Arranged as *NxM* matrix, with each column a variable.
- Values for each variable are non-negative integers on a contiguous range starting at 0, e.g. *[0,1,2,...n]*. Variables can have different value ranges.
- Continuous variables discretized as above.
- Missing values represented by a negative integer.

Mist doesn't massage the input data to fit the requirements because we don't wish to make any invisible changes to the data. The program will fail with an exception if these requirements aren't met.

The data can be loaded from a CSV file ...

::

    import libmist
    search = libsearch.Mist()
    search.load_file('/path/to/data.csv')

... or a Python `Numpy ndarray <https://numpy.org/doc/stable/reference/generated/numpy.ndarray.html>`_.

::

    x = numpy.ndarray(...)
    search.load_ndarray(x)

Mist does not copy the input ndarray. As such the the array must have signed integer data type and C-style memory layout. If either is wrong the program will throw an error.


Missing values
^^^^^^^^^^^^^^

Mist throws away rows that have a missing value in any of the variables of a tuple. Thus the *effective* sample size of the tuple is less than or equal to each Variable's sample size. The effective sample size may vary widely depending on the missing values pattern. You should check the effective sample size when validating the results for a tuple.

For example, you mave have a missing value rate of about 50% for each variable, but the effective sample size for a pair of variables may be much smaller than the others.

::

    Variable Tuple   Missing or Present?    Effective Sample Size
    (V0)             *****-----             5
    (V1)             ----******             6
    (V2)             ----******             6
    (V0,V1)          ----*-----             1
    (V0,V2)          ----*-----             1
    (V1,V2)          ----******             6


In this contrived example, pairs involving *V0* have a much smaller sample size because its missing value pattern is opposite that of the other variables. A similar situation can arise in real data, say when one variable systematically missed one half the population while another variable systematically missed the other half.

Mist computes joint entropy estimations that are sensitive to sample size. If the effective sample size is very low, the estimate can be much larger or smaller than the true entropy. Joint entropy estimations are also saved in memory to be reused in higher-order calculations, such as Symmetric Delta. The different effective sample sizes of the subcalculations and the final calculation can cause strange results, e.g. a sign change for Symmetric Delta.

Check the effective sample size of any tuples with interesting signals.

Information Theory Measures
---------------------------

Select the measure you want to compute with `Mist::set_measure <api.html#_CPPv2N4mist4Mist11set_measureERNSt6stringE>`_. The appropriate measure depends on the data and what question you are trying to answer.

::

    import libmist
    search = libsearch.Mist()
    search.measure = "SymmetricDelta"


Joint Entropy
^^^^^^^^^^^^^

An estimate of the joint entropy of two or more variables, computed using the naive approach [Shannon1949]_.

Symmetric Delta
^^^^^^^^^^^^^^^

A novel symmetric measure of functional dependence constructed from joint entropies [Galas2014]_. Values are always reported as positive real numbers, with larger values indicating stronger signal. Missing values may cause a sign change for low-signal tuples, but these can be ignored.


Define the search Space
-----------------------

Mist computes the IT Measure for each tuple in the search space.

Default (Exhaustive) search space
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The default search space is the set of all variable tuples. For *N* variables and tuples size *T*, the default space contains *N choose T* tuples. This space is called "exhaustive" or "complete" because it contains all possible unique tuples for a set a variables.

Set the size of tuples in the default space with `Mist::set_tuple_size <api.html#_CPPv2N4mist4Mist14set_tuple_sizeEi>`_.

::

  search.tuple_size = 3

Beware the size of the exhaustive space: a very large number of variables and tuple size leads to combinatorial explosion, e.g. 5000 variables in 3-tuples is over 20 billion tuples!


Custom (Limited) search space
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

There are many search problems where you don't need to compute all possible tuples. Perhaps you're only interested in functional relationships involving a specific variable, and so you'd like to skip tuples that don't include it.

You can define a smaller search space using the `TupleSpace <api.html#_CPPv2N4mist9algorithm10TupleSpaceE>`_ class. A tuple space is made by defining groups of variables, and then specifying how variables from each group should combine to form the tuples. Follow these steps:

**1. Create a TupleSpace object**

::

    import libmist
    ts = libsearch.TupleSpace()

**2. Define Variable Groups**

A *Variable Group* is simply a named set of variables. Variables are referenced by their column position, *[0,N-1]*. Add a group with `TupleSpace::addVariableGroup <api.html#_CPPv2N4mist9algorithm10TupleSpace21addVariableGroupTupleERNSt6vectorINSt6stringEEE>`_. Variable groups are usually disjoint, but they do not need to be ordered or contiguous.

::

    ts.addVariableGroup("group1", [0,1,2,9])
    ts.addVariableGroup("group2", [4])

The size of variable groups may impact performance, see `below <userguide.html#tuple-completion-vs-batch-algorithm>`_.

**3. Define Variable Group Tuples**

A *Variable Group Tuple* is a set of Variable Groups that define the tuples in the search space. Add a group tuple with `TupleSpace::addVariableGroupTuple <api.html#_CPPv2N4mist9algorithm10TupleSpace21addVariableGroupTupleER10tuple_type>`_.

The group tuple is the blueprint for the variable tuples. An algorithm generates variable tuples by replacing the group name with variables from that group. Through iteration it generates all variables tuples. E.g. for variable groups *A=[a1,a2,...,aN]* and *B=[b1,b2,...,bM]*, the group tuple *[A,B]* would generate variable tuples *[a1,b1]*, *[a1,b2]*, and so on through *[aN,bM]*.

The algorithm becomes clear through example:

::

    ts.addVariableGroupTuple(["group1", "group2"])
    # this group tuple generates variable tuples:
    # 0,4
    # 1,4
    # 2,4
    # 4,9

You can list a variable group any number of times, in any order:

::

    ts.addVariableGroupTuple(["group1", "group2", "group1"])
    # this group tuple generates variable tuples:
    # 0,1,4
    # 0,2,4
    # 0,3,4
    # 1,2,4
    # 1,4,9
    # 2,4,9


**4. Set the TupleSpace**

Finally, load the TupleSpace object to set the tuple space. Now when you run the computation only the desired tuples will be included.

::

    search.tuple_space = ts

Note: tuple_space and tuple_size parameters are mutually exclusive. The tuple_space parameter takes precedence.

Genetics Example
****************

Consider a more realistic example in genetics. There may be a single phenotype of interest and 5000 genotypes that might be related. You may only be interested in finding functional dependencies between the genotypes the single phenotype; in which case we should exclude tuples containing only genotypes. Suppose our phenotype variable is in the first position with all other variables genotypes, we might use

::

    ts.addVariableGroup("phenotype", [0])
    ts.addVariableGroup("genotypes", list(range(1, 5000)))
    ts.addVariableGroupTuple(["genotypes", "phenotype"])
    search.tuple_space = ts

This custom search space reduces the size from roughly 20 billion tuples to 12.5 million.

Compute
-------

Before starting the computation you should configure the output file with `Mist::set_outfile <api.html#_CPPv2N4mist4Mist11set_outfileERNSt6stringE>`_. For small search spaces this could be the stdout stream, but more often you will pick file destination.

::

    search.outfile = "/dev/stdout"

Finally run the computation.

::

    search.start()

This may take anywhere from seconds to days depending on the size of the space. It's a good idea to start small to get an idea of the runtime. Start with tuples size 2 and variables less than 1000 and increase.

Performance Tuning
------------------

The most important factors affecting the overall runtime of a search are the size of the search space and the number of threads. We already covered how to narrow the search space above. Set the number of threads with `Mist::set_threads <api.html#_CPPv2N4mist4Mist11set_threadsEi>`_.

::

    search.threads = 10

Advanced
^^^^^^^^

The following are more fine-tuned options that should be considered for advanced uses.

Probability Distribution Algorithms
***********************************

Counting probability distributions is the most time-consuming part of computing an IT Measure. See `Mist::set_probability_algorithm <api.html#_CPPv2N4mist4Mist25set_probability_algorithmERNSt6stringE>`_ for a list of available algorithms.

For very "tall" data (many rows for each variable) we can speed up the algorithm by casting each variable as series of bitsets, rather than the typical vector representation. This allows faster entropy calculation at the cost of some memory and computation overhead. This option is not advantageous for "short" data, and disasterous if variables have a large number of value bins.

It's worth experimenting with this options if your variable have three or fewer bins, and/or your variables have thousands or ten's of thousands of rows.
