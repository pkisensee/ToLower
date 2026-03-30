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
// Note: std::coroutines could be useful if input was async, but that's not true in
// this case

//#define ENABLE_OPENCL 1
//#define ENABLE_CUDA 1

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
#include "GetLower.h"
#include "GetRandomStr.h"
#include "LowerCaseAsciiTable.h"
#include "LowerOriginal.h"
#include "OpenCLKernels.h"
#include "Profiler.h"
#include "ToLower.h"
#include "ToLowerAuto.h"
#include "ToLowerAutoConstexpr.h"
#include "ToLowerAVX2.h"
#include "ToLowerAVX512.h"
#include "ToLowerClassicForEach.h"
#include "ToLowerClassicTransform.h"
#include "ToLowerCUDA.h"
#include "ToLowerExecPolicy.h"
#include "ToLowerInner.h"
#include "ToLowerNoReturn.h"
#include "ToLowerOpenCL.h"
#include "ToLowerPtrNoStrlen.h"
#include "ToLowerRangeBasedFor.h"
#include "ToLowerRangesForEach.h"
#include "ToLowerRangesTransform.h"
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

// Constants
static constexpr size_t kValidateStrSize = 1024uz * 100uz; // 100K random string for validation
static constexpr size_t kMeasureStrSize = 1024uz * 1024uz * 10uz; // 10 MB baseline for scale=1.0
static std::string_view kOpenCLKernelName = "OpenCLKernels.cl"sv;

// Establish profiler data
std::string Profiler::validateStr_ = GetRandomStr( kValidateStrSize );
std::string Profiler::measureStr_ = GetRandomStr( kMeasureStrSize );
std::ofstream Profiler::outputFile;
size_t Profiler::measurementRunCount_ = 50;

// Aliases
namespace execution = std::execution;
using SeqPolicy = execution::sequenced_policy;
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
  std::println( "5th bit visualization" );
  std::string bit5ASCII( 127uz - ' ', '\0' );
  std::ranges::iota( bit5ASCII, ' ' );
  std::println( "{}", bit5ASCII );
  ToLowerInner5thBit( bit5ASCII );
  std::println( "{}", bit5ASCII );
}

///////////////////////////////////////////////////////////////////////////////

void ValidateAndMeasure()
{
#if defined( ENABLE_OPENCL )
  std::println( "\nLoading OpenCL kernels" );
  std::array<std::string, 5> openCLkernels =
  { {
    { kOpenCLKernelVector },
    { kOpenCLKernelLookupTable },
    { kOpenCLKernelBranch },
    { kOpenCLKernel5thBit },
    { kOpenCLKernelCondMove },
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
//  { "original",             LowerOriginal         }, // 10,156,452,893 us = 169.3 minutes = ~3 hours
    { "strlen hoisted out",   ToLowerPtrNoStrlen    },
    { "ToLower",              ToLower<std::string&> },

    // Evaluate different GetLowerChar versions
    /*
    { "inner tolower",        ToLowerInnerDefault     <std::string&> },
    { "inner lookup table",   ToLowerInnerLookupTable <std::string&> },
    { "inner cond move",      ToLowerInnerCondMove    <std::string&> },
    { "inner bit 5",          ToLowerInner5thBit      <std::string&> },
    { "inner branch",         ToLowerInnerBranch      <std::string&> },
    */

    // Evaluate looping choices
    /*
    { "classic for_each",     ToLowerClassicForEach   <std::string&> },
    { "ranges for_each",      ToLowerRangesForEach    <std::string&> },
    { "classic transform",    ToLowerClassicTransform <std::string&> },
    { "ranges transform",     ToLowerRangesTransform  <std::string&> },
    { "range-based for",      ToLowerRangeBasedFor    <std::string&> },
    */

    // Evaluate different interfaces
    /*
    { "span<char>",           ToLowerSpan                         },
    { "std::string&",         ToLowerStrRef                       },
    { "auto&&",               ToLowerAuto          <std::string&> },
    { "auto&& no return",     ToLowerNoReturn      <std::string&> },
    { "templatized",          ToLowerTemplatized   <std::string&> },
    { "constexpr noexcept",   ToLowerAutoConstexpr <std::string&> },
    */

    // Multithreading
    { "threads",           ToLowerThreads          <std::string&>   },
    { "thread pool",       ToLowerThreadPool       <std::string&, ThreadPoolType&>, &threadPool },
    /*
    { "thread pool bit 5", ToLowerThreadPool5thBit <std::string&, ThreadPoolType&>, &threadPool },
    */

    // Execution policies
    //{ "execution::seq",       ToLowerExecPolicy <std::string&, const SeqPolicy&>, execution::seq            },
    { "execution::par",       ToLowerExecPolicy <std::string&, const ParPolicy&>, execution::par            },
    //{ "execution::unseq",     ToLowerExecPolicy <std::string&, const UnseqPolicy&>, execution::unseq        },
    //{ "execution::par_unseq", ToLowerExecPolicy <std::string&, const ParUnseqPolicy&>, execution::par_unseq },

#if defined( ENABLE_OPENCL )
    { "OpenCL CPU branch",    ToLowerOpenCL<std::string&>, &openCLContextCPU, kOpenCLKernelBranch      },
    { "OpenCL CPU cond move", ToLowerOpenCL<std::string&>, &openCLContextCPU, kOpenCLKernelCondMove    },
    { "OpenCL CPU lookup",    ToLowerOpenCL<std::string&>, &openCLContextCPU, kOpenCLKernelLookupTable },
    { "OpenCL CPU bit 5",     ToLowerOpenCL<std::string&>, &openCLContextCPU, kOpenCLKernel5thBit      },
    { "OpenCL CPU vector",    ToLowerOpenCL<std::string&>, &openCLContextCPU, kOpenCLKernelVector      },

    { "OpenCL GPU branch",    ToLowerOpenCL<std::string&>, &openCLContextGPU, kOpenCLKernelBranch      },
    { "OpenCL GPU cond move", ToLowerOpenCL<std::string&>, &openCLContextGPU, kOpenCLKernelCondMove    },
    { "OpenCL GPU lookup",    ToLowerOpenCL<std::string&>, &openCLContextGPU, kOpenCLKernelLookupTable },
    { "OpenCL GPU bit 5",     ToLowerOpenCL<std::string&>, &openCLContextGPU, kOpenCLKernel5thBit      },
    { "OpenCL GPU vector",    ToLowerOpenCL<std::string&>, &openCLContextGPU, kOpenCLKernelVector      },
#endif

#if defined( ENABLE_CUDA )
    { "CUDA lookup",     ToLowerCUDA<std::string&>, &cudaContext, ToLowerCUDALookupTable },
    { "CUDA bit 5",      ToLowerCUDA<std::string&>, &cudaContext, ToLowerCUDA5thBit      },
    { "CUDA branch",     ToLowerCUDA<std::string&>, &cudaContext, ToLowerCUDABranch      },
    { "CUDA cond move",  ToLowerCUDA<std::string&>, &cudaContext, ToLowerCUDACondMove    },
#endif

    // SIMD
    /*
    { "SSE",               ToLowerSSE              <std::string&>    },
    { "SSE bit 5",         ToLowerSSE5thBit        <std::string&>    },
    { "SSE bit 5 aligned", ToLowerSSE5thBitAligned <std::string&>    },
    { "AVX2 with bit 5",   ToLowerAVX2             <std::string&>    },
    { "AVX512 bit 5",      ToLowerAVX512           <std::string&>    }, // "bit 5" because of fall back to AVX2
    { "thread pool SSE",   ToLowerThreadPoolSSE    <std::string&, ThreadPoolType&>, &threadPool },
    */

    // Return by value
    //{ "return by value",   GetLower                <std::string&>   },
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
    //Bit5Visualizatiaon();
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
