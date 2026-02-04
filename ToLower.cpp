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
#include <thread>
#include <utility>
#include <vector>
#include "LowerCaseAsciiTable.h"

///////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
#define test(e) assert(e)
#else
#define test(e) static_cast<void>( (e) || ( __debugbreak(), 0 ) )
#endif

#include "lowerOriginal.h"
#include "toLowerPtrNoStrlen.h"
#include "toLowerSpan.h"
#include "toLowerStrRef.h"
#include "toLowerStrView.h"
#include "toLowerAutoNoReturn.h"
#include "toLowerTemplatized.h"
#include "toLowerAuto.h"
#include "toLowerAutoConstexpr.h"
#include "toLowerExecPolicy.h"
#include "toLower5thBit.h"
#include "toLowerLookupTable.h"
#include "toLowerThreads.h"
#include "toLowerD.h"

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
  std::vector<std::jthread> threadPool( std::max( std::thread::hardware_concurrency(), 1u ) );

  test( std::ranges::equal( kLowerCaseAsciiTable, kLowerCaseAsciiTableHardCoded ) );

  // Original
  char a1[] = "AbCdEf";
  char a2[] = "AbCdEf";
  VerifyEqual( lowerOriginal( a1 ), "abcdef" );
  std::cout << lowerOriginal( a2 ) << '\n';
  // lower0( nullptr ); // crash!

  // PtrNoStrlen
  char b1[] = "AbCdEf";
  char b2[] = "AbCdEf";
  VerifyEqual( toLowerPtrNoStrlen( b1, strlen( b1 ) ), "abcdef");
  std::cout << toLowerPtrNoStrlen( b2, strlen( b2 ) ) << '\n';
  toLowerPtrNoStrlen( nullptr, 0 ); // safe

  // Span
  char c1[] = "AbCdEf";
  char c2[] = "AbCdEf";
  VerifyEqual( toLowerSpan( std::span{ c1 } ), "abcdef" );
  std::cout << toLowerSpan( c2 ) << '\n';
  toLowerSpan( std::span<char>() ); // safe

  // StrRef
  std::string d1{ "AbCdEf" };
  std::string d2{ "AbCdEf" };
  VerifyEqual( toLowerStrRef( d1 ), "abcdef" );
  std::cout << toLowerStrRef( d2 ) << '\n';

  // StrView -- doesn't work; can't modify string_view
  //std::string_view e1{ "AbCdEf" };
  //std::string_view e2{ "AbCdEf" };
  //VerifyEqual( toLowerStrView( e1 ), "abcdef" ); // fails; can't modify string_view
  //std::cout << toLowerStrView( e2 ) << '\n'; // mixed case output

  // AutoNoReturn
  std::string_view f1{ "AbCdEf" };
  std::span f2{ "AbCdEf" };
  char f3[] = "AbCdEf";
  char f4[] = "AbCdEf";
  std::span f5{ f4 };
  std::string f6{ "AbCdEf" };

  toLowerAutoNoReturn( f3 );
  VerifyEqual( f3, "abcdef" );
  toLowerAutoNoReturn( f5 );
  VerifyEqual( std::string_view{ f5.data(), f5.size()-1 }, "abcdef" );
  std::cout << f5 << '\n';
  toLowerAutoNoReturn( f6 );
  VerifyEqual( f6, "abcdef" );
  std::cout << f6 << '\n';
  // std::cout << toLowerAutoNoReturn( f6 ) << '\n'; // compiler error; toLowerAutoNoReturn doesn't return anything

  // Templatized
  char g3[] = "AbCdEf";
  char g4[] = "AbCdEf";
  std::span g5{ g4 };
  std::string g6{ "AbCdEf" };

  VerifyEqual( toLowerTemplatized( g3 ), "abcdef");
  toLowerTemplatized( g5 );
  VerifyEqual( std::string_view{ g5.data(), g5.size()-1 }, "abcdef" );
  std::cout << g5 << '\n';
  std::cout << toLowerTemplatized( g6 ) << '\n';

  // LowerAuto
  char g3a[] = "AbCdEf";
  char g4a[] = "AbCdEf";
  std::span g5a{ g4a };
  std::string g6a{ "AbCdEf" };

  VerifyEqual( toLowerAuto( g3a ), "abcdef" );
  toLowerAuto( g5a );
  VerifyEqual( std::string_view{ g5a.data(), g5a.size() - 1 }, "abcdef" );
  std::cout << g5a << '\n';
  std::cout << toLowerAuto( g6a ) << '\n';

  // AutoConstexpr
  char g3ab[] = "AbCdEf";
  char g4ab[] = "AbCdEf";
  std::span g5ab{ g4ab };
  std::string g6ab{ "AbCdEf" };

  VerifyEqual( toLowerAutoConstexpr( g3ab ), "abcdef" );
  toLowerAutoConstexpr( g5ab );
  VerifyEqual( std::string_view{ g5ab.data(), g5ab.size() - 1 }, "abcdef" );
  std::cout << g5ab << '\n';
  std::cout << toLowerAutoConstexpr( g6ab ) << '\n';

  // ExecPolicy
  char j3[] = "AbCdEf";
  char j4[] = "AbCdEf";
  std::span j5{ j4 };
  std::string j6{ "AbCdEf" };

  VerifyEqual( toLowerExecPolicy( j3, std::execution::par_unseq ), "abcdef" );
  toLowerExecPolicy( j5, std::execution::par_unseq );
  VerifyEqual( std::string_view{ j5.data(), j5.size()-1 }, "abcdef" );
  std::cout << j5 << '\n';
  std::cout << toLowerExecPolicy( j6, std::execution::par_unseq ) << '\n';

  // 5thBit
  std::string k1{ "AbCdEf" };
  std::string k2{ "AbCdEf" };
  VerifyEqual( toLower5thBit( k1 ), "abcdef" );
  std::cout << toLower5thBit( k2 ) << '\n';

  // LookupTable
  std::string m1{ "AbCdEf" };
  std::string m2{ "AbCdEf" };
  toLowerLookupTable( m1 );
  VerifyEqual( m1, "abcdef" );
  std::cout << toLowerLookupTable(m2) << '\n';

  // Threads
  std::string n1{ "AbCdEf" };
  std::string n2{ "AbCdEf" };
  VerifyEqual( toLowerThreads( n1 ), "abcdef" );
  std::cout << toLowerThreads( n2 ) << '\n';

  // toLowerD
  std::string n1a{ "AbCdEf" };
  std::string n2a{ "AbCdEf" };
  VerifyEqual( toLowerD( n1a, threadPool ), "abcdef" );
  std::cout << toLowerD( n2a, threadPool ) << '\n';

  // lowerc -> toLowerE
  std::string p1{ "ABCdEfGhIjKlMnOpQrStUvWxYz" };
  std::string p2{ "ABCdEfGhIjKlMnOpQrStUvWxYz" };
  VerifyEqual( lowerc( p1 ), "abcdefghijklmnopqrstuvwxyz" );
  std::cout << lowerc( p2 ) << '\n';

  // lowerd -> toLowerF
  std::string q1{ "ABCdEfGhIjKlMnOpQrStUvWxYzABCdEfGhIjKlMnOpQrStUvWxYz" };
  std::string q2{ "ABCdEfGhIjKlMnOpQrStUvWxYzABCdEfGhIjKlMnOpQrStUvWxYz" };
  //VerifyEqual( lowerd( q1 ), "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz" );
  //std::cout << lowerd( q2 ) << '\n';

  // lowere -> toLowerG
  std::string r1{ "ABCdEfGhIjKlMnOpQrStUvWxYzABCdEfGhIjKlMnOpQrStUvWxYz" };
  std::string r2{ "ABCdEfGhIjKlMnOpQrStUvWxYzABCdEfGhIjKlMnOpQrStUvWxYz" };
  //VerifyEqual( lowere( r1 ), "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz" );
  //std::cout << lowere( r2 ) << '\n';

  // lowerf -> toLowerH
  std::string s1{ "ABCdEfGhIjKlMnOpQrStUvWxYzABCdEfGhIjKlMnOpQrStUvWxYz" };
  std::string s2{ "ABCdEfGhIjKlMnOpQrStUvWxYzABCdEfGhIjKlMnOpQrStUvWxYz" };
  //VerifyEqual( lowerf( s1 ), "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz" );
  //std::cout << lowerf( s2 ) << '\n';

  // lowerg -> toLowerI
  std::string t1{ "ABCdEfGhIjKlMnOpQrStUvWxYzABCdEfGhIjKlMnOpQrStUvWxYz" };
  std::string t2{ "ABCdEfGhIjKlMnOpQrStUvWxYzABCdEfGhIjKlMnOpQrStUvWxYz" };
  //VerifyEqual( lowerg( t1 ), "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz" );
  //std::cout << lowerg( t2 ) << '\n';

  // multithreaded SIMD
  // CUDA, Vulkan, Halide
  // https://lemire.me/blog/2024/08/03/converting-ascii-strings-to-lower-case-at-crazy-speeds-with-avx-512/

  // Bit 5 trick visualization
  std::string z1;
  z1.resize( 127uz - ' ' );
  std::ranges::iota( z1, ' ' );
  std::cout << z1 << '\n';
  toLower5thBit( z1 );
  std::cout << z1 << '\n';

  //if (z1.size()) return 0; TODO

  // Timing runs
  std::string rs = getRandomStr( 1024uz * 1024uz * 10uz ); // 10MB

  { // Original
    std::string tmp = rs;
    tmp.resize( tmp.size() / 100 ); // temporary TODO
    Timer timer;
    // Extremely slow because of repeated calls to strlen
    lowerOriginal( tmp.data() );
    std::cout << "Original/100: " << timer.getElapsedMicroseconds() << " microSec\n";
    //Verify( rs, tmp );
  }

  { // PtrNoStrlen
    std::string tmp = rs;
    Timer timer;
    toLowerPtrNoStrlen( tmp.data(), tmp.size() );
    std::cout << "PtrNoStrlen: " << timer.getElapsedMicroseconds() << " microSec\n";
    Verify( rs, tmp );
  }

  MEASURE( "Span", toLowerSpan );
  MEASURE( "StrRef", toLowerStrRef );
  //MEASURE( "StrView", toLowerStrView ); // invalid, so no need to measure
  MEASURE( "AutoNoReturn", toLowerAutoNoReturn );
  MEASURE( "Templatized", toLowerTemplatized );
  MEASURE( "Auto", toLowerAuto );
  MEASURE( "AutoConstexpr", toLowerAutoConstexpr );
  MEASURE1( "ExecPolicy par", toLowerExecPolicy, std::execution::par );
  MEASURE1( "ExecPolicy par_unseq", toLowerExecPolicy, std::execution::par_unseq );
  MEASURE( "5thBit", toLower5thBit );
  MEASURE( "LookupTable", toLowerLookupTable );
  MEASURE( "Threads", toLowerThreads );
  MEASURE1( "toLowerD", toLowerD, threadPool );
  MEASURE( "lowerc", lowerc );
  MEASURE( "lowerd", lowerd );
  MEASURE( "lowere", lowere );
  MEASURE( "lowerf", lowerf );
  MEASURE_BYVAL( "lowerg", lowerg );
}
