///////////////////////////////////////////////////////////////////////////////
//
// toLower6.h
//
// Notes: 
//    * Input is any character sequence that can be modified in place
//    * Returns a reference to the input sequence
//    * Correctly handles input that's an rvalue reference (temporary)
//
// Issues:
//    * Potential inscrutable compiler errors
//    * Not constexpr or noexcept

#pragma once

#include <algorithm>
#include <ranges>
#include <utility>
#include "getLowerChar.h"

template <typename String>
String&& toLower6( String&& s )
{
  std::ranges::transform( s, std::begin( s ), []( char c )
  {
    return getLowerChar( c );
  } );
  return std::forward< String >( s );
}

///////////////////////////////////////////////////////////////////////////////
