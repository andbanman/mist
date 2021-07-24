#include <iostream>
#include <functional>

#include "algorithm/Worker.hpp"
#include "it/Entropy.hpp"
#include "it/SymmetricDelta.hpp"

using namespace mist;
using namespace mist::algorithm;

void
Worker::process_tuple(count_t tuple_no, tuple_t const& tuple)
{
  auto result = this->measure->compute(*this->calc, tuple);
  for (auto& out : out_streams) {
    if (this->output_all) {
      out->push(tuple_no, tuple, result);
    } else {
      out->push(tuple_no, tuple,
                it::Measure::result_type(result.end() - 1, result.end()));
    }
  }
}

void
Worker::process_tuple_entropy(count_t tuple_no, tuple_t const& tuple, it::Entropy const& e)
{
  auto result = this->measure->compute(*this->calc, tuple, e);
  for (auto& out : out_streams) {
    if (this->output_all) {
      out->push(tuple_no, tuple, result);
    } else {
      out->push(tuple_no, tuple,
                it::Measure::result_type(result.end() - 1, result.end()));
    }
  }
}

void
Worker::start()
{
  TupleSpace::index_t total = (limit) ? limit : ts.count_tuples();
  TupleSpace::index_t step = (total + this->ranks - 1) / this->ranks; // round up
  TupleSpace::index_t start = step * (this->rank);
  TupleSpace::index_t stop = start + step;
  bool full = measure->full_entropy();

  if (rank == ranks - 1) {
    stop = total;
  }

  // rounding can cause the start to go past the end, stop if theres' no work
  // for this rank to do
  if (start >= total) {
    return;
  }

  if (full) {
    ts.traverse_entropy(start, stop, *calc.get(), *this);
  }
  else {
    ts.traverse(start, stop,  *this);
  }
}

Worker::~Worker() {}
Worker::Worker() {}

Worker::Worker(int rank,
               int ranks,
               long limit,
               TupleSpace const& ts,
               entropy_calc_ptr calc,
               std::vector<output_stream_ptr> out_streams,
               measure_ptr measure)
  : ts(ts)
  , rank(rank)
  , ranks(ranks)
  , limit(limit)
  , calc(calc)
  , out_streams(out_streams)
  , measure(measure)
{
  // cannot set these in member initialization list?
  if (ts.getVariableGroups().empty()) {
    throw WorkerException("Worker", "TuplesSpace variable groups empty.");
  }
  if (ts.getVariableGroupTuples().empty()) {
    throw WorkerException("Worker", "TuplesSpace variable group tuples empty.");
  }
  for (auto& out : out_streams) {
    if (!out) {
      throw WorkerException("Worker", "Invalid output stream");
    }
  }
}
