#pragma once

#include <memory>

#include "algorithm/TupleSpace.hpp"
#include "io/OutputStream.hpp"
#include "it/Distribution.hpp"
#include "it/Entropy.hpp"
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
  using result_t = it::entropy_type;

  // Typedefs for convenience expressing the algorithm

  ~Worker();
  Worker();
  /** Construct and configure a Worker instance.
   *
   * @param ts TupleSpace that defines the tuple search space
   * @param start Start processing at start tuple number
   * @param stop Stop processing when stop tuple number is reached
   * @param cutoff Discard all tuples from output with a measure less than cutoff
   * @param out_streams Collection OutputStream pointers to send results
   * @param measure The it::Measure to calculate the results
   */
  Worker(TupleSpace const& ts,
         count_t start,
         count_t stop,
         result_t cutoff,
         entropy_calc_ptr calc,
         std::vector<output_stream_ptr> out_streams,
         measure_ptr measure);

  /** Construct and configure a Worker instance.
   *
   * Cutoff is not used in the this instance.
   */
  Worker(TupleSpace const& ts,
         count_t start,
         count_t stop,
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
  count_t start_no;
  count_t stop_no;
  result_t cutoff;
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
