///////////////////////////////////////////////////////////////////////////////
//
// lowerb.cpp
//
// Notes: 
//    * multi-threaded solution
//
// Issues:
//    * how do you decide how many threads?

#include <algorithm>
#include <cctype>
#include <ranges>
#include <span>
#include <string>
#include <thread>
#include <vector>

char toLowerChar( char c )
{
  auto uchar = static_cast<unsigned char>( c );
  auto lowered = std::tolower( uchar );
  return static_cast<char>( lowered );
}

std::string& lowerb( std::string& s ) noexcept
{ 
  auto toLowerWorker = []( auto substr )
  {
    std::ranges::transform( substr, substr.begin(), []( auto c )
    {
      return toLowerChar( c );
    } );
  };

  // Kick off threads to do case conversion
  const size_t threadCount = std::max( std::thread::hardware_concurrency(), 1u );
  const size_t substrLen = s.size() / threadCount;
  size_t strOffset = 0; 
  std::vector<std::jthread> pool( threadCount - 1 );
  for( auto& thread : pool )
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
  return s;
}
