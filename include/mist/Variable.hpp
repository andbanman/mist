#pragma once

#include <memory>
#include <stdexcept>
#include <vector>

#define VARIABLE_MISSING_VAL(x) (x < 0)

namespace mist {

using VariableData = int;

/** Variable wraps a pointer to a data column.
 */
class Variable {
private:
    std::shared_ptr<VariableData> data;
    std::size_t _size;
    std::size_t _index;
    std::size_t _bins;

public:
    using data_type = VariableData;
    using data_ptr = std::shared_ptr<data_type>;
    using tuple = std::vector<Variable>;
    using indexes = std::vector<int>;

    using iterator = data_type*;
    using const_iterator = const data_type*;

    Variable();

    /**
     * Variable constructor.
     *
     * Wrap a shared pointer to column data along with metadata.
     *
     * @param src Shared pointer to memory allocated for the data column
     * @param size Number of rows in the data column
     * @param index Identifying column index into data matrix
     * @param bins Number of data value bins
     * @pre src data has been allocated memory for at least size elements.
     * @pre src data values are binned to a contiguous non-negative integer
     *      array starting at 0.
     * @pre src missing data values are represented by negative integers.
     * @exception invalid_argument data stored ptr, size, or bin argument is zero.
     */
    //
    // It's technically possible to compute the bins on the fly, but we
    // encourage better performance code by requiring it upfront, e.g.
    // precomputed in a table.
    //
    Variable(data_ptr src, std::size_t size, std::size_t index, std::size_t bins);

    std::size_t bins() const;
    std::size_t size() const;
    std::size_t index() const;

    /** Test if data at position is missing.
     *
     * @exception std::out_of_range
     */
    inline bool missing(std::size_t pos) const;

    /** Test if value is classified as missing.
     */
    static bool missingVal(data_type const val);

    data_type & operator[](std::size_t pos);
    data_type const& operator[](std::size_t pos) const;

    /**
     * @exception out_of_range
     */
    data_type & at(std::size_t const pos);
    data_type const& at(std::size_t const pos) const;

    /**
     * Variable uses default move and copy constructors that are shallow and
     * maintain const requirement on underlying data. A deep copy made with
     * this extra method.
     */
    Variable deepCopy();

    /**
     * Will resort to a deep inspection so two Variables with identical
     * content in different memory locations are equivalent.
     * Returns false if either Variable has invalid data, e.g. as a sideeffect
     * of std::move.
     */
    bool operator==(Variable const& other) const noexcept;

    /**
     * Variable inequality test.
     */
    bool operator!=(Variable const& other) const noexcept;

    std::string to_string();

    const_iterator begin() const;
    const_iterator end() const;
    iterator begin();
    iterator end();
};

class VariableException : public std::exception {
private:
    std::string msg;
public:
    VariableException(std::string const& method, std::string const& msg, int index) :
        msg("Variable::" + method + " : [index" + std::to_string(index) + "] " + msg) { }
    virtual const char* what() const throw() {
        return msg.c_str();
    };
};

class VariableOutOfRange : public std::out_of_range {
public:
    VariableOutOfRange(std::string const& method, int index, int pos, int size) :
        out_of_range("Variable::" + method + " : [index" + std::to_string(index) + "] Position " + std::to_string(pos) + " out of range [0," + std::to_string(size) ) { }
};

using VariableTuple = std::vector<Variable>;

} // mist
