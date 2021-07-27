#include <iostream>
#include <functional>

#include "algorithm/Worker.hpp"
#include "it/Entropy.hpp"
#include "it/EntropyCalculator.hpp"
#include "it/SymmetricDelta.hpp"

using namespace mist;
using namespace mist::algorithm;

void
Worker::process_tuple(count_t tuple_no, tuple_t const& tuple)
{
  this->measure->compute(*this->calc, tuple, this->result);
  if (result.back() < cutoff) {
    return;
  }
  for (auto& out : out_streams) {
    if (this->output_all) {
      out->push(tuple_no, tuple, result);
    } else {
      out->push(tuple_no, tuple, result.back());
    }
  }
}

void
Worker::process_tuple_entropy(count_t tuple_no, tuple_t const& tuple, it::Entropy const& e)
{
  this->measure->compute(*this->calc, tuple, e, this->result);
  if (result.back() < cutoff) {
    return;
  }
  for (auto& out : out_streams) {
    if (this->output_all) {
      out->push(tuple_no, tuple, result);
    } else {
      out->push(tuple_no, tuple, result.back());
    }
  }
}

void
Worker::start()
{
  bool full = measure->full_entropy();

  if (full) {
    ts->traverse_entropy(start_no, stop_no, *calc.get(), *this);
  }
  else {
    ts->traverse(start_no, stop_no, *this);
  }
}

Worker::~Worker() {}
Worker::Worker() {}

Worker::Worker(tuple_space_ptr const& ts,
               count_t start,
               count_t stop,
               entropy_calc_ptr & calc,
               std::vector<output_stream_ptr> const& out_streams,
               measure_ptr const& measure)
  : Worker(ts,
           start,
           stop,
           -std::numeric_limits<double>::infinity(),
           calc,
           out_streams,
           measure)
{
}

Worker::Worker(Worker const& other)
  : ts(other.ts)
  , start_no(other.start_no)
  , stop_no(other.stop_no)
  , cutoff(other.cutoff)
  , calc(new it::EntropyCalculator(*other.calc))
  , out_streams(other.out_streams)
  , measure(other.measure)
{}

Worker&
Worker::operator=(Worker const& other)
{
  ts = other.ts;
  start_no = other.start_no;
  stop_no = other.stop_no;
  cutoff = other.cutoff;
  calc = entropy_calc_ptr(new it::EntropyCalculator(*other.calc));
  out_streams = other.out_streams;
  measure = other.measure;
  return *this;
}

Worker::Worker(tuple_space_ptr const& ts,
               count_t start,
               count_t stop,
               result_t cutoff,
               entropy_calc_ptr & calc,
               std::vector<output_stream_ptr> const& out_streams,
               measure_ptr const& measure)
  : ts(ts)
  , start_no(start)
  , stop_no(stop)
  , cutoff(cutoff)
  , calc(std::move(calc))
  , out_streams(out_streams)
  , measure(measure)
{
  // cannot set these in member initialization list?
  if (ts->getVariableGroups().empty()) {
    throw WorkerException("Worker", "TuplesSpace variable groups empty.");
  }
  if (ts->getVariableGroupTuples().empty()) {
    throw WorkerException("Worker", "TuplesSpace variable group tuples empty.");
  }
  for (auto& out : out_streams) {
    if (!out) {
      throw WorkerException("Worker", "Invalid output stream");
    }
  }
}
