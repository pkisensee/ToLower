///////////////////////////////////////////////////////////////////////////////
//
// 7) lowerb
//
// Notes: 
//    * clever ASCII designers created an easy conversion; for all upper case
//      letters, you can set the 5th bit to convert to lower case. Because
//      almost all other printable ASCII characters already have the 5th bit set,
//      this trick works for anything except: @ [ ] ^ _ \
//
// Issues:
//    * doesn't work for input: @ [ ] ^ _ \
//      @ -> `
//      [ -> {
//      \ -> |
//      ] -> }
//      ^ -> ~
//      _ -> ?
//    * doesn't work for much of extended ASCII (above 127)

#include <algorithm>
#include <cctype>
#include <ranges>
#include <string>

void lowerb( std::string& s )
{
  static constexpr unsigned char kBit5Set = 0b0010'0000;
  std::ranges::transform( s, std::begin( s ), []( unsigned char c )
    {
      auto lower = c | kBit5Set;
      return static_cast<std::string::value_type>( lower );
    } );
}
