///////////////////////////////////////////////////////////////////////////////
//
// toLower5.h
//
// Notes: 
//    * Returns nothing
//    * Input is any character sequence that can be modified in place
//
// Issues:
//    * Potential inscrutable compiler errors
//    * Could be useful to return a reference to the input

#pragma once

#include <algorithm>
#include <ranges>
#include "getLowerChar.h"

void toLower5( auto&& s )
{
  std::ranges::transform( s, std::begin( s ), []( char c )
  {
    return getLowerChar( c );
  } );
}

///////////////////////////////////////////////////////////////////////////////
