///////////////////////////////////////////////////////////////////////////////
//
// getLowerChar.h
//
// std::tolower has UB if incoming value is not representable as unsigned char, 
// hence requirement to cast.
// 
// See https://en.cppreference.com/w/cpp/string/byte/tolower.html
//
// Note: std::tolower can't be constexpr because the behavior depends on the
// current locale, which is runtime state.

#pragma once
#include <cctype>

// Converts the incoming character to lowercase
inline char getLowerChar( char c ) noexcept
{
  auto uchar = static_cast<unsigned char>( c );
  auto lowered = std::tolower( uchar );
  return static_cast<char>( lowered );
}

///////////////////////////////////////////////////////////////////////////////
