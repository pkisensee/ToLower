///////////////////////////////////////////////////////////////////////////////
//
// ToLowerExecPolicy.h
//
// Notes: 
//   * Execution policy support
//   * Can't use std::ranges, since it doesn't support execution policies
//
// Issues:
//   * Potential inscrutable compiler errors
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
template <CharRange Str, typename Policy>
constexpr Str&& ToLowerExecPolicy( Str&& s, Policy&& execPolicy ) noexcept
{
  auto begin = std::begin( s );
  auto end = std::end( s );
  std::transform( execPolicy, begin, end, begin, []( char c )
  {
    return GetLowerChar( c );
  } );
  return std::forward<Str>( s );
}

///////////////////////////////////////////////////////////////////////////////
