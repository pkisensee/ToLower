///////////////////////////////////////////////////////////////////////////////
//
// ToLowerSpan.h
//
// Notes: 
//   * Uses std::span (C++20)
//   * Range-based for!
//   * Automatically handles nullptr and empty strings
//   * Caller doesn't need to use strlen; safer
//   * Automatically converts char[] (and similar) to std::span
//
// Issues:
//   * Non-obvious to the reader that characters are being changed (non-const char)
//   * Weird mismatch of range-based for and bracket operator
//   * std::span<char> is a code smell
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
#include <span>
#include <utility>
#include "GetLowerChar.h"

// Convert char sequence to lower case in place. Returns modified sequence.
constexpr char* ToLowerSpan( std::span<char> s ) noexcept
{
  std::ranges::transform( s, std::begin( s ), []( char c )
  {
    return GetLowerCharBranch( c );
  } );
  return s.data();
}

///////////////////////////////////////////////////////////////////////////////
