///////////////////////////////////////////////////////////////////////////////
//
//  toLowerOpenCL.h
//
//  Notes: 
//    * OpenCL solution
//    * Requires initialized OpenCLContext with compiled kernel function
//
//  Issues:
//    * Must install OpenCL SDK
//    * Must install runtime/drivers for AMD or NVIDIA GPU (Windows)
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

#include "OpenCLContext.h"

// Convert char sequence to lower case in place. Returns modified sequence.
constexpr auto&& toLowerOpenCL( auto&& s, OpenCLContext& openCLContext ) noexcept
{
  return openCLContext.ToLower( s );
}

///////////////////////////////////////////////////////////////////////////////
