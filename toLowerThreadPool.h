///////////////////////////////////////////////////////////////////////////////
//
//  toLowerThreadPool.h
//
//  Notes: 
//    * Thread pool passed as parameter
//    * No memory allocation
//    * Explicitly joins all threads before exit
//
//  Issues:
//    * Still calling getLowerChar, which calls tolower()
//    * Might be faster with look up table
//
//  Copyright © Pete Isensee (PKIsensee@msn.com).
//  All rights reserved worldwide.
//
//  Permission to copy, modify, reproduce or redistribute this source code is
//  granted provided the above copyright notice is retained in the resulting 
//  source code.
// 
//  This software is provided "as is" and without any express or implied
//  warranties.

#pragma once

#include <algorithm>
#include <cassert>
#include <ranges>
#include <span>
#include <string>
#include <thread>
#include "getLowerChar.h"

// Convert char sequence to lower case in place. Returns modified sequence.
constexpr auto&& toLowerThreadPool( auto&& s, auto&& threadPool ) noexcept
{
  assert( !threadPool.empty() );

  auto toLowerWorker = []( auto substr )
  {
    std::ranges::transform( substr, substr.begin(), []( auto c )
    {
      return getLowerChar( c );
    } );
  };

  // Divide up the work
  const size_t threadCount = threadPool.size();
  const size_t substrLen = s.size() / threadCount;

  // Kick off all but last thread to do case conversion
  size_t strOffset = 0;
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

  // Wait for case conversion to complete
  std::ranges::for_each( threadPool, []( auto& thread ) { thread.join(); } );
  return std::forward<decltype( s )>( s );
}

///////////////////////////////////////////////////////////////////////////////
