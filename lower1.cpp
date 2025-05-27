///////////////////////////////////////////////////////////////////////////////
//
// lower1.cpp
//
// Notes: 
//    * checks for nullptr (good)
//    * requires a size and bounds to that size (good)
//    * doesn't call strlen every iteration
//    * proper casts
//
// Issues:
//    * C-like; doesn't use C++ standard libraries
//    * caller has to pass the length -- prone to error
//
// ChatGPT suggests this (safer, faster and more readable), 
// but note the call to strlen, then the traversal of the string: O(2N)

/*
char* lower0( char* s ) {
  if( !s ) return nullptr;  // null check

  size_t len = std::strlen( s );
  for( size_t i = 0; i < len; ++i ) {
    // std::tolower returns int; ensure safe conversion
    unsigned char c = static_cast<unsigned char>( s[i] );
    s[i] = static_cast<char>( std::tolower( c ) );
  }
  return s;
}
*/

#include <cctype>
#include <cstring>

char* lower1( char* s, size_t count )
{
  if( s == nullptr )
    return nullptr;
  
  for( size_t i = 0; s[i] != '\0' && i < count; ++i )
  {
    auto u = static_cast<unsigned char>( s[i] );
    auto lower = std::tolower( u );
    s[i] = static_cast<char>( lower );
  }
  return s;
}
