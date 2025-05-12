///////////////////////////////////////////////////////////////////////////////
//
// lowerc.cpp
//
// Notes: 
//    * vectorized solution
//
// Issues:
//    * hard to read
//
// TODO: diagram algorithm

#include <emmintrin.h>
#include <string>

static const __m128i kAAAAs = _mm_set1_epi8( 'A' - 1 ); // no mm_cmpgte, so offset -1
static const __m128i kZZZZs = _mm_set1_epi8( 'Z' + 1 ); // no mm_cmplte, so offset +1
static const __m128i kCaseOffset = _mm_set1_epi8( 'a' - 'A' ); // from upper to lower
static const size_t kSSEbytes = sizeof(__m128i);
void lowerc( std::string& s )
{
  // Convert each kSSEbyte block
  size_t len = s.size();
  char* data = s.data();
  for ( ; len >= kSSEbytes; len -= kSSEbytes, data += kSSEbytes)
  {
    __m128i* strSSE = reinterpret_cast<__m128i*>(data);
    __m128i input = _mm_loadu_si128(strSSE);
    __m128i gteA = _mm_cmpgt_epi8(input, kAAAAs); // >=A then 0xFF else 0
    __m128i lteZ = _mm_cmplt_epi8(input, kZZZZs); // <=Z then 0xFF else 0
    __m128i isUpper = _mm_and_si128(gteA, lteZ);  // A-Z then 0xFF else 0
    __m128i caseAdd = _mm_and_si128(isUpper, kCaseOffset); // A-Z then ('a'-'A') else 0
    __m128i lower = _mm_add_epi8(input, caseAdd); // convert any A-Z to a-z
    _mm_storeu_si128(strSSE, lower);
  }
  // Convert remaining <16 chars directly
  for( ; len > 0; ++data, --len )
  {
    auto c = static_cast<unsigned char>( *data );
    *data = static_cast<char>( std::tolower( c ) );
  }
}
