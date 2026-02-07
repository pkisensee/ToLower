///////////////////////////////////////////////////////////////////////////////
//
//  ToLower.cpp
// 
//  Test and performance module for evaluating correctness and performance
//  of various case conversion functions.
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
//
///////////////////////////////////////////////////////////////////////////////

#include <cassert>
#include <clocale>
#include <execution>
#include <filesystem>
#include <iostream>
#include <span>
#include <string>
#include <thread>
#include <utility>
#include <vector>

#include "getLowerByValue.h"
#include "LowerCaseAsciiTable.h"
#include "lowerOriginal.h"
#include "Profile.h"
#include "toLower5thBit.h"
#include "toLowerAuto.h"
#include "toLowerAutoConstexpr.h"
#include "toLowerAutoNoReturn.h"
#include "toLowerAVX2.h"
#include "toLowerExecPolicy.h"
#include "toLowerLookupTable.h"
#include "toLowerOpenCL.h"
#include "toLowerPtrNoStrlen.h"
#include "toLowerSpan.h"
#include "toLowerSSE.h"
#include "toLowerStrRef.h"
#include "toLowerStrView.h"
#include "toLowerTemplatized.h"
#include "toLowerThreadPool.h"
#include "toLowerThreads.h"

///////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
#define test(e) assert(e)
#else
#define test(e) static_cast<void>( (e) || ( __debugbreak(), 0 ) )
#endif

size_t kRandomStrSize = 1024uz * 1024uz * 1uz; // 1 MB baseline for scale=1.0
std::string Profile::testStr_ = getRandomStr( kRandomStrSize );

///////////////////////////////////////////////////////////////////////////////

int __cdecl main()
{
  std::cout << "5th bit trick visualization\n";
  std::string bit5ASCII( 127uz - ' ', '\0' );
  std::ranges::iota( bit5ASCII, ' ' );
  std::cout << bit5ASCII << '\n';
  toLower5thBit( bit5ASCII );
  std::cout << bit5ASCII << '\n';

  std::array<std::string, 5> openCLkernels =
  { {
    { "lowerLUT" },
    { "lowerBranch" },
    { "lowerBranchless" },
    { "lower5thBit" },
    { "lowerVector" },
  } };

  std::cout << "\nCompiling OpenCL kernels\n";
  OpenCLContext openCLContextCPU{ CL_DEVICE_TYPE_CPU, "lower.cl", openCLkernels };
  OpenCLContext openCLContextGPU{ CL_DEVICE_TYPE_GPU, "lower.cl", openCLkernels };

  std::vector<std::jthread> threadPool( std::max( std::thread::hardware_concurrency(), 1u ) );

  test( std::ranges::equal( kLowerCaseAsciiTable, kLowerCaseAsciiTableHardCoded ) );

  // multithreaded SIMD
  // CUDA, Vulkan, Halide
  // https://lemire.me/blog/2024/08/03/converting-ascii-strings-to-lower-case-at-crazy-speeds-with-avx-512/

  namespace execution = std::execution;
  using ParPolicy = execution::parallel_policy;
  using ParUnseqPolicy = std::execution::parallel_unsequenced_policy;

  std::vector<Profile> profiles =
  { {
    { "Original",             lowerOriginal                    , 0.001 },
    { "PtrNoStrlen",          toLowerPtrNoStrlen                       },
    { "Span",                 toLowerSpan                              },
    { "StrRef",               toLowerStrRef                            },
    { "Auto",                 toLowerAuto         <std::string&>       },
    { "AutoNoReturn",         toLowerAutoNoReturn <std::string&>       },
    { "ByValue",              getLowerByValue     <std::string&>       },
    { "Templatized",          toLowerTemplatized  <std::string&>       },
    { "AutoConstexpr",        toLowerAutoConstexpr<std::string&>       },
    { "5thBit",               toLower5thBit       <std::string&>       },
    { "LookupTable",          toLowerLookupTable  <std::string&>       },
    { "Threads",              toLowerThreads      <std::string&>       },
    { "SSE",                  toLowerSSE          <std::string&>       },
    { "AVX2 5thBit",          toLowerAVX2         <std::string&>       },
    { "ExecPolicy par",       toLowerExecPolicy   <std::string&, const ParPolicy&>, execution::par },
    { "ExecPolicy par_unseq", toLowerExecPolicy   <std::string&, const ParUnseqPolicy&>, execution::par_unseq },
    { "ThreadPool",           toLowerThreadPool   <std::string&, std::vector<std::jthread>&>, &threadPool},

    { "OpenCL CPU LUT",       toLowerOpenCL, "lowerLUT",        &openCLContextCPU },
    { "OpenCL CPU Branch",    toLowerOpenCL, "lowerBranch",     &openCLContextCPU },
    { "OpenCL CPU Branchless",toLowerOpenCL, "lowerBranchless", &openCLContextCPU },
    { "OpenCL CPU 5thBit",    toLowerOpenCL, "lower5thBit",     &openCLContextCPU },
    { "OpenCL CPU Vector",    toLowerOpenCL, "lowerVector",     &openCLContextCPU },

    { "OpenCL GPU LUT",       toLowerOpenCL, "lowerLUT",        &openCLContextGPU },
    { "OpenCL GPU Branch",    toLowerOpenCL, "lowerBranch",     &openCLContextGPU },
    { "OpenCL GPU Branchless",toLowerOpenCL, "lowerBranchless", &openCLContextGPU },
    { "OpenCL GPU 5thBit",    toLowerOpenCL, "lower5thBit",     &openCLContextGPU },
    { "OpenCL GPU Vector",    toLowerOpenCL, "lowerVector",     &openCLContextGPU },
  } };

  std::cout << "Validating implementations\n";
  for ( const auto& profile : profiles )
    profile.Validate();

  std::cout << "\nMeasuring implementations\n";
  for ( const auto& profile : profiles )
    profile.Measure();
}

///////////////////////////////////////////////////////////////////////////////
