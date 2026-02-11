///////////////////////////////////////////////////////////////////////////////
//
// Stopwatch.h
//
// Simple performance stopwatch. Usage:
//
//   Stopwatch stopwatch;
//   foo(); // measure foo
//   std::println( "{} microseconds", stopwatch.getElapsedMicroseconds() );
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
#include <chrono>

class Stopwatch
{
  using hirez_clock = std::chrono::high_resolution_clock;

public:
  Stopwatch() :
    start_{ hirez_clock::now() }
  {
  }

  void Start()
  {
    start_ = hirez_clock::now();
  }

  double GetElapsedMicroseconds() const
  {
    auto end = hirez_clock::now();
    std::chrono::duration<double, std::micro> duration = end - start_;
    return duration.count();
  }

private:

  std::chrono::time_point< hirez_clock > start_;

}; // Stopwatch

///////////////////////////////////////////////////////////////////////////////
