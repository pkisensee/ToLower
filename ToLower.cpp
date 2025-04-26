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
// #include <debugging> requires C++26
#include <execution>
#include <functional>
#include <iostream>
#include <span>
#include <string>

///////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
#define test(e) assert(e)
#else
// #define test(e) static_cast<void>( (e) || ( std::breakpoint(), 0 ) ) // requires C++26
#define test(e) static_cast<void>( (e) || ( __debugbreak(), 0 ) )
#endif

#include "lower.h"                // lower.h (4-8)

char* lower0( char* );            // lower0.cpp
void lower1( char*, size_t );     // lower1.cpp
void lower2( std::span<char> );   // lower2.cpp
void lower3( std::string& );      // lower3.cpp
void lower4( std::string_view );  // lower4.cpp
void lower9( std::string& );      // lower9.cpp
void lowera( std::string& );      // lowera.cpp
void lowerb( std::string& );      // lowerb.cpp
void lowerc( std::string& );      // lowerc.cpp
void lowerd( std::string& );      // lowerd.cpp

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

void Verify( std::string_view original, std::string_view lower )
{
  test( original.size() == lower.size() );
  auto orig = std::begin( original );
  for( auto low : lower )
  {
    // If original was upper case, verify that conversion happened and that inverse is also true
    if( std::isupper( *orig ) )
    {
      test( low == std::tolower( *orig ) );
      test( std::toupper( low ) == *orig );
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

///////////////////////////////////////////////////////////////////////////////

int main()
{
  // lower0
  char a1[] = "AbCdEf";
  char a2[] = "AbCdEf";
  Verify( a1, "abcdef" );
  std::cout << lower0( a2 ) << '\n';
  // lower0( nullptr ); // crash!

  // lower1
  char b1[] = "AbCdEf";
  char b2[] = "AbCdEf";
  lower1( b1, strlen( b1 ) );
  Verify( b1, "abcdef" );
  lower1( b2, strlen( b2 ) );
  std::cout << b2 << '\n';
  lower1( nullptr, 0 ); // safe

  // lower2
  char c1[] = "AbCdEf";
  char c2[] = "AbCdEf";
  lower2( std::span{ c1 } );
  Verify( c1, "abcdef" );
  lower2( c2 ); // note: c2 automatically converted to span
  std::cout << b2 << '\n';
  lower2( std::span<char>() ); // safe

  // lower3
  std::string d1{ "AbCdEf" };
  std::string d2{ "AbCdEf" };
  lower3( d1 );
  Verify( d1, "abcdef" );
  lower3( d2 );
  std::cout << d2 << '\n';

  // lower4
  std::string_view e1{ "AbCdEf" };
  std::string_view e2{ "AbCdEf" };
  lower4( e1 );
  //Verify( e1, "abcdef" );
  lower4( e2 );
  //std::cout << e2 << '\n';

  // lower5
  std::string_view f1{ "AbCdEf" };
  std::span f2{ "AbCdEf" };
  char f3[] = "AbCdEf";
  char f4[] = "AbCdEf";
  std::span f5{ f4 };
  std::string f6{ "AbCdEf" };

  //lower5( f1 ); // compiler error; can't modify string_view (it's a view!)
  //lower5( f2 ); // compiler error; f2 is const char*
  lower5( f3 );
  Verify( f3, "abcdef" );
  lower5( f5 );
  Verify( std::string_view{ f5.data(), f5.size()-1 }, "abcdef" );
  std::cout << f5 << '\n';
  lower5( f6 );
  Verify( f6, "abcdef" );
  std::cout << f6 << '\n';
  // std::cout << lower5( f6 ) << '\n'; // compiler error; lower5 doesn't return anything

  // lower6
  char g3[] = "AbCdEf";
  char g4[] = "AbCdEf";
  std::span g5{ g4 };
  std::string g6{ "AbCdEf" };

  lower6( g3 );
  Verify( g3, "abcdef" );
  lower6( g5 );
  Verify( std::string_view{ g5.data(), g5.size()-1 }, "abcdef" );
  std::cout << g5 << '\n';
  std::cout << lower6( g6 ) << '\n'; // OK, this works

  // lower7
  char h3[] = "AbCdEf";
  char h4[] = "AbCdEf";
  std::span h5{ h4 };
  std::string h6{ "AbCdEf" };

  lower7( h3 );
  Verify( h3, "abcdef" );
  lower7( h5 );
  Verify( std::string_view{ h5.data(), h5.size()-1 }, "abcdef" );
  std::cout << h5 << '\n';
  std::cout << lower7( h6 ) << '\n';

  // lower8
  char j3[] = "AbCdEf";
  char j4[] = "AbCdEf";
  std::span j5{ j4 };
  std::string j6{ "AbCdEf" };

  lower8( j3, std::execution::par_unseq );
  Verify( j3, "abcdef" );
  lower8( j5, std::execution::par_unseq );
  Verify( std::string_view{ j5.data(), j5.size()-1 }, "abcdef" );
  std::cout << j5 << '\n';
  std::cout << lower8( j6, std::execution::par_unseq ) << '\n';

  // lower9
  std::string k1{ "AbCdEf" };
  std::string k2{ "AbCdEf" };
  lower9( k1 );
  Verify( k1, "abcdef" );
  lower9( k2 );
  std::cout << k2 << '\n';

  // lowera
  std::string m1{ "AbCdEf" };
  std::string m2{ "AbCdEf" };
  lowera( m1 );
  Verify( m1, "abcdef" );
  lowera( m2 );
  std::cout << m2 << '\n';

  // lowerb
  std::string n1{ "AbCdEf" };
  std::string n2{ "AbCdEf" };
  lowerb( n1 );
  Verify( n1, "abcdef" );
  lowerb( n2 );
  std::cout << n2 << '\n';

  // lowerc
  std::string p1{ "ABCdEfGhIjKlMnOpQrStUvWxYz" };
  std::string p2{ "ABCdEfGhIjKlMnOpQrStUvWxYz" };
  lowerc( p1 );
  Verify( p1, "abcdefghijklmnopqrstuvwxyz" );
  lowerc( p2 );
  std::cout << p2 << '\n';

  // B)
  std::string q1{ "ABCdEfGhIjKlMnOpQrStUvWxYzABCdEfGhIjKlMnOpQrStUvWxYz" };
  std::string q2{ "ABCdEfGhIjKlMnOpQrStUvWxYzABCdEfGhIjKlMnOpQrStUvWxYz" };
  lowerd( q1 );
  Verify( q1, "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz" );
  lowerd( q2 );
  std::cout << q2 << '\n';

  // C multithreaded SIMD
  // D CUDA or GPU solutions
  // https://lemire.me/blog/2024/08/03/converting-ascii-strings-to-lower-case-at-crazy-speeds-with-avx-512/

  // Bit 5 trick visualization
  std::string z1;
  z1.resize( 127 - ' ' );
  std::ranges::iota( z1, ' ' );
  std::cout << z1 << '\n';
  lower9( z1 );
  std::cout << z1 << '\n';

  // Timing runs
  std::string rs = getRandomStr( 1024 * 1024 * 10 ); // 10MB

  { // lower0
    std::string tmp = rs;
    Timer timer;
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
  MEASURE( "lower3", lower3 );
  // MEASURE( "lower4", lower4 ); // lower4 uses string_view, which can't be modified
  MEASURE( "lower5", lower5 );
  MEASURE( "lower6", lower6 );
  MEASURE( "lower7", lower7 );
  MEASURE1( "lower8", lower8, std::execution::par_unseq );
  MEASURE( "lower9", lower9 );
  MEASURE( "lowera", lowera );
  MEASURE( "lowerb", lowerb );
  MEASURE( "lowerc", lowerc );
  MEASURE( "lowerd", lowerd );

}
