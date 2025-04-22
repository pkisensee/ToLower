///////////////////////////////////////////////////////////////////////////////
//
// 9) lowerav
//
// Notes: 
//    * AVX2 vectorized solution
//
// Issues:
//    * hard to read

#include <immintrin.h>
#include <string>

static constexpr char kMask = 0b0010'0000;
static const __m256i kBit5Set = _mm256_set1_epi8( kMask );
static const size_t kAVX2bytes = sizeof( __m256i );

void lowerav( std::string& s )
{ 
  // doesn't handle @, [, ], ^, _, and '\'
  size_t len = s.size(); 
  char* data = s.data(); 
  for ( ; len >= kAVX2bytes; len -= kAVX2bytes, data += kAVX2bytes )
  {
    __m256i* strAVX2 = reinterpret_cast<__m256i*>(data);
    __m256i input = _mm256_loadu_si256(strAVX2);
    __m256i lower = _mm256_or_si256(input, kBit5Set);
    _mm256_storeu_si256(strAVX2, lower);
  }
  // Convert remaining <32 chars directly
  for( ; len > 0; ++data, --len )
  {
    auto lower = *data | kMask;
    *data = static_cast<char>( lower );
  }
}