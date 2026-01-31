///////////////////////////////////////////////////////////////////////////////
//
// toLower8.h
//
// Notes: 
//    * constexpr and noexcept
//
// Issues:
//    * Potential inscrutable compiler errors

#pragma once

#include <algorithm>
#include <ranges>
#include <utility>
#include "getLowerChar.h"

constexpr auto&& toLower8( auto&& s ) noexcept
{
  std::ranges::transform( s, std::begin( s ), []( char c )
  {
    return getLowerChar( c );
  } );
  return std::forward< decltype( s ) >( s );
}

///////////////////////////////////////////////////////////////////////////////


