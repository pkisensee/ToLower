///////////////////////////////////////////////////////////////////////////////
//
// lower.h (5, 6, 7, 8)
//
// Notes: 
//    * flexibility; works with string& or std::span<char> or char*
//    * doesn't work on strings that can't be modified, e.g. const string&, string_view, const char*
//
// Issues:
//    * 

#pragma once

#include <algorithm>
#include <cctype>
#include <cwctype>
#include <ranges>
#include <string>
#include <type_traits>

inline char toLowerChar( char c )
{
  auto uchar = static_cast<unsigned char>( c );
  auto lowered = std::tolower( uchar );
  return static_cast<char>( lowered );
}

// Return nothing
template<typename S> // S is anything string-like that can be modified in place
void lower5( S&& s )
{
  std::ranges::transform( s, std::begin( s ), []( auto c )
    {
      return toLowerChar( c );
    } );
}

template<typename S>
S&& lower6( S&& s ) // return reference
{
  std::ranges::transform( s, std::begin( s ), []( auto c )
    {
      return toLowerChar( c );
    } );
  return std::forward<S>( s );
}

template<typename S>
constexpr S&& lower7( S&& s ) noexcept // constexpr, noexcept
{
  std::ranges::transform( s, std::begin( s ), []( auto c )
    {
      return toLowerChar( c );
    } );
  return std::forward<S>( s );
}

constexpr auto&& lower7a( auto&& s ) noexcept
{
  std::ranges::transform( s, std::begin( s ), []( auto c )
    {
      return toLowerChar( c );
    } );
  return std::forward<decltype( s )>( s );
}

template<typename S, typename ExecPolicy>
constexpr S&& lower8( S&& s, ExecPolicy&& ep ) noexcept // execution policy
{
  std::transform( ep, std::begin( s ), std::end( s ), std::begin( s ), []( auto c ) // can't use std::ranges
    {
      return toLowerChar( c );
    } );
  return std::forward<S>( s );
}

///////////////////////////////////////////////////////////////////////////////
