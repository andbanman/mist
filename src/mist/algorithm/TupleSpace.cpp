#include <algorithm>
#include <cmath>
#include <map>
#include <set>

#if BOOST_PYTHON_EXTENSIONS
#include <boost/python/extract.hpp>
#endif

#include "algorithm/TupleSpace.hpp"

using namespace mist;
using namespace mist::algorithm;

TupleSpace::TupleSpace()
  : tuple_size(0){};
TupleSpace::~TupleSpace(){};

// default space for N variables in tuples size d
TupleSpace::TupleSpace(int N, int d)
{
  if (N == 0 || d == 0) {
    throw TupleSpaceException("TupleSpace",
                              "Number of variables and dimension cannot be zero.");
  }
  tuple_t vars(N);
  for (int ii = 0; ii < N; ii++) {
    vars[ii] = ii;
  }
  this->addVariableGroup("default", vars);
  tuple_t groupTuple(d, 0);
  this->addVariableGroupTuple(groupTuple);
};

std::vector<std::string>
TupleSpace::names() const
{
  return variableNames;
}

void
TupleSpace::set_names(std::vector<std::string> const& names)
{
  variableNames = names;
}

int
TupleSpace::tupleSize() const
{
  return tuple_size;
}

void
TupleSpace::addVariableGroupTuple(std::vector<std::string> const& groupNames)
{
  TupleSpace::tuple_t groupIndexes;
  for (auto& name : groupNames) {
    try {
      groupIndexes.push_back(variableGroupNames[name]);
    } catch (std::out_of_range& e) {
      throw TupleSpaceException("addVariableGroupTuple",
                                "group named " + name + " does not exist.");
    }
  }
  addVariableGroupTuple(groupIndexes);
}

void
TupleSpace::addVariableGroupTuple(TupleSpace::tuple_t const& groupIndexes)
{
  // validate tuple size
  if (!tuple_size) {
    tuple_size = groupIndexes.size();
  } else if (tuple_size != groupIndexes.size()) {
    throw TupleSpaceException(
      "addVariableGroupTuple",
      "Could not add group tuple, all tuples must be the same size");
  }
  // validate group indexes
  for (auto group : groupIndexes) {
    if (group >= variableGroups.size()) {
      throw TupleSpaceException("addVariableGroupTuple",
                                "variable group index " +
                                  std::to_string(group) + " out of range.");
    }
  }
  variableGroupTuples.push_back(groupIndexes);
}

int
TupleSpace::addVariableGroup(std::string const& name,
                             TupleSpace::tuple_t const& vars)
{
  std::set<int> unique_vars;
  tuple_t group;
  for (auto var : vars) {
    // ignore duplicates within variable group
    if (unique_vars.find(var) == unique_vars.end()) {
      unique_vars.insert(var);
      group.push_back(var);
      // check for overlap with other variable groups
      if (seen_vars.find(var) != seen_vars.end()) {
        throw TupleSpaceException(
          "addVariableGroup",
          "WARNING: variable " + std::to_string(var) +
            " listed twice in variable group definitions. This exception can "
            "be caught if overlapping variable groups are desired (unusual).");
      }
      seen_vars.emplace(var);
    }
  }
  std::sort(group.begin(), group.end());
  variableGroups.push_back(group);
  variableGroupSizes.push_back(group.size());
  int index = variableGroups.size() - 1;
  variableGroupNames.emplace(name, index);
  return index;
}

TupleSpace::tuple_t const&
TupleSpace::getVariableGroup(int index) const
{
  try {
    return variableGroups.at(index);
  } catch (std::out_of_range& e) {
    throw TupleSpaceException("getVariableGroup",
                              "group index " + std::to_string(index) +
                                " out of range.");
  }
}

TupleSpace::tuple_t const&
TupleSpace::getVariableGroup(std::string const& name) const
{
  try {
    return variableGroups.at(variableGroupNames.at(name));
  } catch (std::out_of_range& e) {
    throw TupleSpaceException("getVariableGroup",
                              "group named " + name + " does not exist.");
  }
}

std::vector<std::size_t> const&
TupleSpace::getVariableGroupSizes() const
{
    return variableGroupSizes;
}

std::vector<TupleSpace::tuple_t> const&
TupleSpace::getVariableGroups() const
{
  return variableGroups;
}

std::vector<TupleSpace::tuple_t> const&
TupleSpace::getVariableGroupTuples() const
{
  return variableGroupTuples;
}

#if BOOST_PYTHON_EXTENSIONS
int
TupleSpace::pyAddVariableGroup(std::string const& name, p::list const& list)
{
  int n = p::len(list);
  // copy list in
  algorithm::TupleSpace::tuple_t vars(n);
  for (int ii = 0; ii < n; ii++) {
    p::extract<int> var(list[ii]);
    if (var.check()) {
      vars[ii] = var;
    } else {
      throw TupleSpaceException("pyAddVariableGroup",
                                "Expected list with elements type int");
    }
  }
  return addVariableGroup(name, vars);
}

void
TupleSpace::pyAddVariableGroupTuple(p::list const& list)
{
  int n = p::len(list); // Doesn't give the right answer ...
  algorithm::TupleSpace::tuple_t groups(n);
  for (int ii = 0; ii < n; ii++) {
    p::extract<int> gint(list[ii]); //TODO segaults
    p::extract<std::string> gname(list[ii]);
    if (gint.check()) {
      groups[ii] = p::extract<int>(list[ii]);
    } else if (gname.check()) {
      std::string name(gname);
      try {
        groups[ii] = variableGroupNames.at(name);
      } catch (std::exception& e) {
        throw TupleSpaceException("pyAddVariableGroupTuple",
                                  "group named " + name + " does not exist.");
      }
    } else {
      throw TupleSpaceException("pyAddVariableGroupTuple",
                                "Expected list with elements type int or str");
    }
  }
  addVariableGroupTuple(groups);
}
#endif

static TupleSpace::tuple_index_t
binomial(long double n, long double r)
{
  if (n == 0 || r == 0 || n == r) {
    return 1;
  } else {
    return (TupleSpace::tuple_index_t)(lroundl(n / (n - r) / r *
                          expl(lgammal(n) - lgammal(n - r) - lgammal(r))));
  }
}

static std::vector<std::size_t>
groupSizes(std::vector<TupleSpace::tuple_t> const& groups)
{
  std::vector<std::size_t> group_sizes;
  for (auto& group : groups) {
    group_sizes.push_back(group.size());
  }
  return group_sizes;
}

static TupleSpace::tuple_t
groupAppearances(int d, TupleSpace::tuple_t const& group_tuple, int pos)
{
  // count group appearances following the current index
  TupleSpace::tuple_t app(d, 0);
  for (int gg = pos; gg < group_tuple.size(); gg++) {
    app[group_tuple[gg]]++;
  }
  return app;
}

static TupleSpace::tuple_t
groupAppearances(int d, TupleSpace::tuple_t const& group_tuple)
{
  return groupAppearances(d, group_tuple, 0);
}

TupleSpace::tuple_index_t
TupleSpace::count_tuples_group_tuple(tuple_t const& group_tuple) const
{
  tuple_index_t total = 1;
  auto const& N = this->variableGroupSizes;
  auto d = N.size();
  auto a = groupAppearances(d, group_tuple);
  for (int ii = 0; ii < d; ii++) {
    total *= binomial(N[ii], a[ii]);
  }
  return total;
}

TupleSpace::tuple_index_t
TupleSpace::count_tuples() const
{
  tuple_index_t total = 0;
  for (auto const& group_tuple : this->variableGroupTuples) {
    total += count_tuples_group_tuple(group_tuple);
  }
  return total;
}

// Fast-forward to the group containig the tuple at the target position.
static int
find_group(long* count, long target, TupleSpace const& ts)
{
  auto const& group_tuples = ts.getVariableGroupTuples();
  int gg = 0;
  for (auto const& group_tuple : group_tuples) {
    long skip = ts.count_tuples_group_tuple(group_tuple);
    if ((skip + *count) > target) {
      break;
    }
    *count += skip;
    gg++;
  }
  return gg;
}

// Fast-forward the index at position pos so that the target tuple is found by
// incrementing indexes in the following positions. E.g. if the target 3-tuple
// is (1,4,8), then
//
//      find_index(..., pos=0, ...) returns 1.
//      find_index(..., pos=1, ...) returns 4.
//      find_index(..., pos=2, ...) returns 8.
//
static int
find_index(TupleSpace::tuple_t const& group_tuple,
           int pos,
           long* count,
           long target,
           std::vector<std::size_t> const& N,
           TupleSpace::tuple_t& starts)
{
  int ii = 0;                // index
  int gi = group_tuple[pos]; // group corresponding to index
  auto app = groupAppearances(N.size(), group_tuple, pos + 1);

  for (ii = starts[gi]; ii < N[gi]; ii++) {
    starts[gi] = ii + 1;
    // count the number of tuples skipped over by incrementing index
    // for last index skip = 1 (could be optimized with a special case)
    long skip = 1;
    for (int gg = 0; gg < N.size(); gg++) {
      // binomial combination reduces to linear when appearances == 1
      long f = 1;
      for (int kk = 0; kk < app[gg]; kk++) {
        skip = skip * (N[gg] - starts[gg] - kk);
        f *= (kk + 1);
      }
      skip /= f;
    }
    if ((skip + *count) > target) {
      break;
    }
    *count += skip;
  }
  if (ii >= N[gi]) {
    throw TupleSpaceException("find_index", "Failed to FFW index, out of range");
  }
  return ii;
}


TupleSpace::tuple_t
TupleSpace::find_tuple(long target) const
{
  // The fast-forward algorithm maintains a skipped tuple count so that when
  // the count equals the target count we have found the target tuple.
  long count = 0;

  auto const& N = this->variableGroupSizes;
  auto const& group_tuples = this->variableGroupTuples;

  // scan ahead to the group tuple that generates the target tuple
  int gg = find_group(&count, target, *this);
  auto tuple_size = group_tuples[gg].size();
  tuple_t ret(tuple_size + 1);
  ret[0] = gg;

  // scan to the target tuple
  tuple_t starts(N.size(), 0);
  for (int ii = 0; ii < tuple_size; ii++) {
    ret[ii + 1] = find_index(group_tuples[gg], ii, &count, target, N, starts);
  }

  return ret;
}
