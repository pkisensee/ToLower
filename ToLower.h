///////////////////////////////////////////////////////////////////////////////
//
// ToLower.h
//
// Notes: 
//   * Version at end of refactoring exercise
//
// Issues:
//   * Potential inscrutable compiler errors
//   * std::forward requires unreadable decltype(s)
//   * Uses GetLowerChar, which calls slow std::tolower
// 
// Copyright © Pete Isensee (PKIsensee@msn.com).
// All rights reserved worldwide.
//
// Permission to copy, modify, reproduce or redistribute this source code is
// granted provided the above copyright notice is retained in the resulting 
// source code.
// 
// This software is provided "as is" and without any express or implied
// warranties.

#pragma once
#include <algorithm>
#include <ranges>
#include <utility>
#include "GetLowerChar.h"

// Convert char sequence to lower case in place. Returns modified sequence.
constexpr auto&& ToLower( auto&& s ) noexcept
{
  std::ranges::transform( s, std::begin( s ), []( char c )
  {
    return GetLowerChar( c );
  } );
  return std::forward<decltype( s )>( s );
}

///////////////////////////////////////////////////////////////////////////////
