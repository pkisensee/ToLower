///////////////////////////////////////////////////////////////////////////////
//
// toLowerOpenCL.h
//
// Notes: 
//    * OpenCL solution
//    * Requires initialized OpenCLContext with compiled kernel function
//
// Issues:
//    * Must install OpenCL SDK
//    * Must install runtime/drivers for AMD or NVIDIA GPU (Windows)

#include "OpenCLContext.h"

// Convert char sequence to lower case in place. Returns modified sequence.
constexpr auto&& toLowerOpenCL( auto&& s, OpenCLContext& openCLContext ) noexcept
{
  return openCLContext.ToLower( s );
}
