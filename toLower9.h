///////////////////////////////////////////////////////////////////////////////
//
// toLower9.h
//
// Notes: 
//    * Execution policy support
//    * Can't use std::ranges, since it doesn't support execution policies
//
// Issues:
//    * Potential inscrutable compiler errors

#pragma once

#include <algorithm>
#include <utility>
#include "getLowerChar.h"

constexpr auto&& toLower9( auto&& s, auto&& execPolicy ) noexcept
{
  auto begin = std::begin( s );
  auto end = std::end( s );
  std::transform( execPolicy, begin, end, begin, []( char c )
  {
    return getLowerChar( c );
  } );
  return std::forward< decltype( s ) >( s );
}

///////////////////////////////////////////////////////////////////////////////





