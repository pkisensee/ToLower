///////////////////////////////////////////////////////////////////////////////
//
//  toLowerTemplatized.h
//
//  Notes: 
//    * Templatized for any character sequence that can be modified in place
//    * Returns a reference to the input sequence
//    * Correctly handles input that's an rvalue reference (temporary)
//
//  Issues:
//    * Potential inscrutable compiler errors
//    * Not constexpr or noexcept
//
//  Copyright © Pete Isensee (PKIsensee@msn.com).
//  All rights reserved worldwide.
//
//  Permission to copy, modify, reproduce or redistribute this source code is
//  granted provided the above copyright notice is retained in the resulting 
//  source code.
// 
//  This software is provided "as is" and without any express or implied
//  warranties.

#pragma once
#include <algorithm>
#include <ranges>
#include <utility>
#include "getLowerChar.h"

// Convert char sequence to lower case in place. Returns modified sequence.
template <typename String>
constexpr String&& toLowerTemplatized( String&& s ) noexcept
{
  std::ranges::transform( s, std::begin( s ), []( char c )
  {
    return getLowerChar( c );
  } );
  return std::forward< String >( s );
}

///////////////////////////////////////////////////////////////////////////////
