///////////////////////////////////////////////////////////////////////////////
//
//  Profile.h
//
//  Information on functions to be profiled, with methods to validate the
//  functions and measure performance.
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

#pragma once
#include <cassert>
#include <execution>
#include <functional>
#include <iostream>
#include <thread>
#include <span>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

#include "getLowerByValue.h"
#include "toLower5thBit.h"
#include "getRandomStr.h"
#include "Stopwatch.h"

class OpenCLContext;

#ifdef _DEBUG
#define test(e) assert(e)
#else
#define test(e) static_cast<void>( (e) || ( __debugbreak(), 0 ) )
#endif

class Profile
{
public:

  Profile( std::string_view name, char* ( *func )( char* ), double scale = 1.0 ) :
    name_( name ),
    func_( std::function<char* ( char* )>( func ) ),
    scale_( scale )
  {
  }

  Profile( std::string_view name, char* ( *func )( char*, size_t ), double scale = 1.0 ) :
    name_( name ),
    func_( std::function<char* ( char*, size_t )>( func ) ),
    scale_( scale )
  {
  }

  Profile( std::string_view name, char* ( *func )( std::span<char> ), double scale = 1.0 ) :
    name_( name ),
    func_( std::function<char* ( std::span<char> )>( func ) ),
    scale_( scale )
  {
  }

  Profile( std::string_view name, void ( *func )( std::string& ), double scale = 1.0 ) :
    name_( name ),
    func_( std::function<void ( std::string& )>( func ) ),
    scale_( scale )
  {
  }

  Profile( std::string_view name, std::string ( *func )( std::string& ), double scale = 1.0 ) :
    name_( name ),
    func_( std::function<std::string ( std::string& )>( func ) ),
    scale_( scale )
  {
  }

  Profile( std::string_view name, std::string& ( *func )( std::string& ), double scale = 1.0 ) :
    name_( name ),
    func_( std::function<std::string& ( std::string& )>( func ) ),
    scale_( scale )
  {
  }

  Profile( std::string_view name, std::string& ( *func )( std::string&, const std::execution::parallel_policy& ), 
           const std::execution::parallel_policy& parPolicy ) :
    name_( name ),
    func_( std::function<std::string& ( std::string&, const std::execution::parallel_policy& )>( func ) ),
    scale_( 1.0 ),
    parPolicy_( parPolicy )
  {
  }

  Profile( std::string_view name, std::string& ( *func )( std::string&, const std::execution::parallel_unsequenced_policy& ),
    const std::execution::parallel_unsequenced_policy& parunseqPolicy ) :
    name_( name ),
    func_( std::function<std::string& ( std::string&, const std::execution::parallel_unsequenced_policy& )>( func ) ),
    scale_( 1.0 ),
    parunseqPolicy_( parunseqPolicy )
  {
  }

  Profile( std::string_view name, std::string& ( *func )( std::string&, std::vector<std::jthread>& ),
    std::vector<std::jthread>* threadPool ) :
    name_( name ),
    func_( std::function<std::string& ( std::string&, std::vector<std::jthread>& )>( func ) ),
    scale_( 1.0 ),
    threadPool_( threadPool )
  {
  }

  Profile( std::string_view name, std::string& ( *func )( std::string&, OpenCLContext&, std::string_view ),
    std::string_view kernel, OpenCLContext* openCLContext ) :
    name_( name ),
    func_( std::function<std::string& ( std::string&, OpenCLContext&, std::string_view )>(func)),
    scale_( 1.0 ),
    kernel_( kernel ),
    openCLContext_( openCLContext )
  {
  }

  Profile() = delete;
  Profile( const Profile& ) = default;
  Profile& operator=( const Profile& ) = delete;
  Profile& operator=( Profile&& ) = delete;

  // Record the time required to run the given function
  void Measure() const
  {
    std::string input = GetTestInput();
    std::cout << name_ << ": ";

    Stopwatch stopwatch;
    switch ( func_.index() )
    {
      case( FunctionType::CharPtr_CharPtr ):
      {
        const auto& func = std::get<FunctionType::CharPtr_CharPtr>( func_ );
        stopwatch.Start();
        func( input.data() );
        break;
      }
      case( FunctionType::CharPtr_CharPtrSizeT ):
      {
        const auto& func = std::get<FunctionType::CharPtr_CharPtrSizeT>( func_ );
        stopwatch.Start();
        func( input.data(), input.size() );
        break;
      }
      case( FunctionType::CharPtr_SpanChar ):
      {
        const auto& func = std::get<FunctionType::CharPtr_SpanChar>( func_ );
        std::span span{ input };
        stopwatch.Start();
        func( span );
        break;
      }
      case( FunctionType::Void_StrRef ):
      {
        const auto& func = std::get<FunctionType::Void_StrRef>( func_ );
        stopwatch.Start();
        func( input );
        break;
      }
      case( FunctionType::Str_StrRef ):
      {
        const auto& func = std::get<FunctionType::Str_StrRef>( func_ );
        stopwatch.Start();
        [[maybe_unused]] std::string result = func( input );
        break;
      }
      case( FunctionType::StrRef_StrRef ):
      {
        const auto& func = std::get<FunctionType::StrRef_StrRef>( func_ );
        stopwatch.Start();
        func( input );
        break;
      }
      case( FunctionType::StrRef_StrRefParPolicy ):
      {
        const auto& func = std::get<FunctionType::StrRef_StrRefParPolicy>( func_ );
        stopwatch.Start();
        func( input, parPolicy_ );
        break;
      }
      case( FunctionType::StrRef_StrRefParUnseqPolicy ):
      {
        const auto& func = std::get<FunctionType::StrRef_StrRefParUnseqPolicy>( func_ );
        stopwatch.Start();
        func( input, parunseqPolicy_ );
        break;
      }
      case( FunctionType::StrRef_StrRefThreadPool ):
      {
        const auto& func = std::get<FunctionType::StrRef_StrRefThreadPool>( func_ );
        stopwatch.Start();
        func( input, *threadPool_ );
        break;
      }
      case( FunctionType::StrRef_StrRefOpenCLContextStrView ):
      {
        const auto& func = std::get<FunctionType::StrRef_StrRefOpenCLContextStrView>( func_ );
        stopwatch.Start();
        func( input, *openCLContext_, kernel_ );
        break;
      }
    }

    // Record the time
    double microSecs = stopwatch.GetElapsedMicroseconds();
    if ( scale_ != 0.0 )
      microSecs /= scale_;
    std::cout << microSecs << " microSec\n";
  }

  // Validate that the function does the correct thing
  void Validate() const
  {
    std::cout << name_ << ": ";

    std::string empty;
    Validate( empty, empty );

    std::string simple      = "AbCdEfGhIJKlmnoPQRSTuVWxyz";
    std::string simpleLower = "abcdefghijklmnopqrstuvwxyz";
    Validate( simple, simpleLower );

    std::string random = GetTestInput();
    std::string randomLower;
    if ( name_.contains( "5thBit" ) )
    {
      randomLower = random;
      toLower5thBit( randomLower );
    }
    else
    {
      randomLower = getLowerByValue( random );
    }
    Validate( random, randomLower );
    std::cout << "Passed\n";
  }

  // Validate that the function converts input to expectedOutput
  void Validate( std::string& input, const std::string& expectedOutput ) const
  {
    switch ( func_.index() )
    {
      case( FunctionType::CharPtr_CharPtr ):
      {
        const auto& func = std::get<FunctionType::CharPtr_CharPtr>( func_ );
        func( input.data() );
        break;
      }
      case( FunctionType::CharPtr_CharPtrSizeT ):
      {
        const auto& func = std::get<FunctionType::CharPtr_CharPtrSizeT>( func_ );
        func( input.data(), input.size() );
        break;
      }
      case( FunctionType::CharPtr_SpanChar ):
      {
        const auto& func = std::get<FunctionType::CharPtr_SpanChar>( func_ );
        std::span span{ input };
        func( span );
        break;
      }
      case( FunctionType::Void_StrRef ):
      {
        const auto& func = std::get<FunctionType::Void_StrRef>( func_ );
        func( input );
        break;
      }
      case( FunctionType::Str_StrRef ):
      {
        const auto& func = std::get<FunctionType::Str_StrRef>( func_ );
        input = func( input );
        break;
      }
      case( FunctionType::StrRef_StrRef ):
      {
        const auto& func = std::get<FunctionType::StrRef_StrRef>( func_ );
        func( input );
        break;
      }
      case( FunctionType::StrRef_StrRefParPolicy ):
      {
        const auto& func = std::get<FunctionType::StrRef_StrRefParPolicy>( func_ );
        func( input, parPolicy_ );
        break;
      }
      case( FunctionType::StrRef_StrRefParUnseqPolicy ):
      {
        const auto& func = std::get<FunctionType::StrRef_StrRefParUnseqPolicy>( func_ );
        func( input, parunseqPolicy_ );
        break;
      }
      case( FunctionType::StrRef_StrRefThreadPool ):
      {
        const auto& func = std::get<FunctionType::StrRef_StrRefThreadPool>( func_ );
        func( input, *threadPool_ );
        break;
      }
      case( FunctionType::StrRef_StrRefOpenCLContextStrView ):
      {
        const auto& func = std::get<FunctionType::StrRef_StrRefOpenCLContextStrView>( func_ );
        func( input, *openCLContext_, kernel_ );
        break;
      }
    }

    test( std::ranges::equal( input, expectedOutput ) );
  }

private:
  std::string GetTestInput() const
  {
    // Create test string of appropriate size based on desired scale
    std::string input{ testStr_ };
    auto scaledSize = static_cast<double>( testStr_.size() ) * scale_;
    auto charCount = static_cast<size_t>( scaledSize );
    while ( input.size() < charCount )
      input += testStr_;
    input.resize( charCount );
    return input;
  }

private:
  enum FunctionType : size_t // Must match FunctionVariant order below
  {
    CharPtr_CharPtr = 0,
    CharPtr_CharPtrSizeT,
    CharPtr_SpanChar,
    Void_StrRef,
    Str_StrRef,
    StrRef_StrRef,
    StrRef_StrRefParPolicy,
    StrRef_StrRefParUnseqPolicy,
    StrRef_StrRefThreadPool,
    StrRef_StrRefOpenCLContextStrView,
  };

  using ParPolicy = std::execution::parallel_policy;
  using ParUnseqPolicy = std::execution::parallel_unsequenced_policy;
  using ThreadPoolType = std::vector<std::jthread>;

  using FunctionVariant = std::variant<
    std::function< char*        ( char* )>,           // e.g. lowerOriginal
    std::function< char*        ( char*, size_t )>,   // e.g. toLowerPtrNoStrlen
    std::function< char*        ( std::span<char> )>, // e.g. toLowerSpan
    std::function< void         ( std::string& )>,    // e.g. toLowerAutoNoReturn
    std::function< std::string  ( std::string& )>,    // e.g. getLowerByValue
    std::function< std::string& ( std::string& )>,    // e.g. toLowerLookupTable
    std::function< std::string& ( std::string&, const ParPolicy& )>, // e.g. toLowerExecPolicy
    std::function< std::string& ( std::string&, const ParUnseqPolicy& )>, // e.g. toLowerExecPolicy
    std::function< std::string& ( std::string&, ThreadPoolType& )>, // e.g. toLowerThreadPool
    std::function< std::string& ( std::string&, OpenCLContext&, std::string_view )> // e.g. toLowerOpenCL
  >;

  std::string_view name_; // e.g. "LookupTable"
  FunctionVariant  func_; // e.g. toLowerLookupTable
  double           scale_ = 1.0; // decrease for slow funcs; increase for fast funcs to improve overall speed

  // For std::algorithms with policies
  std::execution::parallel_policy parPolicy_ = std::execution::par;
  std::execution::parallel_unsequenced_policy parunseqPolicy_ = std::execution::par_unseq;

  // For functions that accept a threadPool
  ThreadPoolType* threadPool_ = nullptr;

  // For OpenCL functions
  std::string_view kernel_; // e.g. "lowerLUT"
  OpenCLContext* openCLContext_ = nullptr;

  // Store a single test string in mixed case
  static std::string testStr_;

}; // class Profile

///////////////////////////////////////////////////////////////////////////////
