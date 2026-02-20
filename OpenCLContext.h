///////////////////////////////////////////////////////////////////////////////
//
// OpenCLContext.h
//
// Wraps common boilerplate for OpenCL, including CL Device, Context,
// CommandQueue, Program and Kernel
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
#include <exception>
#include <filesystem>
#include <print>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

#pragma warning(push)
#pragma warning(disable: 4365 4623 4626 5027 5039 5246)
#include "CL/OpenCL.hpp"
#pragma warning(pop)

inline const std::string kBuildOptions{ "-cl-std=CL3.0 -Werror" };

///////////////////////////////////////////////////////////////////////////////

class OpenCLContext
{
public:
  OpenCLContext() = delete;

  // Establish context given device, .cl file and desired function
  // kernelFunction must have prototype:
  //    void toLower( __global char* s, unsigned long len )
  OpenCLContext( cl_device_type preferredDevice, 
                 const std::filesystem::path& sourceCodeFile,
                 const auto& kernelNames ) :
    device_{ GetDefaultDevice( preferredDevice ) },
    context_{ device_ },
    queue_{ context_, device_ }
  {
    // Attempt to load from binary cache first
    auto cacheFile = GetCacheFilePath( sourceCodeFile, device_ );
    if ( IsCacheValid( sourceCodeFile, cacheFile ) )
    {
      try
      {
        auto binary = LoadBinaryFromCache( cacheFile );
        cl::Program::Binaries binaries{ binary };
        std::vector<cl::Device> devices{ device_ };

        program_ = cl::Program( context_, devices, binaries );
        auto result = program_.build( device_, kBuildOptions );
        if ( result == CL_SUCCESS )
        {
          InitializeKernels( kernelNames );
          return;
        }
      }
      catch ( ... ) {} // Rebuild if errors loading
    }

    // No cache or cache didn't load properly; compile from source
    program_ = cl::Program( context_, GetSourceCode( sourceCodeFile ) );
    auto result = program_.build( device_, kBuildOptions );
    if ( result != CL_SUCCESS )
    {
      std::println( "{}", program_.getBuildInfo<CL_PROGRAM_BUILD_LOG>( device_ ) );
      throw std::exception( "Error compiling OpenCL kernel" );
    }

    // Save compiled binary to cache
    SaveBinaryToCache( program_, cacheFile );
    InitializeKernels( kernelNames );
  }

  // Convert char sequence to lower case in place. Returns modified sequence.
  // Can't be noexcept because enqueue can throw.
  constexpr auto&& ToLower( auto&& s, std::string_view kernelName )
  {
    // Grab relevant kernel
    auto& kernel = kernels_[ kernelName ];

    // Point CL buffer to incoming char sequence
    size_t len = std::size( s );
    char* data = std::data( s );
    cl::Buffer stringDataBuffer( context_, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR,
                                 len, data );

    // Establish arguments to OpenCL kernel function
    // void toLower( __global char* s, unsigned long len )
    kernel.setArg( 0, stringDataBuffer );
    kernel.setArg( 1, static_cast<uint64_t>( len ) );

    // Execute kernel on device
    auto offset = cl::NullRange;           // process the entire string
    auto threadCount = cl::NDRange{ len }; // 1 thread per character
    auto localWork = cl::NullRange;        // no local threads
    queue_.enqueueNDRangeKernel( kernel, offset, threadCount, localWork );
    queue_.finish();

    // Read results back into s
    cl_bool blocking = CL_TRUE;
    queue_.enqueueReadBuffer( stringDataBuffer, blocking, 0, len, data );

    return std::forward<decltype( s )>( s );
  }

private:

  void InitializeKernels( const auto& kernelNames )
  {
    for ( const auto& kernelName : kernelNames )
    {
      cl::Kernel kernel{ program_, kernelName };
      kernels_.insert( { kernelName, kernel } );
    }
  }

  static cl::Device GetDefaultDevice( cl_device_type );
  static std::string GetSourceCode( const std::filesystem::path& );
  static std::filesystem::path GetCacheFilePath( const std::filesystem::path&, const cl::Device& );
  static bool IsCacheValid( const std::filesystem::path&, const std::filesystem::path& );
  static void SaveBinaryToCache( const cl::Program&, const std::filesystem::path& );
  static std::vector<unsigned char> LoadBinaryFromCache( const std::filesystem::path& );

private:

  cl::Device        device_;
  cl::Context       context_;
  cl::CommandQueue  queue_;
  cl::Program       program_;

  using Key = std::string_view;
  using Value = cl::Kernel;
  std::unordered_map< Key, Value > kernels_;
};

/////////////////////////////////////////////////////////////////////////////// 
