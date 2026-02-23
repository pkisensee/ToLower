///////////////////////////////////////////////////////////////////////////////
//
// Lower.cu
//
// CUDA kernel functions and invocation that require NVCC
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

#include "cuda_runtime.h"
#include "device_launch_parameters.h"

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

void __cdecl ToLowerCUDA( char* s, size_t len )
{
  constexpr size_t kThreadsPerBlock = 256; // configurable; tune for hardware
  const auto blockCount = ( len + kThreadsPerBlock - 1 ) / kThreadsPerBlock;
  const auto blockCountUInt = static_cast<unsigned int>( blockCount );
  ToLowerKernel <<< blockCountUInt, kThreadsPerBlock >>> ( s, len );
}

/////////////////////////////////////////////////////////////////////////////// 
