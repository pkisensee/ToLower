///////////////////////////////////////////////////////////////////////////////
//
// ToLower5thBit.h
//
// Notes: 
//  * Uses 5th bit ASCII trick. See GetLowerChar5thBit for details.
//
// Issues:
//   * Doesn't work for non-printable chars <0x20
//   * Translates some characters incorrectly, such as @, [, \, ], ^, and _
//   * Doesn't work for much of extended ASCII ( > 0x7F )
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
constexpr Str&& ToLower5thBit( Str&& s ) noexcept
{
  std::ranges::transform( s, std::begin( s ), []( char c )
  {
    return GetLowerChar5thBit( c );
  } );
  return std::forward<Str>( s );
}

///////////////////////////////////////////////////////////////////////////////
