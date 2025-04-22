///////////////////////////////////////////////////////////////////////////////
//
// 9) lowermt
//
// Notes: 
//    * multi-threaded solution
//
// Issues:
//    * how do you decide how many threads?

#include <algorithm>
#include <cctype>
#include <ranges>
#include <string>
#include <thread>

// TODO simplify if possible
void lowermt(std::string& s)
{ 
  auto toLowerWorker = [](char* s, size_t len)
    { 
      for( size_t i = 0; i < len; ++i, ++s )
      {
        auto c = static_cast<unsigned char>( *s );
        *s = static_cast<char>( std::tolower( c ) );
      }
    }; 
  
  constexpr size_t threadCount = 4; 
  const size_t subStrLength = s.size() / threadCount;
  const size_t extraLetterCount = s.size() % threadCount; 
  size_t strOffset = 0; 
  std::array<std::jthread, threadCount> pool;
  for( size_t i = 0; i < threadCount - 1; ++i, strOffset += subStrLength )
  {
    pool[i] = std::jthread( toLowerWorker, s.data() + strOffset, subStrLength );
  }
  // final thread includes extra ltrs
  pool[threadCount - 1] = std::jthread( toLowerWorker, s.data() + strOffset, subStrLength + extraLetterCount );
}
