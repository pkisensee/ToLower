///////////////////////////////////////////////////////////////////////////////
//
// toLowerA.h
//
// Notes: 
//    * clever ASCII designers created an easy conversion; for all upper case
//      letters, set the 5th bit to convert to lower case. Because almost all 
//      printable ASCII characters already have the 5th bit set, this trick
//      works in many cases.
//
// Issues:
//    * doesn't work for non-printable chars <0x20
//    * translates the following characters
//      0x40 @  ->  0x60 `
//      0x5B [  ->  0x7B {
//      0x5C \  ->  0x7C |
//      0x5D ]  ->  0x7D }
//      0x5E ^  ->  0x7E ~
//      0x5F _  ->  0x7F   [non-printable]
//    * doesn't work for much of extended ASCII ( > 0x7F )

#pragma once

#include <algorithm>
#include <ranges>
#include <string>
#include <utility>

constexpr auto&& toLowerA( auto&& s ) noexcept
{
  static constexpr unsigned char kBit5Set = 0b0010'0000; // 0x20
  std::ranges::transform( s, std::begin( s ), []( char c )
  {
    auto uchar = static_cast<unsigned char>( c );
    auto lower = uchar | kBit5Set;
    return static_cast<char>( lower );
  } );
  return std::forward<decltype( s )>( s );
}

///////////////////////////////////////////////////////////////////////////////
