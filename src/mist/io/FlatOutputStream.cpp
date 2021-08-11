#include <new>

#include "io/FlatOutputStream.hpp"
#include "it/Entropy.hpp"

using namespace mist;
using namespace mist::io;

FlatOutputStream::FlatOutputStream()
  : FlatOutputStream(0, 0, 0)
{
};

FlatOutputStream::FlatOutputStream(std::size_t rowsize, std::size_t offset)
  : FlatOutputStream(0, rowsize, offset)
{
};

FlatOutputStream::FlatOutputStream(std::size_t offset)
  : FlatOutputStream(0, 0, offset)
{
};

FlatOutputStream::FlatOutputStream(std::size_t size, std::size_t rowsize, std::size_t offset)
  : OutputStream(mutex_ptr(new mutex_type))
  , size(size)
  , rowsize(rowsize)
  , offset(offset)
{
  try {
    data = new std::vector<data_t>((size) * rowsize);
  } catch (std::bad_alloc) {
    throw FlatOutputStreamException("FlatOutputStream", "Not enough memory to init array");
  }
};

FlatOutputStream::~FlatOutputStream()
{
};

void
FlatOutputStream::push(std::size_t tuple_no, tuple_type const& tuple, result_type const& result)
{
  // out of range, no new data can be saved
  if (size && (tuple_no-offset) >= size) {
    throw FlatOutputStreamException("push", "Tuple number out of range");
  }
  if (tuple.size() + result.size() != rowsize) {
    throw FlatOutputStreamException("push", "Unexpected tuple and result length");
  }

  // push into store
  if (!size) {
    try {
      data->insert(data->end(), tuple.begin(), tuple.end());
      data->insert(data->end(), result.begin(), result.end());
    } catch (std::bad_alloc &e) {
      throw FlatOutputStreamException("push", "Could not push result, out of memory");
    }
  }
  else {
    std::size_t index = (tuple_no-offset)*rowsize;
    for (auto d : tuple) {
      (*data)[index] = d;
      index++;
    }
    for (auto d : result) {
      (*data)[index] = d;
      index++;
    }
  }
}

void
FlatOutputStream::push(std::size_t tuple_no, tuple_type const& tuple, it::entropy_type result)
{
  // out of range, no new data can be saved
  if (size && (tuple_no-offset) >= size) {
    throw FlatOutputStreamException("push", "Tuple number out of range");
  }
  if (tuple.size() + 1 != rowsize) {
    throw FlatOutputStreamException("push", "Unexpected tuple and result length");
  }

  // push into store
  if (!size) {
    try {
      data->insert(data->end(), tuple.begin(), tuple.end());
      data->push_back(result);
    } catch (std::bad_alloc &e) {
      throw FlatOutputStreamException("push", "Could not push result, out of memory");
    }
  }
  else {
    std::size_t index = (tuple_no-offset)*rowsize;
    for (auto d : tuple) {
      (*data)[index] = d;
      index++;
    }
    (*data)[index] = result;
  }
}

std::vector<FlatOutputStream::data_t> const&
FlatOutputStream::get_results()
{
  return *data;
}

// move the data from other into this
// other is empty after the operation
void
FlatOutputStream::relocate(FlatOutputStream &other)
{
  try {
    data->insert(data->end(), other.data->begin(), other.data->end());
    other.data->clear();
  } catch (std::bad_alloc &e) {
    throw FlatOutputStreamException("relocate", "Could not move data, not enough memory for temporary copy.");
  }
}

#if BOOST_PYTHON_EXTENSIONS
np::ndarray
FlatOutputStream::py_get_results()
{
  // TODO should this also include tuples, or keep them separate?
	return np::from_data(data->data(),
                       np::dtype::get_builtin<data_t>(),
                       p::make_tuple(data->size() / rowsize, rowsize),
                       p::make_tuple(sizeof(data_t)*rowsize,sizeof(data_t)*1),
                       p::object());
}
#endif
