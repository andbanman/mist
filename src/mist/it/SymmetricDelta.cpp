#include <stdexcept>

#include "it/SymmetricDelta.hpp"

using namespace mist;
using namespace mist::it;

using sub2 = SymmetricDelta::sub_calc_2d;
using sub3 = SymmetricDelta::sub_calc_3d;

SymmetricDelta::result_type
compute_2d(EntropyCalculator& ecalc, Variable::indexes const& vars)
{
  SymmetricDelta::result_type res((std::size_t)sub2::size);

  auto e0 = ecalc.entropy({ vars[0] });
  auto e1 = ecalc.entropy({ vars[1] });
  auto e01 = ecalc.entropy({ vars[0], vars[1] });
  SymmetricDelta::data_t DD = e0 + e1 - e01;
  res[(int)sub2::entropy0] = e0;
  res[(int)sub2::entropy1] = e1;
  res[(int)sub2::entropy01] = e01;
  res[(int)sub2::symmetric_mist] = DD;

  return res;
}

SymmetricDelta::result_type
compute_2d(EntropyCalculator& ecalc,
           Variable::indexes const& vars,
           Entropy const& entropy)
{
  SymmetricDelta::result_type res((std::size_t)sub2::size);

  auto e0 = entropy[(int)d2::e0];
  auto e1 = entropy[(int)d2::e1];
  auto e01 = entropy[(int)d2::e01];
  SymmetricDelta::data_t DD = e0 + e1 - e01;
  res[(int)sub2::entropy0] = e0;
  res[(int)sub2::entropy1] = e1;
  res[(int)sub2::entropy01] = e01;
  res[(int)sub2::symmetric_mist] = DD;

  return res;
}

SymmetricDelta::result_type
compute_3d(EntropyCalculator& ecalc, Variable::indexes const& vars)
{
  SymmetricDelta::result_type res((std::size_t)sub3::size);

  auto e0 = ecalc.entropy({ vars[0] });
  auto e1 = ecalc.entropy({ vars[1] });
  auto e2 = ecalc.entropy({ vars[2] });
  auto e01 = ecalc.entropy({ vars[0], vars[1] });
  auto e02 = ecalc.entropy({ vars[0], vars[2] });
  auto e12 = ecalc.entropy({ vars[1], vars[2] });
  auto e012 = ecalc.entropy({ vars[0], vars[1], vars[2] });
  auto I01 = e0 + e1 - e01;
  auto I02 = e0 + e2 - e02;
  auto I12 = e1 + e2 - e12;
  auto I012 = e0 + e1 + e2 - e01 - e02 - e12 + e012;
  auto D0 = I012 - I12;
  auto D1 = I012 - I02;
  auto D2 = I012 - I01;
  auto DD = D0 * D1 * D2;
  // sign change to force positive values
  DD = (DD) ? -1 * DD : 0;

  res[(int)sub3::entropy0] = e0;
  res[(int)sub3::entropy1] = e1;
  res[(int)sub3::entropy2] = e2;
  res[(int)sub3::entropy01] = e01;
  res[(int)sub3::entropy02] = e02;
  res[(int)sub3::entropy12] = e12;
  res[(int)sub3::entropy012] = e012;
  res[(int)sub3::jointInfo01] = I01;
  res[(int)sub3::jointInfo02] = I02;
  res[(int)sub3::jointInfo12] = I12;
  res[(int)sub3::jointInfo012] = I012;
  res[(int)sub3::diffInfo0] = D0;
  res[(int)sub3::diffInfo1] = D1;
  res[(int)sub3::diffInfo2] = D2;
  res[(int)sub3::symmetric_mist] = DD;

  return res;
}

SymmetricDelta::result_type
compute_3d(EntropyCalculator& ecalc,
           Variable::indexes const& vars,
           Entropy const& entropy)
{
  SymmetricDelta::result_type res((std::size_t)sub3::size);

  auto e0 = entropy[(int)d3::e0];
  auto e1 = entropy[(int)d3::e1];
  auto e2 = entropy[(int)d3::e2];
  auto e01 = entropy[(int)d3::e01];
  auto e02 = entropy[(int)d3::e02];
  auto e12 = entropy[(int)d3::e12];
  auto e012 = entropy[(int)d3::e012];
  auto I01 = e0 + e1 - e01;
  auto I02 = e0 + e2 - e02;
  auto I12 = e1 + e2 - e12;
  auto I012 = e0 + e1 + e2 - e01 - e02 - e12 + e012;
  auto D0 = I012 - I12;
  auto D1 = I012 - I02;
  auto D2 = I012 - I01;
  auto DD = D0 * D1 * D2;
  // sign change to force positive values
  DD = (DD) ? -1 * DD : 0;

  res[(int)sub3::entropy0] = e0;
  res[(int)sub3::entropy1] = e1;
  res[(int)sub3::entropy2] = e2;
  res[(int)sub3::entropy01] = e01;
  res[(int)sub3::entropy02] = e02;
  res[(int)sub3::entropy12] = e12;
  res[(int)sub3::entropy012] = e012;
  res[(int)sub3::jointInfo01] = I01;
  res[(int)sub3::jointInfo02] = I02;
  res[(int)sub3::jointInfo12] = I12;
  res[(int)sub3::jointInfo012] = I012;
  res[(int)sub3::diffInfo0] = D0;
  res[(int)sub3::diffInfo1] = D1;
  res[(int)sub3::diffInfo2] = D2;
  res[(int)sub3::symmetric_mist] = DD;

  return res;
}

SymmetricDelta::result_type
SymmetricDelta::compute(EntropyCalculator& ecalc,
                        Variable::indexes const& tuple) const
{
  SymmetricDelta::result_type ret;
  auto size = tuple.size();
  switch (size) {
    case 2:
      ret = compute_2d(ecalc, tuple);
      break;
    case 3:
      ret = compute_3d(ecalc, tuple);
      break;
    default:
      throw SymmetricDeltaException("compute",
                                    "Unsupported tuple size " +
                                      std::to_string(size) +
                                      ", valid range [2,3]");
  }
  return ret;
}

SymmetricDelta::result_type
SymmetricDelta::compute(EntropyCalculator& ecalc,
                        Variable::indexes const& tuple,
                        Entropy const& e) const
{
  SymmetricDelta::result_type ret;
  auto size = tuple.size();
  switch (size) {
    case 2:
      ret = compute_2d(ecalc, tuple, e);
      break;
    case 3:
      ret = compute_3d(ecalc, tuple, e);
      break;
    default:
      throw SymmetricDeltaException("compute",
                                    "Unsupported tuple size " +
                                      std::to_string(size) +
                                      ", valid range [2,3]");
  }
  return ret;
}

const std::vector<std::string> names_d2 = {"v0","v1","SymmetricDelta"};
const std::vector<std::string> names_d3 = {"v0","v1","v2","SymmetricDelta"};
const std::vector<std::string> names_d2_full = {"v0","v1","entropy0","entropy1","entropy01","SymmetricDelta"};
const std::vector<std::string> names_d3_full = {"v0","v1","v2",
          "entropy0" ,"entropy1"
          ,"entropy2" ,"entropy01"
          ,"entropy02" ,"entropy12"
          ,"entropy012" ,"jointInfo01"
          ,"jointInfo02" ,"jointInfo12"
          ,"jointInfo012" ,"diffInfo0"
          ,"diffInfo1" ,"diffInfo2"
          ,"SymmetricDelta"};

std::vector<std::string> const&
SymmetricDelta::names(int d, bool full_output) const
{
  switch (d) {
    case 2:
      return (full_output) ? names_d2_full : names_d2;
      break;
    case 3:
      return (full_output) ? names_d3_full : names_d3;
      break;
    default:
      throw SymmetricDeltaException("names",
                                    "Unsupported tuple size " +
                                      std::to_string(d) +
                                      ", valid range [2,3]");
  }
}

std::string
SymmetricDelta::header(int d, bool full_output) const
{
  auto n = names(d,full_output);
  std::string h = n.front();
  auto N = n.size();
  for (int ii = 1; ii < N; ii++) {
    h += "," + n[ii];
  }
  return h;
}
