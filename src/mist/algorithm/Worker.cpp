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
  if (result.back() < cutoff) {
    return;
  }
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
  if (result.back() < cutoff) {
    return;
  }
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
  bool full = measure->full_entropy();

  if (full) {
    ts.traverse_entropy(start_no, stop_no, *calc.get(), *this);
  }
  else {
    ts.traverse(start_no, stop_no,  *this);
  }
}

Worker::~Worker() {}
Worker::Worker() {}

Worker::Worker(TupleSpace const& ts,
               count_t start,
               count_t stop,
               entropy_calc_ptr calc,
               std::vector<output_stream_ptr> out_streams,
               measure_ptr measure)
  : Worker(
      ts,
      start,
      stop,
      -std::numeric_limits<double>::infinity(),
      calc,
      out_streams,
      measure)
{
}

Worker::Worker(TupleSpace const& ts,
               count_t start,
               count_t stop,
               result_t cutoff,
               entropy_calc_ptr calc,
               std::vector<output_stream_ptr> out_streams,
               measure_ptr measure)
  : ts(ts)
  , start_no(start)
  , stop_no(stop)
  , cutoff(cutoff)
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
