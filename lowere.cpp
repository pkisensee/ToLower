///////////////////////////////////////////////////////////////////////////////
//
// lowere.cpp
//
// Notes: 
//    * OpenCL solution
//
// Issues:
//    * Must install OpenCL SDK
//    * Must install Windows drivers for AMD or NVIDIA GPU
//    * CL_TARGET_OPENCL_VERSION=300
//    * Lots of setup/teardown code
//    * Kernel compilation time

#include <cassert>
#include <fstream>
#include <iostream>
#include <string>
#include "CL/cl.h"

void lowere( std::string& s )
{
  // Read kernel source
  std::ifstream kernelFile( "lower.cl" );
  std::string src( std::istreambuf_iterator<char>( kernelFile ), {} );
  const char* kernelSrc = src.c_str();

  // Set up CL device, context, program and kernel
  cl_platform_id platform;
  auto result = clGetPlatformIDs( 1, &platform, nullptr );
  assert( result == CL_SUCCESS );

  cl_device_id device;
  result = clGetDeviceIDs( platform, CL_DEVICE_TYPE_GPU, 1, &device, nullptr );
  assert( result == CL_SUCCESS );

  cl_context context = clCreateContext( nullptr, 1, &device, nullptr, nullptr, &result );
  assert( context != nullptr );
  assert( result == CL_SUCCESS );
  cl_command_queue queue = clCreateCommandQueue( context, device, 0, &result );
  assert( result == CL_SUCCESS );
  cl_mem clBuffer = clCreateBuffer( context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
                                    s.size(), s.data(), &result );
  assert( result == CL_SUCCESS );

  cl_program program = clCreateProgramWithSource( context, 1, &kernelSrc, nullptr, &result );
  assert( result == CL_SUCCESS );
  result = clBuildProgram( program, 1, &device, nullptr, nullptr, nullptr );
  if( result == CL_BUILD_PROGRAM_FAILURE )
  {
    size_t logSize = 0;
    result = clGetProgramBuildInfo( program, device, CL_PROGRAM_BUILD_LOG, 0, nullptr, &logSize );
    assert( result == CL_SUCCESS );
    std::string log;
    log.resize( logSize );
    result = clGetProgramBuildInfo( program, device, CL_PROGRAM_BUILD_LOG, logSize, log.data(), nullptr );
    assert( result == CL_SUCCESS );
    std::cout << log << '\n';
    return;
  }
  assert( result == CL_SUCCESS );

  cl_kernel kernel = clCreateKernel( program, "lower_kernel", &result );
  assert( result == CL_SUCCESS );
  result = clSetKernelArg( kernel, 0, sizeof( clBuffer ), &clBuffer );
  assert( result == CL_SUCCESS );
  size_t len = s.size();
  result = clSetKernelArg( kernel, 1, sizeof( len ), &len );
  assert( result == CL_SUCCESS );

  // Run kernel
  result = clEnqueueNDRangeKernel( queue, kernel, 1, nullptr, &len, nullptr, 0, nullptr, nullptr );
  assert( result == CL_SUCCESS );

  // Read result
  clEnqueueReadBuffer( queue, clBuffer, CL_TRUE, 0, s.size(), s.data(), 0, nullptr, nullptr );

  // Cleanup
  clReleaseMemObject( clBuffer );
  clReleaseKernel( kernel );
  clReleaseProgram( program );
  clReleaseCommandQueue( queue );
  clReleaseContext( context );
}
