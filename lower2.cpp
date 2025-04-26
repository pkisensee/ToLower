///////////////////////////////////////////////////////////////////////////////
//
// lower2.cpp
//
// Notes: 
//    * std::span! (C++20)
//    * Range-based for!
//    * Automatically handles nullptr and empty strings
//    * Caller doesn't need to use strlen; safer
//    * Automatically converts char[] (and similar) to std::span
//
// Issues:
//    * Non-obvious to the reader that characters are being changed (non-const char)
//    * Weird mismatch of range-based for and bracket operator
//    * std::span<char> is a code smell -- there's a better solution

#include <cctype>
#include <span>

void lower2( std::span<char> s )
{
  size_t i = 0;
  for( char c : s )
  {
    auto u = static_cast<unsigned char>( c );
    auto lower = std::tolower( u );
    s[i++] = static_cast<char>( lower );
  }
}
