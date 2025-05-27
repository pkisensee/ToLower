///////////////////////////////////////////////////////////////////////////////
//
// lowerf.cpp
//
// Notes: 
//    * OpenCL CPU solution
//    * Performs a one-time compilation of an OpenCL kernel
//    * Uses fastest CPU method with branchless math
//
// Issues:
//    * Must install OpenCL SDK
//    * Must install runtime/drivers for Intel or AMD CPU (Windows)

#include "OpenCL.h"

std::string& lowerf( std::string& s )
{
  static OpenCL openCL( CL_DEVICE_TYPE_CPU, "lowerBranchless" );
  return openCL.ToLower( s );
}
