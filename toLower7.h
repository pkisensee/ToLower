///////////////////////////////////////////////////////////////////////////////
//
// toLower7.h
//
// Notes: 
//    * Uses auto instead of templates
//
// Issues:
//    * Potential inscrutable compiler errors
//    * Not constexpr or noexcept

#pragma once

#include <algorithm>
#include <ranges>
#include <utility>
#include "getLowerChar.h"

auto&& toLower7( auto&& s )
{
  std::ranges::transform( s, std::begin( s ), []( char c )
  {
    return getLowerChar( c );
  } );
  return std::forward< decltype(s) >(s);
}

///////////////////////////////////////////////////////////////////////////////

