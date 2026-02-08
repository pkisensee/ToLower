///////////////////////////////////////////////////////////////////////////////
//
//  ToLowerAutoNoReturn.h
//
//  Notes: 
//    * Returns nothing
//    * Input is any character sequence that can be modified in place
//
//  Issues:
//    * Potential inscrutable compiler errors
//    * Could be useful to return a reference to the input
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
#include "GetLowerChar.h"

// Convert char sequence to lower case in place. Returns modified sequence.
void ToLowerAutoNoReturn( auto&& s )
{
  std::ranges::transform( s, std::begin( s ), []( char c )
  {
    return GetLowerChar( c );
  } );
}

///////////////////////////////////////////////////////////////////////////////
