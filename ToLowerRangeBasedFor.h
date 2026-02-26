///////////////////////////////////////////////////////////////////////////////
//
// ToLowerRangeBasedFor.h
//
// Notes: 
//   * range based for loop
//   * simple and easy to read/understand
//
// Issues:
//   * not as flexible as using algorithms
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
#include "Concepts.h"
#include "GetLowerChar.h"

// Convert char sequence to lower case in place. Returns modified sequence.
template <CharRange Str>
constexpr Str&& ToLowerRangeBasedFor( Str&& s ) noexcept
{
  for( char& c : s )
    c = GetLowerChar( c );

  return std::forward<Str>( s );
}

///////////////////////////////////////////////////////////////////////////////

