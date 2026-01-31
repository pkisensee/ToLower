///////////////////////////////////////////////////////////////////////////////
//
// toLower3.h
//
// Notes: 
//    * uses std::string
//    * obvious that input is being modified (passed by non-const ref)
//    * uses ranges
//    * uses standard algorithms
//    * uses relevant and obvious name: transform
//    * proven containers and algorithms
//    * less ambiguous function name (lower() could be noun or adj)
//
// Issues:
//    * doesn't work for anything but std::string

#pragma once

#include <algorithm>
#include <ranges>
#include <string>
#include "getLowerChar.h"

inline std::string& toLower3( std::string& s )
{
  std::ranges::transform( s, std::begin( s ), []( char c )
  {
    return getLowerChar( c );
  } );
  return s;
}

///////////////////////////////////////////////////////////////////////////////
