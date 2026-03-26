///////////////////////////////////////////////////////////////////////////////
//
// GetRandomStr.h
//
// Generates a string with random ASCII characters from ' ' through 0xFF.
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
#include <random>
#include <ranges>
#include <string>

inline std::string GetRandomStr( size_t len )
{
  auto seed = std::random_device{}( );
  std::mt19937 randGenerator( seed );
  std::uniform_int_distribution<int> distributionAscii( ' ', 0xFF ); // [' ', 0xFF]
  std::string s( len, '\0' );
  std::ranges::transform( s, std::begin(s), [&]( char /* unused */ )
  {
    return static_cast<char>( distributionAscii( randGenerator ) );
  } );
  return s;
}

///////////////////////////////////////////////////////////////////////////////
