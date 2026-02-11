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
#include "ToLowerAVX512.h"
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

size_t kRandomStrSize = 1024uz * 1024uz * 10uz; // 10 MB baseline for scale=1.0
std::string Profile::testStr_ = GetRandomStr( kRandomStrSize );
std::ofstream Profile::outputFile;

///////////////////////////////////////////////////////////////////////////////

int __cdecl main()
{
  static_assert( kLowerCaseAsciiTable == kLowerCaseAsciiTableHardCoded );

  if ( !HasSIMDSupport( SIMDLevel::AVX2 ) )
    std::cout << "Note: No AVX2 support\n";
  if ( !HasSIMDSupport( SIMDLevel::AVX512 ) )
    std::cout << "Note: No AVX-512 support\n";

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
  //OpenCLContext openCLContextCPU{ CL_DEVICE_TYPE_CPU, "Lower.cl", openCLkernels };
  //OpenCLContext openCLContextGPU{ CL_DEVICE_TYPE_GPU, "Lower.cl", openCLkernels };

  ThreadPoolType threadPool( std::max( std::thread::hardware_concurrency(), 1u ) );

  std::vector<Profile> profiles =
  { {
    { "original",             LowerOriginal                    , 0.001 },
    { "ptr no strlen",        ToLowerPtrNoStrlen                       },
    { "span<char>",           ToLowerSpan                              },
    { "std::string&",         ToLowerStrRef                            },
    { "templatized",          ToLowerTemplatized      <std::string&>   },
    { "auto&&",               ToLowerAuto             <std::string&>   },
    { "auto&& no return",     ToLowerAutoNoReturn     <std::string&>   },
    { "constexpr noexcept",   ToLowerAutoConstexpr    <std::string&>   },
    { "return by value",      GetLowerByValue         <std::string&>   },
    { "bit 5 trick",          ToLower5thBit           <std::string&>   },
    { "lookup table",         ToLowerLookupTable      <std::string&>   },
    { "threads",              ToLowerThreads          <std::string&>   },
    { "thread pool",          ToLowerThreadPool       <std::string&, ThreadPoolType&>, &threadPool      },
    { "thread pool bit 5",    ToLowerThreadPool5thBit <std::string&, ThreadPoolType&>, &threadPool      },
    { "thread pool SSE",      ToLowerThreadPoolSSE    <std::string&, ThreadPoolType&>, &threadPool      },
    { "execution::par",       ToLowerExecPolicy       <std::string&, const ParPolicy&>, execution::par  },
    { "execution::par_unseq", ToLowerExecPolicy       <std::string&, const ParUnseqPolicy&>, execution::par_unseq },
    { "AVX2 with bit 5",      ToLowerAVX2             <std::string&>    },
    { "AVX512 bit 5",         ToLowerAVX512           <std::string&>    }, // flagged "bit 5" because of fall back to AVX2
    { "SSE",                  ToLowerSSE              <std::string&>    },

    /*
    { "OpenCL CPU LUT",       ToLowerOpenCL, "lowerLUT",        &openCLContextCPU },
    { "OpenCL CPU branch",    ToLowerOpenCL, "lowerBranch",     &openCLContextCPU },
    { "OpenCL CPU branchless",ToLowerOpenCL, "lowerBranchless", &openCLContextCPU },
    { "OpenCL CPU bit 5",     ToLowerOpenCL, "lower5thBit",     &openCLContextCPU },
    { "OpenCL CPU unrolled",  ToLowerOpenCL, "lowerVector",     &openCLContextCPU },

    { "OpenCL GPU LUT",       ToLowerOpenCL, "lowerLUT",        &openCLContextGPU },
    { "OpenCL GPU branch",    ToLowerOpenCL, "lowerBranch",     &openCLContextGPU },
    { "OpenCL GPU branchless",ToLowerOpenCL, "lowerBranchless", &openCLContextGPU },
    { "OpenCL GPU bit 5",     ToLowerOpenCL, "lower5thBit",     &openCLContextGPU },
    { "OpenCL GPU unrolled",  ToLowerOpenCL, "lowerVector",     &openCLContextGPU },
    */
  } };

  std::cout << "Validating implementations\n";
  for ( const auto& profile : profiles )
    profile.Validate();


  Profile::outputFile.open( "benchmark.txt" );
  std::cout << "\nMeasuring implementations\n";
  for ( const auto& profile : profiles )
    profile.Measure();

  Profile::outputFile.close();

  // Additional options to consider:
  //  * std::simd (currently experimental; expected in C++26)
  //  * CUDA (NVIDIA GPUs)
  //  * Vulkan compute shader (GPU)
  //  * Halide (fast image processing)
  //  * AMD ROCm
  //  * VMX (PowerPC CPUs)
  //  * Neon (ARM)
  //  * OpenMP

}

///////////////////////////////////////////////////////////////////////////////
