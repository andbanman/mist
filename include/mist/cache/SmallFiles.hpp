#pragma once

#include <exception>
#include <fstream>
#include <functional>
#include <stdexcept>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

// TODO IF boost version less than ...
#include <boost/functional/hash.hpp>
// else
//#include <boost/container_hash/hash.hpp>

#include "cache/Cache.hpp"

namespace mist {
namespace cache {

class SmallFilesOutOfRange : public std::out_of_range
{
public:
  SmallFilesOutOfRange(std::string const& method,
                       std::string const& file,
                       std::string const& key)
    : out_of_range("SmallFiles::" + method + " : File '" + file + "' for key " +
                   key + "does not exist or could not be opened.")
  {}
};

/** Filesystem cache with each value a small file.
 */
template<class V>
class SmallFiles : public Cache<V>
{

public:
  SmallFiles()
    : dir("./"){};
  SmallFiles(std::string const& dir)
    : dir(dir){};
  ~SmallFiles(){};

  bool has(K const& key)
  {
    std::string filename =
      this->dir + "/" + std::to_string(boost::hash<K>{}(key));
    std::ifstream f(filename.c_str());
    return f.good();
  }

  std::pair<K, V> put(K const& key, V const& val)
  {
    std::string filename =
      this->dir + "/" + std::to_string(boost::hash<K>{}(key));
    std::ofstream ofs(filename);

    {
      boost::archive::text_oarchive oa(ofs);
      // overwrites data, could leave stale bits past end of object
      // not space efficient but correct
      oa << val;
    }

    ofs.flush();

    return std::make_pair(K(), V());
  }

  std::shared_ptr<V> get(K const& key)
  {
    V* val = new V();
    std::string filename =
      this->dir + "/" + std::to_string(boost::hash<K>{}(key));
    std::ifstream ifs(filename);

    if (!ifs.is_open()) {
      ++this->_misses;
      throw SmallFilesOutOfRange("get", filename, this->key_to_string(key));
    } else {
      ++this->_hits;
      boost::archive::text_iarchive ia(ifs);
      ia >> *val; // can throw
    }

    return std::shared_ptr<V>(val);
  }

  std::size_t size()
  {
    return 0; // TODO
  }

  std::size_t bytes()
  {
    return 0; // TODO
  }

  void flush()
  {
    // flush each write
  }

private:
  std::string dir;
};

} // cache
} // mist
