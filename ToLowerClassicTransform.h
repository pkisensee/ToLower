///////////////////////////////////////////////////////////////////////////////
//
// ToLowerClassicTransform.h
//
// Notes: 
//   * std::transform
//   * constexpr and noexcept
//   * Uses concepts for improved compiler error messages
//
// Issues:
//   * Potential inscrutable compiler errors
//   * std::forward requires unreadable decltype(s)
//   * In theory, decltype(s) always yields an lvalue, so although this
//     works with most compilers, it is not technically correct
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
#include <utility>
#include "Concepts.h"
#include "GetLowerChar.h"

// Convert char sequence to lower case in place. Returns modified sequence.
constexpr CharRange auto&& ToLowerClassicTransform( CharRange auto&& s ) noexcept
{
  std::transform( std::begin( s ), std::end( s ), std::begin( s ), []( char c )
  {
    return GetLowerCharBranch( c );
  } );
  return std::forward<decltype( s )>( s );
}

///////////////////////////////////////////////////////////////////////////////

