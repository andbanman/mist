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

DataMatrix::~DataMatrix()
{
}

// each column a variable
DataMatrix::DataMatrix(std::size_t ncol, std::size_t nrow, data_t b)
  : ncol(ncol)
  , nrow(nrow)
  , nvar(ncol)
  , svar(nrow)
{
  for (std::size_t ii = 0; ii < ncol; ii++) {
    vectors.push_back(
      mist::Variable::data_ptr(new mist::Variable::data_t[nrow]));
    for (std::size_t jj = 0; jj < nrow; jj++) {
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

// each column a variable
DataMatrix::DataMatrix(data_t data[], std::size_t ncol, std::size_t nrow)
  : ncol(ncol)
  , nrow(nrow)
  , nvar(ncol)
  , svar(nrow)
{
  for (std::size_t ii = 0; ii < ncol; ii++) {
    vectors.push_back(
      mist::Variable::data_ptr(new mist::Variable::data_t[nrow]));
    data_t bin = 0;
    for (std::size_t jj = 0; jj < nrow; jj++) {
      bin = std::max(bin, data_t(data[nrow * ii + jj] + 1));
      vectors[ii].get()[jj] = data[nrow * ii + jj];
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
  auto required_dtype = np::dtype::get_builtin<Variable::data_t>();
  if (np.get_dtype() != required_dtype) {
    throw DataMatrixException(
      "DataMatrix",
      "Invalid ndarray dtype, must be " +
        std::string(p::extract<char const*>(p::str(required_dtype))) +
        ", check numpy.ndarray.dtype");
  }

  auto data = (Variable::data_t*)np.get_data();
  nrow = np.shape(0);
  ncol = np.shape(1);

  bool rowmajor = np.get_flags() & np::ndarray::bitflag::C_CONTIGUOUS;
  nvar = (rowmajor) ? nrow : ncol;
  svar = (rowmajor) ? ncol : nrow;

  for (int ii = 0; ii < nvar; ii++) {
    auto var_ptr = data + ii * svar;
    // we don't own the memory, so use an empty shared pointer
    vectors.push_back(Variable::data_ptr(Variable::data_ptr(), var_ptr));
    data_t bin = 0;
    for (int jj = 0; jj < svar; jj++) {
      bin = std::max(bin, (data_t)(var_ptr[jj] + 1));
    }
    if (!bin) {
      // sanity check, probably never get here
      throw DataMatrixException("DataMatrix",
                                "Variable " + std::to_string(ii) +
                                  " has zero value bins. Remove column from "
                                  "input data before proceeding.");
    }
    bins.push_back(bin);
  }
}
#endif

DataMatrix::DataMatrix(std::string const& filename, bool rowmajor)
  : nrow(0)
  , ncol(0)
  , nvar(0)
  , svar(0)
{
  std::string line;
  std::string data_line;

  // open file for reading
  std::ifstream ifs(filename, std::ifstream::binary);
  if (!ifs.is_open()) {
    throw DataMatrixException("DataMatrix",
                              "Could not open input file '" + filename +
                                "': " + std::strerror(errno));
  }

  // count number of rows
  auto data_start_pos = ifs.tellg();
  bool skip = true;
  while (std::getline(ifs, line)) {
    // scan past header lines
    if (skip) {
      for (char c : line) {
        if (!issep(c) && !std::isdigit(c) && c!='-') {
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
    nrow++;
  }

  // count number of columns
  ncol++;
  char p = line[0];
  for (char c : data_line) {
    if ((issep(c) && !issep(p)) || c == '\n') {
      ncol++;
    }
    p = c;
  }

  svar = (rowmajor) ? ncol : nrow;
  nvar = (rowmajor) ? nrow : ncol;

  // initialize vectors
  for (int ii = 0; ii < nvar; ii++) {
    vectors.push_back(mist::Variable::data_ptr(
      new mist::Variable::data_t[svar])); // TODO: valgrind error on new
  }

  // Read data into matrix
  ifs.clear(); // XXX need to clear bits after getline goes through end
  ifs.seekg(data_start_pos, std::ios::beg);
  int row = 0;
  while (std::getline(ifs, line)) {
    // each line (row) parse the value of each vector (col)
    int col = 0;
    int pos = 0;
    int* vec = (rowmajor) ? &row : &col; //this vector
    int* elm = (rowmajor) ? &col : &row; //this element of vector
    char int_string[100];
    bool is_negative = false;
    for (auto c : line) {
      if (c == '-') {
        is_negative = true;
      }
      if (!std::isdigit(c) && pos) {
        auto val  = std::atoi(int_string);
        vectors[*vec].get()[*elm] = (is_negative) ? -1 * val : val;
        int_string[0] = '\0';
        pos = 0;
        col++;
        if (col >= ncol) {
          throw DataMatrixException(
            "DataMatrix",
            "Error loading file " + filename + ":" + std::to_string(row) +
              " - number of columns greater than expected.");
        }
        is_negative = false;
      } else if (std::isdigit(c)) {
        int_string[pos] = c;
        int_string[pos + 1] = 0;
        pos++;
      }
    }
    if (pos) {
      vectors[*vec].get()[*elm] = std::atoi(int_string);
      col++;
    }
    if (col != ncol) {
      throw DataMatrixException(
        "DataMatrix",
        "Error loading file " + filename + ":" + std::to_string(row) +
          " - number of columns less than expected.");
    }
    row++;
  }

  // determine number of bins for each variable
  for (auto const& v : vectors) {
    data_t bin = 0;
    for (index_t ii = 0; ii < svar; ii++) {
      bin = std::max(bin, (data_t)(v.get()[ii] + 1));
    }
    bins.push_back(bin);
  }
}

// Default rowmajor is a variable
DataMatrix::DataMatrix(std::string const& filename)
  : DataMatrix::DataMatrix(filename, true)
{
}

Variable
DataMatrix::get_variable(index_t i)
{
  return mist::Variable(vectors[i], svar, i, bins[i]);
};

DataMatrix::variables_ptr
DataMatrix::variables()
{
  if (!this->_variables) {
    this->_variables = variables_ptr(new Variable::tuple(nvar));
  }
  for (index_t ii = 0; ii < nvar; ii++) {
    (*this->_variables)[ii] = this->get_variable(ii);
  }
  return this->_variables;
}

void
DataMatrix::write_file(std::string const& filename, char sep)
{
  std::ofstream ofs(filename);
  if (!ofs.is_open()) {
    throw DataMatrixException(
      "write_file", "Could not open file " + filename + " for writing");
  }
  for (int ii = 0; ii < nvar; ii++) {
    for (int jj = 0; jj < svar; jj++) {
      ofs << std::to_string(vectors[ii].get()[jj]);
      if (jj < svar - 1 && svar > 1) {
        ofs << sep;
      }
    }
    ofs << std::endl;
  }
}

unsigned long DataMatrix::get_nvar() const { return nvar; }
unsigned long DataMatrix::get_svar() const { return svar; }
unsigned long DataMatrix::get_nrow() const { return nrow; }
unsigned long DataMatrix::get_ncol() const { return ncol; }

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
