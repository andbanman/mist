#pragma once

#include <map>

#include "OutputStream.hpp"
#include "it/Entropy.hpp"
#include "py.hpp"

namespace mist {
namespace io {

class FlatOutputStream : public OutputStream
{
public:
  using results_t = std::vector<result_type>;
  using tuples_t = std::vector<tuple_type>;
  FlatOutputStream(std::size_t size, std::size_t rowsize, std::size_t offset);
  FlatOutputStream(std::size_t rowsize, std::size_t offset);
  FlatOutputStream(std::size_t offset);
  FlatOutputStream();
  ~FlatOutputStream();

  void push(std::size_t tuple_no, tuple_type const& tuple, result_type const& result);
  void push(std::size_t tuple_no, tuple_type const& tuple, it::entropy_type result);
  //void combine(FlatOutputStream const& other);
  std::vector<data_t> const& get_results();
  //tuples_t const& get_tuples();
  /** Move all data in other to this object
   */
  void relocate(FlatOutputStream &other);
#if BOOST_PYTHON_EXTENSIONS
  np::ndarray py_get_results();
#endif

private:
  std::vector<data_t> *data;
  std::size_t size;
  std::size_t rowsize;
  std::size_t offset; // for parallel algorithims when we're only caputing a subset of the TupleSpace, the offset moves our index back to zero
};

class FlatOutputStreamException : public std::exception
{
private:
  std::string msg;
public:
  FlatOutputStreamException(std::string const& method, std::string const& msg)
    : msg("FlatOutputStream::" + method + ": " + msg)
  {}
  virtual const char* what() const throw() { return msg.c_str(); };
};

} // io
} // mist
