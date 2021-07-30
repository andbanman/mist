#pragma once
#include <cctype>
constexpr inline
std::size_t
binomial(std::size_t n, std::size_t k) noexcept
{
  return
    (        k> n  )? 0 :        // out of range
    (k==0 || k==n  )? 1 :        // edge
    (k==1 || k==n-1)? n :        // first
    (     k+k < n  )?            // recursive:
    (binomial(n-1,k-1) * n)/k :  //  path to k=1   is faster
    (binomial(n-1,k) * n)/(n-k); //  path to k=n-1 is faster
}
