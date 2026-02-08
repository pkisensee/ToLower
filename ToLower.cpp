///////////////////////////////////////////////////////////////////////////////
//
// ToLower.cpp
// 
// Test and performance module for evaluating correctness and performance
// of various case conversion functions.
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

#include "GetLowerByValue.h"
#include "GetRandomStr.h"
#include "LowerCaseAsciiTable.h"
#include "LowerOriginal.h"
#include "Profile.h"
#include "ToLower5thBit.h"
#include "ToLowerAuto.h"
#include "ToLowerAutoConstexpr.h"
#include "ToLowerAutoNoReturn.h"
#include "ToLowerAVX2.h"
#include "ToLowerExecPolicy.h"
#include "ToLowerLookupTable.h"
#include "ToLowerOpenCL.h"
#include "ToLowerPtrNoStrlen.h"
#include "ToLowerSpan.h"
#include "ToLowerSSE.h"
#include "ToLowerStrRef.h"
#include "ToLowerStrView.h"
#include "ToLowerTemplatized.h"
#include "ToLowerThreadPool.h"
#include "ToLowerThreads.h"

///////////////////////////////////////////////////////////////////////////////

size_t kRandomStrSize = 1024uz * 1024uz * 1uz; // 1 MB baseline for scale=1.0
std::string Profile::testStr_ = GetRandomStr( kRandomStrSize );

///////////////////////////////////////////////////////////////////////////////

int __cdecl main()
{
  namespace execution = std::execution;
  using ParPolicy = execution::parallel_policy;
  using ParUnseqPolicy = std::execution::parallel_unsequenced_policy;
  using ThreadPoolType = std::vector<std::jthread>;

  std::cout << "5th bit trick visualization\n";
  std::string bit5ASCII( 127uz - ' ', '\0' );
  std::ranges::iota( bit5ASCII, ' ' );
  std::cout << bit5ASCII << '\n';
  ToLower5thBit( bit5ASCII );
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
  OpenCLContext openCLContextCPU{ CL_DEVICE_TYPE_CPU, "Lower.cl", openCLkernels };
  OpenCLContext openCLContextGPU{ CL_DEVICE_TYPE_GPU, "Lower.cl", openCLkernels };

  ThreadPoolType threadPool( std::max( std::thread::hardware_concurrency(), 1u ) );

  assert( std::ranges::equal( kLowerCaseAsciiTable, kLowerCaseAsciiTableHardCoded ) );

  // inline
  // multithreaded SIMD
  // CUDA, Vulkan, Halide
  // https://lemire.me/blog/2024/08/03/converting-ascii-strings-to-lower-case-at-crazy-speeds-with-avx-512/

  std::vector<Profile> profiles =
  { {
    { "Original",             LowerOriginal                    , 0.001 },
    { "PtrNoStrlen",          ToLowerPtrNoStrlen                       },
    { "Span",                 ToLowerSpan                              },
    { "StrRef",               ToLowerStrRef                            },
    { "Auto",                 ToLowerAuto         <std::string&>       },
    { "AutoNoReturn",         ToLowerAutoNoReturn <std::string&>       },
    { "ByValue",              GetLowerByValue     <std::string&>       },
    { "Templatized",          ToLowerTemplatized  <std::string&>       },
    { "AutoConstexpr",        ToLowerAutoConstexpr<std::string&>       },
    { "5thBit",               ToLower5thBit       <std::string&>       },
    { "LookupTable",          ToLowerLookupTable  <std::string&>       },
    { "Threads",              ToLowerThreads      <std::string&>       },
    { "SSE",                  ToLowerSSE          <std::string&>       },
    { "AVX2 5thBit",          ToLowerAVX2         <std::string&>       },
    { "ExecPolicy par",       ToLowerExecPolicy   <std::string&, const ParPolicy&>, execution::par },
    { "ExecPolicy par_unseq", ToLowerExecPolicy   <std::string&, const ParUnseqPolicy&>, execution::par_unseq },
    { "ThreadPool",           ToLowerThreadPool   <std::string&, ThreadPoolType&>, &threadPool},

    { "OpenCL CPU LUT",       ToLowerOpenCL, "lowerLUT",        &openCLContextCPU },
    { "OpenCL CPU Branch",    ToLowerOpenCL, "lowerBranch",     &openCLContextCPU },
    { "OpenCL CPU Branchless",ToLowerOpenCL, "lowerBranchless", &openCLContextCPU },
    { "OpenCL CPU 5thBit",    ToLowerOpenCL, "lower5thBit",     &openCLContextCPU },
    { "OpenCL CPU Vector",    ToLowerOpenCL, "lowerVector",     &openCLContextCPU },

    { "OpenCL GPU LUT",       ToLowerOpenCL, "lowerLUT",        &openCLContextGPU },
    { "OpenCL GPU Branch",    ToLowerOpenCL, "lowerBranch",     &openCLContextGPU },
    { "OpenCL GPU Branchless",ToLowerOpenCL, "lowerBranchless", &openCLContextGPU },
    { "OpenCL GPU 5thBit",    ToLowerOpenCL, "lower5thBit",     &openCLContextGPU },
    { "OpenCL GPU Vector",    ToLowerOpenCL, "lowerVector",     &openCLContextGPU },
  } };

  std::cout << "Validating implementations\n";
  for ( const auto& profile : profiles )
    profile.Validate();

  std::cout << "\nMeasuring implementations\n";
  for ( const auto& profile : profiles )
    profile.Measure();
}

///////////////////////////////////////////////////////////////////////////////
