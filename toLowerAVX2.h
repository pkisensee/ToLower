///////////////////////////////////////////////////////////////////////////////
//
// ToLowerAVX2.h
//
// Notes: 
//   * Same flexible interface
//   * AVX2 vectorized solution
//   * 32 characters at a time
//   * Simpler than SSE solution; fewer instructions
//
// Issues:
//   * Unreadable
//   * Can't be constexpr because AVX requires intrinsics
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
#include "Concepts.h"
#include "GetLowerChar.h"
#include "ToLowerSSE.h"

// Convert char sequence to lower case in place. Returns modified sequence.
template <CharRange Str>
Str&& ToLowerAVX2( Str&& s ) noexcept
{ 
  if ( !HasSIMDSupport( SIMDLevel::AVX2 ) )
    return ToLowerSSE( s ); // fallback

  static constexpr unsigned char kBit5Set = 0b0010'0000; // 0x20
  static const __m256i kBit5Set256 = _mm256_set1_epi8( kBit5Set );
  static const size_t kAVX2bytes = sizeof( __m256i ); // typically 32

  // Convert each kAVX2bytes block
  size_t len = std::size( s );
  char* data = std::data( s );
  for ( ; len >= kAVX2bytes; len -= kAVX2bytes, data += kAVX2bytes )
  {
    __m256i* strAVX2 = reinterpret_cast<__m256i*>( data );
    __m256i input = _mm256_loadu_si256( strAVX2 );
    __m256i lower = _mm256_or_si256( input, kBit5Set256 );
    _mm256_storeu_si256( strAVX2, lower );
  }

  // Convert any remaining <kAVX2bytes chars directly
  for( ; len > 0; ++data, --len )
    *data = GetLowerCharBranch( *data );

  return std::forward<Str>( s );
}

///////////////////////////////////////////////////////////////////////////////
