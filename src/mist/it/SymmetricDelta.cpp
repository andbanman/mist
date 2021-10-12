#include <stdexcept>

#include "it/SymmetricDelta.hpp"

using namespace mist;
using namespace mist::it;

using sub2 = SymmetricDelta::sub_calc_2d;
using sub3 = SymmetricDelta::sub_calc_3d;
using sub4 = SymmetricDelta::sub_calc_4d;

void
compute_2d(EntropyCalculator& ecalc, Variable::indexes const& vars, SymmetricDelta::result_type& res)
{
  if (res.size() != (std::size_t)sub2::size) {
    res.resize((std::size_t)sub2::size);
  }
  auto e0 = ecalc.entropy({ vars[0] });
  auto e1 = ecalc.entropy({ vars[1] });
  auto e01 = ecalc.entropy({ vars[0], vars[1] });
  SymmetricDelta::data_t DD = e0 + e1 - e01;
  res[(int)sub2::entropy0] = e0;
  res[(int)sub2::entropy1] = e1;
  res[(int)sub2::entropy01] = e01;
  res[(int)sub2::symmetric_mist] = DD;
}

void
compute_2d(EntropyCalculator& ecalc,
           Variable::indexes const& vars,
           Entropy const& entropy,
           SymmetricDelta::result_type &res)
{
  if (res.size() != (std::size_t)sub2::size) {
    res.resize((std::size_t)sub2::size);
  }
  auto e0 = entropy[(int)d2::e0];
  auto e1 = entropy[(int)d2::e1];
  auto e01 = entropy[(int)d2::e01];
  SymmetricDelta::data_t DD = e0 + e1 - e01;
  res[(int)sub2::entropy0] = e0;
  res[(int)sub2::entropy1] = e1;
  res[(int)sub2::entropy01] = e01;
  res[(int)sub2::symmetric_mist] = DD;
}

void
compute_3d(EntropyCalculator& ecalc, Variable::indexes const& vars, SymmetricDelta::result_type& res)
{
  if (res.size() != (std::size_t)sub3::size) {
    res.resize((std::size_t)sub3::size);
  }
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
}

void
compute_3d(EntropyCalculator& ecalc,
           Variable::indexes const& vars,
           Entropy const& entropy,
           SymmetricDelta::result_type& res)
{
  if (res.size() != (std::size_t)sub3::size) {
    res.resize((std::size_t)sub3::size);
  }
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
}

void
compute_4d(EntropyCalculator& ecalc, Variable::indexes const& vars, SymmetricDelta::result_type& res)
{
  if (res.size() != (std::size_t)sub4::size) {
    res.resize((std::size_t)sub4::size);
  }
  auto e0 = ecalc.entropy({ vars[0] });
  auto e1 = ecalc.entropy({ vars[1] });
  auto e2 = ecalc.entropy({ vars[2] });
  auto e3 = ecalc.entropy({ vars[3] });
  auto e01 = ecalc.entropy({ vars[0], vars[1] });
  auto e02 = ecalc.entropy({ vars[0], vars[2] });
  auto e03 = ecalc.entropy({ vars[0], vars[3] });
  auto e12 = ecalc.entropy({ vars[1], vars[2] });
  auto e13 = ecalc.entropy({ vars[1], vars[3] });
  auto e23 = ecalc.entropy({ vars[2], vars[3] });
  auto e012 = ecalc.entropy({ vars[0], vars[1], vars[2] });
  auto e013 = ecalc.entropy({ vars[0], vars[1], vars[3] });
  auto e023 = ecalc.entropy({ vars[0], vars[2], vars[3] });
  auto e123 = ecalc.entropy({ vars[1], vars[2], vars[3] });
  auto e0123 = ecalc.entropy({ vars[0], vars[1], vars[2], vars[3] });

  auto I012 = e0 + e1 + e2 - e01 - e02 - e12 + e012;
  auto I013 = e0 + e1 + e3 - e01 - e03 - e13 + e013;
  auto I023 = e0 + e2 + e3 - e02 - e03 - e23 + e023;
  auto I123 = e1 + e2 + e3 - e12 - e13 - e23 + e123;
  auto I0123 = e0 + e1 + e2 + e3
             - e01 - e02 - e03 - e12 - e13 - e23
             + e012 + e013 + e023 + e123
             - e0123;

  auto D0 = I0123 - I123;
  auto D1 = I0123 - I023;
  auto D2 = I0123 - I013;
  auto D3 = I0123 - I012;
  auto DD = D0 * D1 * D2 * D3;

  res[(int)sub4::entropy0] = e0;
  res[(int)sub4::entropy1] = e1;
  res[(int)sub4::entropy2] = e2;
  res[(int)sub4::entropy3] = e3;
  res[(int)sub4::entropy01] = e01;
  res[(int)sub4::entropy02] = e02;
  res[(int)sub4::entropy03] = e03;
  res[(int)sub4::entropy12] = e12;
  res[(int)sub4::entropy13] = e13;
  res[(int)sub4::entropy23] = e23;
  res[(int)sub4::entropy012] = e012;
  res[(int)sub4::entropy013] = e013;
  res[(int)sub4::entropy023] = e023;
  res[(int)sub4::entropy123] = e123;
  res[(int)sub4::entropy0123] = e0123;
  res[(int)sub4::jointInfo012] = I012;
  res[(int)sub4::jointInfo013] = I013;
  res[(int)sub4::jointInfo023] = I023;
  res[(int)sub4::jointInfo123] = I123;
  res[(int)sub4::diffInfo0] = D0;
  res[(int)sub4::diffInfo1] = D1;
  res[(int)sub4::diffInfo2] = D2;
  res[(int)sub4::diffInfo3] = D3;
  res[(int)sub4::symmetric_delta] = DD;
}

void
compute_4d(EntropyCalculator& ecalc,
           Variable::indexes const& vars,
           Entropy const& entropy,
           SymmetricDelta::result_type& res)
{
  if (res.size() != (std::size_t)sub4::size) {
    res.resize((std::size_t)sub4::size);
  }
  auto e0 = entropy[(int)d4::e0];
  auto e1 = entropy[(int)d4::e1];
  auto e2 = entropy[(int)d4::e2];
  auto e01 = entropy[(int)d4::e01];
  auto e02 = entropy[(int)d4::e02];
  auto e12 = entropy[(int)d4::e12];
  auto e012 = entropy[(int)d4::e012];

  auto e3 = ecalc.entropy({ vars[3] });
  auto e03 = ecalc.entropy({ vars[0], vars[3] });
  auto e13 = ecalc.entropy({ vars[1], vars[3] });
  auto e23 = ecalc.entropy({ vars[2], vars[3] });
  auto e013 = ecalc.entropy({ vars[0], vars[1], vars[3] });
  auto e023 = ecalc.entropy({ vars[0], vars[2], vars[3] });
  auto e123 = ecalc.entropy({ vars[1], vars[2], vars[3] });
  auto e0123 = ecalc.entropy({ vars[0], vars[1], vars[2], vars[3] });

  auto I012 = e0 + e1 + e2 - e01 - e02 - e12 + e012;
  auto I013 = e0 + e1 + e3 - e01 - e03 - e13 + e013;
  auto I023 = e0 + e2 + e3 - e02 - e03 - e23 + e023;
  auto I123 = e1 + e2 + e3 - e12 - e13 - e23 + e123;
  auto I0123 = e0 + e1 + e2 + e3
             - e01 - e02 - e03 - e12 - e13 - e23
             + e012 + e013 + e023 + e123
             - e0123;

  auto D0 = I0123 - I123;
  auto D1 = I0123 - I023;
  auto D2 = I0123 - I013;
  auto D3 = I0123 - I012;
  auto DD = D0 * D1 * D2 * D3;

  res[(int)sub4::entropy0] = e0;
  res[(int)sub4::entropy1] = e1;
  res[(int)sub4::entropy2] = e2;
  res[(int)sub4::entropy3] = e3;
  res[(int)sub4::entropy01] = e01;
  res[(int)sub4::entropy02] = e02;
  res[(int)sub4::entropy03] = e03;
  res[(int)sub4::entropy12] = e12;
  res[(int)sub4::entropy13] = e13;
  res[(int)sub4::entropy23] = e23;
  res[(int)sub4::entropy012] = e012;
  res[(int)sub4::entropy013] = e013;
  res[(int)sub4::entropy023] = e023;
  res[(int)sub4::entropy123] = e123;
  res[(int)sub4::entropy0123] = e0123;
  res[(int)sub4::jointInfo012] = I012;
  res[(int)sub4::jointInfo013] = I013;
  res[(int)sub4::jointInfo023] = I023;
  res[(int)sub4::jointInfo123] = I123;
  res[(int)sub4::diffInfo0] = D0;
  res[(int)sub4::diffInfo1] = D1;
  res[(int)sub4::diffInfo2] = D2;
  res[(int)sub4::diffInfo3] = D3;
  res[(int)sub4::symmetric_delta] = DD;
}

SymmetricDelta::result_type
SymmetricDelta::compute(EntropyCalculator& ecalc,
                        Variable::indexes const& tuple) const
{
  result_type result;
  compute(ecalc, tuple, result);
  return result;
}

void
SymmetricDelta::compute(EntropyCalculator& ecalc,
                        Variable::indexes const& tuple,
                        result_type &result) const
{
  auto size = tuple.size();
  switch (size) {
    case 2:
      compute_2d(ecalc, tuple, result);
      break;
    case 3:
      compute_3d(ecalc, tuple, result);
      break;
    case 4:
      compute_4d(ecalc, tuple, result);
      break;
    default:
      throw SymmetricDeltaException("compute",
                                    "Unsupported tuple size " +
                                      std::to_string(size) +
                                      ", valid range [2,4]");
  }
}

SymmetricDelta::result_type
SymmetricDelta::compute(EntropyCalculator& ecalc,
                        Variable::indexes const& tuple,
                        Entropy const& e) const
{
  result_type result;
  compute(ecalc, tuple, e, result);
  return result;
}

void
SymmetricDelta::compute(EntropyCalculator& ecalc,
                        Variable::indexes const& tuple,
                        Entropy const& e,
                        result_type &result) const
{
  auto size = tuple.size();
  switch (size) {
    case 2:
      compute_2d(ecalc, tuple, e, result);
      break;
    case 3:
      compute_3d(ecalc, tuple, e, result);
      break;
    case 4:
      compute_4d(ecalc, tuple, e, result);
      break;
    default:
      throw SymmetricDeltaException("compute",
                                    "Unsupported tuple size " +
                                      std::to_string(size) +
                                      ", valid range [2,4]");
  }
}

const std::vector<std::string> names_d2 = {"v0","v1","SymmetricDelta"};
const std::vector<std::string> names_d3 = {"v0","v1","v2","SymmetricDelta"};
const std::vector<std::string> names_d4 = {"v0","v1","v2","v3","SymmetricDelta"};
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
const std::vector<std::string> names_d4_full = {"v0","v1","v2","v3"
          ,"entropy0" ,"entropy1" ,"entropy2" ,"entropy3"
          ,"entropy01" ,"entropy02" ,"entropy03" ,"entropy12"
          ,"entropy13" ,"entropy23" ,"entropy012" ,"entropy013"
          ,"entropy023" ,"entropy123" , "entropy0123" ,"jointInfo012"
          ,"jointInfo013" ,"jointInfo023" ,"jointInfo123"
          ,"diffInfo0" ,"diffInfo1" ,"diffInfo2" ,"diffInfo3"
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
    case 4:
      return (full_output) ? names_d4_full : names_d4;
      break;
    default:
      throw SymmetricDeltaException("names",
                                    "Unsupported tuple size " +
                                      std::to_string(d) +
                                      ", valid range [2,4]");
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
