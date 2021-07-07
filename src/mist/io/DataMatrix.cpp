#include <cctype>
#include <cerrno>
#include <cstring>

#include "io/DataMatrix.hpp"

using namespace mist;
using namespace mist::io;

static inline bool
issep(char c)
{
  return (std::isspace(c) || c == ',');
}

DataMatrix::DataMatrix(int n, int m, int b)
  : n(n)
  , m(m)
{
  for (int ii = 0; ii < n; ii++) {
    vectors.push_back(
      mist::Variable::data_ptr(new mist::Variable::data_type[m]));
    for (int jj = 0; jj < m; jj++) {
      vectors[ii].get()[jj] = 0;
    }
    if (!b) {
      throw DataMatrixException("DataMatrix",
                                "Column " + std::to_string(ii) +
                                  " has zero value bins. Remove column from "
                                  "input data before proceeding.");
    }
    bins.push_back(b);
  }
}

DataMatrix::DataMatrix(int data[], int n, int m)
  : n(n)
  , m(m)
{
  for (int ii = 0; ii < n; ii++) {
    vectors.push_back(
      mist::Variable::data_ptr(new mist::Variable::data_type[m]));
    int bin = 0;
    for (int jj = 0; jj < m; jj++) {
      bin = std::max(bin, data[m * ii + jj] + 1);
      vectors[ii].get()[jj] = data[m * ii + jj];
    }
    if (!bin) {
      throw DataMatrixException("DataMatrix",
                                "Column " + std::to_string(ii) +
                                  " has zero value bins. Remove column from "
                                  "input data before proceeding.");
    }
    bins.push_back(bin);
  }
}

#ifdef BOOST_PYTHON_EXTENSIONS
DataMatrix::DataMatrix(np::ndarray const& np)
{
  // To support direct data use without copying, the input must be
  // the correct type and c-style contiguous.
  // Verify type agreement.
  auto required_dtype = np::dtype::get_builtin<Variable::data_type>();
  if (np.get_dtype() != required_dtype) {
    throw DataMatrixException(
      "DataMatrix",
      "Invalid ndarray dtype, must be " +
        std::string(p::extract<char const*>(p::str(required_dtype))) +
        ", check numpy.ndarray.dtype");
  }

  // Verify memory layout
  auto flags = np.get_flags();
  if (!(flags & np::ndarray::bitflag::C_CONTIGUOUS)) {
    throw DataMatrixException(
      "DataMatrix",
      "Input ndarray is not C contiguous, check numpy.ndarray.flags");
  }

  auto data = (Variable::data_type*)np.get_data();
  auto n = np.shape(0);
  auto m = np.shape(1);
  for (int ii = 0; ii < n; ii++) {
    auto column_ptr = data + ii * m;
    // we don't own the memory, so use an empty shared pointer
    vectors.push_back(Variable::data_ptr(Variable::data_ptr(), column_ptr));
    int bin = 0;
    for (int jj = 0; jj < m; jj++) {
      bin = std::max(bin, column_ptr[jj] + 1);
    }
    if (!bin) {
      throw DataMatrixException("DataMatrix",
                                "Column " + std::to_string(ii) +
                                  " has zero value bins. Remove column from "
                                  "input data before proceeding.");
    }
    bins.push_back(bin);
  }
  this->n = n;
  this->m = m;
}
#endif

// each column is a variable
DataMatrix::DataMatrix(std::string const& filename)
{
  this->m = 0;
  this->n = 0;
  std::string line;
  std::string data_line;

  // open file for reading
  std::ifstream ifs(filename, std::ifstream::binary);
  if (!ifs.is_open()) {
    throw DataMatrixException("DataMatrix",
                              "Could not open input file '" + filename +
                                "': " + std::strerror(errno));
  }

  // count number of measurements
  auto data_start_pos = ifs.tellg();
  bool skip = true;
  while (std::getline(ifs, line)) {
    // scan past header lines
    if (skip) {
      for (char c : line) {
        if (!issep(c) && !std::isdigit(c)) {
          skip = true;
          break;
        }
        skip = false;
      }
      if (skip) {
        data_start_pos = ifs.tellg();
        continue;
      } else {
        // done scanning, record positing
        data_line = line;
      }
    }
    this->m++;
  }

  // count number of variables in last line
  this->n++;
  char p = line[0];
  for (char c : data_line) {
    if ((issep(c) && !issep(p)) || c == '\n') {
      this->n++;
    }
    p = c;
  }

  // initialize vectors
  for (int ii = 0; ii < n; ii++) {
    vectors.push_back(mist::Variable::data_ptr(
      new mist::Variable::data_type[m])); // TODO: valgrind error on new
  }

  // Read data into matrix
  ifs.clear(); // XXX need to clear bits after getline goes through end
  ifs.seekg(data_start_pos, std::ios::beg);
  int ii = 0;
  while (std::getline(ifs, line)) {
    // each line (ii) parse the value of each vector (jj)
    int jj = 0;
    int pos = 0;
    char int_string[100];
    for (auto c : line) {
      if (!std::isdigit(c) && pos) {
        vectors[jj].get()[ii] = std::atoi(int_string);
        int_string[0] = '\0';
        pos = 0;
        jj++;
        if (jj >= n) {
          throw DataMatrixException(
            "DataMatrix",
            "Error loading file " + filename + ":" + std::to_string(ii) +
              " - number of columns greater than expected.");
        }
      } else if (std::isdigit(c)) {
        int_string[pos] = c;
        int_string[pos + 1] = 0;
        pos++;
      }
    }
    if (pos) {
      vectors[jj].get()[ii] = std::atoi(int_string);
    }
    ii++;
  }

  // determine number of bins for each variable
  for (auto& v : vectors) {
    int bin = 0;
    for (int ii = 0; ii < this->m; ii++) {
      bin = std::max(bin, v.get()[ii] + 1);
    }
    bins.push_back(bin);
  }
}

mist::Variable
DataMatrix::getColumn(int i)
{
  return mist::Variable(vectors[i], m, i, bins[i]);
};

Variable::tuple
DataMatrix::variables()
{
  Variable::tuple ret(n);
  for (int ii = 0; ii < n; ii++) {
    ret[ii] = this->getColumn(ii);
  }
  return ret;
}

void
DataMatrix::write_file(std::string const& filename, char sep)
{
  std::ofstream ofs(filename);
  if (!ofs.is_open()) {
    throw DataMatrixException(
      "write_file", "Could not open file " + filename + " for writing");
  }
  for (int ii = 0; ii < m; ii++) {
    for (int jj = 0; jj < n; jj++) {
      ofs << vectors[jj].get()[ii];
      if (jj < n - 1 && n > 1) {
        ofs << sep;
      }
    }
    ofs << std::endl;
  }
}

void
DataMatrix::write_file(std::string const& filename)
{
  write_file(filename, ',');
}

#ifdef BOOST_PYTHON_EXTENSIONS
void
DataMatrix::python_write_file(std::string const& filename)
{
  this->write_file(filename, ',');
}
#endif
