///////////////////////////////////////////////////////////////////////////////
//
//  Stopwatch.h
//
//  Simple performance stopwatch. Usage:
//
//    Stopwatch stopwatch;
//    foo(); // measure foo
//    std::cout << stopwatch.getElapsedMicroseconds() << " microseconds\n";

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
