#include <boost/optional/optional.hpp>
#include <boost/program_options/value_semantic.hpp>
#include <iostream>
#include <vector>

#include <boost/program_options.hpp>
#include <boost/optional.hpp>
#include <boost/optional/optional_io.hpp>
#include <boost/algorithm/string.hpp>

#include "Search.hpp"
#include "yaml-cpp/yaml.h"

const std::string config_help = "The config file passed with option -c accepts this YAML structure: \n\
    ---\n\
    variable_groups: \n\
      - name: \"\"        #format String, group array \n\
        variables: []   #format Integer array, variables in the group \n\
    tuples: \n\
      - [\"\"]            #format String array, variable group names to form a tuple\n";

namespace po = boost::program_options;

struct Parameters {
    std::string measure;
    std::string tuple_algorithm;
    std::string pd_algorithm;
    std::string infile;
    std::string outfile;
    std::string configfile;
    int tuple_size;
    long tuple_limit;
    int num_threads;
    bool pd_cache;
};

void printParameters(Parameters const& p) {
    std::cout << "measure: " << p.measure << "\n";
    std::cout << "tuple_algorithm: " << p.tuple_algorithm << "\n";
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

void load_yml_config(std::string file, mist::Search &mist) {
    mist::algorithm::TupleSpace ts;
    YAML::Node config = YAML::LoadFile(file);

    // parse variable groups
    // TODO: error processing / exception handling
    auto variableGroupsYml = config["variable_groups"];
    for (std::size_t ii = 0; ii < variableGroupsYml.size(); ii++) {
        auto variableGroupYml = variableGroupsYml[ii];
        // parse out variables
        // TODO: interpret ranges
        auto variablesYml = variableGroupYml["variables"];
        std::vector<int> vars;
        for (std::size_t jj = 0; jj < variablesYml.size(); jj++) {
            vars.push_back(variablesYml[jj].as<int>());
        }
        // parse out name
        ts.addVariableGroup(variableGroupYml["name"].as<std::string>(), vars);
    }

    // parse variable groups tuples
    auto variableGroupTuplesYml = config["tuples"];
    for (std::size_t ii = 0; ii < variableGroupTuplesYml.size(); ii++) {
        std::vector<std::string> groupTuple;
        auto variableGroupTupleYml = variableGroupTuplesYml[ii];
        for (std::size_t jj = 0; jj < variableGroupTupleYml.size(); jj++) {
            groupTuple.push_back(variableGroupTupleYml[jj].as<std::string>());
        }
        // parse out name
        ts.addVariableGroupTuple(groupTuple);
    }

    mist.set_tuple_space(ts);
}

int main(int argc, char *argv[]) {
    // Parameters
    Parameters param;

    // defaults
    Parameters dparam;
    dparam.tuple_size = 2;
    dparam.tuple_limit = 0;
    dparam.num_threads = 2;
    dparam.pd_cache = true;
    dparam.measure = "symmetricdelta";
    dparam.tuple_algorithm = "completion";
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
        ("config-file,c", po::value(&param.configfile), "YML Config file")
        ("tuple-size,s", po::value(&param.tuple_size)->default_value(dparam.tuple_size), "Number of variables in each tuple")
        ("tuple-limit,l", po::value(&param.tuple_limit)->default_value(dparam.tuple_limit), "Maximum number of tuples to process")
        ("measure,m", po::value(&param.measure)->default_value(dparam.measure), "Information Theory Measure")
        ("version,v", "Print version string and exit")
    ;

    po::options_description opt_perf("Performance-tuning options");
    opt_perf.add_options()
        ("pd-algorithm", po::value(&param.pd_algorithm)->default_value(dparam.pd_algorithm), "Probabilty distribution counting algorithm")
        ("threads,t", po::value(&param.num_threads)->default_value(dparam.num_threads), "Number of threads")
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

    mist::Search mist;

    if (!param.configfile.empty()) {
        load_yml_config(param.configfile, mist);
    }

    if (help) {
        std::cout << usage << opts << "\n";
        std::cout << config_help;
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
    mist.set_probability_algorithm(param.pd_algorithm);
    mist.set_ranks(param.num_threads);
    mist.set_tuple_size(param.tuple_size);
    mist.set_outfile(param.outfile);
    mist.load_file(param.infile);
    mist.start();
    if (debug)
        mist.printCacheStats();
    return 0;
}
