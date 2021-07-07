#include "Variable.hpp"

using namespace mist;

// inherit constructors
// using std::shared_ptr<data_type>::shared_ptr;
Variable::Variable(){};

//!
//! Variable constructor.
//!
//! Wrap a shared pointer to column data along with metadata.
//!
//! @param src Shared pointer to memory allocated for the data column
//! @param size Number of rows in the data column
//! @param index Identifying column index into data matrix
//! @param bins Number of data value bins
//! @pre src data has been allocated memory for at least size elements.
//! @pre src data values are binned to a contiguous non-negative integer
//       array starting at 0.
//! @pre src missing data values are represented by negative integers.
//! @exception invalid_argument data stored ptr, size, or bin argument is zero.
//!
//
// It's technically possible to compute the bins on the fly, but we
// encourage better performance code by requiring it upfront, e.g.
// precomputed in a table.
//
Variable::Variable(data_ptr src,
                   std::size_t size,
                   std::size_t index,
                   std::size_t bins)
  : data(src)
  , _size(size)
  , _index(index)
  , _bins(bins)
{
  if (!src.get()) {
    throw VariableException(
      "Variable", "src stored pointer cannot be null", index);
  }
  if (!size) {
    throw VariableException("Variable", "size cannot be zero", index);
  }
  if (!bins) {
    throw VariableException("Variable", "bins cannot be zero", index);
  }
};

std::size_t
Variable::bins() const
{
  return _bins;
}
std::size_t
Variable::size() const
{
  return _size;
}
std::size_t
Variable::index() const
{
  return _index;
}

//!
//! Test if data at position is missing.
//!
//! @exception std::out_of_range
//!
inline bool
Variable::missing(std::size_t pos) const
{
  if (pos >= _size) {
    throw VariableOutOfRange("missing", _index, pos, _size);
  }
  return this->data.get()[pos] < 0;
};

bool
Variable::missingVal(Variable::data_type const val)
{
  return VARIABLE_MISSING_VAL(val);
}

//!
//! Access data value at position.
//!
Variable::data_type& Variable::operator[](std::size_t pos)
{
  return this->data.get()[pos];
};
Variable::data_type const& Variable::operator[](std::size_t pos) const
{
  return this->data.get()[pos];
};

//!
//! Access data value at position.
//!
//! @exception out_of_range
Variable::data_type&
Variable::at(std::size_t const pos)
{
  if (pos >= _size) {
    throw VariableOutOfRange("at", _index, pos, _size);
  }
  return this->data.get()[pos];
};
Variable::data_type const&
Variable::at(std::size_t const pos) const
{
  if (pos >= _size) {
    throw VariableOutOfRange("at", _index, pos, _size);
  }
  return this->data.get()[pos];
};

//!
//! Variable deep copy
//!
//! Variable uses default move and copy constructors that are shallow and
//! maintain const requirement on underlying data. A deep copy made with
//! this extra method.
Variable
Variable::deepCopy()
{
  // TODO test for leaks
  // deep copy the data
  data_ptr data(new Variable::data_type[this->_size]);
  for (std::size_t ii = 0; ii < this->_size; ii++) {
    data.get()[ii] = this->data.get()[ii];
  }

  // new variable
  return Variable(data, this->_size, this->_index, this->_bins);
};

//!
//! Variable equality test.
//!
//! Will resort to a deep inspection so two Variables with identical
//! content in different memory locations are equivalent.
//! Returns false if either Variable has invalid data, e.g. as a sideeffect
//! of std::move.
bool
Variable::operator==(Variable const& other) const noexcept
{
  if (!this->data || !other.data) {
    return false;
  }
  if ((this->_size != other._size) || (this->_index != other._index) ||
      (this->_bins != other._bins)) {
    return false;
  } else if (this->data.get() == other.data.get()) {
    return true;
  } else {
    for (std::size_t ii = 0; ii < this->_size; ii++) {
      if (this->data.get()[ii] != other.data.get()[ii]) {
        return false;
      }
    }
  }
  return true;
};

//!
//! Variable inequality test.
//!
bool
Variable::operator!=(Variable const& other) const noexcept
{
  return !(*this == other);
};

std::string
Variable::to_string()
{
  std::string s;
  int size = this->_size;
  for (int ii = 0; ii < size; ii++) {
    s += std::to_string(this->data.get()[ii]);
    if (ii < size - 1) {
      s += ",";
    }
  }
  return s;
}

Variable::const_iterator
Variable::begin() const
{
  return this->data.get();
};
Variable::const_iterator
Variable::end() const
{
  return this->data.get() + _size;
};
Variable::iterator
Variable::begin()
{
  return this->data.get();
};
Variable::iterator
Variable::end()
{
  return this->data.get() + _size;
};
