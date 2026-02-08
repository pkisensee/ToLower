///////////////////////////////////////////////////////////////////////////////
//
// ToLowerSSE.h
//
// Notes: 
//   * Same flexible interface
//   * Streaming SIMD Extensions (SSE) vectorized solution
//   * 16 characters at a time
//
// Issues:
//   * Unreadable
//
// Copyright © Pete Isensee (PKIsensee@msn.com).
// All rights reserved worldwide.
//
// Permission to copy, modify, reproduce or redistribute this source code is
// granted provided the above copyright notice is retained in the resulting 
// source code.
// 
// This software is provided "as is" and without any express or implied
// warranties.

#pragma once
#include <emmintrin.h>
#include <iterator>
#include <utility>
#include "GetLowerChar.h"
#include "SIMDSupport.h"

// Convert char sequence to lower case in place. Returns modified sequence.
constexpr auto&& ToLowerSSE( auto&& s ) noexcept
{
  if ( !HasSIMDSupport( SIMDLevel::SSE2 ) )
    return s;

  static const __m128i kAAAAs = _mm_set1_epi8( 'A' - 1 ); // no mm_cmpgte, so offset -1
  static const __m128i kZZZZs = _mm_set1_epi8( 'Z' + 1 ); // no mm_cmplte, so offset +1
  static const __m128i kCaseOffset = _mm_set1_epi8( 'a' - 'A' );
  static const size_t kSSEbytes = sizeof( __m128i ); // typically 16

  // Convert each kSSEbyte block
  size_t len = std::size( s );
  char* data = std::data( s );
  for ( ; len >= kSSEbytes; len -= kSSEbytes, data += kSSEbytes )
  {
    // Implements the following:
    // --------------------------------------------------------------
    // uchar isUpper = (input >= 'A' && input <= 'Z') ? 0xFF : 0;
    // uchar lower   = input + ( isUpper & kCaseOffset );
    //
    // More explicitly:
    // --------------------------------------------------------------
    // gteA    = ( input >= 'A' ) ? 0xFF : 0x0;
    // lteZ    = ( input <= 'Z' ) ? 0xFF : 0x0;
    // isUpper = gteA & lteZ;           // 0xFF or 0x0
    // caseAdd = isUpper & kCaseOffset; // 0x20 or 0x0
    // lower   = input + caseAdd;       // any A-Z -> a-z
    //
    // Example input: "AaBb!_Zz"
    // 
    //              0  1  2  3  4  5  6  7
    // -----------------------------------
    // input        A  a  B  b  !  _  Z  z
    // input (hex) 41 61 42 62 21 5D 5A 7A
    // gteA        FF FF FF FF  0 FF FF FF // 'A' == 0x41
    // lteZ        FF  0 FF  0 FF  0 FF  0 // 'Z' == 0x5A
    // isUpper     FF  0 FF  0  0  0 FF  0 // gteA & lteZ
    // caseAdd     20  0 20  0  0  0 20  0 // isUpper & kCaseOffset
    // lower (hex) 61 61 62 62 21 5D 7A 7A // input + caseAdd
    // lower        a  a  b  b  !  _  z  z

    __m128i* strSSE = reinterpret_cast<__m128i*>( data );
    __m128i input   = _mm_loadu_si128( strSSE );
    __m128i gteA    = _mm_cmpgt_epi8( input, kAAAAs ); // >=A ? 0xFF : 0
    __m128i lteZ    = _mm_cmplt_epi8( input, kZZZZs ); // <=Z ? 0xFF : 0
    __m128i isUpper = _mm_and_si128( gteA, lteZ );     // A-Z ? 0xFF : 0
    __m128i caseAdd = _mm_and_si128( isUpper, kCaseOffset ); // A-Z ? ('a'-'A') : 0
    __m128i lower   = _mm_add_epi8( input, caseAdd );  // convert any A-Z to a-z
    _mm_storeu_si128( strSSE, lower );
  }

  // Convert any remaining <kSSEbytes chars directly
  for( ; len > 0; ++data, --len )
  {
    *data = GetLowerChar( *data );
  }

  return std::forward<decltype( s )>( s );
}

///////////////////////////////////////////////////////////////////////////////
