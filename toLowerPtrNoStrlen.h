///////////////////////////////////////////////////////////////////////////////
//
//  toLowerPtrNoStrlen.h
//
//  Notes: 
//    * checks for nullptr (good)
//    * requires a size and bounds to that size (good)
//    * doesn't call strlen every iteration
//    * proper casts
//
//  Issues:
//    * C-like; doesn't use C++ standard libraries
//    * caller has to pass the length -- prone to error
//
//  Copyright © Pete Isensee (PKIsensee@msn.com).
//  All rights reserved worldwide.
//
//  Permission to copy, modify, reproduce or redistribute this source code is
//  granted provided the above copyright notice is retained in the resulting 
//  source code.
// 
//  This software is provided "as is" and without any express or implied
//  warranties.

#pragma once
#include "getLowerChar.h"

// ChatGPT suggests this (safer, faster and more readable), 
// but note the call to strlen, then the traversal of the string: O(2N)

/*
char* lower0( char* s ) {
  if( !s ) return nullptr;  // null check

  size_t len = std::strlen( s );
  for( size_t i = 0; i < len; ++i ) {
    // std::tolower returns int; ensure safe conversion
    unsigned char c = static_cast<unsigned char>( s[i] );
    s[i] = static_cast<char>( std::tolower( c ) );
  }
  return s;
}
*/

// Convert char sequence to lower case in place. Returns modified sequence.
inline char* toLowerPtrNoStrlen( char* s, size_t count )
{
  if( s == nullptr )
    return nullptr;
  
  for( size_t i = 0; s[i] != '\0' && i < count; ++i )
  {
    s[ i ] = getLowerChar( s[ i ] );
  }
  return s;
}

///////////////////////////////////////////////////////////////////////////////
