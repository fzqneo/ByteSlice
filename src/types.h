/*******************************************************************************
 * Copyright (c) 2015
 * The Hong Kong Polytechnic University, Database Group
 *
 * Author: Ziqiang Feng (cszqfeng AT comp DOT polyu.edu.hk)
 *
 * See file LICENSE.md for details.
 *******************************************************************************/
#ifndef TYPES_H
#define TYPES_H

#include    <x86intrin.h>
#include    <cstdint>
#include    <iostream>

namespace byteslice{

typedef uint64_t WordUnit;
typedef uint8_t ByteUnit;
typedef __m256i AvxUnit;

constexpr size_t kNumWordBits = 8*sizeof(WordUnit);
constexpr size_t kNumAvxBits = 8*sizeof(AvxUnit);

enum class ColumnType{
    kNaive,
    kByteSlicePadRight,
    kByteSlicePadLeft
};


enum class Bitwise{
    kSet,
    kAnd,
    kOr
};

enum class Comparator{
    kEqual,
    kInequal,
    kLess,
    kGreater,
    kLessEqual,
    kGreaterEqual
};

enum class Direction{
    kLeft,
    kRight
};


//for debug use
std::ostream& operator<< (std::ostream &out, ColumnType type);
std::ostream& operator<< (std::ostream &out, Comparator comp);

}   // namespace

#endif //TYPES_H
