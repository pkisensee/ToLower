///////////////////////////////////////////////////////////////////////////////
//
// toLowerC.cpp
//
// Notes: 
//    * multi-threaded solution
//
// Issues:
//    * threads could be a parameter
//    * if thread count not a parameter, how should thread count be chosen?
//    * could use existing thread pool

#include <algorithm>
#include <ranges>
#include <span>
#include <string>
#include <thread>
#include <vector>
#include "getLowerChar.h"

constexpr auto&& toLowerC( auto&& s ) noexcept
{
  auto toLowerWorker = []( auto substr )
  {
    std::ranges::transform( substr, substr.begin(), []( auto c )
    {
      return getLowerChar( c );
    } );
  };

  // Divide up the work
  const size_t threadCount = std::max( std::thread::hardware_concurrency(), 1u );
  const size_t substrLen = s.size() / threadCount;

  // Kick off threads to do case conversion
  size_t strOffset = 0;
  std::vector<std::jthread> pool( threadCount - 1 );
  for ( auto& thread : pool )
  {
    std::span substr{ s.data() + strOffset, substrLen };
    thread = std::jthread( toLowerWorker, substr );
    strOffset += substrLen;
  }

  // Last thread handles remaining letters
  const size_t remainingLetterCount = s.size() % threadCount;
  std::span substr{ s.data() + strOffset, substrLen + remainingLetterCount };
  std::jthread lastThread = std::jthread( toLowerWorker, substr );

  // jthreads automatically join, so conversion is complete upon function return
  return std::forward<decltype( s )>( s );
}

///////////////////////////////////////////////////////////////////////////////
