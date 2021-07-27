#pragma once

#include <stdexcept>
#include <vector>

#include <boost/serialization/split_member.hpp>
#include <boost/serialization/vector.hpp>

#include "../Variable.hpp"

namespace mist {
namespace it {

class DistributionOutOfRange : public std::out_of_range
{
public:
  DistributionOutOfRange(std::string const& method, int pos, int size)
    : out_of_range("Distribution::" + method + " : Position " +
                   std::to_string(pos) + " out of range [0," +
                   std::to_string(size))
  {}
};

using DistributionData = double;

/** Joint probability array for N variables
 */
class Distribution
{
private:
  std::vector<DistributionData> data;
  std::vector<int> factors;
  std::size_t size;
  std::size_t nvar;

public:
  using Data = DistributionData;
  using value_type = Data;

  Distribution()
    : data(0)
    , factors(0)
    , size(0)
    , nvar(0)
  {};

  /** Construct directly from dimension strides
   */
  template<class Container>
  Distribution(Container const& strides)
    : Distribution()
  {
    // Dynamically resize factors
    std::size_t next_nvar = strides.size();
    if (this->nvar < next_nvar) {
      this->factors.resize(next_nvar);
    }
    this->nvar = next_nvar;

    // Dynamically resize data
    std::size_t next_size = 1;
    for (std::size_t ii = 0; ii < this->nvar; ii++) {
      this->factors[ii] = next_size;
      next_size *= strides[ii];
    }
    if (this->size < next_size) {
      this->data.resize(next_size);
    }
    this->size = next_size;
    this->data.assign(this->size, 0);
  };

  void
  initialize(Variable::tuple const& vars, Variable::indexes const& indexes)
  {
    // Dynamically resize factors
    std::size_t next_nvar = indexes.size();
    if (this->nvar < next_nvar) {
      this->factors.resize(next_nvar);
    }
    this->nvar = next_nvar;

    // Dynamically resize data
    std::size_t next_size = 1;
    for (size_t ii = 0; ii < nvar; ii++) {
      this->factors[ii] = next_size;
      next_size *= vars[indexes[ii]].bins();
    }
    if (this->size < next_size) {
      this->data.resize(next_size);
    }
    this->size = next_size;
    this->data.assign(this->size, 0); // SEGFAULT
  };

  /** Construct from a Variable tuple
   */
  Distribution(Variable::tuple const& vars)
    : Distribution()
  {
    Variable::indexes indexes(vars.size());
    int ii = 0;
    for (auto& index : indexes) {
      index = ii;
      ii++;
    }
    initialize(vars, indexes);
  }

  Distribution(Variable::tuple const& vars, Variable::indexes const& indexes)
    : Distribution()
  {
    initialize(vars, indexes);
  }


  //
  // accessors
  //
  template<typename Integer>
  Data const& operator[](Integer index) const
  {
    return this->data[index];
  }
  template<typename Integer>
  Data& operator[](Integer index)
  {
    return this->data[index];
  }

  template<typename Iter>
  Data const& operator()(Iter it, Iter end) const
  {
    int index = 0;
    for (int ii = 0; it != end; ++it, ii++)
      index += this->factors[ii] * (*it);
    return this->data[index];
  }
  template<typename Iter>
  Data& operator()(Iter it, Iter end)
  {
    int index = 0;
    for (int ii = 0; it != end; ++it, ii++)
      index += this->factors[ii] * (*it);
    return this->data[index];
  }
  template<typename Container>
  Data const& operator()(Container const& indexes) const
  {
    return this->operator()(indexes.begin(), indexes.end());
  }
  template<typename Container>
  Data& operator()(Container const& indexes)
  {
    return this->operator()(indexes.begin(), indexes.end());
  }

  //
  // at exception throwing accessors
  //
  template<typename Iter>
  Data const& at(Iter it, Iter end) const
  {
    int index = 0;
    for (int ii = 0; it != end; ++it, ii++)
      index += this->factors[ii] * (*it);
    if (index >= this->size)
      throw DistributionOutOfRange("at", index, this->size);
    return this->data[index];
  }
  template<typename Iter>
  Data& at(Iter it, Iter end)
  {
    int index = 0;
    for (int ii = 0; it != end; ++it, ii++)
      index += this->factors[ii] * (*it);
    if (index >= this->size)
      throw DistributionOutOfRange("at", index, this->size);
    return this->data[index];
  }
  template<typename Container>
  Data const& at(Container const& indexes) const
  {
    return this->at(indexes.begin(), indexes.end());
  }
  template<typename Container>
  Data& at(Container const& indexes)
  {
    return this->at(indexes.begin(), indexes.end());
  }

  //
  // Plain data templates
  //
  template<typename Integer>
  Data const& operator()(int d, Integer indices[]) const
  {
    int index = 0;
    for (int ii = 0; ii < d; ii++)
      index += this->factors[ii] * indices[ii];
    return this->data[index];
  };
  template<typename Integer>
  Data& operator()(int d, Integer indices[])
  {
    int index = 0;
    for (int ii = 0; ii < d; ii++)
      index += this->factors[ii] * indices[ii];
    return this->data[index];
  };

  //
  // Peformance critical indexing
  //
  Data const& operator()(int v0, int b0) const { return this->data[v0]; };
  Data& operator()(int v0, int b0) { return this->data[v0]; };
  Data const& operator()(int v0, int v1, int b0, int b1) const
  {
    return this->data[v0 + b0 * v1];
  };
  Data& operator()(int v0, int v1, int b0, int b1)
  {
    return this->data[v0 + b0 * v1];
  };
  Data const& operator()(int v0, int v1, int v2, int b0, int b1, int b2) const
  {
    return this->data[v0 + b0 * v1 + b0 * b1 * v2];
  };
  Data& operator()(int v0, int v1, int v2, int b0, int b1, int b2)
  {
    return this->data[v0 + b0 * v1 + b0 * b1 * v2];
  };

  bool operator==(Distribution const& other) const noexcept
  {
    return this->data == other.data && this->factors == other.factors;
  }

  bool empty() { return data.empty(); }

  using iterator = std::vector<Data>::iterator;
  using const_iterator = std::vector<Data>::const_iterator;
  const_iterator begin() const { return this->data.begin(); };
  const_iterator end() const { return this->data.end(); };
  iterator begin() { return this->data.begin(); };
  iterator end() { return this->data.end(); };

  /** Multiply each value in distribution by factor
   */
  void scale(double factor)
  {
    for (std::size_t i = 0; i < this->size; i++) {
      this->data[i] *= factor;
    }
  };

  /** Normalize distribution
   */
  void normalize()
  {
    std::size_t norm = 0;
    for (std::size_t i = 0; i < this->size; i++) {
      norm += this->data[i];
    }
    if (norm)
      this->scale(1.0 / norm);
  };

#if 0
  //
  // Serialization
  //
  friend class boost::serialization::access;

  template<class Archive>
  void load(Archive& ar, const unsigned int version)
  {
    // members
    ar& factors;

    // explicit data
    // since this class specializes std::vector need push/pop each element
    std::size_t size = 0;
    ar& size;
    this->data.resize(size);
    for (std::size_t i = 0; i < size; i++) {
      Data dat;
      ar& dat;
      this->data[i] = dat;
    }
  }
  template<class Archive>
  void save(Archive& ar, const unsigned int version) const
  {
    // members
    ar& factors;

    // explicit data
    // since this class specializes std::vector need push/pop each element
    auto size = this->size;
    ar& size;
    for (std::size_t i = 0; i < size; i++) {
      ar(&this->data[i]);
    }
  }
  BOOST_SERIALIZATION_SPLIT_MEMBER()
#endif
};

} // it
} // mist
