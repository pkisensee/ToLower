///////////////////////////////////////////////////////////////////////////////
//
// lowerg.cpp
//
// Notes: 
//    * by value solution using lowera
//
// Issues:
//    * Always going to be slower because a copy is required

#include <string>
#include "lowera.h"

// Note: can't be constexpr function because allocates memory internally
std::string lowerg( const std::string& str ) noexcept
{
  std::string s{ str };
  return toLowerA( s );
}
