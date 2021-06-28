#include <boost/predef/version.h>
#if BOOST_PREDEF_VERSION < BOOST_VERSION_NUMBER(1, 6, 0)
#include <boost/numpy.hpp>
#else
#include <boost/python/numpy.hpp>
#endif

#include "algorithm/TupleSpace.hpp"
#include "io/DataMatrix.hpp"
#include "Search.hpp"

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

    p::class_<io::DataMatrix>("DataMatrix", p::init<int,int,int>())
        .def(p::init<std::string>())
        .def(p::init<np::ndarray>())
        .def("write_file", &io::DataMatrix::python_write_file)
        .def_readonly("n", &io::DataMatrix::n)
        .def_readonly("m", &io::DataMatrix::m)
    ;

    p::class_<algorithm::TupleSpace>("TupleSpace")
        .def("addVariableGroup", &algorithm::TupleSpace::pyAddVariableGroup)
        .def("addVariableGroupTuple", &algorithm::TupleSpace::pyAddVariableGroupTuple)
    ;

    p::class_<Search>("Search")
        .def("load_ndarray", &Search::load_ndarray)
        .def("load_file", &Search::load_file)
        .def("set_outfile", &Search::set_outfile)
        //.def("get_results", &Search::python_get_results)
        .def("compute", &Search::compute)
        .def("set_measure", &Search::set_measure)
        .def("set_threads", &Search::set_threads)
        .def("set_tuple_size", &Search::set_tuple_size)
        .def("set_tuple_space", &Search::set_tuple_space)
        .def("set_probability_algorithm", &Search::set_probability_algorithm)
        .def("full_output", &Search::full_output)
        .def("version", &Search::version)
    ;
}
