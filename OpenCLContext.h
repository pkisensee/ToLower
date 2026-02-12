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
#include <cassert>
#include <exception>
#include <filesystem>
#include <fstream>
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
inline const std::string kCacheFolder{ ".opencl_cache" };

///////////////////////////////////////////////////////////////////////////////

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

// Convert file into string
inline std::string GetSourceCode( const std::filesystem::path& fileName )
{
  std::ifstream fileStream( fileName );
  return std::string{ std::istreambuf_iterator<char>( fileStream ), {} };
}

// Generate cache filename based on source file and device
inline std::filesystem::path GetCacheFilePath( const std::filesystem::path& sourceFile,
  const cl::Device& device )
{
  auto deviceName = device.getInfo< CL_DEVICE_NAME >();
  // Remove everything but alphanumeric characters from name
  std::erase_if( deviceName, []( char c ) { return std::isspace( c ) || !std::isalnum( c ); } );

  auto cacheDir = sourceFile.parent_path() / kCacheFolder;
  std::filesystem::create_directories( cacheDir );

  std::filesystem::path cacheFile = cacheDir / 
    ( sourceFile.stem().string() + "_" + deviceName + ".bin" );
  return cacheFile;
}

// Check if cached binary exists and is newer than source
inline bool IsCacheValid( const std::filesystem::path& sourceFile,
  const std::filesystem::path& cacheFile )
{
  if ( !std::filesystem::exists( cacheFile ) )
    return false;

  auto sourceTime = std::filesystem::last_write_time( sourceFile );
  auto cacheTime = std::filesystem::last_write_time( cacheFile );

  return cacheTime >= sourceTime;
}

// Save compiled binary to cache
inline void SaveBinaryToCache( const cl::Program& program,
  const std::filesystem::path& cacheFile )
{
  auto binaries = program.getInfo< CL_PROGRAM_BINARIES >();
  if ( binaries.empty() || binaries[0].empty() )
    return;

  std::ofstream file( cacheFile, std::ios::binary );
  auto size = binaries[ 0 ].size();
  std::streamsize ssize = static_cast<std::streamsize>( size );
  const auto* udata = binaries[ 0 ].data();
  const auto* data = reinterpret_cast<const char*>( udata );

  file.write( data, ssize );
}

// Load binary from cache
inline std::vector<unsigned char> LoadBinaryFromCache( const std::filesystem::path& cacheFile )
{
  std::ifstream file( cacheFile, std::ios::binary );
  return std::vector<unsigned char>{ std::istreambuf_iterator<char>( file ), {} };
}

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
      try // TODO need this?
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

  void InitializeKernels( const auto& kernelNames )
  {
    for ( const auto& kernelName : kernelNames )
    {
      cl::Kernel kernel{ program_, kernelName };
      kernels_.insert( { kernelName, kernel } );
    }
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
