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

#include <execution>
#include <functional>
#include <iostream>
#include <span>
#include <string>
#include <string_view>
#include <variant>

#include "getRandomStr.h"
#include "Stopwatch.h"

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

  Profile( std::string_view name, std::string& ( *func )( std::string& ), double scale = 1.0 ) :
    name_( name ),
    func_( std::function<std::string& ( std::string& )>( func ) ),
    scale_( scale )
  {
  }

  Profile( std::string_view name, std::string& ( *func )( std::string&, const std::execution::parallel_policy& ), const std::execution::parallel_policy& execPolicy ) :
    name_( name ),
    func_( std::function<std::string& ( std::string&, const std::execution::parallel_policy& )>( func ) ),
    scale_( 1.0 ),
    execPolicy_( execPolicy )
  {
  }

  Profile() = delete;
  Profile( const Profile& ) = delete;
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
      case( FunctionType::StrRef_StrRef ):
      {
        const auto& func = std::get<FunctionType::StrRef_StrRef>( func_ );
        stopwatch.Start();
        func( input );
        break;
      }
      case( FunctionType::StrRef_StrRefPolicy ):
      {
        const auto& func = std::get<FunctionType::StrRef_StrRefPolicy>( func_ );
        stopwatch.Start();
        func( input, execPolicy_ );
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
    std::string input = GetTestInput();
    // func_( tmp ); TODO
    // Verify( str_, tmp );
  }

private:
  std::string GetTestInput() const
  {
    if ( testStr_.size() != testStringSize_ )
      testStr_ = getRandomStr( testStringSize_ );

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
    StrRef_StrRef,
    StrRef_StrRefPolicy,
  };

  using FunctionVariant = std::variant<
    std::function< char*        ( char* )>,           // e.g. lowerOriginal
    std::function< char*        ( char*, size_t )>,   // e.g. toLowerPtrNoStrlen
    std::function< char*        ( std::span<char> )>, // e.g. toLowerSpan
    std::function< void         ( std::string& )>,    // e.g. toLowerAutoNoReturn
    std::function< std::string& ( std::string& )>,    // e.g. toLowerLookupTable
    std::function< std::string& ( std::string&, const std::execution::parallel_policy& )> // e.g. toLowerExecPolicy
  >;

  std::string_view name_; // e.g. "LookupTable"
  FunctionVariant  func_; // e.g. toLowerLookupTable
  double           scale_ = 1.0; // decrease for slow funcs; increase for fast funcs to improve overall speed
  std::execution::parallel_policy execPolicy_;

  static std::string testStr_;
  static size_t testStringSize_;

}; // class Profile

///////////////////////////////////////////////////////////////////////////////
