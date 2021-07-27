#pragma once

#include "../Variable.hpp"

#include "Entropy.hpp"
#include "EntropyCalculator.hpp"
#include "Measure.hpp"

namespace mist {
namespace it {

class EntropyMeasure : public Measure
{
public:
  EntropyMeasure(){};
  ~EntropyMeasure(){};

  result_type compute(EntropyCalculator& ecalc,
                      Variable::indexes const& tuple) const;
  void compute(EntropyCalculator& ecalc,
               Variable::indexes const& tuple,
               result_type& result) const;
  result_type compute(EntropyCalculator& ecalc,
                      Variable::indexes const& tuple,
                      Entropy const& e) const;
  void compute(EntropyCalculator& ecalc,
                      Variable::indexes const& tuple,
                      Entropy const& e,
                      result_type& result) const;
  std::string header(int d, bool full_output) const;
  std::vector<std::string> const& names(int d, bool full_output) const;

  enum struct sub_calc_1d : int
  {
    entropy0,
    size
  };

  enum struct sub_calc_2d : int
  {
    entropy01,
    size
  };

  enum struct sub_calc_3d
  {
    entropy012,
    size
  };

  bool full_entropy() const { return false; };
};

class EntropyMeasureException : public std::exception
{
private:
  std::string msg;

public:
  EntropyMeasureException(std::string const& method, std::string const& msg)
    : msg("EntropyMeasure::" + method + " : " + msg)
  {}
  virtual const char* what() const throw() { return msg.c_str(); };
};

} // it
} // mist
