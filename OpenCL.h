#pragma once

///////////////////////////////////////////////////////////////////////////////
//
// OpenCL.h
//

#include <cassert>
#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#pragma warning(push)
#pragma warning(disable: 4365 4623 4626 5027 5039 5246)
#include "CL/OpenCL.hpp" // C++ header
#pragma warning(pop)

//#include "CL/cl.h" // C header

static constexpr std::string_view kSourceCodeFile = "lower.cl";
static const std::string kBuildOptions{ "-cl-std=CL3.0 -Werror" };
static const std::string kKernelFunction{ "lowerLUT" };

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

inline std::string GetSourceCode( const std::filesystem::path& fileName )
{
  std::ifstream fileStream( fileName );
  return std::string{ std::istreambuf_iterator<char>( fileStream ), {} };
}

class OpenCL
{
public:
  OpenCL() = delete;

  OpenCL( cl_device_type preferredDevice, const std::string& kernelFunction ) :
    device_{ GetDefaultDevice( preferredDevice ) },
    context_{ device_ },
    queue_{ context_, device_ },
    program_{ context_, GetSourceCode( kSourceCodeFile ) },
    kernel_{} // must build before referencing kernel function
  {
    // Build the kernel
    auto result = program_.build( device_, kBuildOptions );
    if( result != CL_SUCCESS )
    {
      std::cout << program_.getBuildInfo<CL_PROGRAM_BUILD_LOG>( device_ );
      throw std::exception( "Error compiling OpenCL kernel" );
    }
    kernel_ = cl::Kernel{ program_, kernelFunction };
    assert( kernel_.get() != nullptr );
  }

  std::string& ToLower( std::string& s )
  {
    cl::Buffer stringDataBuffer( context_, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR, s.size(), s.data());

    // void lower( __global char* s, unsigned long len )
    kernel_.setArg( 0, stringDataBuffer );  // s
    kernel_.setArg( 1, static_cast<uint64_t>( s.size() ) ); // len

    // Execute kernel on the device
    auto offset = cl::NullRange;                // process the entire string
    auto threadCount = cl::NDRange{ s.size() }; // 1 thread per character
    auto localWork = cl::NullRange;             // no local threads
    queue_.enqueueNDRangeKernel( kernel_, offset, threadCount, localWork );
    queue_.finish();

    // Read results back into s
    cl_bool blocking = CL_TRUE;
    queue_.enqueueReadBuffer( stringDataBuffer, blocking, 0, s.size(), s.data() );
    return s;
  }

private:

  cl::Device device_;
  cl::Context context_;
  cl::CommandQueue queue_;
  cl::Program program_;
  cl::Kernel kernel_;

};

///////////////////////////////////////////////////////////////////////////////
