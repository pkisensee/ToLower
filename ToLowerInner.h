///////////////////////////////////////////////////////////////////////////////
//
// ToLowerInner.h
//
// Notes: 
//   * Used to evaluate various GetLowerChar implementations
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

// kernel; pass desired lowercase conversion function
inline constexpr CharRange auto&& ToLowerInner( CharRange auto&& s, auto&& GetLowerCharFn ) noexcept
{
  std::ranges::transform( s, std::begin( s ), [GetLowerCharFn]( char c )
  {
    return GetLowerCharFn( c );
  } );
  return std::forward<decltype( s )>( s );
}

inline CharRange auto&& ToLowerInnerDefault( CharRange auto&& s ) noexcept
{
  return ToLowerInner( s, GetLowerChar );
}

inline constexpr CharRange auto&& ToLowerInnerBranch( CharRange auto&& s ) noexcept
{
  return ToLowerInner( s, GetLowerCharBranch );
}

inline constexpr CharRange auto&& ToLowerInnerCondMove( CharRange auto&& s ) noexcept
{
  return ToLowerInner( s, GetLowerCharCondMove );
}

inline constexpr CharRange auto&& ToLowerInnerLookupTable( CharRange auto&& s ) noexcept
{
  return ToLowerInner( s, GetLowerCharLookupTable );
}

inline constexpr CharRange auto&& ToLowerInner5thBit( CharRange auto&& s ) noexcept
{
  return ToLowerInner( s, GetLowerChar5thBit );
}

///////////////////////////////////////////////////////////////////////////////
