///////////////////////////////////////////////////////////////////////////////
//
// toLowerStrRef.h
//
// Notes: 
//    * Uses std::string
//    * Obvious that input is being modified (passed by non-const ref)
//    * Uses ranges
//    * Uses standard algorithms
//    * Uses relevant and obvious name: transform
//    * Proven containers and algorithms
//    * Less ambiguous function name (lower() could be noun or adj)
//
// Issues:
//    * Doesn't work for anything but std::string

#pragma once

#include <algorithm>
#include <ranges>
#include <string>
#include "getLowerChar.h"

inline std::string& toLowerStrRef( std::string& s )
{
  std::ranges::transform( s, std::begin( s ), []( char c )
  {
    return getLowerChar( c );
  } );
  return s;
}

///////////////////////////////////////////////////////////////////////////////
