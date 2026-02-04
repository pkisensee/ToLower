///////////////////////////////////////////////////////////////////////////////
//
// toLowerLookupTable.h
//
// Notes: 
//    * Look up table solution -- fast
//    * Only 256 bytes of memory required
//
// Issues:
//    * Doesn't scale to larger size characters, such as 16-bit values

#pragma once

#include <algorithm>
#include <ranges>
#include "LowerCaseAsciiTable.h"

// VS may give false positive if constexpr var (kLowerCaseAsciiTable) used in templates
#pragma warning(push)
#pragma warning(disable: 5264)

// Convert char sequence to lower case in place. Returns modified sequence.
constexpr auto&& toLowerLookupTable( auto&& s ) noexcept
{
  std::ranges::transform( s, std::begin( s ), []( char c )
  {
    auto uchar = static_cast<unsigned char>( c );
    auto index = static_cast<size_t>( uchar );
    return kLowerCaseAsciiTable[ index ];
  } );
  return std::forward<decltype( s )>( s );
}

#pragma warning(pop)

///////////////////////////////////////////////////////////////////////////////
