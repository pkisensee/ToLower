///////////////////////////////////////////////////////////////////////////////
//
//  GetLowerByValue.h
//
//  Notes: 
//    * By value solution
//    * Input is not modified
//    * Returns a copy of the input sequence converted to lower case
//
//  Issues:
//    * Two copies required: copy to temp, and return by value
//    * Can't be constexpr because std::string allocates memory
//    * Can't be noexcept because std::string allocation could throw
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

#pragma once
#include <string>
#include "ToLowerSSE.h"

// Returns a copy of the input character sequence converted to lower case.
std::string GetLowerByValue( const auto& s )
{
  std::string tmp{ s };
  return ToLowerSSE( tmp );
}

///////////////////////////////////////////////////////////////////////////////
