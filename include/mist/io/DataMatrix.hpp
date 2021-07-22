#pragma once

#include <fstream>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <vector>

#ifdef BOOST_PYTHON_EXTENSIONS
// TODO compiler #if BUILD_PYTHON
#include <boost/predef/version.h>
#if BOOST_PREDEF_VERSION < BOOST_VERSION_NUMBER(1, 6, 0)
#include <boost/numpy.hpp>
#else
#include <boost/python/numpy.hpp>
#endif
#endif

#include "Variable.hpp"

#ifdef BOOST_PYTHON_EXTENSIONS
namespace p = boost::python;
#if BOOST_PREDEF_VERSION < BOOST_VERSION_NUMBER(1, 6, 0)
namespace np = boost::numpy;
#else
namespace np = boost::python::numpy;
#endif
#endif

namespace mist {
namespace io {

/** N x M input data matrix.
 *
 * Columns are interpreted as variables with each row a sample.
 */
class DataMatrix
{
public:
  // allocate empty matrix
  DataMatrix(int ncol, int nrow, int b);
  DataMatrix(int data[], int ncol, int nrow);
#ifdef BOOST_PYTHON_EXTENSIONS
  DataMatrix(np::ndarray const& np);
#endif
  // each column is a variable
  DataMatrix(std::string const& filename);
  DataMatrix(std::string const& filename, bool rowmajor);

  Variable get_variable(int i);
  Variable::tuple variables();
  unsigned long get_nvar() const;
  unsigned long get_svar() const;
  unsigned long get_ncol() const;
  unsigned long get_nrow() const;

  void write_file(std::string const& filename, char sep);
  void write_file(std::string const& filename);
#ifdef BOOST_PYTHON_EXTENSIONS
  void python_write_file(std::string const& filename);
#endif

  std::vector<mist::Variable::data_ptr> vectors;
  std::vector<int> bins;
private:
  std::size_t ncol;
  std::size_t nrow;
  std::size_t nvar;
  std::size_t svar;
};

class DataMatrixException : public std::exception
{
private:
  std::string msg;

public:
  DataMatrixException(std::string const& method, std::string const& msg)
    : msg("DataMatrix::" + method + " : " + msg)
  {}
  virtual const char* what() const throw() { return msg.c_str(); };
};

} // io
} // mist
