///////////////////////////////////////////////////////////////////////////////
//
// lower0.cpp
//
// Issues: 
//    * Compiler warnings; strlen returns size_t (unsigned) but loop uses int (signed)
//    * UB if input is null
//    * strlen is an unsafe function used for buffer overflow attacks
//    * Returns a pointer, which is confusing because it's not obvious to the caller what the return value represents, e.g. does the caller need to free the result?
//    * Calls strlen every loop iteration, which is order n^2. Testing with 1MB string -> 5 minutes
//    * Not const correct (by design); changes the input value
//    * Non-obvious that s itself is converted
//    * UB to call tolower() without first converting the input to unsigned char (https://en.cppreference.com/w/cpp/string/byte/tolower)

#include <cctype>
#include <cstring>

#pragma warning(push)
#pragma warning(disable: 4242 4244 4388)

char* lower0( char* s )
{
  for( int i = 0; i < std::strlen( s ); ++i ) // '<': signed/unsigned mismatch
    s[i] = std::tolower( s[i] );              // '=': conversion from int to char
  return s;
}

#pragma warning(pop)
