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
//   * Can't be noexcept, because vector can throw
//   * Better solution would chunk boundaries to cache size to prevent
//     false sharing
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
    std::ranges::transform( substr, std::begin( substr ), []( char c )
    {
      return GetLowerCharBranch( c );
    } );
  };

  // Divide up the work
  const size_t threadCount = std::max( std::thread::hardware_concurrency(), 1u );
  const size_t substrLen = std::size( s ) / threadCount;

  // Kick off all but last thread to do case conversion
  size_t strOffset = 0;
  std::vector<std::jthread> threadPool;
  threadPool.reserve( threadCount );
  for( size_t thread = 0; thread < threadCount - 1; ++thread )
  {
    std::span substr{ s.data() + strOffset, substrLen };
    threadPool.emplace_back( toLowerWorker, substr );
    strOffset += substrLen;
  }

  // Last thread handles final segment plus any remaining letters
  const size_t remainingLetterCount = s.size() % threadCount;
  std::span substr{ s.data() + strOffset, substrLen + remainingLetterCount };
  threadPool.emplace_back( toLowerWorker, substr );

  // jthreads automatically join when thread vector exits scope,
  // so conversion is complete upon function return
  return std::forward<Str>( s );
}

///////////////////////////////////////////////////////////////////////////////
