#pragma once

#include <map>
#include <memory>
#include <vector>

#ifdef BOOST_PYTHON_EXTENSIONS
#include <boost/python.hpp>
namespace p = boost::python;
#endif

#include "../Variable.hpp"

namespace mist {
namespace algorithm {

/** Tuple Space defines the set of tuples over which to run a computation search.
 */
class TupleSpace {
public:
    using tuple_type = Variable::indexes;
    TupleSpace();
    ~TupleSpace();
    /** Define a named logical group of variables
     * @param name group name
     * @param vars set of variables in the group, duplicates will be ignored
     * @return index of created variable group
     */
    int addVariableGroup(std::string const& name, tuple_type const& vars);
    /** Add a variable group tuple
     *
     * The cross product of groups in the group tuple generates a set of
     * variable tuples that will be added to the TupleSpace by
     * TupleSpaceTupleProducer.
     *
     * @param groups Array of group names
     */
    void addVariableGroupTuple(std::vector<std::string> const& groups);
    /** Add a variable group tuple
     *
     * The cross product of groups in the group tuple generates a set of
     * variable tuples that will be added to the TupleSpace by
     * TupleSpaceTupleProducer.
     *
     * @param groups Array of group indexed by order created
     */
    void addVariableGroupTuple(tuple_type const& groups);
    /** Get variable names
     */
    std::vector<std::string> names() const;
    /** Set variable names
     */
    void set_names(std::vector<std::string> const& names);

#if BOOST_PYTHON_EXTENSIONS
    int pyAddVariableGroup(std::string const& name, p::list const& list);
    void pyAddVariableGroupTuple(p::list const& list);
#endif

    tuple_type const& getVariableGroup(int index) const;
    tuple_type const& getVariableGroup(std::string const& name) const;
    std::vector<tuple_type> const& getVariableGroups() const;
    std::vector<tuple_type> const& getVariableGroupTuples() const;

private:
    // variable names, e.g. from data header
    std::vector<std::string> variableNames;
    // groups of variables that construct a tuple
    std::vector<tuple_type> variableGroups;
    // names for the variable groups and their identifying index
    std::map<std::string, int> variableGroupNames;
    // list of groups that define variable tuples
    std::vector<tuple_type> variableGroupTuples;
};

class TupleSpaceException : public std::exception {
private:
    std::string msg;
public:
    TupleSpaceException(std::string const& method, std::string const& msg) :
        msg("TupleSpace::" + method + " : " + msg) { }
    virtual const char* what() const throw() {
        return msg.c_str();
    };
};

} // algorithm
} // mist
