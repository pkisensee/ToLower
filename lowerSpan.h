///////////////////////////////////////////////////////////////////////////////
//
// lowerSpan.h
//
// Notes: 
//    * Uses std::span (C++20)
//    * Range-based for!
//    * Automatically handles nullptr and empty strings
//    * Caller doesn't need to use strlen; safer
//    * Automatically converts char[] (and similar) to std::span
//
// Issues:
//    * Non-obvious to the reader that characters are being changed (non-const char)
//    * Weird mismatch of range-based for and bracket operator
//    * std::span<char> is a code smell

#pragma once

#include <span>
#include "getLowerChar.h"

inline char* lowerSpan( std::span<char> s )
{
  size_t i = 0;
  for( char c : s )
  {
    s[ i++ ] = getLowerChar( c );
  }
  return s.data();
}

///////////////////////////////////////////////////////////////////////////////