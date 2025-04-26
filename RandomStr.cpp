#include <random>
#include <string>

std::string getRandomStr( size_t len )
{
  auto seed = std::random_device{}( );
  std::mt19937 randGenerator( seed );
  std::uniform_int_distribution<int> distributionAscii( ' ', '~' );
  std::string s;
  s.resize( len );
  std::ranges::transform( s, s.begin(), [&]( unsigned char ) {
    return static_cast<char>( distributionAscii( randGenerator ) );
    } );
  return s;
}