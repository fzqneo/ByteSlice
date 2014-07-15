/*******************************************************************************
 * Copyright (c) 2015
 * The Hong Kong Polytechnic University, Database Group
 *
 * Author: Ziqiang Feng (cszqfeng AT comp.polyu.edu.hk)
 *
 * See file LICENSE.md for details.
 *******************************************************************************/

#include    <cstdlib>
#include    <cstdint>
#include    <cstdio>

#include    "gtest/gtest.h"

#include 	"src/avx-utility.h"

namespace byteslice{

class AvxUtilityTest: public ::testing::Test{
public:
    virtual void SetUp(){
        std::srand(std::time(0));
    }

    virtual void TearDown(){
    }
};

TEST_F(AvxUtilityTest, FlipTest){
    // byte
    for(size_t i = 0; i < 20; i++){
        unsigned char a = std::rand()%256;
        unsigned char b = std::rand()%256;
        signed char x = static_cast<signed char>(FLIP<unsigned char>(a));
        signed char y = static_cast<signed char>(FLIP<unsigned char>(b));
        EXPECT_TRUE((a<b) == (x<y));
        EXPECT_TRUE((a==b) == (a==b));
    }
}

TEST_F(AvxUtilityTest, Set1Test){
    // byte
    {
        unsigned char a1 = 0x3e;
        __m256i m1 = avx_set1<unsigned char>(a1);
        int *p = (int*)(&m1);
        EXPECT_EQ(0x3e3e3e3e, *p);
    }

    // double byte
    {
        uint16_t a1 = 0x12ae;
        __m256i m1 = avx_set1<uint16_t>(a1);
        uint16_t *p = (uint16_t*)(&m1);
        EXPECT_EQ(0x12ae, p[1]);
        uint8_t *q = (uint8_t*)(&m1);
        EXPECT_EQ(0xae, q[0]);
        EXPECT_EQ(0x12, q[1]);
    }

}

TEST_F(AvxUtilityTest, CompareTest){
    // int32
    __m256i a = _mm256_set_epi32(0xef, 0xcd, 0xab, 0x89, 0x67, 0x45, 0x23, 0x01);
    __m256i b = avx_set1<int>(0x89);
    int mmask;

    __m256i m_lt = avx_cmplt<int>(a, b);
    mmask = _mm256_movemask_ps((__m256)m_lt);
    EXPECT_EQ(0x0f, mmask);

    __m256i m_gt = avx_cmpgt<int>(a, b);
    mmask = _mm256_movemask_ps(__m256(m_gt));
    EXPECT_EQ(0xe0, mmask);
    
    __m256i m_eq = avx_cmpeq<int>(a, b);
    mmask = _mm256_movemask_ps(__m256(m_eq));
    EXPECT_EQ(0x10, mmask);

}

}   // namespace
