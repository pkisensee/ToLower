///////////////////////////////////////////////////////////////////////////////
//
// ToLowerThreads.h
//
// Notes: 
//   * Multi-threaded solution
//
// Issues:
//   * Threads are created within the function
//   * Allocates memory
//   * In practice, this function should create the jthread vector and
//     then invoke toLowerD
//   * Can't be noexcept, because vector can throw
//
// Copyright © Pete Isensee (PKIsensee@msn.com).
// All rights reserved worldwide.
//
// Permission to copy, modify, reproduce or redistribute this source code is
// granted provided the above copyright notice is retained in the resulting 
// source code.
// 
// This software is provided "as is" and without any express or implied
// warranties.

#pragma once
#include <algorithm>
#include <ranges>
#include <span>
#include <string>
#include <thread>
#include <vector>
#include <utility>
#include "Concepts.h"
#include "GetLowerChar.h"

// Convert char sequence to lower case in place. Returns modified sequence.
template <CharRange Str>
constexpr Str&& ToLowerThreads( Str&& s )
{
  auto toLowerWorker = []( auto substr )
  {
    std::ranges::transform( substr, substr.begin(), []( auto c )
    {
      return GetLowerCharCondMove( c );
    } );
  };

  // Divide up the work
  const size_t threadCount = std::max( std::thread::hardware_concurrency(), 1u );
  const size_t substrLen = std::size( s ) / threadCount;

  // Kick off all but last thread to do case conversion
  size_t strOffset = 0;
  std::vector<std::jthread> threadPool{ threadCount };
  auto pool = threadPool | std::views::take( threadCount - 1 );
  for ( auto& thread : pool )
  {
    std::span substr{ s.data() + strOffset, substrLen };
    thread = std::jthread{ toLowerWorker, substr };
    strOffset += substrLen;
  }

  // Last thread handles final segment plus any remaining letters
  const size_t remainingLetterCount = s.size() % threadCount;
  std::span substr{ s.data() + strOffset, substrLen + remainingLetterCount };
  threadPool.back() = std::jthread{ toLowerWorker, substr };

  // jthreads automatically join when thread vector exits scope,
  // so conversion is complete upon function return
  return std::forward<Str>( s );
}

///////////////////////////////////////////////////////////////////////////////
