///////////////////////////////////////////////////////////////////////////////
//
// ToLowerAVX512.h
//
// Notes: 
//   * Same flexible interface
//   * AVX-512 vectorized solution
//   * Follows same pattern as SSE solution
//   * 64 characters at a time
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
#include <immintrin.h>
#include <iterator>
#include <string>
#include <utility>
#include "ToLowerAVX2.h"

// Convert char sequence to lower case in place. Returns modified sequence.
constexpr auto&& ToLowerAVX512( auto&& s ) noexcept
{
  if ( !HasSIMDSupport( SIMDLevel::AVX512 ) )
    return ToLowerAVX2( s );

  static const size_t kAVX512size = sizeof( __m512i ); // typically 64
  static const __m512i kAAAAs = _mm512_set1_epi8( 'A' );
  static const __m512i kZZZZs = _mm512_set1_epi8( 'Z' );
  static const __m512i kCaseOffset = _mm512_set1_epi8( 'a' - 'A' );

  // Convert each kAVX512size block
  size_t len = std::size( s );
  char* data = std::data( s );
  for ( ; len >= kAVX512size; len -= kAVX512size, data += kAVX512size )
  {
    __m512i*  strAVX  = reinterpret_cast<__m512i*>( data );
    __m512i   input   = _mm512_loadu_epi32( strAVX );
    __mmask64 gteA    = _mm512_cmpge_epi8_mask( input, kAAAAs ); // >=A ? 0xFF : 0
    __mmask64 lteZ    = _mm512_cmple_epi8_mask( input, kZZZZs ); // <=Z ? 0xFF : 0
    __mmask64 isUpper = _kand_mask64( gteA, lteZ );              // A-Z ? 0xFF : 0
    __m512i   lower   = _mm512_mask_add_epi8( input, isUpper, input, kCaseOffset );
    _mm512_store_epi32( strAVX, lower );
  }

  // Convert any remaining <kAVX512bytes chars directly
  for ( ; len > 0; ++data, --len )
  {
    *data = GetLowerChar( *data );
  }

  return std::forward<decltype( s )>( s );
}

///////////////////////////////////////////////////////////////////////////////
