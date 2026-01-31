///////////////////////////////////////////////////////////////////////////////
//
// toLower4.h
//
// Notes: 
//    * Uses std::string_view C++17 -- more flexible
//
// Issues:
//    * Compiler error: std::string_views are immutable; this is a good thing!

#pragma once

#include <algorithm>
#include <ranges>
#include <string_view>
#include "getLowerChar.h"

inline std::string_view toLower4( std::string_view s )
{
  // can't modify a string_view -- it's a read-only view!
  /*
  std::ranges::transform( s, std::begin( s ), []( char c )
  {
    return getLowerChar( c );
  } );
  */
  return s;
}

///////////////////////////////////////////////////////////////////////////////
