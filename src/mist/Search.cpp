#include <algorithm>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <limits>
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

using namespace mist;

using data_ptr = std::shared_ptr<io::DataMatrix>;
using output_stream_ptr = std::shared_ptr<io::OutputStream>;
using file_stream_ptr = std::shared_ptr<io::FileOutputStream>;
using map_stream_ptr = std::shared_ptr<io::MapOutputStream>;
using flat_stream_ptr = std::shared_ptr<io::FlatOutputStream>;
using measure_ptr = std::shared_ptr<it::Measure>;
using cache_ptr = it::EntropyCalculator::cache_ptr_type;
using entropy_calc_ptr = std::shared_ptr<it::EntropyCalculator>;

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
  entropy_calc_ptr calculator;
};

struct Search::impl
{
  // structures
  data_ptr data;
  file_stream_ptr file_output;
  measure_ptr measure;
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
  // whether this Search is participating in a parallel search
  bool parallel_search = false;
  int ranks;
  int start_rank;
  int total_ranks;
  int tuple_size;
  probability_algorithms probability_algorithm;
  std::string probability_algorithm_str;
  std::string outfile;
  algorithm::TupleSpace tuple_space;
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
  if (size < 2 || size > 3) {
    throw SearchException("set_tuple_size",
                          "Invalid tuple size " + std::to_string(size) +
                            ", valid range is [2,3]");
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
  pimpl->tuple_space = ts;
  pimpl->tuple_size = ts.tupleSize();
}
algorithm::TupleSpace
Search::get_tuple_space()
{
  return pimpl->tuple_space;
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

entropy_calc_ptr
makeEntropyCalc(probability_algorithms const& type,
                Variable::tuple const& variables,
                std::vector<cache_ptr>& caches)
{
  it::EntropyCalculator* calc = 0;
  switch (type) {
    case probability_algorithms::vector:
      calc = new it::EntropyCalculator(
        variables,
        std::shared_ptr<it::VectorCounter>(new it::VectorCounter()),
        caches);
      break;
    case probability_algorithms::bitset:
      calc = new it::EntropyCalculator(
        variables,
        std::shared_ptr<it::BitsetCounter>(new it::BitsetCounter(variables)),
        caches);
      break;
  }
  return entropy_calc_ptr(calc);
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

  pimpl->shared_caches.resize(ncache);

  // By taking each dimension on its own we prevent any two threads from
  // seeing the same tuple. Only safe for pre-sized caches, e.g. Flat, that
  // are thread-safe for unique puts. For other cache types use a single
  // worker.

  // fill caches
  for (int cc = 0; cc < ncache; cc++) {
    int d = cc + 1;
    try {
      if (pimpl->cache_size_bytes) {
        pimpl->shared_caches[cc] =
          cache_ptr(new cache::Flat<it::entropy_type>(nvar, d, pimpl->cache_size_bytes));
      } else {
        pimpl->shared_caches[cc] =
          cache_ptr(new cache::Flat<it::entropy_type>(nvar, d));
      }
    } catch (std::bad_alloc const& ba) {
      // cannot allocate this cache, stop the cache init
      break;
    }
    std::vector<algorithm::Worker> workers(ranks);
    std::vector<std::thread> threads(ranks - 1);
    std::vector<cache_ptr> caches = { pimpl->shared_caches[cc] };
    auto ts = algorithm::TupleSpace(nvar, d); //TODO: make it closer to the real TupleSpace ...
    auto tuple_count = ts.count_tuples();
    auto rank_bounds = divide_tuple_space(ranks, tuple_count);
    for (int ii = 0; ii < ranks; ii++) {
      auto calc =
        makeEntropyCalc(pimpl->probability_algorithm, variables, caches);
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

  // load default tuplespace if one has not been set yet
  if (!pimpl->tuple_space.tupleSize()) {
    pimpl->tuple_space = algorithm::TupleSpace(nvar, tuple_size);
  }

  // Divide the tuple space into chunks for each rank
  auto max_tuples = pimpl->tuple_space.count_tuples();
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
  std::vector<std::thread> threads(ranks - 1);
  // Create Workers
  for (int ii = 0; ii < ranks; ii++) {
    // each worker gets own calc, with own buffer probability distribution
    auto calc = makeEntropyCalc(
      pimpl->probability_algorithm, variables, pimpl->shared_caches);
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
  for (int ii = 0; ii < ranks - 1; ii++) {
    threads[ii] = std::thread(&algorithm::Worker::start, &workers[ii]);
  }
  workers.back().start();

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
