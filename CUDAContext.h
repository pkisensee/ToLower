///////////////////////////////////////////////////////////////////////////////
//
// CUDAContext.h
//
// Wraps common boilerplate for CUDA
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

#pragma once
#include <cassert>
#include <span>
#include <string>
#include <stdexcept>
#include <string_view>
#include <utility>
#include "cuda_runtime.h"
#include "CUDAKernels.h"
#include "ToLowerSSE.h"

///////////////////////////////////////////////////////////////////////////////
// 
// CUDA memory base class

class CUDABuffer
{
public:

  CUDABuffer() = default;
  CUDABuffer( const CUDABuffer& ) = delete;
  CUDABuffer& operator=( const CUDABuffer& ) = delete;
  CUDABuffer( CUDABuffer&& ) noexcept = delete; // not currently needed

  explicit CUDABuffer( size_t size ) :
    size_( size )
  {
  }

  const char* data() const
  {
    return data_;
  }

  char* data()
  {
    return data_;
  }

  size_t size() const
  {
    return size_;
  }

  void Read( const char* data, size_t size )
  {
    assert( size <= size_ );
    assert( data_ != nullptr );
    std::memcpy( data_, data, size );
  }

protected:

  char* data_ = nullptr;
  size_t size_ = 0;

}; // CUDABuffer

///////////////////////////////////////////////////////////////////////////////

class CUDADeviceBuffer : public CUDABuffer
{
public:

  explicit CUDADeviceBuffer( size_t size ) :
    CUDABuffer( size )
  {
    if ( size == 0 )
      return;
    auto result = cudaMalloc( reinterpret_cast<void**>( &data_ ), size_ );
    if ( result != cudaSuccess )
      throw std::runtime_error( "cudaMalloc failed" );
  }

  ~CUDADeviceBuffer()
  {
    Free();
  }

  CUDADeviceBuffer& operator=( CUDADeviceBuffer&& rhs ) noexcept // move assign
  {
    if ( this == &rhs )
      return *this;
    Free();
    data_ = std::exchange( rhs.data_, nullptr );
    size_ = std::exchange( rhs.size_, 0 );
    return *this;
  }

  CUDADeviceBuffer() = default;
  CUDADeviceBuffer( const CUDADeviceBuffer& ) = delete;
  CUDADeviceBuffer& operator=( const CUDADeviceBuffer& ) = delete;
  CUDADeviceBuffer( CUDADeviceBuffer&& ) noexcept = delete; // not currently needed

private:

  void Free()
  {
    if ( data_ != nullptr )
    {
      cudaFree( data_ );
      data_ = nullptr;
    }
  }

}; // CUDADeviceBuffer

///////////////////////////////////////////////////////////////////////////////

class CUDAHostBuffer : public CUDABuffer
{
public:

  explicit CUDAHostBuffer( size_t size ) :
    CUDABuffer( size )
  {
    if ( size == 0 )
      return;
    auto result = cudaMallocHost( reinterpret_cast<void**>( &data_ ), size_ );
    if ( result != cudaSuccess )
      throw std::runtime_error( "cudaMallocHost failed" );
  }

  ~CUDAHostBuffer()
  {
    Free();
  }

  CUDAHostBuffer& operator=( CUDAHostBuffer&& rhs ) noexcept // move assign
  {
    if ( this == &rhs )
      return *this;
    Free();
    data_ = std::exchange( rhs.data_, nullptr );
    size_ = std::exchange( rhs.size_, 0 );
    return *this;
  }

  CUDAHostBuffer() = default;
  CUDAHostBuffer( const CUDAHostBuffer& ) = delete;
  CUDAHostBuffer& operator=( const CUDAHostBuffer& ) = delete;
  CUDAHostBuffer( CUDAHostBuffer&& ) noexcept = delete; // not currently needed

private:

  void Free()
  {
    if ( data_ != nullptr )
    {
      cudaFreeHost( data_ );
      data_ = nullptr;
    }
  }

}; // CUDAHostBuffer

///////////////////////////////////////////////////////////////////////////////

class CUDAContext
{
  static constexpr size_t kInitialCapacity = 1024 * 1024;

public:
  CUDAContext() :
    hostBuffer_( SupportsCuda() ? kInitialCapacity : 0 ),
    deviceBuffer_( SupportsCuda() ? kInitialCapacity : 0 )
  {
  }

  ~CUDAContext() = default;
  CUDAContext( const CUDAContext& ) = delete;
  CUDAContext& operator=( const CUDAContext& ) = delete;

  // Convert char sequence to lower case in place. Returns modified sequence.
  // Can't be noexcept because CUDA operations can throw.
  constexpr auto&& ToLower( auto&& s, CUDAKernelFnPtr kernelFn )
  {
    if ( !SupportsCuda() )
      return ToLowerSSE( s ); // fallback

    const size_t len = std::size( s );
    char* data = std::data( s );
    if ( len == 0 )
      return s;

    // Update buffers if too small
    if ( len > deviceBuffer_.size() )
    {
      deviceBuffer_ = CUDADeviceBuffer( len );
      hostBuffer_ = CUDAHostBuffer( len );
    }

    // Copy string to pinned buffer, then DMA to device
    hostBuffer_.Read( data, len );
    auto result = cudaMemcpy( deviceBuffer_.data(), hostBuffer_.data(), len, cudaMemcpyHostToDevice );
    if ( result != cudaSuccess )
      throw std::runtime_error( "cudaMemcpy host to device failed" );

    // Launch kernel
    kernelFn( deviceBuffer_.data(), len ); // see CUDAKernels.cu
    result = cudaGetLastError();
    if ( result != cudaSuccess )
      throw std::runtime_error( "CUDA kernel launch failed" );

    // DMA result back into pinned buffer
    result = cudaMemcpy( hostBuffer_.data(), deviceBuffer_.data(), len, cudaMemcpyDeviceToHost );
    if ( result != cudaSuccess )
      throw std::runtime_error( "cudaMemcpy device to host failed" );

    // Read results back into s
    std::memcpy( data, hostBuffer_.data(), len );

    return std::forward<decltype( s )>( s );
  }

  static bool SupportsCuda()
  {
    static bool checkedForCudaSupport = false;
    static bool supportsCuda = false;
    if ( !checkedForCudaSupport )
    {
      int deviceCount = 0;
      cudaError_t err = cudaGetDeviceCount( &deviceCount );
      checkedForCudaSupport = true;
      if ( err == cudaSuccess && deviceCount > 0 )
      {
        int driverVersion = 0;
        int runtimeVersion = 0;
        cudaDriverGetVersion( &driverVersion );
        cudaRuntimeGetVersion( &runtimeVersion );
        supportsCuda = ( driverVersion >= runtimeVersion );
      }
    }
    return supportsCuda;
  }

private:

  CUDAHostBuffer hostBuffer_;
  CUDADeviceBuffer deviceBuffer_;

}; // CUDAContext

/////////////////////////////////////////////////////////////////////////////// 
