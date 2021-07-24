#pragma once

#include <memory>

#include "algorithm/TupleSpace.hpp"
#include "io/OutputStream.hpp"
#include "it/EntropyCalculator.hpp"
#include "it/Measure.hpp"
#include "Variable.hpp"

namespace mist {
namespace algorithm {

/** The Worker class divides and conquers the tuple search space.
 *
 * The Worker processes each tuple in the configured search space, or a portion
 * of the search space depending on the rank parameters. It is common for each
 * computing thread on the system to have a unique Worker instance.
 */
class Worker : public TupleSpaceTraverser
{
public:
  using entropy_calc_ptr = std::shared_ptr<it::EntropyCalculator>;
  using output_stream_ptr = std::shared_ptr<io::OutputStream>;
  using measure_ptr = std::shared_ptr<it::Measure>;
  using tuple_t = Variable::indexes;
  using count_t = TupleSpace::count_t;

  // Typedefs for convenience expressing the algorithm

  ~Worker();
  Worker();
  /** Construct and configure a Worker instance.
   *
   * @param rank Zero-indexed rank number [0, ranks]
   * @param ranks Total number of Workers participating in the search
   * @param limit Upper limit on number of tuples to processes by all Workers
   * @param ts TupleSpace that defines the tuple search space
   * @param out_streams Collection OutputStream pointers to send results
   * @param measure The it::Measure to calculate the results
   */
  Worker(int rank,
         int ranks,
         long limit,
         TupleSpace const& ts,
         entropy_calc_ptr calc,
         std::vector<output_stream_ptr> out_streams,
         measure_ptr measure);

  /** Start the Worker search space execution. Returns when all tuples in the
   * search space have been processed.
   */
  void start();

  bool output_all = false;

  void process_tuple(count_t tuple_no, tuple_t const& tuple);
  void process_tuple_entropy(count_t tuple_no, tuple_t const& tuple, it::Entropy const& e);

private:
  TupleSpace ts;
  entropy_calc_ptr calc;
  std::vector<output_stream_ptr> out_streams;
  measure_ptr measure;
  int rank;
  int ranks;  // total number of ranks
  long limit; // maximum number of tuples to process

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
