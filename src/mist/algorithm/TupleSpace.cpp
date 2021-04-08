#include <map>

#include "algorithm/TupleSpace.hpp"

using namespace mist;
using namespace mist::algorithm;

struct group {
    TupleSpace::tuple vars;
    short min;
    short max;
    bool replace;
};

struct TupleSpace::impl {
    std::vector<group> groups;
    std::map<std::string, int> names;
    int tuplesize;
    int tuples;
};

TupleSpace::TupleSpace(int size) : pimpl(std::make_unique<impl>()) {
    pimpl->tuplesize = size;
}

TupleSpace::~TupleSpace() { };

int TupleSpace::tuple_size() {
    return pimpl->tuplesize;
}

std::vector<std::string> TupleSpace::names() {
    std::vector<std::string> names;
    for (auto it = pimpl->names.begin(); it != pimpl->names.end(); ++it)
      names.push_back(it->first);
    return names;
}

void TupleSpace::set_names(std::vector<std::string> const& names) {
    int ii = 0;
    for (auto name : names) {
        pimpl->names.emplace(name, ii);
        ii++;
    }
}

void TupleSpace::addGroup(std::vector<std::string> const& names, short min, short max, bool replace) {
    group newgroup;
    if (names.empty())
        throw TupleSpaceException("addGroup", "variable names were not set, missing call to TupleSpace::set_names");
    for (auto name : names) {
        try {
            newgroup.vars.push_back(pimpl->names[name]);
        }
        catch(std::out_of_range &e) {
            throw TupleSpaceException("addGroup", "variable name '"+ name + "' not found");
        }
    }
    newgroup.min = min;
    newgroup.max = max;
    newgroup.replace = replace;
    pimpl->groups.push_back(newgroup);
}

void TupleSpace::addGroup(tuple const& vars, short min, short max, bool replace) {
    group newgroup;
    newgroup.vars = vars;
    newgroup.min = min;
    newgroup.max = max;
    newgroup.replace = replace;
    pimpl->groups.push_back(newgroup);
}
