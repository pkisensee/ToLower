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
//    * TODO what about extended characters?

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
    std::ranges::iota( lut, char{} );

    // Replace upper case with lower case
    auto upperA = std::begin( lut ) + 'A';
    auto upperZ = std::begin( lut ) + 'Z';
    std::iota( upperA, upperZ + 1, 'a' );

    return lut;
  }();

// TODO can this be constexpr?
std::string& lowera( std::string& s ) noexcept
{
  std::ranges::transform( s, std::begin( s ), []( unsigned char c )
    {
      return kLowerCaseAsciiTable[c];
    } );
  return s;
}
