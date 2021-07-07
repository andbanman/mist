#include <boost/filesystem/operations.hpp>
#include <string>

#include <dirent.h>

#include <boost/filesystem.hpp>
#include <boost/test/unit_test.hpp>

#include "cache/SmallFiles.hpp"

using namespace mist;

BOOST_AUTO_TEST_CASE(CacheSmallFiles_test)
{
  // create the test directory
  std::string test_cache_dir = "testcache";
  boost::filesystem::path dir(test_cache_dir);
  boost::filesystem::remove_all(dir);
  bool dir_create = boost::filesystem::create_directory(dir);
  BOOST_TEST(dir_create);

  cache::SmallFiles<std::string> cache("testcache");

  cache.put({ 0 }, "hello");
  cache.put({ 1 }, "world");
  cache.put({ 0 }, "overwrite");

  std::string res = "";
  BOOST_TEST(*cache.get({ 0 }) == "overwrite");
  BOOST_TEST(*cache.get({ 1 }) == "world");
}
