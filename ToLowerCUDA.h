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
//   * Must ensure match between CUDA compiler code generation and drivers
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
#include "CUDAContext.h"
#include "CUDAKernels.h"

// Convert char sequence to lower case in place. Returns modified sequence.
// Can't be noexcept because cudaContext.ToLower() can throw.
constexpr auto&& ToLowerCUDA( auto&& s, 
  CUDAContext& cudaContext, CUDAKernelFnPtr cudaKernel )
{
  return cudaContext.ToLower( s, cudaKernel );
}

///////////////////////////////////////////////////////////////////////////////

