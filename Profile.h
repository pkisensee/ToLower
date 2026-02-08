///////////////////////////////////////////////////////////////////////////////
//
// Profile.h
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

class OpenCLContext;

class Profile
{
public:

  using ParPolicy = std::execution::parallel_policy;
  using ParUnseqPolicy = std::execution::parallel_unsequenced_policy;
  using ThreadPoolType = std::vector<std::jthread>;

  Profile( std::string_view, char* ( *func )( char* ), double = 1.0 );
  Profile( std::string_view, char* ( *func )( char*, size_t ), double = 1.0 );
  Profile( std::string_view, char* ( *func )( std::span<char> ), double = 1.0 );
  Profile( std::string_view, void ( *func )( std::string& ), double = 1.0 );
  Profile( std::string_view, std::string( *func )( std::string& ), double = 1.0 );
  Profile( std::string_view, std::string& ( *func )( std::string& ), double = 1.0 );
  Profile( std::string_view, std::string& ( *func )( std::string&, ThreadPoolType& ), ThreadPoolType* );
  Profile( std::string_view, std::string& ( *func )( std::string&, OpenCLContext&, std::string_view ),
           std::string_view kernel, OpenCLContext* );

  template <typename Policy>
  Profile( std::string_view name, std::string& ( *func )( std::string&, const Policy& ), const Policy& policy ) :
    name_( name ),
    func_( func )
  {
    if constexpr ( std::is_same_v<Policy, ParPolicy> )
      parPolicy_ = policy;
    else
      parunseqPolicy_ = policy;
  }

  Profile( const Profile& ) = default; // for storage in std::vector

  Profile() = delete;
  Profile& operator=( const Profile& ) = delete;
  Profile& operator=( Profile&& ) = delete;

  void Validate() const; // Validate that the function does the correct thing
  void Measure() const; // Record the time required to run the given function

  static std::ofstream outputFile; // Optional output file for storing benchmarks

private:

  void Validate( std::string& input, const std::string& expectedOutput ) const;
  std::string GetTestInput() const;
  void InvokeFunction( std::string& ) const;

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

  using FunctionVariant = std::variant<
    std::function< char*        ( char* )>,           // e.g. LowerOriginal
    std::function< char*        ( char*, size_t )>,   // e.g. ToLowerPtrNoStrlen
    std::function< char*        ( std::span<char> )>, // e.g. ToLowerSpan
    std::function< void         ( std::string& )>,    // e.g. ToLowerAutoNoReturn
    std::function< std::string  ( std::string& )>,    // e.g. GetLowerByValue
    std::function< std::string& ( std::string& )>,    // e.g. ToLowerLookupTable
    std::function< std::string& ( std::string&, const ParPolicy& )>, // e.g. ToLowerExecPolicy
    std::function< std::string& ( std::string&, const ParUnseqPolicy& )>, // e.g. ToLowerExecPolicy
    std::function< std::string& ( std::string&, ThreadPoolType& )>, // e.g. ToLowerThreadPool
    std::function< std::string& ( std::string&, OpenCLContext&, std::string_view )> // e.g. ToLowerOpenCL
  >;

  std::string_view name_; // e.g. "LookupTable"
  FunctionVariant  func_; // e.g. ToLowerLookupTable
  double           scale_ = 1.0; // decrease for slow funcs; increase for fast funcs to improve Measure() speed

  // For std::algorithms with policies
  ParPolicy parPolicy_ = std::execution::par;
  ParUnseqPolicy parunseqPolicy_ = std::execution::par_unseq;

  ThreadPoolType* threadPool_ = nullptr; // For functions that accept a threadPool

  // For OpenCL functions
  std::string_view kernel_; // e.g. "lowerLUT"
  OpenCLContext* openCLContext_ = nullptr;

  static std::string testStr_; // Store a single test string in mixed case

}; // class Profile

///////////////////////////////////////////////////////////////////////////////
