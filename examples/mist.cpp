#include <boost/optional/optional.hpp>
#include <boost/program_options/value_semantic.hpp>
#include <iostream>
#include <vector>

#include <boost/program_options.hpp>
#include <boost/optional.hpp>
#include <boost/optional/optional_io.hpp>
#include <boost/algorithm/string.hpp>

#include "Mist.hpp"

namespace po = boost::program_options;

struct Parameters {
    std::string measure;
    std::string thread_algorithm;
    std::string pd_algorithm;
    std::string infile;
    std::string outfile;
    int tuple_size;
    int num_threads;
    bool pd_cache;
};

void printParameters(Parameters const& p) {
    std::cout << "measure: " << p.measure << "\n";
    std::cout << "thread_algorithm: " << p.thread_algorithm << "\n";
    std::cout << "pd_algorithm: " << p.pd_algorithm << "\n";
    std::cout << "infile: " << p.infile << "\n";
    std::cout << "outfile: " << p.outfile << "\n";
    std::cout << "tuple_size: " << p.tuple_size << "\n";
    std::cout << "num_threads: " << p.num_threads << "\n";
    std::cout << "pd_cache: " << p.pd_cache << "\n";
}

void printVersion(std::string const& version) {
    std::cout << "Mist library version " << version << "\n";
}

int main(int argc, char *argv[]) {
    // Parameters
    Parameters param;

    // defaults
    Parameters dparam;
    dparam.tuple_size = 2;
    dparam.num_threads = 2;
    dparam.pd_cache = true;
    dparam.measure = "symmetricdelta";
    dparam.thread_algorithm = "completion";
    dparam.pd_algorithm = "vector";
    dparam.outfile = "/dev/stdout";

    //
    // Argument processing
    //
    std::vector<std::string> tokens;
    std::string cmdname = boost::split(tokens, argv[0], boost::is_any_of("/")).back();
    std::string usage("Usage:\n  " + cmdname + " [options] input-file\n");

    po::options_description opt_basic("Basic options");
    opt_basic.add_options()
        ("debug", "Turn on debug messages")
        ("help,h", "Print this help")
        ("input-file,i", po::value(&param.infile), "Input NxM matrix file, CSV and TSV formats accepted")
        ("output-file,o", po::value(&param.outfile)->default_value(dparam.outfile), "Results output file")
        ("tuple-size,s", po::value(&param.tuple_size)->default_value(dparam.tuple_size), "Number of variables in each tuple")
        ("measure,m", po::value(&param.measure)->default_value(dparam.measure), "Information Theory Measure")
        ("version,v", "Print version string and exit")
    ;

    po::options_description opt_perf("Performance-tuning options");
    opt_perf.add_options()
        ("pd-algorithm", po::value(&param.pd_algorithm)->default_value(dparam.pd_algorithm), "Probabilty distribution counting algorithm")
        ("pd-cache", po::value(&param.pd_cache)->default_value(dparam.pd_cache), "Toggle probability distribution caching")
        ("threads,t", po::value(&param.num_threads)->default_value(dparam.num_threads), "Number of threads")
        ("thread-algorithm", po::value(&param.thread_algorithm)->default_value(dparam.thread_algorithm), "Thread work-sharing algorithm")
    ;

    // combine options groups
    po::options_description opts;
    opts.add(opt_basic).add(opt_perf);

    // add in positional options
    po::positional_options_description p;
    p.add("input-file", -1);

    // parse options
    po::variables_map vm;
    try {
        po::store(po::command_line_parser(argc, argv).  options(opts).positional(p).run(), vm);
    } catch (std::exception &e) {
        std::cerr << e.what() << "\n\n" << usage << opts;
        return 1;
    }
    po::notify(vm);
    bool help = vm.count("help");
    bool debug = vm.count("debug");
    bool version = vm.count("version");

    mist::Mist mist;

    if (help) {
        std::cout << usage << opts << "\n";
        return 0;
    }
    if (version) {
        printVersion(mist.version());
        return 0;
    }
    if (param.infile.empty()) {
        std::cerr << "Error: input-file is required" << "\n\n";
        std::cerr << usage << opts << "\n";
        return 1;
    }
    if (debug)
        printParameters(param);

    //
    // Run computation
    //
    mist.set_thread_algorithm(param.thread_algorithm);
    mist.set_probability_algorithm(param.pd_algorithm);
    mist.set_threads(param.num_threads);
    mist.set_tuple_size(param.tuple_size);
    mist.set_outfile(param.outfile);
    mist.load_file(param.infile);
    if (!param.pd_cache) {
        mist.disable_cache_d1();
        mist.disable_cache_d2();
    } else {
        mist.enable_cache_d1();
        mist.enable_cache_d2();
    }
    mist.compute();
    if (debug)
        mist.printCacheStats();
    return 0;
}
