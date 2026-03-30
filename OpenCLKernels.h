///////////////////////////////////////////////////////////////////////////////
//
// OpenCLKernels.h
//
// OpenCL kernel interface names
// Implementations in OpenCLKernels.cl
//
// Copyright © Pete Isensee (PKIsensee@msn.com).
// All rights reserved worldwide.
//
// Permission to copy, modify, reproduce or redistribute this source code is
// granted provided the above copyright notice is retained in the resulting 
// source code.
// 
// This software is provided "as is" and without any express or implied
// warranties.

#pragma once
#include <string>

static inline std::string kOpenCLKernelBranch      = "GetLowerChar_OpenCLBranch";
static inline std::string kOpenCLKernelCondMove    = "GetLowerChar_OpenCLCondMove";
static inline std::string kOpenCLKernelLookupTable = "GetLowerChar_OpenCLLookupTable";
static inline std::string kOpenCLKernel5thBit      = "GetLowerChar_OpenCL5thBit";
static inline std::string kOpenCLKernelVector      = "GetLowerChar_OpenCLVector";

///////////////////////////////////////////////////////////////////////////////
