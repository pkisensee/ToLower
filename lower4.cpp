///////////////////////////////////////////////////////////////////////////////
//
// lower4.cpp
//
// Notes: 
//    * uses std::string_view C++17 -- more flexible
//    * compiler error: std::string_views are immutable; this is a good thing!
//
// Issues:
//    * still needed to know that tolower requires unsigned input

#include <algorithm>
#include <cctype>
#include <ranges>
#include <string>

void lower4( std::string_view s )
{
  /*
  // can't modify a string_view -- it's a view!
  std::ranges::transform( s, std::begin( s ), []( unsigned char c )
    {
      auto lower = std::tolower( c );
      return static_cast<std::string::value_type>( lower );
    } );
  */
  (void)s;
}
