///////////////////////////////////////////////////////////////////////////////
//
// Profiler.h
//
// Information on functions to be profiled, with methods to validate the
// functions and measure performance.
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
#include <execution>
#include <fstream>
#include <functional>
#include <thread>
#include <span>
#include <string>
#include <string_view>
#include <variant>
#include <vector>
#include "CUDAKernels.h"

// Forward declarations
class OpenCLContext;
class CUDAContext;

class Profiler
{
public:

  // Policy aliases
  using SeqPolicy = std::execution::sequenced_policy;
  using ParPolicy = std::execution::parallel_policy;
  using UnseqPolicy = std::execution::unsequenced_policy;
  using ParUnseqPolicy = std::execution::parallel_unsequenced_policy;
  using ThreadPoolType = std::vector<std::jthread>;

  // Function aliases
  using LowerOriginalFn = char* ( char* );
  using LowerOriginalFnPtr = LowerOriginalFn*;

  using ToLowerPtrNoStrlenFn = char* ( char*, size_t );
  using ToLowerPtrNoStrlenFnPtr = ToLowerPtrNoStrlenFn*;

  using ToLowerSpanFn = char* ( std::span<char> );
  using ToLowerSpanFnPtr = ToLowerSpanFn*;

  using ToLowerAutoNoReturnFn = void( std::string& );
  using ToLowerAutoNoReturnFnPtr = ToLowerAutoNoReturnFn*;

  using GetLowerByValueFn = std::string( std::string& );
  using GetLowerByValueFnPtr = GetLowerByValueFn*;

  using ToLowerLookupTableFn = std::string& ( std::string& );
  using ToLowerLookupTableFnPtr = ToLowerLookupTableFn*;

  using ToLowerExecPolicySeqFn = std::string& ( std::string&, const SeqPolicy& );
  using ToLowerExecPolicyParFn = std::string& ( std::string&, const ParPolicy& );
  using ToLowerExecPolicyUnseqFn = std::string& ( std::string&, const UnseqPolicy& );
  using ToLowerExecPolicyParUnseqFn = std::string& ( std::string&, const ParUnseqPolicy& );

  using ToLowerThreadPoolFn = std::string& ( std::string&, ThreadPoolType& );
  using ToLowerThreadPoolFnPtr = ToLowerThreadPoolFn*;

  using ToLowerOpenCLFn = std::string& ( std::string&, OpenCLContext&, std::string_view );
  using ToLowerOpenCLFnPtr = ToLowerOpenCLFn*;

  using ToLowerCUDAFn = std::string& ( std::string&, CUDAContext&, CUDAKernelFnPtr );
  using ToLowerCUDAFnPtr = ToLowerCUDAFn*;

  Profiler( std::string_view, LowerOriginalFnPtr, double = 1.0 );
  Profiler( std::string_view, ToLowerPtrNoStrlenFnPtr );
  Profiler( std::string_view, ToLowerSpanFnPtr );
  Profiler( std::string_view, ToLowerAutoNoReturnFnPtr );
  Profiler( std::string_view, GetLowerByValueFnPtr );
  Profiler( std::string_view, ToLowerLookupTableFnPtr );
  Profiler( std::string_view, ToLowerThreadPoolFnPtr, ThreadPoolType* );
  Profiler( std::string_view, ToLowerOpenCLFnPtr, OpenCLContext*, std::string_view );
  Profiler( std::string_view, ToLowerCUDAFnPtr, CUDAContext*, CUDAKernelFnPtr );

  template <typename Policy>
  Profiler( std::string_view name, 
            std::string& ( *func )( std::string&, const Policy& ), 
            const Policy& policy ) :
    name_( name ),
    func_( func )
  {
    if constexpr ( std::is_same_v<Policy, SeqPolicy> )
      seqPolicy_ = policy;
    else if constexpr ( std::is_same_v<Policy, ParPolicy> )
      parPolicy_ = policy;
    else if constexpr ( std::is_same_v<Policy, UnseqPolicy> )
      unseqPolicy_ = policy;
    else
      parunseqPolicy_ = policy;
  }

  Profiler( const Profiler& ) = default; // for storing in std::vector

  Profiler() = delete;
  Profiler& operator=( const Profiler& ) = delete;
  Profiler& operator=( Profiler&& ) = delete;

  void Validate() const; // Validate that the function does the correct thing
  void Measure() const; // Record the time required to run the given function

  static std::ofstream outputFile; // Optional output file for storing benchmarks

private:

  void Validate( std::string& input, const std::string& expectedOutput ) const;
  std::string GetMeasureInput() const;
  void InvokeFunction( std::string& ) const;
  static double GetSpeedupFactor( double );

  using FunctionVariant = std::variant<
    std::function< LowerOriginalFn >,
    std::function< ToLowerPtrNoStrlenFn >,
    std::function< ToLowerSpanFn >,
    std::function< ToLowerAutoNoReturnFn >,
    std::function< GetLowerByValueFn >,
    std::function< ToLowerLookupTableFn >,
    std::function< ToLowerExecPolicySeqFn >,
    std::function< ToLowerExecPolicyParFn >,
    std::function< ToLowerExecPolicyUnseqFn >,
    std::function< ToLowerExecPolicyParUnseqFn >,
    std::function< ToLowerThreadPoolFn >,
    std::function< ToLowerOpenCLFn >,
    std::function< ToLowerCUDAFn >
  >;

  std::string_view name_; // e.g. "LookupTable"
  FunctionVariant func_; // e.g. ToLowerLookupTable
  double scale_ = 1.0; // decrease for slow funcs; increase to improve Measure() speed

  // For std::algorithms with policies
  SeqPolicy      seqPolicy_      = std::execution::seq;
  ParPolicy      parPolicy_      = std::execution::par;
  UnseqPolicy    unseqPolicy_    = std::execution::unseq;
  ParUnseqPolicy parunseqPolicy_ = std::execution::par_unseq;

  ThreadPoolType* threadPool_ = nullptr; // For functions that accept a threadPool

  // For OpenCL functions
  OpenCLContext*   openCLContext_ = nullptr;
  std::string_view kernel_; // e.g. "lowerLUT"

  // For CUDA functions
  CUDAContext*    cudaContext_ = nullptr;
  CUDAKernelFnPtr cudaKernel_ = nullptr; // e.g. ToLowerCUDABranch

  static std::string validateStr_; // mixed case string used by Validate()
  static std::string measureStr_;  // mixed case string used by Measure()
  static size_t measurementRunCount_;  // the fastest run is recorded

}; // class Profiler

///////////////////////////////////////////////////////////////////////////////
