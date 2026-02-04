///////////////////////////////////////////////////////////////////////////////
//
// getRandomStr.h
//
// Generates a string with random ASCII characters from ' ' through 0xFF

#pragma once

#include <algorithm>
#include <random>
#include <ranges>
#include <string>

inline std::string getRandomStr( size_t len )
{
  auto seed = std::random_device{}( );
  std::mt19937 randGenerator( seed );
  std::uniform_int_distribution<int> distributionAscii( ' ', 0xFF );
  std::string s( len, '\0' );
  std::ranges::transform( s, std::begin(s), [&]( char /* unused */ )
  {
    return static_cast<char>( distributionAscii( randGenerator ) );
  } );
  return s;
}

///////////////////////////////////////////////////////////////////////////////
