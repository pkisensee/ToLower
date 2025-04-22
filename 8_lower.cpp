///////////////////////////////////////////////////////////////////////////////
//
// 8) lowert
//
// Notes: 
//    * LUT solution
//    * only 255 bytes
//
// Issues:
//    * doesn't scale to larger size characters, such as 16-bit values

#include <algorithm>
#include <cctype>
#include <numeric>
#include <ranges>
#include <string>

// Compile-time generated table
// All characters are lower case
static constexpr size_t kAsciiTableSize = 256;
using LowerCaseAsciiTable = std::array<char, kAsciiTableSize>;
static constexpr LowerCaseAsciiTable kLowerCaseAsciiTable = []
  {
    LowerCaseAsciiTable lut;
    std::ranges::iota( lut, char{}); // std ASCII table

    // Replace upper case with lower case
    auto begin = std::begin( lut ) + 'A';
    auto end = std::begin(lut) + 'Z' + 1;
    std::iota( begin, end, 'a' );

    return lut;
  }();

void lowert( std::string& s )
{
  std::ranges::transform( s, std::begin( s ), []( unsigned char c )
    {
      return kLowerCaseAsciiTable[c];
    } );
}
