///////////////////////////////////////////////////////////////////////////////
//
// ToLowerStrRef.h
//
// Notes: 
//   * Uses std::string
//   * Obvious that input is being modified (passed by non-const ref)
//
// Issues:
//   * Doesn't work for anything but std::string
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
#include <string>
#include "GetLowerChar.h"

// Convert string to lower case in place. Returns modified string.
constexpr std::string& ToLowerStrRef( std::string& s ) noexcept
{
  std::ranges::transform( s, std::begin( s ), []( char c )
  {
    return GetLowerCharCondMove( c );
  } );
  return s;
}

///////////////////////////////////////////////////////////////////////////////
