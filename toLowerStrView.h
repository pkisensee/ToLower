///////////////////////////////////////////////////////////////////////////////
//
//  toLowerStrView.h
//
//  Notes: 
//    * Uses std::string_view C++17 -- more flexible
//
//  Issues:
//    * Compiler error: std::string_views are immutable; this is a good thing!
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
#include <string_view>
#include "getLowerChar.h"

inline std::string_view toLowerStrView( std::string_view s )
{
  // can't modify a string_view -- it's a read-only view!
  /*
  std::ranges::transform( s, std::begin( s ), []( char c )
  {
    return getLowerChar( c );
  } );
  */
  return s;
}

///////////////////////////////////////////////////////////////////////////////
