#include <algorithm>
#include <exception>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <vector>

#include "Search.hpp"
#include "algorithm/TupleProducer.hpp"

using namespace mist;

using data_ptr          = std::shared_ptr<io::DataMatrix>;
using output_stream_ptr = std::shared_ptr<io::OutputStream>;
using file_stream_ptr   = std::shared_ptr<io::FileOutputStream>;
using measure_ptr       = std::shared_ptr<it::Measure>;
using cache_ptr         = it::EntropyCalculator::cache_ptr_type;
using entropy_calc_ptr  = std::shared_ptr<it::EntropyCalculator>;
using producer_ptr      = std::shared_ptr<algorithm::TupleProducer>;
using consumer_ptr      = std::shared_ptr<algorithm::TupleConsumer>;

std::string Search::version() {
    return MIST_VERSION;
}

enum struct search_types : int {
    exhaustive,
    tuplespace
};

enum struct probability_algorithms : int {
    vector,
    bitset
};

enum struct cache_types : int {
    none,
    memory,
    small_files
};

struct thread_config {
    measure_ptr measure;
    output_stream_ptr output_stream;
    std::vector<cache_ptr> caches;
    entropy_calc_ptr calculator;
};

struct Search::impl {
    // structures
    data_ptr data;
    file_stream_ptr file_output;
    measure_ptr measure;
    std::vector<cache_ptr> shared_caches;
    std::vector<thread_config> threads;
    cache_types prev_cache_type = cache_types::none;

    // config
    algorithm::TupleProducer::algorithm tuple_algorithm = algorithm::TupleProducer::algorithm::completion;
    bool cache_d1_enabled = true;
    bool cache_d2_enabled = true;
    bool full_output = false;
    int no_thread;
    int tuple_size;
    search_types search_type;
    probability_algorithms probability_algorithm;
    int cache_dimensions = 2; // TODO: right now we only support 3D so 2 is complete
    std::size_t cache_memory_size = 0 ;
    std::string cache_files_root;
    std::string outfile;
    algorithm::TupleSpace tupleSpace;

    // state
    cache_types cache_type = cache_types::none;
    std::size_t prev_cache_memory_size = 0;
};

Search::Search() : pimpl(std::make_unique<impl>()) {
    // structures
    pimpl->measure = measure_ptr(new it::SymmetricDelta());
    pimpl->data = 0;

    // default config
    pimpl->no_thread = std::thread::hardware_concurrency();
    pimpl->tuple_size = 2;
    pimpl->search_type = search_types::exhaustive;
    pimpl->probability_algorithm = probability_algorithms::vector;
    pimpl->cache_type = cache_types::memory;
};

Search::Search(const Search& other) : pimpl(std::make_unique<impl>()) {
    *this->pimpl = *other.pimpl;
};

Search::Search(Search&&) = default;
Search& Search::operator=(Search&&) = default;
Search::~Search() {}

void Search::set_measure(std::string const& measure) {
    std::string test(measure);
    transform(test.begin(), test.end(), test.begin(), ::tolower);

    if (test == "symmetricdelta")
        pimpl->measure = measure_ptr(new it::SymmetricDelta());
    else if (test == "entropy")
        pimpl->measure = measure_ptr(new it::EntropyMeasure());
    else
        throw SearchException("set_measure", "Invalid measure: " + measure + ", allowed: [SymmetricDelta,Entropy]");
}

void Search::set_search_type(std::string const& search_type) {
    std::string test(search_type);
    transform(test.begin(), test.end(), test.begin(), ::tolower);

    if (test == "exhaustive")
        pimpl->search_type = search_types::exhaustive;
    else if (test == "tuplespace")
        pimpl->search_type = search_types::tuplespace;
    else
        throw SearchException("set_search_type", "Invalid search space : " + search_type + ", allowed: [Exhaustive, TupleSpace]");
}

void Search::set_tuple_algorithm(std::string const& tuple_algorithm) {
    std::string test(tuple_algorithm);
    transform(test.begin(), test.end(), test.begin(), ::tolower);

    if (test == "completion")
        pimpl->tuple_algorithm = algorithm::TupleProducer::algorithm::completion;
    else if (test == "batch")
        pimpl->tuple_algorithm = algorithm::TupleProducer::algorithm::batch;
    else
        throw SearchException("set_tuple_algorithm", "Invalid tuple algorithm: " + tuple_algorithm + ", allowed: [Completion, Batch]");
}

void Search::set_probability_algorithm(std::string const& algorithm) {
    std::string test(algorithm);
    transform(test.begin(), test.end(), test.begin(), ::tolower);

    if (test == "bitset")
        pimpl->probability_algorithm = probability_algorithms::bitset;
    else if (test == "vector")
        pimpl->probability_algorithm = probability_algorithms::vector;
    else
        throw SearchException("set_probability_algorithm", "Invalid probability algorithm : " + algorithm + ", allowed: [bitset, vector]");
}

#if 0
//TODO: not implemented
void Search::set_cache_files_root(std::string const& directory) {
    throw SearchException("set_cache_files", "Not yet implemented.");
    pimpl->cache_type = cache_types::small_files;
    pimpl->cache_files_root = directory;
    // TODO validate directory
}
#endif

#if 0
//TODO: not implemented
void Search::set_cache_memory(std::size_t size) {
    throw SearchException("set_cache_memory", "Not yet implemented.");
    pimpl->cache_type = cache_types::memory;
    pimpl->cache_memory_size = size;
}
#endif

void Search::set_outfile(std::string const& filename) {
    // Thread copies of FileOutputStream should be constructed from
    // this object so they share an underlying file stream
    pimpl->outfile = filename;
}

#if 0
#if BOOST_PYTHON_EXTENSIONS
//
// Returns python numpy ndarray of variable indexes with the result set
//
np::ndarray Search::python_get_results() {
    if (pimpl->file_output)
        throw SearchException("python_get_results", "Results stored in output file '" + pimpl->file_output->get_filename() + "', not in memory");

    // infer results dimensions
    auto ptr = dynamic_cast<io::MapOutputStream*>(pimpl->threads.front().output_stream.get());
    if (!ptr)
        throw SearchException("python_get_results", "Failed to cast output stream to MapOutputStream");
    auto& results = ptr->get_results();
    int num_variables = results.begin()->first.size();
    int num_results = results.begin()->second.size();

    // determine number of tuples collected in the output streams
    int num_tuples = 0;
    for (auto& thread : pimpl->threads) {
        auto ptr = dynamic_cast<io::MapOutputStream*>(thread.output_stream.get());
        auto& results = ptr->get_results();
        num_tuples += results.size();
    }

    // initialize the output ndarray
    np::ndarray ret = np::zeros(p::make_tuple(num_tuples, num_variables + num_results),
                                              np::dtype::get_builtin<double>());

    // copy results into output ndarray
    int ii = 0;
    for (auto& thread : pimpl->threads) {
        auto ptr = dynamic_cast<io::MapOutputStream*>(thread.output_stream.get());
        auto& results = ptr->get_results();

        for (auto& item : results) {
            // fill in variable index tuple
            for (int jj = 0; jj < num_variables; jj++)
                ret[ii][jj] = (double) item.first[jj];
            // fill in results
            for (int jj = 0; jj < num_results; jj++)
                ret[ii][num_variables + jj] = (double) item.second[jj];
            ii++;
        }
    }

    return ret;
}
#endif

// TODO test this
// Returns a copy of the results collected into one data structure
io::MapOutputStream::map_type Search::get_results() {
    if (pimpl->file_output)
        throw SearchException("get_results", "Results stored in output file '" + pimpl->file_output->get_filename() + "', not in memory");

    io::MapOutputStream::map_type ret;
    for (auto& thread : pimpl->threads) {
        auto ptr = dynamic_cast<io::MapOutputStream*>(thread.output_stream.get());
        if (!ptr)
            throw SearchException("get_results", "Failed to cast output stream to MapOutputStream");
        auto& results = ptr->get_results();
        ret.insert(results.begin(), results.end());
    }
    return ret;
}
#endif

void Search::set_tuple_size(int size) {
    if (size < 2 || size > 3)
        throw SearchException("set_tuple_size", "Invalid tuple size " + std::to_string(size) + ", valid range is [2,3]");
    pimpl->tuple_size = size;
}

void Search::set_tuple_space(algorithm::TupleSpace const& ts) {
    pimpl->tupleSpace = ts;
    pimpl->search_type = search_types::tuplespace;
}

void Search::set_threads(int threads) { pimpl->no_thread = threads; }
void Search::enable_cache_d1() { pimpl->cache_d1_enabled = true; };
void Search::enable_cache_d2() { pimpl->cache_d2_enabled = true; };
void Search::disable_cache_d1() { pimpl->cache_d1_enabled = false; };
void Search::disable_cache_d2() { pimpl->cache_d2_enabled = false; };
void Search::full_output() { pimpl->full_output = true; };

void Search::load_file(std::string const& filename) {
    // TODO invalidate previous results
    pimpl->data = data_ptr(new io::DataMatrix(filename));
    if (!pimpl->data)
        throw SearchException("load_file", "Failed to create DataMatrix from file '" + filename + "'");
}

#if BOOST_PYTHON_EXTENSIONS
void Search::load_ndarray(np::ndarray const& np) {
    // TODO invalidate previous results
    pimpl->data = data_ptr(new io::DataMatrix(np));
    if (!pimpl->data)
        throw SearchException("load_ndarray", "Failed to create DataMatrix from ndarray");
}
#endif

void Search::printCacheStats() {
    int thread_no = 0;
    for (auto const& thread : pimpl->threads) {
        int cache_no = 1;
        if (!thread.caches.empty()) {
            for (auto cache : thread.caches) {
                if (cache) {
                    std::cerr << "Thread "      << thread_no;
                    std::cerr << " cache "      << cache_no;
                    std::cerr << " hits "       << cache->hits();
                    std::cerr << " misses "     << cache->misses();
                    std::cerr << " evictions "  << cache->evictions();
                    std::cerr << std::endl;
                }
                cache_no++;
            }
        }
        thread_no++;
    }
}

bool Search::cacheInvalid() {
    return (
            pimpl->cache_type != pimpl->prev_cache_type ||
            pimpl->cache_memory_size != pimpl->prev_cache_memory_size
           );
}

void Search::configureThreads() {
    int nvar = pimpl->data->n;
    int tuple_size = pimpl->tuple_size;
    //TODO: threaded 1 producer and 1 consumer not possible, see Coordinator.cpp
    int num_consumers = (pimpl->no_thread > 1) ? pimpl->no_thread - 1 : 1;
    auto variables = pimpl->data->variables();
    auto rebuildCache = cacheInvalid();

    // Reconfigure threads
    pimpl->threads.resize(num_consumers);
    for (auto& thread : pimpl->threads) {
        // outputs
        if (pimpl->file_output)
            thread.output_stream = std::shared_ptr<io::OutputStream>(new io::FileOutputStream(*pimpl->file_output));
        else
            thread.output_stream = std::shared_ptr<io::OutputStream>(new io::MapOutputStream());

        thread.measure = pimpl->measure;

        if (pimpl->cache_type == cache_types::memory) {
            //TODO: configure to use thread-local caches
            thread.caches.clear();
            thread.caches.resize(pimpl->shared_caches.size());
            if (pimpl->cache_d1_enabled)
                thread.caches[0] = pimpl->shared_caches[0];
            if (pimpl->cache_d2_enabled)
                thread.caches[1] = pimpl->shared_caches[1];
        } else {
            thread.caches.clear();
        }

        // entropy calculator
        if (pimpl->probability_algorithm == probability_algorithms::bitset) {
            thread.calculator = entropy_calc_ptr(
                    new it::EntropyCalculator(variables,
                        std::shared_ptr<it::BitsetCounter>(
                            new it::BitsetCounter(variables)), thread.caches));
        } else if (pimpl->probability_algorithm == probability_algorithms::vector) {
            thread.calculator = entropy_calc_ptr(
                    new it::EntropyCalculator(variables,
                        std::shared_ptr<it::VectorCounter>(
                            new it::VectorCounter()), thread.caches));
        }
    }
}

void Search::primeCaches() {
    auto entropy_measure = measure_ptr(new it::EntropyMeasure());
    int nvar = pimpl->data->n;

    // By taking each dimension on its own we prevent any two threads from
    // seeing the same tuple. Only safe for pre-sized caches, e.g. Flat, that
    // are thread-safe for unique puts. For other cache types use a single
    // consumer.

    if (pimpl->cache_d1_enabled && pimpl->tuple_size > 1) {
        producer_ptr producer;
        std::vector<consumer_ptr> consumers;
        for (auto const& thread : pimpl->threads)
            consumers.push_back(consumer_ptr(new algorithm::BatchTupleConsumer(
                            thread.calculator, 0, entropy_measure)));
        producer = producer_ptr(new algorithm::BatchTupleProducer(1, nvar));
        algorithm::Coordinator coord(producer, consumers);
        coord.start();
    }

    if (pimpl->cache_d2_enabled && pimpl->tuple_size > 2) {
        producer_ptr producer;
        std::vector<consumer_ptr> consumers;
        for (auto const& thread : pimpl->threads)
            consumers.push_back(consumer_ptr(new algorithm::ExhaustiveTupleConsumer(
                            thread.calculator, 0, entropy_measure, nvar)));
        producer = producer_ptr(new algorithm::ExhaustiveTupleProducer(2, nvar));
        algorithm::Coordinator coord(producer, consumers);
        coord.start();
    }

    pimpl->prev_cache_type = pimpl->cache_type;
    pimpl->prev_cache_memory_size = pimpl->cache_memory_size;
}

//
// Run full algoirithm as configured
//
void Search::compute() {
    // sanity checks
    if (!pimpl->data)
        throw SearchException("compute", "No data loaded, use load_file or load_ndarray.");
    if (!pimpl->measure)
        throw SearchException("compute", "No IT Measure selected, use set_measure.");

    int nvar = pimpl->data->n;
    int tuple_size = pimpl->tuple_size;

    // initialize output file stream
    if (!pimpl->outfile.empty()) {
        auto header = pimpl->measure->header(tuple_size, pimpl->full_output);
        pimpl->file_output = file_stream_ptr(new io::FileOutputStream(pimpl->outfile, header));
        if (!pimpl->file_output)
            throw SearchException("compute", "Failed to create FileOutputStream from file '" + pimpl->outfile + "'");
    }

    // Create shared caches
    if (cacheInvalid()) {
        pimpl->shared_caches.resize(pimpl->cache_dimensions);
        pimpl->shared_caches[0] = cache_ptr(new cache::Flat<it::Entropy>(nvar, 1));
        pimpl->shared_caches[1] = cache_ptr(new cache::Flat<it::Entropy>(nvar, 2));
    }

    configureThreads();

    if (cacheInvalid()) {
        primeCaches();
    }

    // Run algorithm for full tuple size
    producer_ptr producer;
    std::vector<consumer_ptr> consumers;

    //producer
    switch (pimpl->search_type) {
        case search_types::exhaustive:
            producer = producer_ptr(new algorithm::ExhaustiveTupleProducer(
                        tuple_size, nvar, pimpl->tuple_algorithm));
            break;
        case search_types::tuplespace:
            producer = producer_ptr(new algorithm::TupleSpaceTupleProducer(
                        pimpl->tupleSpace, pimpl->tuple_algorithm));
            break;
    }

    // consumer(s)
    // all TupleProducers are capable of queuing in batch mode
    if (pimpl->tuple_algorithm == algorithm::TupleProducer::algorithm::batch) {
       for (auto const& thread : pimpl->threads)
           consumers.push_back(consumer_ptr(new algorithm::BatchTupleConsumer(
                           thread.calculator, thread.output_stream, thread.measure, pimpl->full_output)));
    } else if (pimpl->tuple_algorithm == algorithm::TupleProducer::algorithm::completion) {
        switch (pimpl->search_type) {
            case search_types::exhaustive:
                for (auto const& thread : pimpl->threads)
                    consumers.push_back(consumer_ptr(new algorithm::ExhaustiveTupleConsumer(
                                    thread.calculator, thread.output_stream, thread.measure, nvar, pimpl->full_output)));
                break;
            case search_types::tuplespace:
                for (auto const& thread : pimpl->threads)
                    consumers.push_back(consumer_ptr(new algorithm::TupleSpaceTupleConsumer(
                                    thread.calculator, thread.output_stream, thread.measure, pimpl->tupleSpace, pimpl->full_output)));
                break;
        }
    }

    algorithm::Coordinator coord(producer, consumers);
    coord.start();
}
