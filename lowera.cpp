///////////////////////////////////////////////////////////////////////////////
//
// lowera.cpp
//
// Notes: 
//    * LUT solution -- very fast
//    * only 256 bytes
//
// Issues:
//    * doesn't scale to larger size characters, such as 16-bit values

#include <algorithm>
#include <cctype>
#include <numeric>
#include <ranges>
#include <string>

// Compile-time generated ASCII table where all chars are lower case
static constexpr size_t kAsciiTableSize = 256;
using LowerCaseAsciiTable = std::array<char, kAsciiTableSize>;
static constexpr LowerCaseAsciiTable kLowerCaseAsciiTable = []
  {
    // Generate standard ASCII table
    LowerCaseAsciiTable lut;
    std::ranges::iota( lut, char{});

    // Replace upper case with lower case
    auto begin = std::begin( lut ) + 'A';
    auto end = std::begin(lut) + 'Z' + 1;
    std::iota( begin, end, 'a' );

    return lut;
  }();

void lowera( std::string& s )
{
  std::ranges::transform( s, std::begin( s ), []( unsigned char c )
    {
      return kLowerCaseAsciiTable[c];
    } );
}
