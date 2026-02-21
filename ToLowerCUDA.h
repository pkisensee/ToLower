///////////////////////////////////////////////////////////////////////////////
//
// ToLowerCUDA.h
//
// Notes: 
//   * CUDA solution
//   * Requires NVIDIA GPU
//
// Issues:
//   * Must install NVIDIA SDK
//   * Must install runtime/drivers
//   * Doesn't work unless NVIDIA GPU is present
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
#include "CudaContext.h"

/*
__device__ char GetLowerCharDevice( char c ) noexcept
{
  if ( c >= 'A' && c <= 'Z' )
    return c + ( 'a' - 'A' );
  return c;
}

__global__ void ToLowerKernel( char* data, size_t len )
{
  const size_t i = blockIdx.x * blockDim.x + threadIdx.x;
  if ( i < len )
    data[ i ] = GetLowerCharDevice( data[ i ] );
}
*/

// Convert char sequence to lower case in place. Returns modified sequence.
// Can't be noexcept because cudaContext.ToLower() can throw.
constexpr auto&& ToLowerCUDA( auto&& s, CUDAContext& cudaContext )
{
  return cudaContext.ToLower( s );
}

///////////////////////////////////////////////////////////////////////////////

