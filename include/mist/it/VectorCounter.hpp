#pragma once

#include "Counter.hpp"

namespace mist {
namespace it {

/**
 * Generates a ProbabilityDistribution from a Variable tuple.
 *
 * Counts using standard algorithm.
 */
class VectorCounter : public Counter
{
public:
  VectorCounter(){};
  ~VectorCounter(){};
  void count(Variable const&, Distribution&);
  void count(Variable::tuple const&, Distribution&);
  void count(Variable::tuple const&, Variable::indexes const&, Distribution&);
};

class VectorCounterException : public std::exception
{
private:
  std::string msg;

public:
  VectorCounterException(std::string const& method, std::string const& msg)
    : msg("Variable::" + method + " : " + msg)
  {}
  virtual const char* what() const throw() { return msg.c_str(); };
};

} // it
} // mist
