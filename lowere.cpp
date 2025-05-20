///////////////////////////////////////////////////////////////////////////////
//
// lowere.cpp
//
// Notes: 
//    * OpenCL GPU solution
//    * Performs a one-time compilation of an OpenCL kernel
//    * Uses fastest GPU method with lookup table
//
// Issues:
//    * Must install OpenCL SDK
//    * Must install runtime/drivers for AMD or NVIDIA GPU (Windows)

#include "OpenCL.h"

void lowere( std::string& s )
{
  static OpenCL openCL( CL_DEVICE_TYPE_GPU, "lowerLUT" );
  openCL.ToLower(s);
}
