///////////////////////////////////////////////////////////////////////////////
//
// Profiler.cpp
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
#include <algorithm>
#include <cassert>
#include <cmath>
#include <print>
#include <ranges>
#include "GetLower.h"
#include "GetRandomStr.h"
#include "Profiler.h"
#include "Stopwatch.h"
#include "ToLowerInner.h"

#ifdef _DEBUG
#define test(e) assert(e)
#else
#define test(e) static_cast<void>( (e) || ( __debugbreak(), 0 ) )
#endif

static constexpr size_t kNameColumnWidth = 22;
static constexpr size_t kTimeColumnWidth = 8;

///////////////////////////////////////////////////////////////////////////////
//
// Constructors

Profiler::Profiler( std::string_view name, LowerOriginalFnPtr func, double scale ) :
  name_( name ),
  func_( std::function<LowerOriginalFn>( func )),
  scale_( scale )
{
}

Profiler::Profiler( std::string_view name, ToLowerPtrNoStrlenFnPtr func ) :
  name_( name ),
  func_( std::function<ToLowerPtrNoStrlenFn>( func ) )
{
}

Profiler::Profiler( std::string_view name, ToLowerSpanFnPtr func ) :
  name_( name ),
  func_( std::function<ToLowerSpanFn>( func ) )
{
}

Profiler::Profiler( std::string_view name, ToLowerAutoNoReturnFnPtr func ) :
  name_( name ),
  func_( std::function<ToLowerAutoNoReturnFn>( func ) )
{
}

Profiler::Profiler( std::string_view name, GetLowerByValueFnPtr func ) :
  name_( name ),
  func_( std::function<GetLowerByValueFn>( func ) )
{
}

Profiler::Profiler( std::string_view name, ToLowerLookupTableFnPtr func ) :
  name_( name ),
  func_( std::function<ToLowerLookupTableFn>( func ) )
{
}

Profiler::Profiler( std::string_view name, ToLowerThreadPoolFnPtr func,
  ThreadPoolType* threadPool ) :
  name_( name ),
  func_( std::function<ToLowerThreadPoolFn>( func ) ),
  threadPool_( threadPool )
{
}

Profiler::Profiler( std::string_view name, ToLowerOpenCLFnPtr func,
  OpenCLContext* openCLContext, std::string_view kernel ) :
  name_( name ),
  func_( std::function<ToLowerOpenCLFn>( func ) ),
  openCLContext_( openCLContext ),
  kernel_( kernel )
{
}

Profiler::Profiler( std::string_view name, ToLowerCUDAFnPtr func,
  CUDAContext* cudaContext, CUDAKernelFnPtr cudaKernel ) :
  name_( name ),
  func_( std::function<ToLowerCUDAFn>( func ) ),
  cudaContext_( cudaContext ),
  cudaKernel_( cudaKernel )
{
}

///////////////////////////////////////////////////////////////////////////////
// 
// Validate that the function does the correct thing

void Profiler::Validate() const
{
  std::print( "{:{}}: ", name_, kNameColumnWidth );

  // Test empty string
  std::string empty;
  Validate( empty, empty );

  // Test small string (small string optimization on stack)
  std::string small = "mNoP";
  std::string smallLower = "mnop";
  Validate( small, smallLower );

  // Test simple string
  std::string simple = "AbCdEfGhIJKlmnoPQRSTuVWxyz";
  std::string simpleLower = "abcdefghijklmnopqrstuvwxyz";
  Validate( simple, simpleLower );

  // Test validation string
  std::string random = validateStr_;
  std::string randomLower = GetLower( random );
  if ( name_.contains( "bit 5" ) )
    ToLowerInner5thBit( randomLower );
  Validate( random, randomLower );
  std::println( "Passed" );
}

///////////////////////////////////////////////////////////////////////////////
// 
// Record the time required to run the given function on large input.
// Executes measurementRuns_ and reports the lowest time measured.

void Profiler::Measure() const
{
  std::print( "{:{}}: ", name_, kNameColumnWidth );
  std::string input = GetMeasureInput();
  std::vector<double> measurements;
  measurements.reserve( measurementRunCount_ );
  for ( size_t run = 0; run < measurementRunCount_; ++run )
  {
    Stopwatch stopwatch;
    InvokeFunction( input ); // Only measuring this call
    double microSecs = stopwatch.GetElapsedMicroseconds();
    if ( scale_ != 0.0 )
      microSecs /= scale_;
    measurements.push_back( microSecs );
  }
  double minRuntimeMicroSecs = std::ranges::min( measurements );

  // Compute speedup factor
  std::println( "{:{}.0f} microsecs ({:.1f}X)", minRuntimeMicroSecs, kTimeColumnWidth, 
    GetSpeedupFactor( minRuntimeMicroSecs ) );

  if ( outputFile.is_open() )
    outputFile << name_ << ", " << minRuntimeMicroSecs << '\n';
}

///////////////////////////////////////////////////////////////////////////////
// 
// Validate that the function converts input to expectedOutput

void Profiler::Validate( std::string& input, 
  const std::string& expectedOutput ) const // private
{
  InvokeFunction( input );
  test( std::ranges::equal( input, expectedOutput ) );
}

///////////////////////////////////////////////////////////////////////////////
// 
// Generate string of appropriate size based on desired scale

std::string Profiler::GetMeasureInput() const // private
{
  std::string input{ measureStr_ };
  auto scaledSize = static_cast<double>( measureStr_.size() ) * scale_;
  auto charCount = static_cast<size_t>( scaledSize );
  while ( input.size() < charCount )
    input += measureStr_;
  input.resize( charCount );
  return input;
}

///////////////////////////////////////////////////////////////////////////////
// 
// Invoke the appropriate function. With std::visit and if constexpr,
// compiler will likely resolve this to a simple switch statement with
// low overhead.

void Profiler::InvokeFunction( std::string& input ) const // private
{
  auto visitorOp = [this, &input]( auto& func )
  {
    using Func = std::decay_t<decltype( func )>;

    // Determine function signature and call appropriately
    if constexpr ( std::is_invocable_v<Func, char*> )
      func( input.data() );
    else if constexpr ( std::is_invocable_v<Func, char*, size_t> )
      func( input.data(), input.size() );
    else if constexpr ( std::is_invocable_v<Func, std::span<char>> )
    {
      std::span span{ input };
      func( span );
    }
    else if constexpr ( std::is_invocable_v<Func, std::string&, const SeqPolicy&> )
      func( input, seqPolicy_ );
    else if constexpr ( std::is_invocable_v<Func, std::string&, const ParPolicy&> )
      func( input, parPolicy_ );
    else if constexpr ( std::is_invocable_v<Func, std::string&, const UnseqPolicy&> )
      func( input, unseqPolicy_ );
    else if constexpr ( std::is_invocable_v<Func, std::string&, const ParUnseqPolicy&> )
      func( input, parunseqPolicy_ );
    else if constexpr ( std::is_invocable_v<Func, std::string&, ThreadPoolType&> )
      func( input, *threadPool_ );
    else if constexpr ( std::is_invocable_v<Func, std::string&, OpenCLContext&, std::string_view> )
      func( input, *openCLContext_, kernel_ );
    else if constexpr ( std::is_invocable_v<Func, std::string&, CUDAContext&, CUDAKernelFnPtr> )
      func( input, *cudaContext_, cudaKernel_ );
    else if constexpr ( std::is_invocable_r_v<std::string, Func, std::string&> )
      input = func( input );
    else // void(std::string&) or std::string&(std::string&)
      func( input );
  };

  std::visit( visitorOp, func_ );
}

///////////////////////////////////////////////////////////////////////////////
// 
// Computes the speedup factor, assuming the first call to this function is
// the baseline measurement

double Profiler::GetSpeedupFactor( double x ) // static
{
  static bool isBaselineFunction = true; // assume first function is baseline
  static double baselineMicroSecs = 1.0;
  if ( isBaselineFunction )
  {
    baselineMicroSecs = x;
    isBaselineFunction = false;
  }
  return baselineMicroSecs / x;
}

///////////////////////////////////////////////////////////////////////////////
