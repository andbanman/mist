#pragma once

#include <memory>

#include "algorithm/TupleSpace.hpp"
#include "io/OutputStream.hpp"
#include "it/EntropyCalculator.hpp"
#include "it/Measure.hpp"

namespace mist {
namespace algorithm {

class Worker
{
public:
  using entropy_calc_ptr = std::shared_ptr<it::EntropyCalculator>;
  using output_stream_ptr = std::shared_ptr<io::OutputStream>;
  using measure_ptr = std::shared_ptr<it::Measure>;

  // Typedefs for convenience expressing the algorithm
  using group_t = TupleSpace::tuple_type;
  using groups_t = std::vector<group_t>;
  using group_tuple_t = TupleSpace::tuple_type;
  using group_tuples_t = std::vector<group_tuple_t>;
  using appearances_t = std::vector<int>;
  using starts_t = std::vector<int>;
  using group_sizes_t = std::vector<int>;

  ~Worker();
  Worker();
  Worker(int rank,
         int threads,
         TupleSpace const& ts,
         entropy_calc_ptr calc,
         output_stream_ptr out,
         measure_ptr measure);

  void start();

private:
  TupleSpace ts;
  entropy_calc_ptr calc;
  output_stream_ptr out;
  measure_ptr measure;
  groups_t groups;
  group_tuples_t group_tuples;
  int rank;
  int ranks; // total number of ranks
  bool output_all = false;

  void processTuple(std::vector<int> const& tuple);
  void processTuple(std::vector<int> const& tuple, it::Entropy const& e);
  void search_d1(long start, long stop, bool full);
  void search_d2(long start, long stop, bool full);
  void search_d3(long start, long stop, bool full);
};

class WorkerException : public std::exception
{
private:
  std::string msg;

public:
  WorkerException(std::string const& method, std::string const& msg)
    : msg("Worker::" + method + " : " + msg)
  {}
  virtual const char* what() const throw() { return msg.c_str(); };
};
} // namespace algorithm
} // namespace mist
