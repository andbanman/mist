#pragma once

#include <map>
#include <memory>
#include <vector>

#include "../Variable.hpp"

namespace mist {
namespace algorithm {

class TupleSpace {
public:
    using tuple_type = Variable::indexes;
    TupleSpace();
    ~TupleSpace();
    // variableGroupTuple expands into a series of variable tuples based on the
    // variable group definitions
    void addVariableGroup(std::string const& name, tuple_type const& vars);
    void addVariableGroupTuple(std::vector<std::string> const& vars);
    void addVariableGroupTuple(tuple_type const& vars);
    std::vector<std::string> names() const;
    void set_names(std::vector<std::string> const& names);

    tuple_type const& getVariableGroup(int index) const;
    tuple_type const& getVariableGroup(std::string const& name) const;
    std::vector<tuple_type> const& getVariableGroups() const;
    std::vector<tuple_type> const& getVariableGroupTuples() const;

    //tuple_type const& getVariableGroupTuple(int index);
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
