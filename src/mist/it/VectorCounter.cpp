#include <stdexcept>
#include <vector>

#include "Variable.hpp"
#include "it/VectorCounter.hpp"

using namespace mist;
using namespace mist::it;

//
// Unrolled count functions are *much* faster.
// Functions operating on a tuple are on performance critical paths
//
static void
count1d(std::size_t varlen,
        Variable::tuple const& vars,
        Variable::indexes const& indexes,
        Distribution& dist)
{
  auto b0 = vars[indexes[0]].bins();
  for (std::size_t jj = 0; jj < varlen; jj++) {
    auto v0 = vars[indexes[0]][jj];
    if (!VARIABLE_MISSING_VAL(v0)) {
      ++dist(v0, b0);
    }
  }
}

static void
count2d(std::size_t varlen,
        Variable::tuple const& vars,
        Variable::indexes const& indexes,
        Distribution& dist)
{
  auto b0 = vars[indexes[0]].bins();
  auto b1 = vars[indexes[1]].bins();
  for (std::size_t jj = 0; jj < varlen; jj++) {
    auto v0 = vars[indexes[0]][jj];
    auto v1 = vars[indexes[1]][jj];
    if (!VARIABLE_MISSING_VAL(v0) && !VARIABLE_MISSING_VAL(v1)) {
      ++dist(v0, v1, b0, b1);
    }
  }
}

static void
count3d(std::size_t varlen,
        Variable::tuple const& vars,
        Variable::indexes const& indexes,
        Distribution& dist)
{
  auto b0 = vars[indexes[0]].bins();
  auto b1 = vars[indexes[1]].bins();
  auto b2 = vars[indexes[2]].bins();
  for (std::size_t jj = 0; jj < varlen; jj++) {
    auto v0 = vars[indexes[0]][jj];
    auto v1 = vars[indexes[1]][jj];
    auto v2 = vars[indexes[2]][jj];
    if (!VARIABLE_MISSING_VAL(v0) && !VARIABLE_MISSING_VAL(v1) &&
        !VARIABLE_MISSING_VAL(v2)) {
      ++dist(v0, v1, v2, b0, b1, b2);
    }
  }
}

//
// Subset variables
//
void
VectorCounter::count(Variable::tuple const& vars,
                     Variable::indexes const& indexes,
                     Distribution& dist)
{
   std::size_t nvars = indexes.size();
   std::size_t varlen = vars.front().size();

  dist.initialize(vars, indexes);

  switch (nvars) {
    case 1:
      count1d(varlen, vars, indexes, dist);
      break;
    case 2:
      count2d(varlen, vars, indexes, dist);
      break;
    case 3:
      count3d(varlen, vars, indexes, dist);
      break;
    default:
      throw VectorCounterException("count",
                                   "Unsupported tuple size " +
                                     std::to_string(nvars) +
                                     ", valid range [1,3]");
  }
}

void
VectorCounter::count(Variable::tuple const& vars, Distribution& dist)
{
  std::size_t nvars = vars.size();
  std::size_t varlen = vars.front().size();

  Variable::indexes indexes(nvars);
  for (std::size_t ii = 0; ii < nvars; ii++) {
    indexes[ii] = ii;
  }

  dist.initialize(vars, indexes);

  switch (nvars) {
    case 1:
      count1d(varlen, vars, indexes, dist);
      break;
    case 2:
      count2d(varlen, vars, indexes, dist);
      break;
    case 3:
      count3d(varlen, vars, indexes, dist);
      break;
    default:
      throw VectorCounterException("count",
                                   "Unsupported tuple size " +
                                     std::to_string(nvars) +
                                     ", valid range [1,3]");
  }
}

void
VectorCounter::count(Variable const& var, Distribution& dist)
{
  std::size_t varlen = var.size();
  Variable::tuple vars(1);
  vars[0] = var;
  Variable::indexes indexes{ 0 };
  dist.initialize(vars, indexes);
  count1d(varlen, vars, indexes, dist);
}
