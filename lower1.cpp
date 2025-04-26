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

#include <cctype>

void lower1( char* s, size_t count )
{
  if( s == nullptr )
    return;
  
  for( size_t i = 0; s[i] != '\0' && i < count; ++i )
  {
    auto u = static_cast<unsigned char>( s[i] );
    auto lower = std::tolower( u );
    s[i] = static_cast<char>( lower );
  }
}
