///////////////////////////////////////////////////////////////////////////////
//
// Profile.cpp
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
#include <cassert>
#include <iostream>
#include "GetLowerByValue.h"
#include "ToLower5thBit.h"
#include "GetRandomStr.h"
#include "Profile.h"
#include "Stopwatch.h"

#ifdef _DEBUG
#define test(e) assert(e)
#else
#define test(e) static_cast<void>( (e) || ( __debugbreak(), 0 ) )
#endif

///////////////////////////////////////////////////////////////////////////////
//
// Constructors

Profile::Profile( std::string_view name, char* ( *func )( char* ), double scale ) :
  name_( name ),
  func_( std::function<char* ( char* )>( func ) ),
  scale_( scale )
{
}

Profile::Profile( std::string_view name, char* ( *func )( char*, size_t ), double scale ) :
  name_( name ),
  func_( std::function<char* ( char*, size_t )>( func ) ),
  scale_( scale )
{
}

Profile::Profile( std::string_view name, char* ( *func )( std::span<char> ), double scale ) :
  name_( name ),
  func_( std::function<char* ( std::span<char> )>( func ) ),
  scale_( scale )
{
}

Profile::Profile( std::string_view name, void ( *func )( std::string& ), double scale ) :
  name_( name ),
  func_( std::function<void( std::string& )>( func ) ),
  scale_( scale )
{
}

Profile::Profile( std::string_view name, std::string( *func )( std::string& ), double scale ) :
  name_( name ),
  func_( std::function<std::string( std::string& )>( func ) ),
  scale_( scale )
{
}

Profile::Profile( std::string_view name, std::string& ( *func )( std::string& ), double scale ) :
  name_( name ),
  func_( std::function<std::string& ( std::string& )>( func ) ),
  scale_( scale )
{
}

Profile::Profile( std::string_view name, std::string& ( *func )( std::string&, ThreadPoolType& ),
  ThreadPoolType* threadPool ) :
  name_( name ),
  func_( std::function<std::string& ( std::string&, std::vector<std::jthread>& )>( func ) ),
  threadPool_( threadPool )
{
}

Profile::Profile( std::string_view name, std::string& ( *func )( std::string&, OpenCLContext&, std::string_view ),
  std::string_view kernel, OpenCLContext* openCLContext ) :
  name_( name ),
  func_( std::function<std::string& ( std::string&, OpenCLContext&, std::string_view )>( func ) ),
  kernel_( kernel ),
  openCLContext_( openCLContext )
{
}

///////////////////////////////////////////////////////////////////////////////
// 
// Validate that the function does the correct thing

void Profile::Validate() const
{
  std::cout << name_ << ": ";

  // Test empty string
  std::string empty;
  Validate( empty, empty );

  // Test simple string
  std::string simple = "AbCdEfGhIJKlmnoPQRSTuVWxyz";
  std::string simpleLower = "abcdefghijklmnopqrstuvwxyz";
  Validate( simple, simpleLower );

  // Test random string
  std::string random = GetTestInput();
  std::string randomLower;
  if ( name_.contains( "5thBit" ) )
  {
    randomLower = random;
    ToLower5thBit( randomLower );
  }
  else
  {
    randomLower = GetLowerByValue( random );
  }
  Validate( random, randomLower );
  std::cout << "Passed\n";
}

///////////////////////////////////////////////////////////////////////////////
// 
// Record the time required to run the given function on large input

void Profile::Measure() const
{
  std::string input = GetTestInput();
  std::cout << name_ << ": ";
  Stopwatch stopwatch;
  InvokeFunction( input );
  double microSecs = stopwatch.GetElapsedMicroseconds();
  if ( scale_ != 0.0 )
    microSecs /= scale_;
  std::cout << microSecs << " microSec\n";
}

///////////////////////////////////////////////////////////////////////////////
// 
// Validate that the function converts input to expectedOutput

void Profile::Validate( std::string& input, const std::string& expectedOutput ) const // private
{
  InvokeFunction( input );
  test( std::ranges::equal( input, expectedOutput ) );
}

///////////////////////////////////////////////////////////////////////////////
// 
// Generate test string of appropriate size based on desired scale

std::string Profile::GetTestInput() const // private
{
  std::string input{ testStr_ };
  auto scaledSize = static_cast<double>( testStr_.size() ) * scale_;
  auto charCount = static_cast<size_t>( scaledSize );
  while ( input.size() < charCount )
    input += testStr_;
  input.resize( charCount );
  return input;
}

///////////////////////////////////////////////////////////////////////////////
// 
// Invoke the appropriate function. With std::visit and if constexpr, optimizing
// compiler can likely resolve this to a simple switch statement with
// minimal overhead.

void Profile::InvokeFunction( std::string& input ) const // private
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
    else if constexpr ( std::is_invocable_v<Func, std::string&, const ParPolicy&> )
      func( input, parPolicy_ );
    else if constexpr ( std::is_invocable_v<Func, std::string&, const ParUnseqPolicy&> )
      func( input, parunseqPolicy_ );
    else if constexpr ( std::is_invocable_v<Func, std::string&, ThreadPoolType&> )
      func( input, *threadPool_ );
    else if constexpr ( std::is_invocable_v<Func, std::string&, OpenCLContext&, std::string_view> )
      func( input, *openCLContext_, kernel_ );
    else if constexpr ( std::is_invocable_r_v<std::string, Func, std::string&> )
      input = func( input );
    else // void(std::string&) or std::string&(std::string&)
      func( input );
  };

  std::visit( visitorOp, func_ );
}

///////////////////////////////////////////////////////////////////////////////
