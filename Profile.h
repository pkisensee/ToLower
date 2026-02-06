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

#include <functional>
#include <iostream>
#include <string>
#include <string_view>
#include <variant>

#include "getRandomStr.h"
#include "Stopwatch.h"

class Profile
{
public:

  Profile( std::string_view name, const std::function<char* ( char* )>& func, double scale = 1.0 ) :
    name_( name ),
    func_( func ),
    scale_( scale )
  {
  }

  Profile( std::string_view name, const std::function<char* ( std::span<char> )>& func, double scale = 1.0 ) :
    name_( name ),
    func_( func ),
    scale_( scale )
  {
  }

  Profile( std::string_view name, const std::function<std::string& ( std::string& )>& func, double scale = 1.0 ) :
    name_( name ),
    func_( func ),
    scale_( scale )
  {
  }

  Profile() = delete;
  Profile( const Profile& ) = delete;
  Profile& operator=( const Profile& ) = delete;
  Profile& operator=( Profile&& ) = delete;

  void Measure() const
  {
    std::string input = GetTestInput();
    std::cout << name_ << ": ";

    // Start the clock
    Stopwatch stopwatch;
    switch ( func_.index() )
    {
      case( FunctionType::CharPtr_CharPtr ):
      {
        const auto& func = std::get<FunctionType::CharPtr_CharPtr>( func_ );
        func( input.data() );
        break;
      }
      case( FunctionType::CharPtr_SpanChar ):
      {
        const auto& func = std::get<FunctionType::CharPtr_SpanChar>( func_ );
        std::span span{ input };
        func( span );
        break;
      }
      case( FunctionType::StrRef_StrRef ):
      {
        const auto& func = std::get<FunctionType::StrRef_StrRef>( func_ );
        func( input );
        break;
      }
    }

    // Record the time
    double microSecs = stopwatch.GetElapsedMicroseconds();
    if ( scale_ != 0.0 )
      microSecs /= scale_;
    std::cout << microSecs << " microSec\n";
  }

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
    CharPtr_SpanChar,
    StrRef_StrRef,
  };

  using FunctionVariant = std::variant<
    std::function<char* ( char* )>,                   // e.g. lowerOriginal
    std::function<char* ( std::span<char> )>,         // e.g. toLowerSpan
    std::function< std::string& ( std::string& )>     // e.g. toLowerLookupTable
  >;

  std::string_view name_; // e.g. "LookupTable"
  FunctionVariant  func_; // e.g. toLowerLookupTable
  double           scale_ = 1.0; // decrease for slow funcs; increase for fast funcs to improve overall speed

  static std::string testStr_;
  static size_t testStringSize_;

}; // class Profile

///////////////////////////////////////////////////////////////////////////////
