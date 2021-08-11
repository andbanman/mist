#include <stdexcept>

#include "it/EntropyMeasure.hpp"

using namespace mist;
using namespace mist::it;

EntropyMeasure::result_type
EntropyMeasure::compute(EntropyCalculator& ecalc,
                        Variable::indexes const& tuple) const
{
  result_type result;
  compute(ecalc, tuple, result);
  return result;
}

EntropyMeasure::result_type
EntropyMeasure::compute(EntropyCalculator& ecalc,
                        Variable::indexes const& tuple,
                        Entropy const& e) const
{
  return compute(ecalc, tuple);
}

void
EntropyMeasure::compute(EntropyCalculator& ecalc,
                        Variable::indexes const& tuple,
                        result_type& result) const
{
  auto size = tuple.size();
  if (result.size() != 1) {
    result.resize(1);
  }
  result[0] = ecalc.entropy(tuple);
}

void
EntropyMeasure::compute(EntropyCalculator& ecalc,
                        Variable::indexes const& tuple,
                        Entropy const& e,
                        result_type& result) const
{
  compute(ecalc, tuple, result);
}

const std::vector<std::string> names_d1 = {"v0","entropy0"};
const std::vector<std::string> names_d2 = {"v0","v1","entropy01"};
const std::vector<std::string> names_d3 = {"v0","v1","v2","entropy012"};

std::vector<std::string> const&
EntropyMeasure::names(int d, bool full_output) const
{
  switch (d) {
    case 1:
      return names_d1;
      break;
    case 2:
      return names_d2;
      break;
    case 3:
      return names_d3;
      break;
    default:
      throw EntropyMeasureException("names",
                                    "Unsupported tuple size " +
                                      std::to_string(d) +
                                      ", valid range [1,3]");
  }
}

std::string
EntropyMeasure::header(int d, bool full_output) const
{
  auto n = names(d,full_output);
  std::string h = n.front();
  auto N = n.size();
  for (int ii = 1; ii < N; ii++) {
    h += "," + n[ii];
  }
  return h;
}
