#include <mutex>
#include <stdexcept>

#include "Variable.hpp"
#include "it/BitsetCounter.hpp"
#include "it/Distribution.hpp"

using namespace mist;
using namespace mist::it;

//
// Populate the bitset representation of variable data
//
static void
populateBitsetVariable(Variable const& v, BitsetVariable& b)
{
  int bins = v.bins();
  int size = v.size();

  // Initialize bitsets
  // last bitset is non-missing mask
  b = BitsetVariable(bins + 1);
  for (int ii = 0; ii < bins+1; ii++) {
    b[ii] = Bitset(size);
  }
  b.back().set(1);

  for (int jj = 0; jj < size; jj++) {
    int val = v[jj];
    if (VARIABLE_MISSING_VAL(val)) {
      b.back()[jj] = 0;
    } else {
      b[v[jj]][jj] = 1;
    }
  }
}

static int
bitsetCount(BitsetTable const& bitsetTable,
            Variable::tuple const& vars,
            Variable::indexes const& indexes,
            int vals[])
{
  int size = vars.size();

  // initialize to non-missing mask
  Bitset result = bitsetTable[vars[indexes[0]].index()].back();
  for (int ii = 1; ii < size; ii++) {
    result &= bitsetTable[vars[indexes[ii]].index()].back();
  }

  // build out distribution
  result &= bitsetTable[vars[indexes[0]].index()][vals[0]];
  for (int ii = 1; ii < size; ii++) {
    result &= bitsetTable[vars[indexes[ii]].index()][vals[ii]];
  }

  return result.count();
}

static void
recursiveBitsetCount(BitsetTable const& bitsetTable,
                     Variable::tuple const& vars,
                     Variable::indexes const& indexes,
                     Distribution& dist,
                     int vals[],
                     int pos)
{
  int size = vars.size();

  if (size == pos) {
    dist(size, vals) = bitsetCount(bitsetTable, vars, indexes, vals);
  } else {
    int nbins = vars[indexes[pos]].bins();
    for (int val = 0; val < nbins; val++) {
      vals[pos] = val;
      recursiveBitsetCount(bitsetTable, vars, indexes, dist, vals, pos + 1);
    }
  }
}

void static count1d(BitsetTable const& bitsetTable,
                    Variable::tuple const& vars,
                    Variable::indexes const& indexes,
                    Distribution& dist)
{
  int bins[1];
  auto n0 = vars[indexes[0]].bins();
  auto v0 = vars[indexes[0]].index();

  for (int b0 = 0; b0 < n0; b0++) {
    bins[0] = b0;
    dist(1, bins) = bitsetTable[v0][b0].count();
  }
}

void static count2d(BitsetTable const& bitsetTable,
                    Variable::tuple const& vars,
                    Variable::indexes const& indexes,
                    Distribution& dist)
{
  int bins[2];
  auto n0 = vars[indexes[0]].bins();
  auto v0 = vars[indexes[0]].index();
  auto n1 = vars[indexes[1]].bins();
  auto v1 = vars[indexes[1]].index();
  auto size = bitsetTable.front().front().size();

  // prepare results bitset
  Bitset result(size);

  for (int b0 = 0; b0 < n0; b0++) {
    for (int b1 = 0; b1 < n1; b1++) {
      result = bitsetTable[v0][b0];
      result &= bitsetTable[v1][b1];
      bins[0] = b0;
      bins[1] = b1;
      dist(2, bins) = result.count();
    }
  }
}

void static count3d(BitsetTable const& bitsetTable,
                    Variable::tuple const& vars,
                    Variable::indexes const& indexes,
                    Distribution& dist)
{
  int bins[3];
  auto n0 = vars[indexes[0]].bins();
  auto v0 = vars[indexes[0]].index();
  auto n1 = vars[indexes[1]].bins();
  auto v1 = vars[indexes[1]].index();
  auto n2 = vars[indexes[2]].bins();
  auto v2 = vars[indexes[2]].index();
  auto size = bitsetTable.front().front().size();

  // preallocate intermediarty results
  Bitset result(size);
  Bitset result01(size);

  for (int b0 = 0; b0 < n0; b0++) {
    for (int b1 = 0; b1 < n1; b1++) {
      result01 = bitsetTable[v0][b0];
      result01 &= bitsetTable[v1][b1];
      for (int b2 = 0; b2 < n2; b2++) {
        result = result01;
        result &= bitsetTable[v2][b2];
        bins[0] = b0;
        bins[1] = b1;
        bins[2] = b2;
        dist(3, bins) = result.count();
      }
    }
  }
}

void
BitsetCounter::count(Variable::tuple const& vars,
                     Variable::indexes const& indexes,
                     Distribution& dist)
{
  int nvars = indexes.size();

  dist.initialize(vars, indexes);

  switch (nvars) {
    // TODO: Weird, for some reason the unrolled are slower???
    case 1:
      count1d(this->bits, vars, indexes, dist);
      break;
    case 2:
      count2d(this->bits, vars, indexes, dist);
      break;
    case 3:
      count3d(this->bits, vars, indexes, dist);
      break;
    default:
      int vals[nvars];
      recursiveBitsetCount(this->bits, vars, indexes, dist, vals, 0);
      break;
  }
}

//! @exception out_of_range Variable index out of range of table whose size set
//! in constructor
void
BitsetCounter::count(Variable::tuple const& vars, Distribution& dist)
{
  auto nvars = vars.size();
  Variable::indexes indexes(nvars);
  for (int ii = 0; ii < nvars; ii++) {
    indexes[ii] = ii;
  }
  this->count(vars, indexes, dist);
}

void
BitsetCounter::count(Variable const& var, Distribution& dist)
{
  Variable::tuple vars(1);
  vars[0] = var;
  this->count(vars, { 0 }, dist);
}

BitsetCounter::BitsetCounter(Variable::tuple const& all_vars)
{
  // wrong value of n ???
  auto n = all_vars.size();
  this->bits = BitsetTable(n);

  for (int ii = 0; ii < n; ii++) {
    if (all_vars[ii].index() >= n) {
      throw BitsetCounterOutOfRange("BitsetCounter", all_vars[ii].index(), n);
    }
    auto& bitsetVar = bits[all_vars[ii].index()];
    populateBitsetVariable(all_vars[ii], bitsetVar);
  }
};
