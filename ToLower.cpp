///////////////////////////////////////////////////////////////////////////////
//
//  ToLower.cpp
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
//
///////////////////////////////////////////////////////////////////////////////

#include <cassert>
#include <chrono>
#include <clocale>
#include <execution>
#include <functional>
#include <iostream>
#include <span>
#include <string>
#include <utility>

///////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
#define test(e) assert(e)
#else
#define test(e) static_cast<void>( (e) || ( __debugbreak(), 0 ) )
#endif

#include   "lower0.h"
#include   "lower1.h"
#include   "lower2.h"
#include "toLower3.h"
#include "toLower4.h"
#include "toLower5.h"
#include "toLower6.h"
#include "toLower7.h"
#include "toLower8.h"
#include "toLower9.h"
#include "toLowerA.h"
#include "lowera.h"

std::string& lowerb( std::string& ) noexcept; // lowerb.cpp
std::string& lowerc( std::string& );          // lowerc.cpp
std::string& lowerd( std::string& );          // lowerd.cpp
std::string& lowere( std::string& );          // lowere.cpp
std::string& lowerf( std::string& );          // lowerf.cpp
std::string lowerg( const std::string& ) noexcept; // lowerg.cpp

std::string getRandomStr( size_t ); // RandomStr.cpp

///////////////////////////////////////////////////////////////////////////////

using namespace std::chrono;
class Timer
{
public:
  Timer() : start_( high_resolution_clock::now() ) {
  }
  double getElapsedMicroseconds() const {
    auto end = high_resolution_clock::now();
    duration<double, std::micro> duration = end - start_;
    return duration.count();
  }
private:
  time_point<high_resolution_clock> start_;
};

///////////////////////////////////////////////////////////////////////////////

template <typename T, size_t N>
std::ostream& operator<<( std::ostream& os, const std::span<T,N>& sp )
{
  for( T c : sp )
    os << c;
  return os;
}

static void VerifyEqual( std::string_view lhs, std::string_view rhs )
{
  test( std::ranges::equal( lhs, rhs ) );
}

static void Verify( std::string_view original, std::string_view lower )
{
  test( original.size() == lower.size() );
  auto orig = std::begin( original );
  for( auto low : lower )
  {
    // If original was upper case, verify that conversion happened and that inverse is also true
    if( std::isupper( static_cast<unsigned char>(*orig) ) )
    {
      auto toLower = std::tolower( static_cast<unsigned char>( *orig ) );
      test( low == toLower );
      auto toUpper = std::toupper( static_cast<unsigned char>( low ) );
      test( toUpper == *orig );
    }
    else
      test( low = *orig ); // should be unchanged
    ++orig;
  }
}

#define MEASURE( name, function ) \
{ \
  std::string tmp = rs; \
  Timer timer; \
  function( tmp ); \
  std::cout << name << ": " << timer.getElapsedMicroseconds() << " microSec\n"; \
  Verify( rs, tmp ); \
}

#define MEASURE1( name, function, arg1 ) \
{ \
  std::string tmp = rs; \
  Timer timer; \
  function( tmp, arg1 ); \
  std::cout << name << ": " << timer.getElapsedMicroseconds() << " microSec\n"; \
  Verify( rs, tmp ); \
}

#define MEASURE_BYVAL( name, function ) \
{ \
  Timer timer; \
  std::string tmp = function( rs ); \
  std::cout << name << ": " << timer.getElapsedMicroseconds() << " microSec\n"; \
  Verify( tmp, rs ); \
}

///////////////////////////////////////////////////////////////////////////////

int main()
{
  // lower0
  char a1[] = "AbCdEf";
  char a2[] = "AbCdEf";
  VerifyEqual( lower0( a1 ), "abcdef" );
  std::cout << lower0( a2 ) << '\n';
  // lower0( nullptr ); // crash!

  // lower1
  char b1[] = "AbCdEf";
  char b2[] = "AbCdEf";
  VerifyEqual( lower1( b1, strlen( b1 ) ), "abcdef");
  std::cout << lower1( b2, strlen( b2 ) ) << '\n';
  lower1( nullptr, 0 ); // safe

  // lower2
  char c1[] = "AbCdEf";
  char c2[] = "AbCdEf";
  VerifyEqual( lower2( std::span{ c1 } ), "abcdef" );
  std::cout << lower2( c2 ) << '\n';
  lower2( std::span<char>() ); // safe

  // toLower3
  std::string d1{ "AbCdEf" };
  std::string d2{ "AbCdEf" };
  VerifyEqual( toLower3( d1 ), "abcdef" );
  std::cout << toLower3( d2 ) << '\n';

  // toLower4
  std::string_view e1{ "AbCdEf" };
  std::string_view e2{ "AbCdEf" };
  //VerifyEqual( toLower4( e1 ), "abcdef" ); // nope; can't modify string_view
  //std::cout << toLower4( e2 ) << '\n'; // mixed case output

  // toLower5
  std::string_view f1{ "AbCdEf" };
  std::span f2{ "AbCdEf" };
  char f3[] = "AbCdEf";
  char f4[] = "AbCdEf";
  std::span f5{ f4 };
  std::string f6{ "AbCdEf" };

  // toLower5( f1 ); // compiler error; can't modify string_view (it's a view!)
  // toLower5( f2 ); // compiler error; f2 is const char*
  toLower5( f3 );
  VerifyEqual( f3, "abcdef" );
  toLower5( f5 );
  VerifyEqual( std::string_view{ f5.data(), f5.size()-1 }, "abcdef" );
  std::cout << f5 << '\n';
  toLower5( f6 );
  VerifyEqual( f6, "abcdef" );
  std::cout << f6 << '\n';
  // std::cout << toLower5( f6 ) << '\n'; // compiler error; toLower5 doesn't return anything

  // toLower6
  char g3[] = "AbCdEf";
  char g4[] = "AbCdEf";
  std::span g5{ g4 };
  std::string g6{ "AbCdEf" };

  VerifyEqual( toLower6( g3 ), "abcdef");
  toLower6( g5 );
  VerifyEqual( std::string_view{ g5.data(), g5.size()-1 }, "abcdef" );
  std::cout << g5 << '\n';
  std::cout << toLower6( g6 ) << '\n';

  // toLower7
  char g3a[] = "AbCdEf";
  char g4a[] = "AbCdEf";
  std::span g5a{ g4a };
  std::string g6a{ "AbCdEf" };

  VerifyEqual( toLower7( g3a ), "abcdef" );
  toLower7( g5a );
  VerifyEqual( std::string_view{ g5a.data(), g5a.size() - 1 }, "abcdef" );
  std::cout << g5a << '\n';
  std::cout << toLower7( g6a ) << '\n';

  // toLower8
  char g3ab[] = "AbCdEf";
  char g4ab[] = "AbCdEf";
  std::span g5ab{ g4ab };
  std::string g6ab{ "AbCdEf" };

  VerifyEqual( toLower8( g3ab ), "abcdef" );
  toLower8( g5ab );
  VerifyEqual( std::string_view{ g5ab.data(), g5ab.size() - 1 }, "abcdef" );
  std::cout << g5ab << '\n';
  std::cout << toLower8( g6ab ) << '\n';

  // toLower9
  char j3[] = "AbCdEf";
  char j4[] = "AbCdEf";
  std::span j5{ j4 };
  std::string j6{ "AbCdEf" };

  VerifyEqual( toLower9( j3, std::execution::par_unseq ), "abcdef" );
  toLower9( j5, std::execution::par_unseq );
  VerifyEqual( std::string_view{ j5.data(), j5.size()-1 }, "abcdef" );
  std::cout << j5 << '\n';
  std::cout << toLower9( j6, std::execution::par_unseq ) << '\n';

  // toLowerA
  std::string k1{ "AbCdEf" };
  std::string k2{ "AbCdEf" };
  VerifyEqual( toLowerA( k1 ), "abcdef" );
  std::cout << toLowerA( k2 ) << '\n';

  // toLowerB
  std::string m1{ "AbCdEf" };
  std::string m2{ "AbCdEf" };
  toLowerB( m1 );
  VerifyEqual( m1, "abcdef" );
  std::cout << toLowerB(m2) << '\n';

  // lowerb
  std::string n1{ "AbCdEf" };
  std::string n2{ "AbCdEf" };
  VerifyEqual( lowerb( n1 ), "abcdef" );
  std::cout << lowerb( n2 ) << '\n';

  // lowerc
  std::string p1{ "ABCdEfGhIjKlMnOpQrStUvWxYz" };
  std::string p2{ "ABCdEfGhIjKlMnOpQrStUvWxYz" };
  VerifyEqual( lowerc( p1 ), "abcdefghijklmnopqrstuvwxyz" );
  std::cout << lowerc( p2 ) << '\n';

  // lowerd
  std::string q1{ "ABCdEfGhIjKlMnOpQrStUvWxYzABCdEfGhIjKlMnOpQrStUvWxYz" };
  std::string q2{ "ABCdEfGhIjKlMnOpQrStUvWxYzABCdEfGhIjKlMnOpQrStUvWxYz" };
  VerifyEqual( lowerd( q1 ), "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz" );
  std::cout << lowerd( q2 ) << '\n';

  // lowere
  std::string r1{ "ABCdEfGhIjKlMnOpQrStUvWxYzABCdEfGhIjKlMnOpQrStUvWxYz" };
  std::string r2{ "ABCdEfGhIjKlMnOpQrStUvWxYzABCdEfGhIjKlMnOpQrStUvWxYz" };
  VerifyEqual( lowere( r1 ), "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz" );
  std::cout << lowere( r2 ) << '\n';

  // lowerf
  std::string s1{ "ABCdEfGhIjKlMnOpQrStUvWxYzABCdEfGhIjKlMnOpQrStUvWxYz" };
  std::string s2{ "ABCdEfGhIjKlMnOpQrStUvWxYzABCdEfGhIjKlMnOpQrStUvWxYz" };
  VerifyEqual( lowerf( s1 ), "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz" );
  std::cout << lowerf( s2 ) << '\n';

  // lowerg
  std::string t1{ "ABCdEfGhIjKlMnOpQrStUvWxYzABCdEfGhIjKlMnOpQrStUvWxYz" };
  std::string t2{ "ABCdEfGhIjKlMnOpQrStUvWxYzABCdEfGhIjKlMnOpQrStUvWxYz" };
  VerifyEqual( lowerg( t1 ), "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz" );
  std::cout << lowerg( t2 ) << '\n';

  // multithreaded SIMD
  // CUDA, Vulkan, Halide
  // https://lemire.me/blog/2024/08/03/converting-ascii-strings-to-lower-case-at-crazy-speeds-with-avx-512/

  // Bit 5 trick visualization
  std::string z1;
  z1.resize( 127uz - ' ' );
  std::ranges::iota( z1, ' ' );
  std::cout << z1 << '\n';
  toLowerA( z1 );
  std::cout << z1 << '\n';
  std::string underscore{ "_" };
  toLowerA( underscore );

  // Timing runs
  std::string rs = getRandomStr( 1024uz * 1024uz * 10uz ); // 10MB

  { // lower0
    std::string tmp = rs;
    Timer timer;
    // This is super slow because of repeated calls to strlen; commented out for now
    //lower0( tmp.data() );
    //std::cout << "lower0: NOT IMPLEMENTED " << timer.getElapsedMicroseconds() << " microSec\n";
    //Verify( rs, tmp );
  }

  { // lower1
    std::string tmp = rs;
    Timer timer;
    lower1( tmp.data(), tmp.size() );
    std::cout << "lower1: " << timer.getElapsedMicroseconds() << " microSec\n";
    Verify( rs, tmp );
  }

  MEASURE( "lower2", lower2 );
  MEASURE( "toLower3", toLower3 );
  //MEASURE( "toLower4", toLower4 ); // doesn't do anything, so no need to measure
  MEASURE( "toLower5", toLower5 );
  MEASURE( "toLower6", toLower6 );
  MEASURE( "toLower7", toLower7 );
  MEASURE( "toLower8", toLower8 );
  MEASURE1( "toLower9", toLower9, std::execution::par_unseq );
  MEASURE( "toLowerA", toLowerA );
  MEASURE( "toLowerB", toLowerB );
  MEASURE( "lowerb", lowerb );
  MEASURE( "lowerc", lowerc );
  MEASURE( "lowerd", lowerd );
  MEASURE( "lowere", lowere );
  MEASURE( "lowerf", lowerf );
  MEASURE_BYVAL( "lowerg", lowerg );
}
