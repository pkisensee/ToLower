///////////////////////////////////////////////////////////////////////////////
//
// Concepts.h
//
// Common concepts
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
#include <concepts>
#include <ranges>

// Contiguous range of type char
template <typename T>
concept CharRange = std::ranges::contiguous_range<T> &&
                    std::same_as<std::ranges::range_value_t<T>, char>;

///////////////////////////////////////////////////////////////////////////////

