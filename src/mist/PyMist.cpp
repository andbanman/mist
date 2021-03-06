#include <boost/predef/version.h>
#if BOOST_PREDEF_VERSION < BOOST_VERSION_NUMBER(1, 6, 0)
#include <boost/numpy.hpp>
#else
#include <boost/python/numpy.hpp>
#endif

#include "Search.hpp"
#include "algorithm/TupleSpace.hpp"
#include "io/DataMatrix.hpp"

namespace p = boost::python;
#if BOOST_PREDEF_VERSION < BOOST_VERSION_NUMBER(1, 6, 0)
namespace np = boost::numpy;
#else
namespace np = boost::python::numpy;
#endif

using namespace mist;

BOOST_PYTHON_MODULE(libmist)
{
  Py_Initialize();
  np::initialize();

  p::class_<io::DataMatrix>("DataMatrix", p::init<int, int, int>())
    .def(p::init<std::string>())
    .def(p::init<std::string, bool>())
    .def(p::init<np::ndarray>())
    .def("write_file", &io::DataMatrix::python_write_file)
    .add_property("n", &io::DataMatrix::get_nvar)
    .add_property("m", &io::DataMatrix::get_svar);

  p::class_<algorithm::TupleSpace>("TupleSpace")
    .def(p::init<int,int>())
    .def("count_tuples", &algorithm::TupleSpace::count_tuples)
    .def("addVariableGroup", &algorithm::TupleSpace::pyAddVariableGroup)
    .def("addVariableGroupTuple",
         &algorithm::TupleSpace::pyAddVariableGroupTuple);

  p::class_<Search>("Search")
    .add_property("cutoff", &Search::get_cutoff, &Search::set_cutoff)
    .add_property("measure", &Search::get_measure, &Search::set_measure)
    .add_property("probability_algorithm",
                  &Search::get_probability_algorithm,
                  &Search::set_probability_algorithm)
    .add_property("outfile", &Search::get_outfile, &Search::set_outfile)
    .add_property("output_intermediate",
                  &Search::get_output_intermediate,
                  &Search::set_output_intermediate)
    .add_property("ranks", &Search::get_ranks, &Search::set_ranks)
    .add_property(
      "start_rank", &Search::get_start_rank, &Search::set_start_rank)
    .add_property("threads", &Search::get_ranks, &Search::set_ranks)
    .add_property(
      "total_ranks", &Search::get_total_ranks, &Search::set_total_ranks)
    .add_property(
      "tuple_limit", &Search::get_tuple_limit, &Search::set_tuple_limit)
    .add_property(
      "tuple_size", &Search::get_tuple_size, &Search::set_tuple_size)
    .add_property(
      "tuple_space", &Search::get_tuple_space, &Search::set_tuple_space)
    .add_property(
      "show_progress", &Search::get_show_progress, &Search::set_show_progress)
    .add_property(
      "cache_enabled", &Search::get_cache_enabled, &Search::set_cache_enabled)
    .add_property(
      "cache_size_bytes",
      &Search::get_cache_size_bytes,
      &Search::set_cache_size_bytes)
    .def("start", &Search::start)
    .def("load_ndarray", &Search::load_ndarray)
    .def("load_file", &Search::load_file)
    .def("load_file_row_major", &Search::load_file_row_major)
    .def("load_file_column_major", &Search::load_file_column_major)
    .def("start", &Search::python_start)
    .def("version", &Search::version);
}
