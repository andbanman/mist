#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <limits>
#include <queue>
#include <memory>
#include <stdexcept>
#include <thread>
#include <vector>

#include "Search.hpp"
#include "algorithm/TupleSpace.hpp"
#include "algorithm/Worker.hpp"
#include "io/DataMatrix.hpp"
#include "io/MapOutputStream.hpp"
#include "it/BitsetCounter.hpp"
#include "it/Entropy.hpp"
#include "it/EntropyCalculator.hpp"
#include "it/SymmetricDelta.hpp"
#include "it/VectorCounter.hpp"

using namespace mist;

using data_ptr = std::shared_ptr<io::DataMatrix>;
using output_stream_ptr = std::shared_ptr<io::OutputStream>;
using file_stream_ptr = std::shared_ptr<io::FileOutputStream>;
using map_stream_ptr = std::shared_ptr<io::MapOutputStream>;
using flat_stream_ptr = std::shared_ptr<io::FlatOutputStream>;
using measure_ptr = std::shared_ptr<it::Measure>;
using cache_ptr = it::EntropyCalculator::cache_ptr_type;
using entropy_calc_ptr = std::unique_ptr<it::EntropyCalculator>;
using counter_ptr = std::shared_ptr<it::Counter>;
using tuple_space_ptr = std::shared_ptr<algorithm::TupleSpace>;
using variables_ptr = std::shared_ptr<Variable::tuple>;

std::string
Search::version()
{
  return MIST_VERSION;
}

enum struct probability_algorithms : int
{
  vector,
  bitset
};

struct thread_config
{
  measure_ptr measure;
  output_stream_ptr output_stream;
  std::vector<cache_ptr> caches;
};

struct Search::impl
{
  // structures
  data_ptr data;
  file_stream_ptr file_output;
  measure_ptr measure;
  counter_ptr counter;
  std::string measure_str;
  std::vector<cache_ptr> shared_caches;
  std::vector<flat_stream_ptr> mem_outputs;
  std::vector<thread_config> threads;

  // config
  it::entropy_type cutoff = -std::numeric_limits<it::entropy_type>::infinity();
  unsigned long cache_size_bytes = 0;
  algorithm::TupleSpace::index_t tuple_limit = 0;
  bool use_cache = true;
  bool full_output = false;
  bool in_memory_output = true;
  bool use_cutoff = false;
  bool show_progress = false;
  // whether this Search is participating in a parallel search
  bool parallel_search = false;
  int ranks;
  int start_rank;
  int total_ranks;
  int tuple_size;
  probability_algorithms probability_algorithm;
  std::string probability_algorithm_str;
  std::string outfile;
  tuple_space_ptr tuple_space;
};

Search::Search()
  : pimpl(std::make_unique<impl>())
{
  // structures
  pimpl->measure = measure_ptr(new it::SymmetricDelta());
  pimpl->measure_str = "SymmetricDelta";
  pimpl->data = 0;

  // default config
  pimpl->ranks = std::thread::hardware_concurrency();
  pimpl->total_ranks = pimpl->ranks;
  pimpl->start_rank = 0;
  pimpl->tuple_size = 2;
  pimpl->probability_algorithm = probability_algorithms::vector;
  pimpl->probability_algorithm_str = "Vector"; // TODO enumerate elswhere
};

Search::Search(const Search& other)
  : pimpl(std::make_unique<impl>())
{
  *this->pimpl = *other.pimpl;
};

Search::Search(Search&&) = default;
Search&
Search::operator=(Search&&) = default;
Search::~Search() {}

void
Search::set_measure(std::string const& measure)
{
  std::string test(measure);
  transform(test.begin(), test.end(), test.begin(), ::tolower);

  if (test == "symmetricdelta") {
    pimpl->measure = measure_ptr(new it::SymmetricDelta());
    pimpl->measure_str = "SymmetricDelta";
  } else if (test == "entropy") {
    pimpl->measure = measure_ptr(new it::EntropyMeasure());
    pimpl->measure_str = "JointEntropy";
  } else {
    throw SearchException("set_measure",
                          "Invalid measure: " + measure +
                            ", allowed: [SymmetricDelta,Entropy]");
  }
}

std::string
Search::get_measure()
{
  return pimpl->measure_str;
}

void
Search::set_cutoff(it::entropy_type cutoff)
{
  pimpl->cutoff = cutoff;
  pimpl->use_cutoff = true;
}
it::entropy_type
Search::get_cutoff()
{
  return pimpl->cutoff;
}


void
Search::set_probability_algorithm(std::string const& algorithm)
{
  std::string test(algorithm);
  transform(test.begin(), test.end(), test.begin(), ::tolower);

  if (test == "bitset") {
    pimpl->probability_algorithm = probability_algorithms::bitset;
    pimpl->probability_algorithm_str = "Bitset";
  } else if (test == "vector") {
    pimpl->probability_algorithm = probability_algorithms::vector;
    pimpl->probability_algorithm_str = "Vector";
  } else {
    throw SearchException("set_probability_algorithm",
                          "Invalid probability algorithm : " + algorithm +
                            ", allowed: [bitset, vector]");
  }
}
std::string
Search::get_probability_algorithm()
{
  return pimpl->probability_algorithm_str;
}

void
Search::set_outfile(std::string const& filename)
{
  // Thread copies of FileOutputStream should be constructed from
  // this object so they share an underlying file stream
  pimpl->outfile = filename;
  pimpl->in_memory_output = false;
}
std::string
Search::get_outfile()
{
  return pimpl->outfile;
}

#if BOOST_PYTHON_EXTENSIONS
//
// Returns python numpy ndarray of variable indexes with the result set
//
np::ndarray
Search::python_get_results()
{
  if (pimpl->mem_outputs.empty()) {
    // don't throw, just give the goose
    return np::zeros(p::make_tuple(0, 0), np::dtype::get_builtin<double>());
  } else {
    return pimpl->mem_outputs.front()->py_get_results();
  }
}
#endif

// Returns a copy of the results collected into one data structure
// TODO reshape to the correct view...
std::vector<it::entropy_type> const&
Search::get_results()
{
  if (pimpl->mem_outputs.empty()) {
    throw SearchException("get_results", "No results in memory");
  } else {
    return pimpl->mem_outputs.front()->get_results();
  }
}

void
Search::set_tuple_size(int size)
{
  if (size < 2 || size > 4) {
    throw SearchException("set_tuple_size",
                          "Invalid tuple size " + std::to_string(size) +
                            ", valid range is [2,4]");
  }
  pimpl->tuple_size = size;
}
int
Search::get_tuple_size()
{
  return pimpl->tuple_size;
}

void
Search::set_tuple_space(algorithm::TupleSpace const& ts)
{
  pimpl->tuple_space = tuple_space_ptr(new algorithm::TupleSpace(ts));
  pimpl->tuple_size = ts.tupleSize();
}
algorithm::TupleSpace
Search::get_tuple_space()
{
  return *pimpl->tuple_space.get();
}

void
Search::set_tuple_limit(long limit)
{
  pimpl->tuple_limit = limit;
}
long
Search::get_tuple_limit()
{
  return pimpl->tuple_limit;
}

void
Search::set_show_progress(bool show_progress)
{
  pimpl->show_progress = show_progress;
}
bool
Search::get_show_progress()
{
  return pimpl->show_progress;
}

void
Search::set_ranks(int ranks)
{
  pimpl->ranks = (!ranks) ? std::thread::hardware_concurrency() : ranks;
}
int
Search::get_ranks()
{
  return pimpl->ranks;
}

void
Search::set_start_rank(int start_rank)
{
  pimpl->start_rank = start_rank;
}
int
Search::get_start_rank()
{
  return pimpl->ranks;
}

void
Search::set_total_ranks(int total_ranks)
{
  pimpl->total_ranks = total_ranks;
  pimpl->parallel_search = true;
}
int
Search::get_total_ranks()
{
  return pimpl->total_ranks;
}

void
Search::set_output_intermediate(bool full)
{
  pimpl->full_output = full;
}
bool
Search::get_output_intermediate()
{
  return pimpl->full_output;
}

void
Search::set_cache_enabled(bool enabled)
{
  pimpl->use_cache = enabled;
}
bool
Search::get_cache_enabled()
{
  return pimpl->use_cache;
}

void
Search::set_cache_size_bytes(unsigned long size)
{
  pimpl->cache_size_bytes = size;
}
unsigned long
Search::get_cache_size_bytes()
{
  return pimpl->cache_size_bytes;
}

void
Search::_load_file(std::string const& filename, bool rowmajor)
{
  // TODO invalidate previous results
  pimpl->data = data_ptr(new io::DataMatrix(filename, rowmajor));
  if (!pimpl->data) {
    throw SearchException(
      "load_file", "Failed to create DataMatrix from file '" + filename + "'");
  }
}

void
Search::load_file(std::string const& filename)
{
  _load_file(filename, true);
}

void
Search::load_file_row_major(std::string const& filename)
{
  _load_file(filename, true);
}

void
Search::load_file_column_major(std::string const& filename)
{
  _load_file(filename, false);
}

#if BOOST_PYTHON_EXTENSIONS
void
Search::load_ndarray(np::ndarray const& np)
{
  // TODO invalidate previous results
  pimpl->data = data_ptr(new io::DataMatrix(np));
  if (!pimpl->data) {
    throw SearchException("load_ndarray",
                          "Failed to create DataMatrix from ndarray");
  }
}
#endif

void
Search::printCacheStats()
{
  int thread_no = 0;
  for (auto const& thread : pimpl->threads) {
    int cache_no = 1;
    if (!thread.caches.empty()) {
      for (auto cache : thread.caches) {
        if (cache) {
          std::cerr << "Thread " << thread_no;
          std::cerr << " cache " << cache_no;
          std::cerr << " hits " << cache->hits();
          std::cerr << " misses " << cache->misses();
          std::cerr << " evictions " << cache->evictions();
          std::cerr << std::endl;
        }
        cache_no++;
      }
    }
    thread_no++;
  }
}

static counter_ptr
make_counter(probability_algorithms const& type,
             variables_ptr const& variables)
{
  switch (type) {
    case probability_algorithms::vector:
      return counter_ptr(new it::VectorCounter());
    case probability_algorithms::bitset:
      return counter_ptr(new it::BitsetCounter(*variables));
    default:
      throw SearchException("make_counter", "Invalid probabilty algorithm");
  }
}

static entropy_calc_ptr
make_calculator(counter_ptr const& counter,
                std::vector<cache_ptr> const& caches,
                variables_ptr const& variables)
{
  return entropy_calc_ptr(new it::EntropyCalculator(variables, counter, caches));
}

using count_t = algorithm::TupleSpace::count_t;
static std::vector<count_t[2]>
divide_tuple_space(count_t total_ranks, count_t tuple_count)
{
  std::vector<count_t[2]> rank_bounds(total_ranks); // [start,stop)
  auto step = tuple_count / total_ranks;
  rank_bounds[0][0] = 0;
  rank_bounds[0][1] = step;
  for (auto rr = 1; rr < total_ranks; rr++) {
    rank_bounds[rr][0] = rank_bounds[rr - 1][1];
    rank_bounds[rr][1] = (rr + 1) * step;
  }
  rank_bounds[total_ranks-1][1] = tuple_count;
  return rank_bounds;
}

void
Search::init_caches()
{
  auto entropy_measure = measure_ptr(new it::EntropyMeasure());
  int nvar = pimpl->data->get_nvar();
  int ranks = pimpl->ranks;
  int ncache = (pimpl->tuple_size > 2) ? 2 : 1;
  auto variables = pimpl->data->variables();

  // Even very large data does not take a seriously long time to populate
  // caches, especially compared to the the full runtime. Thus we can get away
  // with not checking if caches can be reused (hard with the no-copy data
  // model). So remake the caches every time.
  pimpl->shared_caches.resize(ncache);
  pimpl->shared_caches.assign(ncache, nullptr);

  auto mem_budget = pimpl->cache_size_bytes;

  // By taking each dimension on its own we prevent any two threads from
  // seeing the same tuple. Cache must be a pre-allocated container, hence
  // thread-safe writes to different elements.
  if (ncache >= 1) {
    try {
      pimpl->shared_caches[0] =
        cache_ptr(new cache::Flat1D(nvar));
    } catch (std::bad_alloc const& ba) {
      // cannot allocate this cache, stop the cache init
      return;
    }
  }
  if (ncache >= 2) {
    try {
      //TODO take from memory budget
      pimpl->shared_caches[1] =
        cache_ptr(new cache::Flat2D(nvar));
    } catch (std::bad_alloc const& ba) {
      // TODO try a smaller size?
    }
  }

  // TODO higher dimensions

  for (int cc = 0; cc < ncache; cc++) {
    if (!pimpl->shared_caches[cc]) {
      continue;
    }
    int d = cc + 1;
    std::vector<algorithm::Worker> workers(ranks);
    std::vector<std::thread> threads(ranks - 1);
    std::vector<cache_ptr> caches = { pimpl->shared_caches[cc] };
    auto ts = tuple_space_ptr(new algorithm::TupleSpace(nvar, d)); //TODO: make it closer to the real TupleSpace ...
    auto tuple_count = ts->count_tuples();
    auto rank_bounds = divide_tuple_space(ranks, tuple_count);
    for (int ii = 0; ii < ranks; ii++) {
      auto calc = make_calculator(pimpl->counter, caches, variables);
      workers[ii] = algorithm::Worker(
        ts, rank_bounds[ii][0], rank_bounds[ii][1], calc, {}, entropy_measure);
    }
    for (int ii = 0; ii < ranks - 1; ii++) {
      threads[ii] = std::thread(&algorithm::Worker::start, &workers[ii]);
    }
    workers.back().start();
    for (auto& thread : threads) {
      thread.join();
    }
  }
}

static void
configure_in_memory_output(std::vector<flat_stream_ptr> &mem_outputs,
                           bool cutoff,
                           std::size_t ranks,
                           std::size_t tuple_count,
                           std::size_t tuple_offset,
                           std::size_t rowsize)
{
  mem_outputs.clear();
  if (cutoff) {
    // can't know real size of the output, use dynamically expanding pattern
    mem_outputs.resize(ranks);
    for (int ii = 0; ii < ranks; ii++) {
      mem_outputs[ii] = flat_stream_ptr(new io::FlatOutputStream(rowsize, tuple_offset));
    }
  } else {
    try {
      // attempt to make single output, preferred for performance
      mem_outputs.resize(1);
      mem_outputs[0] = flat_stream_ptr(new io::FlatOutputStream(tuple_count, rowsize, tuple_offset));
    } catch (io::FlatOutputStreamException &e) {
      throw SearchException("start", "Could not allocate space for all output tuples. Consider shrinking the TupleSpace, using a cutoff measure value, or switching to file-only output.");
    }
  }
}

static count_t
get_progress(std::vector<algorithm::Worker> const& workers)
{
  count_t count = 0;
  for (auto const& worker : workers) {
    count += worker.tuple_count();
  }
  return count;
}

static void
wait_print_progress(std::vector<algorithm::Worker> const& workers,
                    count_t tuple_count)
{
  std::size_t max_sample_size = 20;
  std::chrono::seconds sleep_duration(1);
  std::queue<count_t> samples;
  samples.push(get_progress(workers));
  auto current = samples.back();
  unsigned remain = -1;
  while (current < tuple_count) {
    // TODO use futures to check when threads are done in case of errors
    // ... very confident in the stopping condition ...
    std::this_thread::sleep_for(sleep_duration);
    samples.push(get_progress(workers));
    if (samples.size() > max_sample_size) {
      samples.pop();
    }
    current = samples.back();
    unsigned percent = 100 * (double) current / (double) tuple_count;
    double tuples = (samples.back() - samples.front());
    double interval = sleep_duration.count() * samples.size();
    double rate = (!tuples) ? 0 : interval / tuples;
    count_t tuple_rate = tuples / interval;
    // managed perceived performance by not allowing remaining time to increment
    remain = std::min(remain, (unsigned) ((tuple_count - current) * rate));
    std::cerr.clear();
    std::cerr.flush();
    std::fprintf(stderr, "Tuples processed: [%%%02u] %lu/%lu Tuples/s: %lu ETA(s): %-40u\r",
                 percent, current, tuple_count, tuple_rate, remain);
    std::cerr.flush();
  }
}

//
// Run full algoirithm as configured
//
void
Search::start()
{
  // sanity checks
  if (!pimpl->data) {
    throw SearchException("start",
                          "No data loaded, use load_file or load_ndarray.");
  }
  if (!pimpl->measure) {
    throw SearchException("start", "No IT Measure selected, use set_measure.");
  }
  if (pimpl->parallel_search && pimpl->total_ranks < pimpl->ranks) {
    throw SearchException(
      "start", "ranks for this Search cannot be greater than total_ranks.");
  }

  int nvar = pimpl->data->get_nvar();
  int tuple_size = pimpl->tuple_size;
  auto variables = pimpl->data->variables();
  auto total_ranks =
    (pimpl->parallel_search) ? pimpl->total_ranks : pimpl->ranks;
  auto start_rank = pimpl->start_rank;
  auto ranks = pimpl->ranks;
  auto num_threads = (pimpl->show_progress) ? pimpl->ranks : (pimpl->ranks - 1);

  // load default tuplespace if one has not been set yet
  if (!pimpl->tuple_space) {
    pimpl->tuple_space = tuple_space_ptr(new algorithm::TupleSpace(nvar, tuple_size));
  }

  // Create the probabilty distribution counter. The counter may recase the
  // data so it needs to have enough memory
  pimpl->counter = make_counter(pimpl->probability_algorithm, variables);

  // Divide the tuple space into chunks for each rank
  auto max_tuples = pimpl->tuple_space->count_tuples();
  auto tuple_count = (pimpl->tuple_limit) ? pimpl->tuple_limit : max_tuples;
  auto rank_bounds = divide_tuple_space(total_ranks, tuple_count);

  // initialize output file stream
  if (!pimpl->outfile.empty()) {
    auto header = pimpl->measure->header(tuple_size, pimpl->full_output);
    pimpl->file_output =
      file_stream_ptr(new io::FileOutputStream(pimpl->outfile, header));
    if (!pimpl->file_output) {
      throw SearchException("start",
                            "Failed to create FileOutputStream from file '" +
                              pimpl->outfile + "'");
    }
  }

  // configure in-memory results output
  if (pimpl->in_memory_output) {
    std::size_t rowsize = pimpl->measure->names(tuple_size, pimpl->full_output).size();
    auto tuple_offset = rank_bounds[start_rank][0];
    auto size = rank_bounds[start_rank+ranks-1][1] - rank_bounds[start_rank][0];
    configure_in_memory_output(pimpl->mem_outputs, pimpl->use_cutoff, ranks, size, tuple_offset, rowsize);
  }

  // Only use caches for measures that use intermediate entropies
  if (pimpl->use_cache && pimpl->measure->full_entropy()) {
    init_caches();
  }

  std::vector<algorithm::Worker> workers(ranks);
  std::vector<std::thread> threads(num_threads);
  // Create Workers
  for (int ii = 0; ii < ranks; ii++) {
    // each worker gets own calc, with own buffer probability distribution
    auto calc = make_calculator(pimpl->counter, pimpl->shared_caches, variables);
    // Configure output streams. Each worker gets separate output streams
    // to avoid collision (single stream coordinated by mutex is too slow).
    std::vector<output_stream_ptr> out_streams;
    if (pimpl->in_memory_output) {
      out_streams.push_back((pimpl->mem_outputs.size() == ranks) ?
          pimpl->mem_outputs[ii] : pimpl->mem_outputs.front());
    }
    if (pimpl->file_output) {
      out_streams.push_back(std::shared_ptr<io::OutputStream>(
        new io::FileOutputStream(*pimpl->file_output)));
    }
    workers[ii] = algorithm::Worker(pimpl->tuple_space,
                                    rank_bounds[start_rank + ii][0],
                                    rank_bounds[start_rank + ii][1],
                                    pimpl->cutoff,
                                    calc,
                                    out_streams,
                                    pimpl->measure);
    workers[ii].output_all = pimpl->full_output;
  }

  // Start child ranks
  for (int ii = 0; ii < num_threads; ii++) {
    threads[ii] = std::thread(&algorithm::Worker::start, &workers[ii]);
  }

  if (pimpl->show_progress) {
    wait_print_progress(workers, tuple_count);
  } else {
    workers.back().start();
  }

  // join other threads
  for (auto& thread : threads) {
    thread.join();
  }

  // combine in-memory output arrays
  int narrays = pimpl->mem_outputs.size();
  for (int ii = 1; ii < narrays; ii++) {
    pimpl->mem_outputs.front()->relocate(*pimpl->mem_outputs[ii]);
  }

  // cleanup output file stream
  pimpl->in_memory_output = true;
  pimpl->file_output = 0;
  pimpl->outfile = "";
}

#if BOOST_PYTHON_EXTENSIONS
np::ndarray
Search::python_start()
{
  start();
  return python_get_results();
}
#endif
