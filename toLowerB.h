///////////////////////////////////////////////////////////////////////////////
//
// toLowerB.h
//
// Notes: 
//    * Look up table solution -- fast
//    * only 256 bytes
//
// Issues:
//    * doesn't scale to larger size characters, such as 16-bit values
//    * doesn't work for UTF-8

#pragma once

#include <algorithm>
#include <array>
#include <numeric> // iota
#include <ranges>
#include <string>

// VS may give false positive if constexpr var (kLowerCaseAsciiTable) used in templates
#pragma warning(push)
#pragma warning(disable: 5264)

static constexpr size_t kAsciiTableSize = 256;
using LowerCaseAsciiTable = std::array<char, kAsciiTableSize>;

// Old school hard coded table
static constexpr LowerCaseAsciiTable kLowerCaseAsciiTableOld =
{ {
  '\x0', '\x1', '\x2', '\x3', '\x4', '\x5', '\x6', '\x7',
  '\x8', '\x9', '\xA', '\xB', '\xC', '\xD', '\xE', '\xF',
  '\x10', '\x11', '\x12', '\x13', '\x14', '\x15', '\x16', '\x17',
  '\x18', '\x19', '\x1A', '\x1B', '\x1C', '\x1D', '\x1E', '\x1F',
  ' ', '!', '\"', '#', '$', '%', '&', '\'',
  '(', ')', '*', '+', ',', '-', '.', '/',
  '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
  ':', ';', '<', '=', '<', '?', '@',
  'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', // normally upper case
  'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r',
  's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
  '[', '\\', ']', '^', '_', '`',
  'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', // lower case letters
  'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r',
  's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
  '{', '|', '}', '~', '\x7F',
  '\x80', '\x81', '\x82', '\x83', '\x84', '\x85', '\x86', '\x87',
  '\x88', '\x89', '\x8A', '\x8B', '\x8C', '\x8D', '\x8E', '\x8F',
  '\x90', '\x91', '\x92', '\x93', '\x94', '\x95', '\x96', '\x97',
  '\x98', '\x99', '\x9A', '\x9B', '\x9C', '\x9D', '\x9E', '\x9F',
  '\xA0', '\xA1', '\xA2', '\xA3', '\xA4', '\xA5', '\xA6', '\xA7',
  '\xA8', '\xA9', '\xAA', '\xAB', '\xAC', '\xAD', '\xAE', '\xAF',
  '\xB0', '\xB1', '\xB2', '\xB3', '\xB4', '\xB5', '\xB6', '\xB7',
  '\xB8', '\xB9', '\xBA', '\xBB', '\xBC', '\xBD', '\xBE', '\xBF',
  '\xC0', '\xC1', '\xC2', '\xC3', '\xC4', '\xC5', '\xC6', '\xC7',
  '\xC8', '\xC9', '\xCA', '\xCB', '\xCC', '\xCD', '\xCE', '\xCF',
  '\xD0', '\xD1', '\xD2', '\xD3', '\xD4', '\xD5', '\xD6', '\xD7',
  '\xD8', '\xD9', '\xDA', '\xDB', '\xDC', '\xDD', '\xDE', '\xDF',
  '\xE0', '\xE1', '\xE2', '\xE3', '\xE4', '\xE5', '\xE6', '\xE7',
  '\xE8', '\xE9', '\xEA', '\xEB', '\xEC', '\xED', '\xEE', '\xEF',
  '\xF0', '\xF1', '\xF2', '\xF3', '\xF4', '\xF5', '\xF6', '\xF7',
  '\xF8', '\xF9', '\xFA', '\xFB', '\xFC', '\xFD', '\xFE', '\xFF',
} };

// Compiler generated table
static LowerCaseAsciiTable kLowerCaseAsciiTable = []
{
  // Create standard ASCII table
  LowerCaseAsciiTable lowerAscii;
  std::ranges::iota( lowerAscii, char{} );

  // Replace upper case with lower case
  const auto asciiBeg = std::begin( lowerAscii );
  const auto upperBeg = asciiBeg + 'A';
  const auto upperEnd = asciiBeg + 'Z' + 1;
  std::iota( upperBeg, upperEnd, 'a' );

  return lowerAscii;
}();

// Convert char sequence to lower case in place. Returns modified sequence.
constexpr auto&& toLowerB( auto&& s ) noexcept
{
  std::ranges::transform( s, std::begin( s ), []( char c )
  {
    auto uchar = static_cast<unsigned char>( c );
    auto index = static_cast<size_t>( uchar );
    return kLowerCaseAsciiTableOld[ index ];
  } );
  return std::forward<decltype( s )>( s );
}

#pragma warning(pop)

///////////////////////////////////////////////////////////////////////////////
