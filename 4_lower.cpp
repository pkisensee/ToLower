///////////////////////////////////////////////////////////////////////////////
//
// 4) lower
//
// Notes: 
//    * uses std::string
//    * obvious that input is being modified (passed by non-const ref)
//    * uses ranges
//    * uses standard algorithms
//    * transform
//    * proven containers and algorithms
//
// Issues:
//    * still needed to know that tolower requires unsigned input

#include <algorithm>
#include <cctype>
#include <ranges>
#include <string>

void lower( std::string& s )
{
  std::ranges::transform( s, std::begin( s ), []( unsigned char c )
    {
      auto lower = std::tolower( c );
      return static_cast<std::string::value_type>( lower );
    } );
}
