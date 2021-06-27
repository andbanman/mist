#include <cmath>

#include <iostream> //DEBUG

#include "algorithm/Worker.hpp"
#include "it/Entropy.hpp"
#include "it/SymmetricDelta.hpp"

using namespace mist;
using namespace mist::algorithm;

using appearances_t = Worker::appearances_t;
using group_t = Worker::group_t;
using groups_t = Worker::groups_t;
using group_tuple_t = Worker::group_tuple_t;
using group_tuples_t = Worker::group_tuples_t;
using group_sizes_t = Worker::group_sizes_t;
using starts_t = Worker::starts_t;

long
binomial(long double n, long double r)
{
  if (n == 0 || r == 0 || n == r) {
    return 1;
  } else {
    return (long)(lroundl(n / (n - r) / r *
                          expl(lgammal(n) - lgammal(n - r) - lgammal(r))));
  }
}

void
Worker::processTuple(std::vector<int> const& tuple)
{
  auto result = this->measure->compute(*this->calc, tuple);
  if (this->out) {
    if (this->output_all) {
      this->out->push(tuple, result);
    } else {
      this->out->push(tuple,
                      it::Measure::result_type(result.end() - 1, result.end()));
    }
  }
}

void
Worker::processTuple(std::vector<int> const& tuple, it::Entropy const& e)
{
  auto result = this->measure->compute(*this->calc, tuple, e);
  if (this->out) {
    if (this->output_all) {
      this->out->push(tuple, result);
    } else {
      this->out->push(tuple,
                      it::Measure::result_type(result.end() - 1, result.end()));
    }
  }
}

static appearances_t
groupAppearances(int d, group_tuple_t const& group_tuple, int pos)
{
  // count group appearances following the current index
  appearances_t app(d, 0);
  for (int gg = pos; gg < group_tuple.size(); gg++) {
    app[group_tuple[gg]]++;
  }
  return app;
}

static appearances_t
groupAppearances(int d, group_tuple_t const& group_tuple)
{
  return groupAppearances(d, group_tuple, 0);
}

static group_sizes_t
groupSizes(groups_t const& groups)
{
  group_sizes_t group_sizes;
  for (auto& group : groups) {
    group_sizes.push_back(group.size());
  }
  return group_sizes;
}

static long
countTuplesGroupTuple(group_sizes_t const& N, group_tuple_t const& group_tuple)
{
  long total = 1;
  auto d = N.size();
  auto a = groupAppearances(d, group_tuple);
  for (int ii = 0; ii < d; ii++) {
    total *= binomial(N[ii], a[ii]);
  }
  return total;
}

static long
countTuples(group_sizes_t const& N, group_tuples_t const& group_tuples)
{
  long total = 0;
  for (auto& group_tuple : group_tuples) {
    total += countTuplesGroupTuple(N, group_tuple);
  }
  return total;
}

// Fast-forward to the group containig the tuple at the target position.
static int
find_group(long* count,
           long target,
           group_tuples_t const& group_tuples,
           group_sizes_t const& N)
{
  int numtuples = group_tuples.size();
  int gg = 0;
  for (; gg < numtuples; gg++) {
    long skip = countTuplesGroupTuple(N, group_tuples[gg]);
    if ((skip + *count) > target) {
      break;
    }
    *count += skip;
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
find_index(group_tuple_t const& group_tuple,
           int pos,
           long* count,
           long target,
           group_sizes_t const& N,
           starts_t& starts)
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
    throw WorkerException("find_index", "Failed to FFW index, out of range");
  }
  return ii;
}

// Given the defined variable groups and group tuples, which generate an
// ordered list of tuples, find the the tuple at postition target.
static std::vector<int>
find_tuple(group_tuples_t const& group_tuples,
           group_sizes_t const& N,
           long target)
{
  // The fast-forward algorithm maintains a skipped tuple count so that when
  // the count equals the target count we have found the target tuple.
  long count = 0;

  // scan ahead to the group tuple that generates the target tuple
  int gg = find_group(&count, target, group_tuples, N);
  auto tuple_size = group_tuples[gg].size();
  std::vector<int> ret(tuple_size + 1);
  ret[0] = gg;

  // scan to the target tuple
  starts_t starts(N.size(), 0);
  for (int ii = 0; ii < tuple_size; ii++) {
    ret[ii + 1] = find_index(group_tuples[gg], ii, &count, target, N, starts);
  }

  return ret;
}

void
Worker::search_d1(long start, long stop, bool full)
{
  int ngroups = groups.size();
  int ngtuples = group_tuples.size();
  auto N = groupSizes(groups);

  // tuple generation state
  bool init = true;
  bool work = true;
  int count = start;
  starts_t starts(ngroups);
  it::Entropy entropy((int)it::d1::size);
  std::vector<int> tuple(1);

  // fast-forward to starting group and tuple
  auto ffw = find_tuple(group_tuples, N, start);

  // computation step
  for (int gg = ffw[0]; gg < ngtuples; gg++) {
    int gi = group_tuples[gg][0];
    for (int ii = (init) ? ffw[1] : 0; ii < N[gi] && work; ii++) {
      tuple[0] = groups[gi][ii];
      if (full) {
        entropy[(int)it::d1::e0] = this->calc->entropy({ tuple[0] });
        processTuple(tuple, entropy);
      } else {
        processTuple(tuple);
      }
      count++;
      init = false;
      work = count < stop;
    }
  }
}

void
Worker::search_d2(long start, long stop, bool full)
{
  int ngroups = groups.size();
  int ngtuples = group_tuples.size();
  auto N = groupSizes(groups);

  // tuple generation state
  bool init = true;
  bool work = true;
  int count = start;
  starts_t starts(ngroups);
  it::Entropy entropy((int)it::d2::size);
  std::vector<int> tuple(2);

  // fast-forward to starting group and tuple
  auto ffw = find_tuple(group_tuples, N, start);

  // computation step
  for (int gg = ffw[0]; gg < ngtuples; gg++) {
    int gi = group_tuples[gg][0];
    int gj = group_tuples[gg][1];
    starts[gi] = 0;

    // loop through all tuples generated by this group tuple
    for (int ii = (init) ? ffw[1] : starts[gi]; ii < N[gi] && work; ii++) {
      starts[gi] = ii + 1;
      tuple[0] = groups[gi][ii];
      if (full) {
        entropy[(int)it::d2::e0] = this->calc->entropy({ tuple[0] });
      }
      for (int jj = (init) ? ffw[2] : starts[gj]; jj < N[gj] && work; jj++) {
        starts[gj] = jj + 1;
        tuple[1] = groups[gj][jj];
        if (full) {
          entropy[(int)it::d2::e1] = this->calc->entropy({ tuple[1] });
          entropy[(int)it::d2::e01] =
            this->calc->entropy({ tuple[0], tuple[1] });
          processTuple(tuple, entropy);
        } else {
          processTuple(tuple);
        }
        count++;
        init = false;
        work = count < stop;
      }
    }
  }
}

void
Worker::search_d3(long start, long stop, bool full)
{
  int ngroups = groups.size();
  int ngtuples = group_tuples.size();
  auto N = groupSizes(groups);

  // tuple generation state
  bool init = true;
  bool work = true;
  int count = start;
  starts_t starts(ngroups);
  it::Entropy entropy((int)it::d3::size);
  std::vector<int> tuple(3);

  // fast-forward to starting group and tuple
  auto ffw = find_tuple(group_tuples, N, start);

  // computation step
  for (int gg = ffw[0]; gg < ngtuples; gg++) {
    int gi = group_tuples[gg][0];
    int gj = group_tuples[gg][1];
    int gk = group_tuples[gg][2];
    starts[gi] = 0;

    // loop through all tuples generated by this group_tuple
    for (int ii = (init) ? ffw[1] : starts[gi]; ii < N[gi] && work; ii++) {
      starts[gi] = ii + 1;
      tuple[0] = groups[gi][ii];
      if (full) {
        entropy[(int)it::d3::e0] = this->calc->entropy({ tuple[0] });
      }
      for (int jj = (init) ? ffw[2] : starts[gj]; jj < N[gj] && work; jj++) {
        starts[gj] = jj + 1;
        tuple[1] = groups[gj][jj];
        if (full) {
          entropy[(int)it::d3::e1] = this->calc->entropy({ tuple[1] });
          entropy[(int)it::d3::e01] =
            this->calc->entropy({ tuple[0], tuple[1] });
        }
        for (int kk = (init) ? ffw[3] : starts[gk]; kk < N[gk] && work; kk++) {
          starts[gk] = kk + 1;
          tuple[2] = groups[gk][kk];
          if (full) {
            entropy[(int)it::d3::e2] = this->calc->entropy({ tuple[2] });
            entropy[(int)it::d3::e02] =
              this->calc->entropy({ tuple[0], tuple[2] });
            entropy[(int)it::d3::e12] =
              this->calc->entropy({ tuple[1], tuple[2] });
            entropy[(int)it::d3::e012] = this->calc->entropy(tuple);
            processTuple(tuple, entropy);
          } else {
            processTuple(tuple);
          }
          count++;
          init = false;
          work = count < stop;
        }
      }
    }
  }
}

void
Worker::start()
{
  int total = countTuples(groupSizes(groups), group_tuples);
  int step = (total + this->ranks - 1) / this->ranks; // round up
  int start = step * (this->rank);
  int stop = start + step;
  int d = group_tuples.front().size();
  bool full = (dynamic_cast<it::SymmetricDelta*>(measure.get()));

  // hard stop the last rank
  if (rank == ranks - 1) {
    stop = total;
  }

  // rounding can cause the start to go past the end, stop if theres' no work
  // for this rank to do
  if (start >= total) {
    return;
  }

  switch (d) {
    case 1:
      search_d1(start, stop, full);
      break;
    case 2:
      search_d2(start, stop, full);
      break;
    case 3:
      search_d3(start, stop, full);
      break;
    default:
      throw WorkerException("start",
                            "Unsupported tuple size " + std::to_string(d) +
                              " for full-entropy measure, valid range [1,3]");
  }
}

Worker::~Worker() {}
Worker::Worker() {}

Worker::Worker(int rank,
               int ranks,
               TupleSpace const& ts,
               entropy_calc_ptr calc,
               output_stream_ptr out,
               measure_ptr measure)
  : rank(rank)
  , ranks(ranks)
  , calc(calc)
  , out(out)
  , measure(measure)
{
  // cannot set these in member initialization list?
  groups = ts.getVariableGroups();
  group_tuples = ts.getVariableGroupTuples();
  if (groups.empty()) {
    throw WorkerException("Worker", "TuplesSpace variable groups empty.");
  }
  if (group_tuples.empty()) {
    throw WorkerException("Worker", "TuplesSpace variable group tuples empty.");
  }
}
