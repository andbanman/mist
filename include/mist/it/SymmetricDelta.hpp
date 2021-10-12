#pragma once

#include "../Variable.hpp"

#include "EntropyCalculator.hpp"
#include "Measure.hpp"

namespace mist {
namespace it {

class SymmetricDelta : public Measure
{
public:
  SymmetricDelta(){};
  ~SymmetricDelta(){};

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

  enum struct sub_calc_2d : int
  {
    entropy0,
    entropy1,
    entropy01,
    symmetric_mist,
    size
  };

  enum struct sub_calc_3d
  {
    entropy0,
    entropy1,
    entropy2,
    entropy01,
    entropy02,
    entropy12,
    entropy012,
    jointInfo01,
    jointInfo02,
    jointInfo12,
    jointInfo012,
    diffInfo0,
    diffInfo1,
    diffInfo2,
    symmetric_mist,
    size
  };

  enum struct sub_calc_4d
  {
    entropy0,
    entropy1,
    entropy2,
    entropy3,
    entropy01,
    entropy02,
    entropy03,
    entropy12,
    entropy13,
    entropy23,
    entropy012,
    entropy013,
    entropy023,
    entropy123,
    entropy0123,
    jointInfo012,
    jointInfo013,
    jointInfo023,
    jointInfo123,
    diffInfo0,
    diffInfo1,
    diffInfo2,
    diffInfo3,
    symmetric_delta,
    size
  };

  bool full_entropy() const { return true; };
};

class SymmetricDeltaException : public std::exception
{
private:
  std::string msg;

public:
  SymmetricDeltaException(std::string const& method, std::string const& msg)
    : msg("SymmetricDelta::" + method + " : " + msg)
  {}
  virtual const char* what() const throw() { return msg.c_str(); };
};

} // it
} // mist
