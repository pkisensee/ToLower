///////////////////////////////////////////////////////////////////////////////
//
// toLower6.h
//
// Notes: 
//    * Input is any character sequence that can be modified in place
//    * Returns a reference to the input sequence
//
// Issues:
//    * Potential inscrutable compiler errors
//    * Not constexpr or noexcept

#pragma once

#include <algorithm>
#include <ranges>
#include "getLowerChar.h"

auto&& toLower6( auto&& s )
{
  std::ranges::transform( s, std::begin( s ), []( char c )
  {
    return getLowerChar( c );
  } );
  return std::forward< decltype( s ) >( s );
}

///////////////////////////////////////////////////////////////////////////////
