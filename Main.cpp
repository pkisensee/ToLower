///////////////////////////////////////////////////////////////////////////////
//
// Main.cpp
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
//
// Additional performance options to consider:
//  * std::simd (currently experimental; expected in C++26)
//  * Vulkan compute shader (GPU)
//  * Halide (fast image processing)
//  * AMD ROCm
//  * VMX (PowerPC CPUs)
//  * Neon (ARM)
// 
// Note: coroutines could be useful if input was async, but that's not true in
// this case

#define ENABLE_OPENCL 1
#define ENABLE_CUDA 1

#include <cassert>
#include <clocale>
#include <execution>
#include <filesystem>
#include <print>
#include <span>
#include <string>
#include <thread>
#include <utility>
#include <vector>

#include "CUDAKernels.h"
#include "GetLowerByValue.h"
#include "GetRandomStr.h"
#include "LowerCaseAsciiTable.h"
#include "LowerOriginal.h"
#include "Profiler.h"
#include "ToLower5thBit.h"
#include "ToLowerAuto.h"
#include "ToLowerAutoConstexpr.h"
#include "ToLowerAutoNoReturn.h"
#include "ToLowerAVX2.h"
#include "ToLowerAVX512.h"
#include "ToLowerCUDA.h"
#include "ToLowerExecPolicy.h"
#include "ToLowerLookupTable.h"
#include "ToLowerOpenCL.h"
#include "ToLowerPtrNoStrlen.h"
#include "ToLowerRangeBasedFor.h"
#include "ToLowerSpan.h"
#include "ToLowerSSE.h"
#include "ToLowerSSE5thBit.h"
#include "ToLowerSSE5thBitAligned.h"
#include "ToLowerStrRef.h"
#include "ToLowerStrView.h"
#include "ToLowerTemplatized.h"
#include "ToLowerThreadPool.h"
#include "ToLowerThreads.h"

///////////////////////////////////////////////////////////////////////////////

using namespace std::literals;

size_t kValidateStrSize = 1024uz * 100uz; // 100K random string for validation
size_t kMeasureStrSize = 1024uz * 1024uz * 10uz; // 10 MB baseline for scale=1.0
std::string Profiler::validateStr_ = GetRandomStr( kValidateStrSize );
std::string Profiler::measureStr_ = GetRandomStr( kMeasureStrSize );
std::ofstream Profiler::outputFile;
size_t Profiler::measurementRuns_ = 10;
std::string_view kOpenCLKernelName = "OpenCLKernels.cl"sv;

namespace execution = std::execution;
using ParPolicy = execution::parallel_policy;
using UnseqPolicy = execution::unsequenced_policy;
using ParUnseqPolicy = execution::parallel_unsequenced_policy;
using ThreadPoolType = std::vector<std::jthread>;

///////////////////////////////////////////////////////////////////////////////

void TestCUDASupport()
{
  std::println( "Note: CUDA support is {}present",
                CUDAContext::SupportsCuda() ? "" : "not " );
}

void TestSIMDSupport()
{
  if ( !HasSIMDSupport( SIMDLevel::AVX2 ) )
    std::println( "Note: No AVX2 support" );
  if ( !HasSIMDSupport( SIMDLevel::AVX512 ) )
    std::println( "Note: No AVX-512 support" );
}

void Bit5Visualizatiaon()
{
  std::println( "5th bit trick visualization" );
  std::string bit5ASCII( 127uz - ' ', '\0' );
  std::ranges::iota( bit5ASCII, ' ' );
  std::println( "{}", bit5ASCII );
  ToLower5thBit( bit5ASCII );
  std::println( "{}", bit5ASCII );
}

///////////////////////////////////////////////////////////////////////////////

void ValidateAndMeasure()
{
#if defined( ENABLE_OPENCL )
  std::println( "\nLoading OpenCL kernels" );
  std::array<std::string, 5> openCLkernels =
  { {
    { "lowerLookupTable" },
    { "lowerBranch" },
    { "lowerCondMove" },
    { "lower5thBit" },
    { "lowerSIMD" },
  } };
  OpenCLContext openCLContextCPU{ CL_DEVICE_TYPE_CPU, kOpenCLKernelName, openCLkernels };
  OpenCLContext openCLContextGPU{ CL_DEVICE_TYPE_GPU, kOpenCLKernelName, openCLkernels };
#endif

#if defined( ENABLE_CUDA )
  CUDAContext cudaContext;
#endif

  ThreadPoolType threadPool( std::max( std::thread::hardware_concurrency(), 1u ) );

  std::vector<Profiler> profilers =
  { {
    { "original",             LowerOriginal,         0.001             },
    { "ptr no strlen",        ToLowerPtrNoStrlen                       },
    { "span<char>",           ToLowerSpan                              },
    { "std::string&",         ToLowerStrRef                            },
    { "templatized",          ToLowerTemplatized      <std::string&>   },
    { "auto&&",               ToLowerAuto             <std::string&>   },
    { "auto&& no return",     ToLowerAutoNoReturn     <std::string&>   },
    { "constexpr noexcept",   ToLowerAutoConstexpr    <std::string&>   },
    { "range based for",      ToLowerRangeBasedFor    <std::string&>   },
    { "return by value",      GetLowerByValue         <std::string&>   },
    { "bit 5 trick",          ToLower5thBit           <std::string&>   },
    { "lookup table",         ToLowerLookupTable      <std::string&>   },
    { "threads",              ToLowerThreads          <std::string&>   },
    { "thread pool",          ToLowerThreadPool       <std::string&, ThreadPoolType&>, &threadPool },
    { "thread pool bit 5",    ToLowerThreadPool5thBit <std::string&, ThreadPoolType&>, &threadPool },
    { "thread pool SSE",      ToLowerThreadPoolSSE    <std::string&, ThreadPoolType&>, &threadPool },
    { "execution::par",       ToLowerExecPolicy       <std::string&, const ParPolicy&>, execution::par            },
    { "execution::unseq",     ToLowerExecPolicy       <std::string&, const UnseqPolicy&>, execution::unseq        },
    { "execution::par_unseq", ToLowerExecPolicy       <std::string&, const ParUnseqPolicy&>, execution::par_unseq },
    { "AVX2 with bit 5",      ToLowerAVX2             <std::string&>    },
    { "AVX512 bit 5",         ToLowerAVX512           <std::string&>    }, // "bit 5" because of fall back to AVX2
    { "SSE",                  ToLowerSSE              <std::string&>    },
    { "SSE bit 5",            ToLowerSSE5thBit        <std::string&>    },
    { "SSE bit 5 aligned",    ToLowerSSE5thBitAligned <std::string&>    },

#if defined( ENABLE_OPENCL )
    { "OpenCL CPU lookup",    ToLowerOpenCL, &openCLContextCPU, "lowerLookupTable" },
    { "OpenCL CPU branch",    ToLowerOpenCL, &openCLContextCPU, "lowerBranch"      },
    { "OpenCL CPU cond move", ToLowerOpenCL, &openCLContextCPU, "lowerCondMove"    },
    { "OpenCL CPU bit 5",     ToLowerOpenCL, &openCLContextCPU, "lower5thBit"      },
    { "OpenCL CPU SIMD",      ToLowerOpenCL, &openCLContextCPU, "lowerSIMD"        },

    { "OpenCL GPU lookup",    ToLowerOpenCL, &openCLContextGPU, "lowerLookupTable" },
    { "OpenCL GPU branch",    ToLowerOpenCL, &openCLContextGPU, "lowerBranch"      },
    { "OpenCL GPU cond move", ToLowerOpenCL, &openCLContextGPU, "lowerCondMove"    },
    { "OpenCL GPU bit 5",     ToLowerOpenCL, &openCLContextGPU, "lower5thBit"      },
    { "OpenCL GPU SIMD",      ToLowerOpenCL, &openCLContextGPU, "lowerSIMD"        },
#endif

#if defined( ENABLE_CUDA )
    { "CUDA lookup",          ToLowerCUDA, &cudaContext, ToLowerCUDALookupTable },
    { "CUDA branch",          ToLowerCUDA, &cudaContext, ToLowerCUDABranch      },
    { "CUDA cond move",       ToLowerCUDA, &cudaContext, ToLowerCUDACondMove    },
    { "CUDA bit 5",           ToLowerCUDA, &cudaContext, ToLowerCUDA5thBit      },
#endif
  } };

  std::println( "Validating implementations" );
  for ( const auto& profiler : profilers )
    profiler.Validate();

  Profiler::outputFile.open( "benchmark.txt" );
  std::println( "\nMeasuring implementations" );
  for ( const auto& profiler : profilers )
    profiler.Measure();

  Profiler::outputFile.close();
}

///////////////////////////////////////////////////////////////////////////////

int __cdecl main()
{
  try
  {
    TestCUDASupport();
    TestSIMDSupport();
    Bit5Visualizatiaon();
    ValidateAndMeasure();
    return 0;
  }
  catch ( std::exception& e )
  {
    std::println( "{}", e.what() );
    return -1;
  }
}

///////////////////////////////////////////////////////////////////////////////
