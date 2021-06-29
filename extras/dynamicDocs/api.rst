API
===
.. toctree::
   :maxdepth: 2
   :glob:

mist is comprised of logically distinct components encapsulated by namespaces. Classes access other namespaces via an interface class. Users typically only need to be concerned with classes in the root namespace, whereas developers will need the rest.

mist
--------
The root namespace includes composition classes and classes common to the sub-namespaces.

.. doxygenclass:: mist::Search
  :members:

.. doxygenclass:: mist::Variable
  :members:

mist::algorithm
-------------------
Algorithms to divide and conquer Information Theory computations.

.. doxygennamespace:: mist::algorithm
   :members:

mist::cache
---------------
Cache intermediate results for performance improvement.

.. doxygennamespace:: mist::cache
   :members:

mist::io
------------
Input/Output

.. doxygennamespace:: mist::io
   :members:

mist::it
------------
Information Theory definitions and algorithms.

.. doxygennamespace:: mist::it
   :members:
