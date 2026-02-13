///////////////////////////////////////////////////////////////////////////////
//
// OpenCLContext.cpp
//
// Helper functions for OpenCL
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

#include <fstream>
#include "OpenCLContext.h"

static const std::string kCacheFolder{ ".OpenCLCache" };

///////////////////////////////////////////////////////////////////////////////
//
// Get the default GPU or CPU device

cl::Device OpenCLContext::GetDefaultDevice( cl_device_type deviceType ) // static
{
  std::vector<cl::Platform> platforms;
  cl::Platform::get( &platforms );
  for ( const auto& platform : platforms )
  {
    std::vector<cl::Device> devices;
    platform.getDevices( deviceType, &devices );
    if ( !devices.empty() )
      return devices[ 0 ];
  }
  switch ( deviceType )
  {
    case CL_DEVICE_TYPE_GPU: throw std::exception( "No OpenCL GPU detected" ); break;
    case CL_DEVICE_TYPE_CPU: throw std::exception( "No OpenCL CPU detected" ); break;
    default: throw std::exception( "Invalid device type" ); break;
  }
}

///////////////////////////////////////////////////////////////////////////////
//
// Convert file into string

std::string OpenCLContext::GetSourceCode( const std::filesystem::path& fileName ) // static
{
  std::ifstream fileStream( fileName );
  return std::string{ std::istreambuf_iterator<char>( fileStream ), {} };
}

///////////////////////////////////////////////////////////////////////////////
// 
// Generate cache filename based on source file and device

std::filesystem::path OpenCLContext::GetCacheFilePath( const std::filesystem::path& sourceFile,
  const cl::Device& device ) // static
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

///////////////////////////////////////////////////////////////////////////////
// 
// Check if cached binary exists and is newer than source

bool OpenCLContext::IsCacheValid( const std::filesystem::path& sourceFile,
  const std::filesystem::path& cacheFile ) // static
{
  if ( !std::filesystem::exists( cacheFile ) )
    return false;

  auto sourceTime = std::filesystem::last_write_time( sourceFile );
  auto cacheTime = std::filesystem::last_write_time( cacheFile );

  return cacheTime >= sourceTime;
}

///////////////////////////////////////////////////////////////////////////////
// 
// Save compiled binary to cache

void OpenCLContext::SaveBinaryToCache( const cl::Program& program, 
  const std::filesystem::path& cacheFile ) // static
{
  auto binaries = program.getInfo< CL_PROGRAM_BINARIES >();
  if ( binaries.empty() || binaries[ 0 ].empty() )
    return;

  std::ofstream file( cacheFile, std::ios::binary );
  auto size = binaries[ 0 ].size();
  std::streamsize ssize = static_cast<std::streamsize>( size );
  const auto* udata = binaries[ 0 ].data();
  const auto* data = reinterpret_cast<const char*>( udata );

  file.write( data, ssize );
}

///////////////////////////////////////////////////////////////////////////////
// 
// Load binary from cache

std::vector<unsigned char> OpenCLContext::LoadBinaryFromCache( 
  const std::filesystem::path& cacheFile ) // static
{
  std::ifstream file( cacheFile, std::ios::binary );
  return std::vector<unsigned char>{ std::istreambuf_iterator<char>( file ), {} };
}

/////////////////////////////////////////////////////////////////////////////// 
