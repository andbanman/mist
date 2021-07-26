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

public:
  using Data = DistributionData;
  using value_type = Data;

  Distribution()
    : factors(){};

  /** Construct directly from dimension strides
   */
  template<class Container>
  Distribution(Container const& strides)
    : factors()
  {
    std::size_t size = 1;
    for (auto stride : strides) {
      factors.push_back(size);
      size *= stride;
    }
    this->data.resize(size);
    for (std::size_t ii = 0; ii < size; ii++)
      this->data[ii] = 0;
  };

  /** Construct from a Variable tuple
   */
  Distribution(Variable::tuple const& vars)
  {
    int size = 1;
    int nvar = vars.size();
    this->factors.resize(nvar);
    for (int ii = 0; ii < nvar; ii++) {
      this->factors[ii] = size;
      size *= vars[ii].bins();
    }
    this->data.resize(size);
    for (int ii = 0; ii < size; ii++)
      this->data[ii] = 0;
  }

  Distribution(Variable::tuple const& vars, Variable::indexes const& indexes)
  {
    int size = 1;
    int nvar = indexes.size();
    this->factors.resize(nvar);
    for (int ii = 0; ii < nvar; ii++) {
      this->factors[ii] = size;
      size *= vars[indexes[ii]].bins();
    }
    this->data.resize(size);
    for (int ii = 0; ii < size; ii++)
      this->data[ii] = 0;
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
      index += factors[ii] * (*it);
    return this->data[index];
  }
  template<typename Iter>
  Data& operator()(Iter it, Iter end)
  {
    int index = 0;
    for (int ii = 0; it != end; ++it, ii++)
      index += factors[ii] * (*it);
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
      index += factors[ii] * (*it);
    if (index >= this->data.size())
      throw DistributionOutOfRange("at", index, this->data.size());
    return this->data[index];
  }
  template<typename Iter>
  Data& at(Iter it, Iter end)
  {
    int index = 0;
    for (int ii = 0; it != end; ++it, ii++)
      index += factors[ii] * (*it);
    if (index >= this->data.size())
      throw DistributionOutOfRange("at", index, this->data.size());
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
      index += factors[ii] * indices[ii];
    return this->data[index];
  };
  template<typename Integer>
  Data& operator()(int d, Integer indices[])
  {
    int index = 0;
    for (int ii = 0; ii < d; ii++)
      index += factors[ii] * indices[ii];
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
    for (auto& val : this->data)
      val *= factor;
  };

  /** Normalize distribution
   */
  void normalize()
  {
    std::size_t norm = 0;
    for (auto val : this->data)
      norm += val;
    if (norm)
      this->scale(1.0 / norm);
  };

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
    int size = 0;
    ar& size;
    this->data.resize(size);
    for (int i = 0; i < size; i++) {
      Data dat;
      ar& dat;
      (this->data)[i] = dat;
    }
  }
  template<class Archive>
  void save(Archive& ar, const unsigned int version) const
  {
    // members
    ar& factors;

    // explicit data
    // since this class specializes std::vector need push/pop each element
    int size = this->data.size();
    ar& size;
    for (int i = 0; i < size; i++) {
      ar&(this->data)[i];
    }
  }
  BOOST_SERIALIZATION_SPLIT_MEMBER()
};

} // it
} // mist
