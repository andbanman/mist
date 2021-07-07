User Guide
==========

This guide explains how to prepare data for Mist, set up and execute a search, and tune the algorithm for best performance. The basic steps are:

1. Prepare the data
2. Select an appropriate IT Measure
3. Define the search space
4. Compute

Here we assume that we have a set of variables representing the components of a system we study and a set of samples representing multiple measurements of these variables. So, input data is represented by a matrix, where each column is a variable and each row is a sample, e.g., a measurement or subject. A variable tuple is a small combination of variables. The set of all variable tuples is the search space. Mist efficiently traverses the search space and computes the Information Theory measure for each tuple, which in turn allows us to estimate the strength of the dependence among variables in the tuple.

The procedure of defining a search space and computing the IT measure for each tuple in the space is simply called a search. Mist uses a parallel algorithm to divide the search among computing threads. The algorithm can be tuned to improve performance for different kinds of searches.

Run modes
---------

There are three ways to run Mist. They all use the same `Mist C++ library <api.html>`_.

Python Module
^^^^^^^^^^^^^

The libmist Python module is the recommended way to run Mist searches. All of the examples in this guide use the Python module.

1. Download `libmist on PyPi <https://pypi.org/project/libmist/>`_ to use the python module.
2. Import the libmist module.

::

  import libmist

All of the classes needed to execute searches are extended to the Python module. For custom applications that need the full API, use the C++ library directly.


mistcli
^^^^^^^

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

    -std=c++14 -llibmist.so

Prepare the Data
----------------

Data should be prepared to meet these requirements:

- Arranged as *NxM* matrix, where each column is a variable and each row is a sample.
- Continuous variables discretized into non-negative integer bins (for best performance, bins should be contiguous and start at 0).
- Missing values represented by a negative integer.

The program fails with an exception if a requirement is not met.

Mist does not modify the input data to fit the requirements. We don’t wish to make any invisible changes to the data that could a) inadvertently introduce bias into the data, or b) make it difficult to reproduce or validate results outside Mist.

The data can be loaded from a CSV file ...

::

    import libmist
    search = libmist.Mist()
    search.load_file('/path/to/data.csv')

... or a Python `Numpy ndarray <https://numpy.org/doc/stable/reference/generated/numpy.ndarray.html>`_.

::

    x = numpy.ndarray(...)
    search.load_ndarray(x)

Mist does not copy the input ndarray. As such the the array must have signed integer data type and C-style memory layout. If either is wrong the program will throw an error.


Missing values
^^^^^^^^^^^^^^

During the computation of an Information measure of a tuple, Mist omits rows (samples) that have a missing value in any of the variables of the tuple. Thus, the effective sample size of the tuple used to calculate the Information measure is less than or equal to the sample size of each variable. The effective sample size may vary widely depending on the missing values pattern.

For example, you mave have a missing value rate of about 50% for each variable, but the effective sample size for a pair of variables may be much smaller than the others.

::

    Variable Tuple   Missing or Present?    Effective Sample Size
    (V0)             *****-----             5
    (V1)             ----******             6
    (V2)             ----******             6
    (V0,V1)          ----*-----             1
    (V0,V2)          ----*-----             1
    (V1,V2)          ----******             6

In this contrived example, pairs involving *V0* have a much smaller effective sample size because its missing value pattern is opposite to that of the other variables. A similar situation can arise in real data, say when one variable systematically missed one half the sample population while another variable systematically missed the other half.

Under the hood, Mist computes joint entropy estimations that are sensitive to small sample size. If the effective sample size is very small, the estimate can have large fluctuations from the true entropy value. Since joint entropy estimations are used to calculate higher-order measures, such as Symmetric Delta, these fluctuation could lead to spurious results. That is why you should always check the effective sample size of any tuples with interesting signals, such as potential outliers or candidates for a functional dependence.


Select an appropriate Information Theory Measure
------------------------------------------------

Select the measure you want to compute with `Mist::set_measure <api.html#_CPPv2N4mist4Mist11set_measureERNSt6stringE>`_.

::

    import libmist
    search = libmist.Mist()
    search.measure = "SymmetricDelta"

The appropriate measure depends on the data and the question you are trying to answer. Currently, there are two measures available: Joint Entropy and Symmetric Delta.

Joint Entropy
^^^^^^^^^^^^^

An estimate of the joint entropy of two or more variables, computed using the naive approach [Shannon1949]_.

Symmetric Delta
^^^^^^^^^^^^^^^

A novel symmetric measure of functional dependence constructed from joint entropies [Galas2014]_. Values are always reported as positive real numbers [1]_, with larger values indicating stronger signal. Missing values may cause a sign change for low-signal tuples, but these can be ignored.


Define the search Space
-----------------------

Mist computes the IT Measure for each tuple in the search space. Currently Mist recognizes two types of search space, Exhaustive and Custom.

Exhaustive (default) search space
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The default search space is the set of all variable tuples. For *N* variables and tuples size *T*, the default space contains *(N choose T)* tuples. This space is called "exhaustive" or "complete" because it contains all possible unique tuples for a set of variables.

Set the size of tuples in the default space with `Mist::set_tuple_size <api.html#_CPPv2N4mist4Mist14set_tuple_sizeEi>`_.

::

  search.tuple_size = 3

Beware of the size of the exhaustive space: a large number of variables and tuple size 3 and greater leads to combinatorial explosion, e.g., the exhaustive search space of 5000 variables in 3-tuples is over 20 billion tuples!


Custom search space
^^^^^^^^^^^^^^^^^^^

There are many search problems where you do not need to compute all possible tuples. Perhaps you're only interested in functional relationships involving a specific variable, and so you'd like to skip tuples that do not include it.

You can define a smaller search space using the `TupleSpace <api.html#_CPPv2N4mist9algorithm10TupleSpaceE>`_ class. A tuple space is made by defining groups of variables, and then specifying how variables from each group should combine to form the tuples. Follow these steps to define the custom search space:

**1. Create a TupleSpace object**

::

    import libmist
    ts = libmist.TupleSpace()

**2. Define Variable Groups**

A *Variable Group* is simply a named set of variables. Variables are referenced by their column position, *[0,N-1]*. Add a group with `TupleSpace::addVariableGroup <api.html#_CPPv2N4mist9algorithm10TupleSpace21addVariableGroupTupleERNSt6vectorINSt6stringEEE>`_. Variable groups are usually disjoint, but they do not need to be ordered or contiguous.

::

    ts.addVariableGroup("A", [0,1,2,9])
    ts.addVariableGroup("B", [4])

Note that the size of variable groups may impact performance, see `below <userguide.html#tuple-completion-vs-batch-algorithm>`_.

**3. Define Variable Group Tuples**

A *Variable Group Tuple* (or a group tuple for short) is a set of Variable Groups that define the tuples in the search space. Add a group tuple with `TupleSpace::addVariableGroupTuple <api.html#_CPPv2N4mist9algorithm10TupleSpace21addVariableGroupTupleER10tuple_type>`_.

The group tuple is the blueprint for the variable tuples. An algorithm generates variable tuples by replacing the group name with variables from that group. Through iteration it generates all variables tuples, e.g. for variable groups *A=[a1,a2,...,aN]* and *B=[b1,b2,...,bM]*, the group tuple *[A,B]* would generate *N\*M* variable tuples *[a1,b1]*, *[a1,b2]*, ..., *[aN,bM]*.

Let us illustrate the algorithm through an example:

::

    ts.addVariableGroupTuple(["A", "B"])

    # this group tuple generates variable tuples:
    # 0,4
    # 1,4
    # 2,4
    # 4,9

You can list a variable group any number of times, in any order:

::

    ts.addVariableGroupTuple(["A", "B", "A"])

    # this group tuple generates variable tuples:
    # 0,1,4
    # 0,2,4
    # 0,3,4
    # 1,2,4
    # 1,4,9
    # 2,4,9

Note that the order in a group tuple is not important, so the group tuples "A,B" and "B,A" result in the same set of variable tuples.

**4. Set the TupleSpace**

Finally, load the TupleSpace object to set the tuple space. Now, when you run the computation, only the desired tuples will be included.

::

    search.tuple_space = ts

Note: tuple_space and tuple_size parameters are mutually exclusive. The tuple_space parameter takes precedence.

Genetics Example
****************

Consider a more realistic example in genetics. Suppose we have a single phenotype of interest and 5000 single nucleotide polymorphisms (SNPs) that might be related. If we are interested only in finding functional dependencies between the SNPs and the single phenotype, then we should exclude tuples containing only SNPs. The following few lines of code specifies this example, assuming our phenotype variable is in position 0 with all other variables being SNPs

::

    ts.addVariableGroup("phenotype", [0])
    ts.addVariableGroup("genotypes", list(range(1, 5000)))
    ts.addVariableGroupTuple(["genotypes", "phenotype"])
    search.tuple_space = ts

This custom search space reduces the size from roughly 20 billion tuples to 12.5 million.

Compute
-------

Before starting the computation of information measures you should configure the output file with `Mist::set_outfile <api.html#_CPPv2N4mist4Mist11set_outfileERNSt6stringE>`_. For small search spaces this could be the ``stdout`` stream, but more often you will pick a file destination.

::

    search.outfile = "/dev/stdout"

Finally run the computation.

::

    search.start()

This may take anywhere from seconds to days depending on the size of the search space. It is a good idea to start small to get an idea of the runtime. Start with tuples size 2 based on a set of less than 1000 variables and then increase the search space.

Performance Tuning
------------------

The most important factors affecting the overall runtime of a search are the size of the search space and the number of threads. We already covered how to narrow the search space in the previous section. Set the number of threads with `Mist::set_threads <api.html#_CPPv2N4mist4Mist11set_threadsEi>`_.

::

    search.threads = 10

The default number of threads is the maximum allowed by the system (e.g. what you get from the ``nproc`` command). Setting threads equal to 0 implies the maximum allowed.

Advanced
^^^^^^^^

The following are more fine-tuned options that should be considered for advanced uses.

Probability Distribution Algorithms
***********************************

Counting probability distributions is the most time-consuming part of computing an IT Measure. See `Mist::set_probability_algorithm <api.html#_CPPv2N4mist4Mist25set_probability_algorithmERNSt6stringE>`_ for a list of available algorithms.

For very "tall" data (many rows for each variable) we can speed up the algorithm by casting each variable as series of bitsets, rather than using the typical vector representation. This allows faster entropy calculation at the cost of some memory and computation overhead. This option is not advantageous for "short" data, and disastrous if variables have many value bins.

It's worth experimenting with this option if your variable have three or fewer bins, and/or your variables have thousands or ten's of thousands of rows.

Notes
-----
.. [1] Symmetric Delta, as described in [Galas2014]_, has negative sign for odd-dimension tuples. In Mist we give the magnitude always so it is clear what tail of the distribution holds the signal.
