///////////////////////////////////////////////////////////////////////////////
//
// ToLowerSSE5thBit.h
//
// Notes: 
//   * Same flexible interface
//   * Streaming SIMD Extensions (SSE) vectorized solution
//   * Bit 5 solution
//   * 16 characters at a time
//
// Issues:
//   * Unreadable, though not as bad as ToLowerSSE
//   * Can't be constexpr because SSE requires intrinsics
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
#include <cassert>
#include <emmintrin.h>
#include <iterator>
#include <utility>
#include "Concepts.h"
#include "SIMDSupport.h"

// Convert char sequence to lower case in place. Returns modified sequence.
template <CharRange Str>
Str&& ToLowerSSE5thBit( Str&& s ) noexcept
{
  assert( HasSIMDSupport( SIMDLevel::SSE2 ) );

  static const size_t kSSEbytes = sizeof( __m128i ); // typically 16
  static const __m128i kBits5Set = _mm_set1_epi8( 0b0010'0000 ); // 0x20 0x20 0x20 0x20...

  // Convert each kSSEbyte block; assumes block may not be aligned
  size_t len = std::size( s );
  char* data = std::data( s );
  for ( ; len >= kSSEbytes; len -= kSSEbytes, data += kSSEbytes )
  {
    __m128i* strSSE = reinterpret_cast<__m128i*>( data );
    __m128i input = _mm_loadu_si128( strSSE );        // load block into register
    __m128i lower = _mm_or_si128( input, kBits5Set ); // set the 5th bit in every byte
    _mm_storeu_si128( strSSE, lower );                // store the result
  }

  // Convert any remaining <kSSEbytes chars directly
  for ( ; len > 0; ++data, --len )
    *data = GetLowerChar5thBit( *data );

  return std::forward<Str>( s );
}

///////////////////////////////////////////////////////////////////////////////
