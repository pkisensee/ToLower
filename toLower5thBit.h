///////////////////////////////////////////////////////////////////////////////
//
// ToLower5thBit.h
//
// Notes: 
//   * Clever ASCII designers created an easy conversion; for all upper case
//     letters, set the 5th bit to convert to lower case. Because almost all 
//     printable ASCII characters already have the 5th bit set, this trick
//     works in many cases.
//
// Issues:
//   * Doesn't work for non-printable chars <0x20
//   * Translates the following characters
//     0x40 @  ->  0x60 `
//     0x5B [  ->  0x7B {
//     0x5C \  ->  0x7C |
//     0x5D ]  ->  0x7D }
//     0x5E ^  ->  0x7E ~
//     0x5F _  ->  0x7F   [non-printable]
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
#include "GetLowerChar.h"

// Convert char sequence to lower case in place. Returns modified sequence.
constexpr auto&& ToLower5thBit( auto&& s ) noexcept
{
  std::ranges::transform( s, std::begin( s ), []( char c )
  {
    return GetLowerChar5thBit( c );
  } );
  return std::forward<decltype( s )>( s );
}

///////////////////////////////////////////////////////////////////////////////
