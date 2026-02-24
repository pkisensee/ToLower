///////////////////////////////////////////////////////////////////////////////
//
// CUDAKernels.h
//
// ToLower C-style CUDA interfaces
// Implementations in CUDAKernels.cu
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

using CUDAKernelFn    = void( char*, size_t );
using CUDAKernelFnPtr = CUDAKernelFn*;

// TODO reorder based on perf
void ToLowerCUDALookupTable( char*, size_t );
void ToLowerCUDABranch( char*, size_t );
void ToLowerCUDACondMove( char*, size_t );
void ToLowerCUDA5thBit( char*, size_t );

/////////////////////////////////////////////////////////////////////////////// 
