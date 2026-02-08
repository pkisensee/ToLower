///////////////////////////////////////////////////////////////////////////////
//
// SIMDSupport.h
//
// Information on functions to be profiled, with methods to validate the
// functions and measure performance.
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
#include <isa_availability.h>

enum class SIMDLevel
{
  SSE2,
  AVX2,
  AVX512,
  SSE42,
  AVX10,
};

namespace
{
  // Asks OS for support level; short-circuit if compiler can do the detection
  inline bool SupportsSIMDFeature( unsigned feature ) noexcept
  {
    return __check_arch_support( feature, 0 ) ||  // compile-time check
           __check_isa_support ( feature, 0 );    // run-time check
  }
};

// Asks OS for support level; short-circuit if compiler can do the detection
inline constexpr bool HasSIMDSupport( SIMDLevel simdLevel ) noexcept
{
  switch ( simdLevel )
  {
    case SIMDLevel::SSE2:   return SupportsSIMDFeature( __IA_SUPPORT_VECTOR128 );
    case SIMDLevel::AVX2:   return SupportsSIMDFeature( __IA_SUPPORT_VECTOR256 );
    case SIMDLevel::AVX512: return SupportsSIMDFeature( __IA_SUPPORT_VECTOR512 );
    case SIMDLevel::SSE42:  return SupportsSIMDFeature( __IA_SUPPORT_SSE42 );
    case SIMDLevel::AVX10:  return SupportsSIMDFeature( __IA_SUPPORT_AVX10 );
  }
  return false;
}

///////////////////////////////////////////////////////////////////////////////
