#pragma once

#include <map>
#include <memory>
#include <vector>

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
     * @param vars set of variables in the group
     */
    void addVariableGroup(std::string const& name, tuple_type const& vars);
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
