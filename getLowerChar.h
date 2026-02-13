///////////////////////////////////////////////////////////////////////////////
//
// GetLowerChar.h
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
#include "LowerCaseAsciiTable.h"

///////////////////////////////////////////////////////////////////////////////
// 
// Converts the incoming character to lowercase using std::tolower
// 
// std::tolower has UB if incoming value is not representable as unsigned char, 
// hence requirement to cast.
// 
// See https://en.cppreference.com/w/cpp/string/byte/tolower.html
//
// Note: std::tolower can't be constexpr because the behavior depends on the
// current locale, which is runtime state.

inline char GetLowerChar( char c ) noexcept
{
  auto uchar = static_cast<unsigned char>( c );
  auto lowered = std::tolower( uchar );
  return static_cast<char>( lowered );
}

///////////////////////////////////////////////////////////////////////////////
//
// Converts the incoming character to lowercase using some branching and math
// Only works for simple ASCII; may not be correct for other locales

inline constexpr char GetLowerCharAZ( char c ) noexcept
{
  static constexpr char kLowerOffset = 'a' - 'A';
  if ( c >= 'A' && c <= 'Z' )
    return c + kLowerOffset;
  return c;
}

///////////////////////////////////////////////////////////////////////////////
//
// Converts the incoming character to lowercase using math and a single branch.
// Compiler will typically generate a conditional move.
// Only works for simple ASCII; may not be correct for other locales

inline constexpr char GetLowerCharCondMove( char c ) noexcept
{
  static constexpr char kLetterCount = 'Z' - 'A' + 1; // 26
  static constexpr char kLowerOffset = 'a' - 'A';     // 32

  // produces 0-25 for upper case letters, else value >25
  auto upperAlphaLetterIndex = static_cast<unsigned char>( c - 'A' );

  // produces kLowerOffset for upper case letters, else 0
  char offset = ( upperAlphaLetterIndex < kLetterCount ) * kLowerOffset;

  return c + offset;
}

///////////////////////////////////////////////////////////////////////////////
//
// Converts the incoming character to lowercase using a lookup table
// Only works for simple ASCII; may not be correct for other locales

inline constexpr char GetLowerCharLookupTable( char c ) noexcept
{
  auto uchar = static_cast<unsigned char>( c );
  auto index = static_cast<size_t>( uchar );
  return kLowerCaseAsciiTable[ index ];
}

///////////////////////////////////////////////////////////////////////////////
// 
// Converts the incoming character to lowercase using 5th bit trick
// Doesn't work correctly for all ASCII chars

inline constexpr char GetLowerChar5thBit( char c ) noexcept
{
  static constexpr unsigned char kBit5Set = 0b0010'0000; // 0x20
  auto uchar = static_cast<unsigned char>( c );
  auto lower = uchar | kBit5Set;
  return static_cast<char>( lower );
}

///////////////////////////////////////////////////////////////////////////////
