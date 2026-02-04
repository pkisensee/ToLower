///////////////////////////////////////////////////////////////////////////////
//
// getLowerByValue.h
//
// Notes: 
//    * By value solution
//    * Input is not modified
//    * Returns a copy of the input sequence converted to lower case
//
// Issues:
//    * Two copies required: copy to temp, and return by value
//    * Can't be constexpr because std::string allocates memory
//    * Can't be noexcept because std::string allocation could throw

#pragma once

#include <string>
#include "toLowerSSE.h"

// Returns a copy of the input character sequence converted to lower case.
std::string getLowerByValue( const auto& s )
{
  std::string tmp{ s };
  return toLowerSSE( tmp );
}

///////////////////////////////////////////////////////////////////////////////
