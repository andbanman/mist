#pragma once

#include <memory>
#include <vector>

#include "../Variable.hpp"

namespace mist {
namespace algorithm {

class TupleSpace {
private:
    class impl;
    std::unique_ptr<impl> pimpl;
public:
    using tuple = Variable::indexes;
    using tuples = std::vector<Variable::indexes>;
    TupleSpace(int tuple_size);
    ~TupleSpace();
    void addGroup(tuple const& vars, short min, short max, bool replace);
    // add group by indices
    void addGroup(tuple const& vars, short min, short max) {
        addGroup(vars, min, max, true);
    }
    void addGroup(tuple const& vars, short min) {
        addGroup(vars, min, 1);
    }
    void addGroup(tuple const& vars) {
        addGroup(vars, 0);
    }
    void addGroup(std::vector<std::string> const& vars, short min, short max, bool replace);
    // add group by names
    void addGroup(std::vector<std::string> const& vars, short min, short max) {
        addGroup(vars, min, max, true);
    }
    void addGroup(std::vector<std::string> const& vars, short min) {
        addGroup(vars, min, 1);
    }
    void addGroup(std::vector<std::string> const& vars) {
        addGroup(vars, 0);
    }
    // member access
    int tuple_size();
    std::vector<std::string> names();
    void set_names(std::vector<std::string> const& names);
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
