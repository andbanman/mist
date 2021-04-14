#include <boost/predef/version.h>
#if BOOST_PREDEF_VERSION < BOOST_VERSION_NUMBER(1, 6, 0)
#include <boost/numpy.hpp>
#else
#include <boost/python/numpy.hpp>
#endif

#include "io/DataMatrix.hpp"
#include "Mist.hpp"

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

    p::class_<Mist>("Mist")
        .def("load_ndarray", &Mist::load_ndarray)
        .def("load_file", &Mist::load_file)
        .def("set_outfile", &Mist::set_outfile)
        .def("get_results", &Mist::python_get_results)
        .def("compute", &Mist::compute)
        .def("set_measure", &Mist::set_measure)
        .def("set_threads", &Mist::set_threads)
        .def("set_tuple_size", &Mist::set_tuple_size)
        .def("set_thread_algorithm", &Mist::set_thread_algorithm)
        .def("set_probability_algorithm", &Mist::set_probability_algorithm)
        .def("enable_cache_d1", &Mist::enable_cache_d1)
        .def("enable_cache_d2", &Mist::enable_cache_d2)
        .def("disable_cache_d1", &Mist::enable_cache_d1)
        .def("disable_cache_d2", &Mist::enable_cache_d2)
        .def("version", &Mist::version)
    ;
}
