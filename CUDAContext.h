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
#include <cassert>
#include <span>
#include <string>
#include <stdexcept>
#include <string_view>
#include <utility>
#include "Concepts.h"
#include "cuda_runtime.h"
#include "CUDAKernels.h"
#include "ToLowerAutoConstexpr.h" // fallback

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

  void CopyFrom( const char* data, size_t size )
  {
    if ( size > size_ || data_ == nullptr )
      throw std::runtime_error( "Buffer overflow in CUDABuffer::CopyFrom" );
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
//
// Pins an existing host allocation for direct DMA by the CUDA driver.

class CUDAPinnedRegion
{
public:

  CUDAPinnedRegion() = default;

  CUDAPinnedRegion( void* data, size_t size ) :
    data_( data )
  {
    auto result = cudaHostRegister( data_, size, cudaHostRegisterDefault );
    isPinned_ = ( result == cudaSuccess );
  }

  ~CUDAPinnedRegion()
  {
    if ( isPinned_ )
      cudaHostUnregister( data_ );
  }

  CUDAPinnedRegion( const CUDAPinnedRegion& ) = delete;
  CUDAPinnedRegion& operator=( const CUDAPinnedRegion& ) = delete;
  CUDAPinnedRegion( CUDAPinnedRegion&& ) noexcept = delete;
  CUDAPinnedRegion& operator=( CUDAPinnedRegion&& ) = delete;

  bool IsPinned() const
  {
    return isPinned_;
  }

private:

  void* data_ = nullptr;
  bool isPinned_ = false;

}; // CUDAPinnedRegion

///////////////////////////////////////////////////////////////////////////////

class CUDAContext
{
  static constexpr size_t kInitialCapacity = 1024 * 1024;

public:
  CUDAContext() :
    deviceBuffer_( SupportsCuda() ? kInitialCapacity : 0 ),
    stagingBuffer_( 0 )
  {
  }

  ~CUDAContext() = default;
  CUDAContext( const CUDAContext& ) = delete;
  CUDAContext& operator=( const CUDAContext& ) = delete;

  // Invoke the given kernel function on s.
  // Can't be constexpr or noexcept because CUDA operations allocate and throw.
  template <CharRange T>
  T&& RunKernel( CUDAKernelFnPtr kernelFn, T&& s )
  {
    const size_t len = std::size( s );
    char* data = std::data( s );
    if ( len == 0 )
      return std::forward<T>( s );

    // Update buffers if too small
    if ( len > deviceBuffer_.size() )
      deviceBuffer_ = CUDADeviceBuffer( len );

    // Attempt to pin the caller's buffer directly for zero-copy DMA.
    CUDAPinnedRegion pinned( data, len );
    if ( pinned.IsPinned() )
    {
      // Typical path for heap allocated memory (e.g. large strings)
      TransferAndRun( data, data, len, kernelFn );
    }
    else
    {
      // Fallback: stage through a pinned host buffer.
      // Typical for small stack-allocated strings or unusual allocators.
      if ( len > stagingBuffer_.size() )
        stagingBuffer_ = CUDAHostBuffer( len );
      stagingBuffer_.CopyFrom( data, len );
      TransferAndRun( stagingBuffer_.data(), stagingBuffer_.data(), len, kernelFn );
      std::memcpy( data, stagingBuffer_.data(), len );
    }

    return std::forward<T>( s );
  }

  static bool SupportsCuda()
  {
    // One-time check
    static bool supportsCuda = []() { return IsCudaSupported(); }( );
    return supportsCuda;
  }

private:

  // DMA -> kernel -> DMA sequence given pinned src/dst pointers.
  // src and dst may be the same pointer (in-place, direct DMA path) or
  // different pointers (staging path, where both point into stagingBuffer_).
  void TransferAndRun( const char* src, char* dst, size_t len, CUDAKernelFnPtr kernelFn )
  {
    // DMA host -> device
    auto result = cudaMemcpy( deviceBuffer_.data(), src, len, cudaMemcpyHostToDevice );
    if ( result != cudaSuccess )
      throw std::runtime_error( "cudaMemcpy host to device failed" );

    // Launch kernel
    kernelFn( deviceBuffer_.data(), len );

    // Catch any launch configuration errors.
    // Kernel execution errors surface asynchronously; catching them requires
    // cudaDeviceSynchronize(), which adds synchronization overhead but can be
    // useful in debug builds.
    result = cudaGetLastError();
    if ( result != cudaSuccess )
      throw std::runtime_error( "CUDA kernel launch failed" );

    // DMA device -> host
    result = cudaMemcpy( dst, deviceBuffer_.data(), len, cudaMemcpyDeviceToHost );
    if ( result != cudaSuccess )
      throw std::runtime_error( "cudaMemcpy device to host failed" );
  }

  // True if there's an NVIDIA GPU and compiled runtime is supported 
  // by current device driver
  static bool IsCudaSupported()
  {
    int deviceCount = 0;
    cudaError_t err = cudaGetDeviceCount( &deviceCount );
    if ( err != cudaSuccess || deviceCount < 1 )
      return false;

    int driverVersion = 0;
    int runtimeVersion = 0;
    cudaDriverGetVersion( &driverVersion );
    cudaRuntimeGetVersion( &runtimeVersion );
    return ( driverVersion >= runtimeVersion );
  }

private:

  CUDADeviceBuffer deviceBuffer_; // for common path
  CUDAHostBuffer stagingBuffer_;  // for fallback path

}; // CUDAContext

/////////////////////////////////////////////////////////////////////////////// 
