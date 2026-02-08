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
#include <span>
#include "GetLowerChar.h"

// Convert char sequence to lower case in place. Returns modified sequence.
inline char* ToLowerSpan( std::span<char> s )
{
  size_t i = 0;
  for( char c : s )
  {
    s[ i++ ] = GetLowerChar( c );
  }
  return s.data();
}

///////////////////////////////////////////////////////////////////////////////
