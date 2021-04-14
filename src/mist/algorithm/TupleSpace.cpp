#include <algorithm>
#include <map>
#include <set>

#if BOOST_PYTHON_EXTENSIONS
#include <boost/python/extract.hpp>
#endif

#include "algorithm/TupleSpace.hpp"

using namespace mist;
using namespace mist::algorithm;

TupleSpace::TupleSpace() { };
TupleSpace::~TupleSpace() { };

std::vector<std::string> TupleSpace::names() const {
    return variableNames;
}

void TupleSpace::set_names(std::vector<std::string> const& names) {
    variableNames = names;
}

void TupleSpace::addVariableGroupTuple(std::vector<std::string> const& groupNames) {
    TupleSpace::tuple_type groupIndexes;
    for (auto& name : groupNames) {
        try {
            groupIndexes.push_back(variableGroupNames[name]);
        } catch (std::out_of_range &e) {
            throw TupleSpaceException("addVariableGroupTuple", "group named " + name + " does not exist.");
        }
    }
    addVariableGroupTuple(groupIndexes);
}

void TupleSpace::addVariableGroupTuple(TupleSpace::tuple_type const& groupIndexes) {
    // validate group indexes
    for (auto group : groupIndexes)
        if (group >= variableGroups.size())
            throw TupleSpaceException("addVariableGroupTuple", "variable group index " + std::to_string(group) + " out of range.");
    variableGroupTuples.push_back(groupIndexes);
}

int TupleSpace::addVariableGroup(std::string const& name, TupleSpace::tuple_type const& vars) {
    std::set<int> unique_vars;
    tuple_type group;
    for (auto var : vars) {
        if (unique_vars.find(var) == unique_vars.end()) {
            unique_vars.insert(var);
            group.push_back(var);
        }
    }
    std::sort(group.begin(), group.end());
    variableGroups.push_back(group);
    int index = variableGroups.size() - 1;
    variableGroupNames.emplace(name, index);
    return index;
}

TupleSpace::tuple_type const& TupleSpace::getVariableGroup(int index) const {
    try {
        return variableGroups.at(index);
    } catch (std::out_of_range &e) {
        throw TupleSpaceException("getVariableGroup", "group index " + std::to_string(index) + " out of range.");
    }
}

TupleSpace::tuple_type const& TupleSpace::getVariableGroup(std::string const& name) const {
    try {
        return variableGroups.at(variableGroupNames.at(name));
    } catch (std::out_of_range &e) {
        throw TupleSpaceException("getVariableGroup", "group named " + name + " does not exist.");
    }
}

std::vector<TupleSpace::tuple_type> const& TupleSpace::getVariableGroups() const {
    return variableGroups;
}

std::vector<TupleSpace::tuple_type> const& TupleSpace::getVariableGroupTuples() const {
    return variableGroupTuples;
}

#if BOOST_PYTHON_EXTENSIONS
int TupleSpace::pyAddVariableGroup(std::string const& name, p::list const& list) {
    int n = p::len(list);
    // copy list in
    algorithm::TupleSpace::tuple_type vars(n);
    for (int ii = 0; ii < n; ii++) {
        p::extract<int> var(list[ii]);
        if (var.check()) {
            vars[ii] = var;
        } else {
            throw TupleSpaceException("pyAddVariableGroup", "Expected list with elements type int");
        }
    }
    return addVariableGroup(name, vars);
}

void TupleSpace::pyAddVariableGroupTuple(p::list const& list) {
    int n = p::len(list);
    algorithm::TupleSpace::tuple_type groups(n);
    for (int ii = 0; ii < n; ii++) {
        p::extract<int> gint(list[ii]);
        p::extract<std::string> gname(list[ii]);
        if (gint.check()) {
            groups[ii] = p::extract<int>(list[ii]);
        } else if (gname.check()) {
            std::string name(gname);
            try {
                groups[ii] = variableGroupNames.at(name);
            } catch (std::exception &e) {
                throw TupleSpaceException("pyAddVariableGroupTuple", "group named " + name + " does not exist.");
            }
        } else {
            throw TupleSpaceException("pyAddVariableGroupTuple", "Expected list with elements type int or str");
        }
    }
    addVariableGroupTuple(groups);
}
#endif
