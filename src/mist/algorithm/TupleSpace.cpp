#include "it/Entropy.hpp"
#include "binomial.hpp"
#include <algorithm>
#include <map>
#include <set>

#if BOOST_PYTHON_EXTENSIONS
#include <boost/python/extract.hpp>
#endif

#include "algorithm/TupleSpace.hpp"
#include "it/EntropyCalculator.hpp"

using namespace mist;
using namespace mist::algorithm;

TupleSpace::TupleSpace()
  : tuple_size(0){};
TupleSpace::~TupleSpace(){};

// default space for N variables in tuples size d
TupleSpace::TupleSpace(int N, int d)
  : tuple_size(d)
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

TupleSpace::count_t
TupleSpace::count_tuples_group_tuple(tuple_t const& group_tuple) const
{
  count_t total = 1;
  auto const& N = this->variableGroupSizes;
  auto d = N.size();
  auto a = groupAppearances(d, group_tuple);
  for (int ii = 0; ii < d; ii++) {
    total *= binomial(N[ii], a[ii]);
  }
  return total;
}

TupleSpace::count_t
TupleSpace::count_tuples() const
{
  count_t total = 0;
  for (auto const& group_tuple : this->variableGroupTuples) {
    total += count_tuples_group_tuple(group_tuple);
  }
  return total;
}

// Fast-forward to the group containig the tuple at the target position.
static int
find_group(TupleSpace::count_t* count, TupleSpace::count_t target, TupleSpace const& ts)
{
  auto const& group_tuples = ts.getVariableGroupTuples();
  int gg = 0;
  for (auto const& group_tuple : group_tuples) {
    TupleSpace::count_t skip = ts.count_tuples_group_tuple(group_tuple);
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
           TupleSpace::count_t* count,
           TupleSpace::count_t target,
           std::vector<std::size_t> const& N,
           TupleSpace::tuple_t& starts)
{
  unsigned ii = 0;                // index
  unsigned gi = group_tuple[pos]; // group corresponding to index
  auto app = groupAppearances(N.size(), group_tuple, pos + 1);

  for (ii = starts[gi]; ii < N[gi]; ii++) {
    starts[gi] = ii + 1;
    // count the number of tuples skipped over by incrementing index
    // for last index skip = 1 (could be optimized with a special case)
    TupleSpace::count_t skip = 1;
    for (unsigned gg = 0; gg < N.size(); gg++) {
      // binomial combination reduces to linear when appearances == 1
      TupleSpace::count_t f = 1;
      for (unsigned kk = 0; kk < app[gg]; kk++) {
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
TupleSpace::find_tuple(count_t target) const
{
  // The fast-forward algorithm maintains a skipped tuple count so that when
  // the count equals the target count we have found the target tuple.
  count_t count = 0;

  auto const& N = this->variableGroupSizes;
  auto const& group_tuples = this->variableGroupTuples;

  // scan ahead to the group tuple that generates the target tuple
  unsigned gg = find_group(&count, target, *this);
  auto tuple_size = group_tuples[gg].size();
  tuple_t ret(tuple_size + 1);
  ret[0] = gg;

  // scan to the target tuple
  tuple_t starts(N.size(), 0);
  for (unsigned ii = 0; ii < tuple_size; ii++) {
    ret[ii + 1] = find_index(group_tuples[gg], ii, &count, target, N, starts);
  }

  return ret;
}

static void
traverse_d1(TupleSpace const& ts, TupleSpace::count_t start, TupleSpace::count_t stop, TupleSpaceTraverser& traverser)
{
  auto const& groups = ts.getVariableGroups();
  auto const& group_tuples = ts.getVariableGroupTuples();
  auto const& N = ts.getVariableGroupSizes();
  unsigned ngroups = groups.size();
  unsigned ngtuples = group_tuples.size();

  // tuple generation state
  bool init = true;
  bool work = true;
  auto count = start;
  TupleSpace::tuple_t starts(ngroups);
  starts.assign(ngroups,0);

  // fast-forward to starting group and tuple
  auto ffw = ts.find_tuple(start);

  // sub tuples on the stack
  TupleSpace::tuple_t t0(1);

  for (unsigned gg = ffw[0]; gg < ngtuples && work; gg++) {
    unsigned g0 = group_tuples[gg][0];

    // loop through all tuples generated by this group_tuple
    for (unsigned i0 = (init) ? ffw[1] : starts[g0]; i0 < N[g0] && work; i0++) {
      starts[g0] = i0 + 1;
      auto v0 = groups[g0][i0];
      t0[0] = v0;
      traverser.process_tuple(count, t0);
      count++;
      init = false;
      work = count < stop;
    }
    starts[g0] = 0;
  }
}

static void
traverse_d2(TupleSpace const& ts, TupleSpace::count_t start, TupleSpace::count_t stop, TupleSpaceTraverser& traverser)
{
  auto const& groups = ts.getVariableGroups();
  auto const& group_tuples = ts.getVariableGroupTuples();
  auto const& N = ts.getVariableGroupSizes();
  unsigned ngroups = groups.size();
  unsigned ngtuples = group_tuples.size();

  // tuple generation state
  bool init = true;
  bool work = true;
  auto count = start;
  TupleSpace::tuple_t starts(ngroups);
  starts.assign(ngroups,0);

  // fast-forward to starting group and tuple
  auto ffw = ts.find_tuple(start);

  // sub tuples on the stack
  TupleSpace::tuple_t t01(2);

  for (unsigned gg = ffw[0]; gg < ngtuples && work; gg++) {
    unsigned g0 = group_tuples[gg][0];
    unsigned g1 = group_tuples[gg][1];

    // loop through all tuples generated by this group_tuple
    for (unsigned i0 = (init) ? ffw[1] : starts[g0]; i0 < N[g0] && work; i0++) {
      starts[g0] = i0 + 1;
      auto v0 = groups[g0][i0];
      t01[0] = v0;
      for (unsigned i1 = (init) ? ffw[2] : starts[g1]; i1 < N[g1] && work; i1++) {
        starts[g1] = i1 + 1;
        auto v1 = groups[g1][i1];
        t01[1] = v1;
        traverser.process_tuple(count, t01);
        count++;
        init = false;
        work = count < stop;
      }
      starts[g1] = 0;
    }
    starts[g0] = 0;
  }
}

static void
traverse_d2_entropy(TupleSpace const& ts, TupleSpace::count_t start, TupleSpace::count_t stop, TupleSpaceTraverser& traverser, it::EntropyCalculator & ecalc)
{
  auto const& groups = ts.getVariableGroups();
  auto const& group_tuples = ts.getVariableGroupTuples();
  auto const& N = ts.getVariableGroupSizes();
  unsigned ngroups = groups.size();
  unsigned ngtuples = group_tuples.size();

  // tuple generation state
  bool init = true;
  bool work = true;
  auto count = start;
  TupleSpace::tuple_t starts(ngroups);
  starts.assign(ngroups,0);
  it::Entropy entropy((unsigned)it::d2::size);

  // fast-forward to starting group and tuple
  auto ffw = ts.find_tuple(start);

  // sub tuples on the stack
  TupleSpace::tuple_t t0(1);
  TupleSpace::tuple_t t1(1);
  TupleSpace::tuple_t t01(2);

  for (unsigned gg = ffw[0]; gg < ngtuples && work; gg++) {
    unsigned g0 = group_tuples[gg][0];
    unsigned g1 = group_tuples[gg][1];

    // loop through all tuples generated by this group_tuple
    for (unsigned i0 = (init) ? ffw[1] : starts[g0]; i0 < N[g0] && work; i0++) {
      starts[g0] = i0 + 1;
      auto v0 = groups[g0][i0];
      t0[0] = v0;
      t01[0] = v0;
      entropy[(unsigned)it::d2::e0] = ecalc.entropy(t0);
      for (unsigned i1 = (init) ? ffw[2] : starts[g1]; i1 < N[g1] && work; i1++) {
        starts[g1] = i1 + 1;
        auto v1 = groups[g1][i1];
        t1[0] = v1;
        t01[1] = v1;
        entropy[(unsigned)it::d2::e1]  = ecalc.entropy(t1);
        entropy[(unsigned)it::d2::e01] = ecalc.entropy(t01);
        traverser.process_tuple_entropy(count, t01, entropy);
        count++;
        init = false;
        work = count < stop;
      }
      starts[g1] = 0;
    }
    starts[g0] = 0;
  }
}

static void
traverse_d3(TupleSpace const& ts, TupleSpace::count_t start, TupleSpace::count_t stop, TupleSpaceTraverser& traverser)
{
  auto const& groups = ts.getVariableGroups();
  auto const& group_tuples = ts.getVariableGroupTuples();
  auto const& N = ts.getVariableGroupSizes();
  unsigned ngroups = groups.size();
  unsigned ngtuples = group_tuples.size();

  // tuple generation state
  bool init = true;
  bool work = true;
  auto count = start;
  TupleSpace::tuple_t starts(ngroups);
  starts.assign(ngroups,0);

  // fast-forward to starting group and tuple
  auto ffw = ts.find_tuple(start);

  // sub tuples on the stack
  TupleSpace::tuple_t t012(3);

  for (unsigned gg = ffw[0]; gg < ngtuples && work; gg++) {
    unsigned g0 = group_tuples[gg][0];
    unsigned g1 = group_tuples[gg][1];
    unsigned g2 = group_tuples[gg][2];

    // loop through all tuples generated by this group_tuple
    for (unsigned i0 = (init) ? ffw[1] : starts[g0]; i0 < N[g0] && work; i0++) {
      starts[g0] = i0 + 1;
      auto v0 = groups[g0][i0];
      t012[0] = v0;
      for (unsigned i1 = (init) ? ffw[2] : starts[g1]; i1 < N[g1] && work; i1++) {
        starts[g1] = i1 + 1;
        auto v1 = groups[g1][i1];
        t012[1] = v1;
        for (unsigned i2 = (init) ? ffw[3] : starts[g2]; i2 < N[g2] && work; i2++) {
          starts[g2] = i2 + 1;
          auto v2 = groups[g2][i2];
          t012[2] = v2;
          traverser.process_tuple(count, t012);
          count++;
          init = false;
          work = count < stop;
        }
        starts[g2] = 0;
      }
      starts[g1] = 0;
    }
    starts[g0] = 0;
  }
}

static void
traverse_d3_entropy(TupleSpace const& ts, TupleSpace::count_t start, TupleSpace::count_t stop, TupleSpaceTraverser& traverser, it::EntropyCalculator & ecalc)
{
  auto const& groups = ts.getVariableGroups();
  auto const& group_tuples = ts.getVariableGroupTuples();
  auto const& N = ts.getVariableGroupSizes();
  unsigned ngroups = groups.size();
  unsigned ngtuples = group_tuples.size();

  // tuple generation state
  bool init = true;
  bool work = true;
  auto count = start;
  TupleSpace::tuple_t starts(ngroups);
  starts.assign(ngroups,0);
  it::Entropy entropy((unsigned)it::d3::size);

  // sub tuples on the stack
  TupleSpace::tuple_t t0(1);
  TupleSpace::tuple_t t1(1);
  TupleSpace::tuple_t t2(1);
  TupleSpace::tuple_t t01(2);
  TupleSpace::tuple_t t02(2);
  TupleSpace::tuple_t t12(2);
  TupleSpace::tuple_t t012(3);

  // fast-forward to starting group and tuple
  auto ffw = ts.find_tuple(start);

  for (unsigned gg = ffw[0]; gg < ngtuples && work; gg++) {
    unsigned g0 = group_tuples[gg][0];
    unsigned g1 = group_tuples[gg][1];
    unsigned g2 = group_tuples[gg][2];

    // loop through all tuples generated by this group_tuple
    for (unsigned i0 = (init) ? ffw[1] : starts[g0]; i0 < N[g0] && work; i0++) {
      starts[g0] = i0 + 1;
      auto v0 = groups[g0][i0];
      t0[0] = v0;
      t02[0] = v0;
      t01[0] = v0;
      t012[0] = v0;
      entropy[(unsigned)it::d3::e0] = ecalc.entropy(t0);
      for (unsigned i1 = (init) ? ffw[2] : starts[g1]; i1 < N[g1] && work; i1++) {
        starts[g1] = i1 + 1;
        auto v1 = groups[g1][i1];
        t1[0] = v1;
        t01[1] = v1;
        t12[0] = v1;
        t012[1] = v1;
        entropy[(unsigned)it::d3::e1]  = ecalc.entropy(t1);
        entropy[(unsigned)it::d3::e01] = ecalc.entropy(t01);
        for (unsigned i2 = (init) ? ffw[3] : starts[g2]; i2 < N[g2] && work; i2++) {
          starts[g2] = i2 + 1;
          auto v2 = groups[g2][i2];
          t2[0] = v2;
          t02[1] = v2;
          t12[1] = v2;
          t012[2] = v2;
          entropy[(unsigned)it::d3::e2]   = ecalc.entropy(t2);
          entropy[(unsigned)it::d3::e02]  = ecalc.entropy(t02);
          entropy[(unsigned)it::d3::e12]  = ecalc.entropy(t12);
          entropy[(unsigned)it::d3::e012] = ecalc.entropy(t012);
          traverser.process_tuple_entropy(count, t012, entropy);
          count++;
          init = false;
          work = count < stop;
        }
        starts[g2] = 0;
      }
      starts[g1] = 0;
    }
    starts[g0] = 0;
  }
}

void
TupleSpace::traverse(TupleSpaceTraverser& traverser) const
{
  traverse(0, -1, traverser);
}

void
TupleSpace::traverse_entropy(it::EntropyCalculator &ecalc, TupleSpaceTraverser& traverser) const
{
  traverse_entropy(0, -1, ecalc, traverser);
}

void
TupleSpace::traverse(count_t start, count_t stop, TupleSpaceTraverser& traverser) const
{
  switch(tuple_size) {
    case 1: traverse_d1(*this, start, stop, traverser); break;
    case 2: traverse_d2(*this, start, stop, traverser); break;
    case 3: traverse_d3(*this, start, stop, traverser); break;
    default:
      throw TupleSpaceException("traverse", "Tuple size unsupported.");
  }
}

void
TupleSpace::traverse_entropy(count_t start, count_t stop, it::EntropyCalculator &ecalc, TupleSpaceTraverser& traverser) const
{
  switch(tuple_size) {
    case 1:
      throw TupleSpaceException("traverse_entropy", "Tuple size 1 unsupported.");
    case 2: traverse_d2_entropy(*this, start, stop, traverser, ecalc); break;
    case 3: traverse_d3_entropy(*this, start, stop, traverser, ecalc); break;
    default:
      throw TupleSpaceException("traverse", "Tuple size greater than 3 unsupported.");
  }
}
