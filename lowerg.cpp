///////////////////////////////////////////////////////////////////////////////
//
// lowerg.cpp
//
// Notes: 
//    * by value solution
//
// Issues:
//    * Always going to be slower because a copy is required
//    * Can't be constexpr because std::string allocates memory

#include <string>
#include "toLowerAutoConstexpr.h"

std::string lowerg( const std::string& s ) noexcept
{
  std::string tmp{ s };
  return toLowerAutoConstexpr( tmp ); // TODO verify this is a good option
}
