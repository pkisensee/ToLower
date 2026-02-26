///////////////////////////////////////////////////////////////////////////////
//
// ToLowerLookupTable.h
//
// Notes: 
//   * Look up table solution -- fast
//   * Only 256 bytes of memory required
//
// Issues:
//   * Doesn't scale to larger size characters, such as 16-bit values
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
constexpr Str&& ToLowerLookupTable( Str&& s ) noexcept
{
  std::ranges::transform( s, std::begin( s ), []( char c )
  {
    return GetLowerCharLookupTable( c );
  } );
  return std::forward<Str>( s );
}

///////////////////////////////////////////////////////////////////////////////
