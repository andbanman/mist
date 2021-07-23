#pragma once

#include <cctype>
#include <cstdint>
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
  using data_t = Variable::data_t;
  using index_t = Variable::index_t;

  // allocate empty matrix
  DataMatrix(std::size_t ncol, std::size_t nrow, data_t b);
  DataMatrix(data_t data[], std::size_t ncol, std::size_t nrow);
#ifdef BOOST_PYTHON_EXTENSIONS
  DataMatrix(np::ndarray const& np);
#endif
  // each column is a variable
  DataMatrix(std::string const& filename);
  DataMatrix(std::string const& filename, bool rowmajor);

  Variable get_variable(index_t i);
  Variable::tuple variables();
  std::size_t get_nvar() const; std::size_t get_svar() const;
  std::size_t get_ncol() const;
  std::size_t get_nrow() const;

  void write_file(std::string const& filename, char sep);
  void write_file(std::string const& filename);
#ifdef BOOST_PYTHON_EXTENSIONS
  void python_write_file(std::string const& filename);
#endif

  std::vector<Variable::data_ptr> vectors;
  std::vector<data_t> bins;

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
