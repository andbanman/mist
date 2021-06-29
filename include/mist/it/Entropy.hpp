#pragma once

#include <vector>

namespace mist {
namespace it {

using entropy_type = double;

using Entropy = std::vector<entropy_type>;

enum struct d1 : int {
    e0,
    size
};

enum struct d2 : int {
    e0,
    e1,
    e01,
    size
};

enum struct d3 : int {
     e0,
     e1,
     e2,
     e01,
     e02,
     e12,
     e012,
     size
};

enum struct d4 : int {
     e0,
     e1,
     e2,
     e3,
     e01,
     e02,
     e03,
     e12,
     e13,
     e23,
     e012,
     e013,
     e023,
     e123,
     e0123,
     size
};

} // it
} // mist
