///////////////////////////////////////////////////////////////////////////////
//
//  toLowerStrRef.h
//
//  Notes: 
//    * Uses std::string
//    * Obvious that input is being modified (passed by non-const ref)
//    * Uses ranges
//    * Uses standard algorithms
//    * Uses relevant and obvious name: transform
//    * Proven containers and algorithms
//    * Less ambiguous function name (lower() could be noun or adj)
//
//  Issues:
//    * Doesn't work for anything but std::string
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

#include <algorithm>
#include <ranges>
#include <string>
#include "getLowerChar.h"

inline std::string& toLowerStrRef( std::string& s )
{
  std::ranges::transform( s, std::begin( s ), []( char c )
  {
    return getLowerChar( c );
  } );
  return s;
}

///////////////////////////////////////////////////////////////////////////////
