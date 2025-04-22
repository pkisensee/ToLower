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
#include <clocale>
// #include <debugging> requires C++26
#include <execution>
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

char* lower( char* );           // 1_lower.cpp
void lower( char*, size_t );    // 2_lower.cpp
void lower( std::span<char> );  // 3_lower.cpp
void lower( std::string& );     // 4_lower.cpp
void lower( std::string_view ); // 5_lower.cpp
#include "6_lower.h"            // 6_lower.h
void lowerb( std::string& );    // 7_lower.cpp
void lowert( std::string& );    // 8_lower.cpp
void lowermt( std::string& );   // 9_lower.cpp
void lowerv( std::string& );    // a_lower.cpp
void lowerav( std::string& );   // b_lower.cpp


///////////////////////////////////////////////////////////////////////////////

template <typename T, size_t N>
std::ostream& operator<<( std::ostream& os, const std::span<T,N>& sp )
{
  for( T c : sp )
    os << c;
  return os;
}

///////////////////////////////////////////////////////////////////////////////

int main()
{
  // 1)
  char a1[] = "AbCdEf";
  char a2[] = "AbCdEf";
  test( strcmp( lower( a1 ), "abcdef" ) == 0 );
  std::cout << lower( a2 ) << '\n';
  // lower( nullptr ); // crash!

  // 2)
  char b1[] = "AbCdEf";
  char b2[] = "AbCdEf";
  lower( b1, strlen( b1 ) );
  test( strcmp( b1, "abcdef" ) == 0 );
  lower( b2, strlen( b2 ) );
  std::cout << b2 << '\n';
  lower( nullptr, 0 ); // safe

  // 3)
  char c1[] = "AbCdEf";
  char c2[] = "AbCdEf";
  lower( std::span{ c1 } );
  test( strcmp( c1, "abcdef") == 0);
  lower( std::span{ c2 } );
  std::cout << b2 << '\n';
  lower( std::span<char>() ); // safe

  // 4)
  std::string d1{ "AbCdEf" };
  std::string d2{ "AbCdEf" };
  lower( d1 );
  test( d1 == "abcdef" );
  lower( d2 );
  std::cout << d2 << '\n';

  // 5)
  std::string_view e1{ "AbCdEf" };
  std::string_view e2{ "AbCdEf" };
  lower( e1 );
  //test( e1 == "abcdef" );
  lower( e2 );
  //std::cout << e2 << '\n';

  // 6-1
  std::string_view f1{ "AbCdEf" };
  std::span f2{ "AbCdEf" };
  char f3[] = "AbCdEf";
  char f4[] = "AbCdEf";
  std::span f5{ f4 };
  std::string f6{ "AbCdEf" };

  //tlower1( f1 ); // compiler error; can't modify string_view (it's a view!)
  //tlower1( f2 ); // compiler error; f2 is const char*
  tlower1( f3 );
  test( strcmp( f3, "abcdef" ) == 0 );
  tlower1( f5 );
  test( strcmp( f5.data(), "abcdef" ) == 0);
  std::cout << f5 << '\n';
  tlower1( f6 );
  test( strcmp( f6.data(), "abcdef" ) == 0 );
  std::cout << f6 << '\n';
  // std::cout << tlower1( f6 ) << '\n'; // compiler error; tlower1 doesn't return anything

  // 6-2
  char g3[] = "AbCdEf";
  char g4[] = "AbCdEf";
  std::span g5{ g4 };
  std::string g6{ "AbCdEf" };

  tlower2( g3 );
  test( strcmp( g3, "abcdef" ) == 0 );
  tlower2( g5 );
  test( strcmp( g5.data(), "abcdef" ) == 0 );
  std::cout << g5 << '\n';
  std::cout << tlower2( g6 ) << '\n'; // OK, this works

  // 6-3
  char h3[] = "AbCdEf";
  char h4[] = "AbCdEf";
  std::span h5{ h4 };
  std::string h6{ "AbCdEf" };

  tlower3( h3 );
  test( strcmp( h3, "abcdef" ) == 0 );
  tlower3( h5 );
  test( strcmp( h5.data(), "abcdef" ) == 0 );
  std::cout << h5 << '\n';
  std::cout << tlower3( h6 ) << '\n';

  // 6-4
  char j3[] = "AbCdEf";
  char j4[] = "AbCdEf";
  std::span j5{ j4 };
  std::string j6{ "AbCdEf" };

  tlower4( j3, std::execution::par_unseq );
  test( strcmp( j3, "abcdef" ) == 0 );
  tlower4( j5, std::execution::par_unseq );
  test( strcmp( j5.data(), "abcdef" ) == 0 );
  std::cout << j5 << '\n';
  std::cout << tlower4( j6, std::execution::par_unseq ) << '\n';

  // 7)
  std::string k1{ "AbCdEf" };
  std::string k2{ "AbCdEf" };
  lowerb( k1 );
  test( k1 == "abcdef" );
  lowerb( k2 );
  std::cout << k2 << '\n';

  // 8)
  std::string m1{ "AbCdEf" };
  std::string m2{ "AbCdEf" };
  lowert( m1 );
  test( m1 == "abcdef" );
  lowert( m2 );
  std::cout << m2 << '\n';

  // 9)
  std::string n1{ "AbCdEf" };
  std::string n2{ "AbCdEf" };
  lowermt( n1 );
  test( n1 == "abcdef" );
  lowermt( n2 );
  std::cout << n2 << '\n';

  // A)
  std::string p1{ "ABCdEfGhIjKlMnOpQrStUvWxYz" };
  std::string p2{ "ABCdEfGhIjKlMnOpQrStUvWxYz" };
  lowerv( p1 );
  test( p1 == "abcdefghijklmnopqrstuvwxyz" );
  lowerv( p2 );
  std::cout << p2 << '\n';

  // B)
  std::string q1{ "ABCdEfGhIjKlMnOpQrStUvWxYzABCdEfGhIjKlMnOpQrStUvWxYz" };
  std::string q2{ "ABCdEfGhIjKlMnOpQrStUvWxYzABCdEfGhIjKlMnOpQrStUvWxYz" };
  lowerav( q1 );
  test( q1 == "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz" );
  lowerav( q2 );
  std::cout << q2 << '\n';

  // C multithreaded SIMD
  // D CUDA or GPU solutions
  // https://lemire.me/blog/2024/08/03/converting-ascii-strings-to-lower-case-at-crazy-speeds-with-avx-512/

  // Bit 5 trick
  std::string z1;
  z1.resize( 256 - '~' );
  std::ranges::iota( z1, '~' );
  std::cout << z1 << '\n';
  lowerb( z1 );
  std::cout << z1 << '\n';

}

