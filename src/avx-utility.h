/*******************************************************************************
 * Copyright (c) 2015
 * The Hong Kong Polytechnic University, Database Group
 *
 * Author: Ziqiang Feng (cszqfeng AT comp DOT polyu.edu.hk)
 *
 * See file LICENSE.md for details.
 *******************************************************************************/
#ifndef AVX_UTILITY_H
#define AVX_UTILITY_H

#include    <cstdint>
#include    <x86intrin.h>

namespace byteslice{


/* T should be uint8/16/32/64_t */

template <typename T>
inline T FLIP(T value){
    constexpr T offset = (static_cast<T>(1) << (sizeof(T)*8 - 1));
    return static_cast<T>(value ^ offset);
}

// Compare less
template <typename T>
inline __m256i avx_cmplt(const __m256i &a, const __m256i &b){
    switch(sizeof(T)){
        case 1:
            return _mm256_cmpgt_epi8(b, a);
        case 2:
            return _mm256_cmpgt_epi16(b, a);
        case 4:
            return _mm256_cmpgt_epi32(b, a);
        case 8:
            return _mm256_cmpgt_epi64(b, a);
    }
}

// Compare greater
template <typename T>
inline __m256i avx_cmpgt(const __m256i &a, const __m256i &b){
    switch(sizeof(T)){
        case 1:
            return _mm256_cmpgt_epi8(a, b);
        case 2:
            return _mm256_cmpgt_epi16(a, b);
        case 4:
            return _mm256_cmpgt_epi32(a, b);
        case 8:
            return _mm256_cmpgt_epi64(a, b);
    }
}

// Compare equal
template <typename T>
inline __m256i avx_cmpeq(const __m256i &a, const __m256i &b){
    switch(sizeof(T)){
        case 1:
            return _mm256_cmpeq_epi8(b, a);
        case 2:
            return _mm256_cmpeq_epi16(b, a);
        case 4:
            return _mm256_cmpeq_epi32(b, a);
        case 8:
            return _mm256_cmpeq_epi64(b, a);
    }
}

// Set1
template <typename T>
inline __m256i avx_set1(T a){
    switch(sizeof(T)){
        case 1:
            return _mm256_set1_epi8(static_cast<int8_t>(a));
        case 2:
            return _mm256_set1_epi16(static_cast<int16_t>(a));
        case 4:
            return _mm256_set1_epi32(static_cast<int32_t>(a));
        case 8:
            return _mm256_set1_epi64x(static_cast<int64_t>(a));
    }
}

// Zero
inline __m256i avx_zero(){
    return _mm256_setzero_si256();
}

// All ones
inline __m256i avx_ones(){
    return _mm256_set1_epi64x(-1ULL);
}

// Bitwise AND
inline __m256i avx_and(const __m256i &a, const __m256i &b){
    return _mm256_and_si256(a, b);
}

// Bitwise OR
inline __m256i avx_or(const __m256i &a, const __m256i &b){
    return _mm256_or_si256(a, b);
}

// Bitwise XOR
inline __m256i avx_xor(const __m256i &a, const __m256i &b){
    return _mm256_xor_si256(a, b);
}

// Bitwise NOT
inline __m256i avx_not(const __m256i &a){
    return _mm256_xor_si256(a, avx_ones());
}

// Bitwise (NOT a) AND b
inline __m256i avx_andnot(const __m256i &a, const __m256i &b){
    return _mm256_andnot_si256(a, b);
}

// Test is zero
inline bool avx_iszero(const __m256i &a){
    return _mm256_testz_si256(a, a);
}


}   // namespace

#endif  //AVX_UTILITY_H
