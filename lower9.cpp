///////////////////////////////////////////////////////////////////////////////
//
// lower9.cpp
//
// Notes: 
//    * clever ASCII designers created an easy conversion; for all upper case
//      letters, you can set the 5th bit to convert to lower case. Because
//      almost all other printable ASCII characters already have the 5th bit set,
//      this trick works for anything except: @ [ ] ^ _ \
//
// Issues:
//    * doesn't work for non-printable chars <0x20
//    * doesn't work for the following characters
//      0x40 @ -> 0x60 `
//      0x5B [ -> 0x7B {
//      0x5C \ -> 0x7C |
//      0x5D ] -> 0x7D }
//      0x5E ^ -> 0x7E ~
//      0x5F _ -> 0x7F  
//    * doesn't work for much of extended ASCII (>0x7F)

#include <algorithm>
#include <cctype>
#include <ranges>
#include <string>

void lower9( std::string& s )
{
  static constexpr unsigned char kBit5Set = 0b0010'0000;
  std::ranges::transform( s, std::begin( s ), []( unsigned char c )
    {
      auto lower = c | kBit5Set;
      return static_cast<std::string::value_type>( lower );
    } );
}
