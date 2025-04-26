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

#include <algorithm>
#include <cctype>
#include <ranges>
#include <string>

template<typename S> // S is anything string-like that can be modified in place
void lower5( S&& s )
{
  std::ranges::transform( s, std::begin( s ), []( unsigned char c )
    {
      auto lower = std::tolower( c );
      return static_cast<std::string::value_type>( lower );
    } );
}

template<typename S>
S&& lower6( S&& s ) // return reference
{
  std::ranges::transform( s, std::begin( s ), []( unsigned char c )
    {
      auto lower = std::tolower( c );
      return static_cast<std::string::value_type>( lower );
    } );
  return std::forward<S>( s );
}

template<typename S>
constexpr S&& lower7( S&& s ) noexcept // constexpr, noexcept
{
  std::ranges::transform( s, std::begin( s ), []( unsigned char c )
    {
      auto lower = std::tolower( c );
      return static_cast<std::string::value_type>( lower );
    } );
  return std::forward<S>( s );
}

template<typename S, typename ExecPolicy>
constexpr S&& lower8( S&& s, ExecPolicy&& ep ) noexcept // execution policy
{
  std::transform( ep, std::begin( s ), std::end( s ), std::begin( s ), []( unsigned char c ) // can't use std::ranges
    {
      auto lower = std::tolower( c );
      return static_cast<std::string::value_type>( lower );
    } );
  return std::forward<S>( s );
}
