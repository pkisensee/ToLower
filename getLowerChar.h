///////////////////////////////////////////////////////////////////////////////
//
// GetLowerChar.h
//
// std::tolower has UB if incoming value is not representable as unsigned char, 
// hence requirement to cast.
// 
// See https://en.cppreference.com/w/cpp/string/byte/tolower.html
//
// Note: std::tolower can't be constexpr because the behavior depends on the
// current locale, which is runtime state.
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
#include <cctype>

// Converts the incoming character to lowercase using std::tolower
inline char GetLowerChar( char c ) noexcept
{
  auto uchar = static_cast<unsigned char>( c );
  auto lowered = std::tolower( uchar );
  return static_cast<char>( lowered );
}

// Converts the incoming character to lowercase using 5th bit trick
inline char GetLowerChar5thBit( char c ) noexcept
{
  static constexpr unsigned char kBit5Set = 0b0010'0000; // 0x20
  auto uchar = static_cast<unsigned char>( c );
  auto lower = uchar | kBit5Set;
  return static_cast<char>( lower );
}

///////////////////////////////////////////////////////////////////////////////
