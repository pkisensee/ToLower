///////////////////////////////////////////////////////////////////////////////
//
//  OpenCLContext.h
//
//  Wraps common boilerplate for OpenCL, including CL Device, Context,
//  CommandQueue, Program and Kernel
//
//  Copyright © Pete Isensee (PKIsensee@msn.com).
//  All rights reserved worldwide.
//
//  Permission to copy, modify, reproduce or redistribute this source code is
//  granted provided the above copyright notice is retained in the resulting 
//  source code.
// 
//  This software is provided "as is" and without any express or implied
//  warranties.

#pragma once
#include <cassert>
#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>
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

// Get the default GPU or CPU device
inline cl::Device GetDefaultDevice( cl_device_type deviceType )
{
  std::vector<cl::Platform> platforms;
  cl::Platform::get( &platforms );
  for( const auto& platform : platforms )
  {
    std::vector<cl::Device> devices;
    platform.getDevices( deviceType, &devices );
    if( !devices.empty() )
      return devices[0];
  }
  switch( deviceType )
  {
  case CL_DEVICE_TYPE_GPU: throw std::exception( "No OpenCL GPU detected" ); break;
  case CL_DEVICE_TYPE_CPU: throw std::exception( "No OpenCL CPU detected" ); break;
  default: throw std::exception( "Invalid device type" ); break;
  }
}

// Convert file into a string
inline std::string GetSourceCode( const std::filesystem::path& fileName )
{
  std::ifstream fileStream( fileName );
  return std::string{ std::istreambuf_iterator<char>( fileStream ), {} };
}

class OpenCLContext
{
public:
  OpenCLContext() = delete;

  // Establish context given device, .cl file and desired function
  // kernelFunction must have prototype: void toLower( __global char* s, unsigned long len )
  OpenCLContext( cl_device_type preferredDevice, 
                 const std::filesystem::path& sourceCodeFile,
                 const auto& kernelNames ) :
    device_{ GetDefaultDevice( preferredDevice ) },
    context_{ device_ },
    queue_{ context_, device_ },
    program_{ context_, GetSourceCode( sourceCodeFile ) }
  {
    // Build the kernels
    auto result = program_.build( device_, kBuildOptions );
    if( result != CL_SUCCESS )
    {
      std::cout << program_.getBuildInfo<CL_PROGRAM_BUILD_LOG>( device_ );
      throw std::exception( "Error compiling OpenCL kernel" );
    }
    for ( const auto& kernelName : kernelNames )
    {
      cl::Kernel kernel{ program_, kernelName };
      kernels_.insert( { kernelName, kernel } );
    }
  }

  // Convert char sequence to lower case in place. Returns modified sequence.
  constexpr auto&& ToLower( auto&& s, std::string_view kernelName ) noexcept
  {
    // Grab relevant kernel
    std::string kn{ kernelName }; // TODO fix
    auto& kernel = kernels_[ kn ];

    // Point CL buffer to incoming char sequence
    size_t len = std::size( s );
    char* data = std::data( s );
    cl::Buffer stringDataBuffer( context_, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR,
                                 len, data );

    // Establish arguments to kernl function
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

  cl::Device        device_;
  cl::Context       context_;
  cl::CommandQueue  queue_;
  cl::Program       program_;

  using Key = std::string;
  using Value = cl::Kernel;
  std::unordered_map< Key, Value > kernels_;
};

/////////////////////////////////////////////////////////////////////////////// 
