///////////////////////////////////////////////////////////////////////////////
//
// ToLowerThreadPool.h
//
// Notes: 
//   * Thread pool passed as parameter
//   * No memory allocation
//   * Explicitly joins all threads before exit
//
// Issues:
//   * Default version still calling GetLowerChar, which calls tolower()
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
#include <cassert>
#include <ranges>
#include <span>
#include <string>
#include <thread>
#include <utility>
#include "Concepts.h"
#include "GetLowerChar.h"
#include "ToLower5thBit.h"
#include "ToLowerSSE.h"

// Helper that accepts the worker function
template <CharRange Str, typename ThreadPool, typename Worker>
constexpr Str&& ToLowerThreadPoolWithWorker( Str&& s,
  ThreadPool&& threadPool, Worker&& toLowerWorker ) noexcept
{
  assert( !threadPool.empty() );

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
  return std::forward<Str>( s );
}

// Convert char sequence to lower case in place. Returns modified sequence.
// Uses ToLowerSSE().
template <CharRange Str, typename ThreadPool>
constexpr Str&& ToLowerThreadPoolSSE( Str&& s, ThreadPool&& threadPool ) noexcept
{
  return ToLowerThreadPoolWithWorker( s, threadPool, []( auto&& substr )
  {
    ToLowerSSE( substr );
  } );
}

// Convert char sequence to lower case in place. Returns modified sequence.
// Uses ToLower5thBit().
template <CharRange Str, typename ThreadPool>
constexpr Str&& ToLowerThreadPool5thBit( Str&& s, ThreadPool&& threadPool ) noexcept
{
  return ToLowerThreadPoolWithWorker( s, threadPool, []( auto&& substr )
  { 
    ToLower5thBit( substr ); 
  } );
}

// Convert char sequence to lower case in place. Returns modified sequence.
// Uses ToLowerAuto().
template <CharRange Str, typename ThreadPool>
constexpr Str&& ToLowerThreadPool( Str&& s, ThreadPool&& threadPool ) noexcept
{
  return ToLowerThreadPoolWithWorker( s, threadPool, []( auto&& substr )
  {
    ToLowerAuto( substr );
  } );
}

///////////////////////////////////////////////////////////////////////////////
