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

std::string& lowerb(std::string& s)
{ 
  auto toLowerWorker = []( std::span<char> substr )
    {
      for( auto& c : substr )
      {
        auto uc = static_cast<unsigned char>( c );
        c = static_cast<char>( std::tolower( uc ) );
      }
    };

  const auto platformThreads = std::thread::hardware_concurrency();
  const size_t threadCount = platformThreads ? platformThreads : 1;
  const size_t substrLen = s.size() / threadCount;
  const size_t extraLetterCount = s.size() % threadCount; 
  size_t strOffset = 0; 
  std::vector<std::jthread> pool(threadCount);
  for( size_t i = 0; i < threadCount - 1; ++i, strOffset += substrLen )
  {
    std::span substr{ s.data() + strOffset, substrLen };
    pool[i] = std::jthread( toLowerWorker, substr );
  }

  // last thread also handles the extra letters
  std::span substr{ s.data() + strOffset, substrLen + extraLetterCount };
  pool[threadCount - 1] = std::jthread( toLowerWorker, substr );

  // jthreads automatically join, so conversion is complete upon function return
  return s;
}
