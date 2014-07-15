/*******************************************************************************
 * Copyright (c) 2015
 * The Hong Kong Polytechnic University, Database Group
 *
 * Author: Ziqiang Feng (cszqfeng AT comp.polyu.edu.hk)
 *
 * See file LICENSE.md for details.
 *******************************************************************************/
#include "../src/bitvector_block.h"

#include    <iostream>

#include "../src/macros.h"
#include "../src/param.h"
#include "../src/types.h"
#include    "gtest/gtest.h"


namespace byteslice{

class BitVectorBlockTest: public ::testing::Test{
public:
    virtual void SetUp(){
    }

    virtual void TearDown(){
    }

};

TEST_F(BitVectorBlockTest, ClearTail){
    //326 = 256 + 64 + 6
    BitVectorBlock* block = new BitVectorBlock(326);
    size_t count;
    for(int i=0; i<6; i++){
        block->SetWordUnit(-1ULL, i);
    }
    count = block->CountOnes();
    EXPECT_EQ(6*64UL, count);
    block->ClearTail();
    count = block->CountOnes();
    EXPECT_EQ(326UL, count);
    EXPECT_EQ(0x3fUL, block->GetWordUnit(5));

    delete block;
}

TEST_F(BitVectorBlockTest, SetZeros){
    BitVectorBlock* block = new BitVectorBlock(1000);
    block->SetZeros();
    size_t count = block->CountOnes();
    EXPECT_EQ(0UL, count);
    EXPECT_EQ(1000UL, block->num());
    EXPECT_EQ(16UL, block->num_word_units());
    delete block;
}

TEST_F(BitVectorBlockTest, SetOnes){
    BitVectorBlock* block = new BitVectorBlock(1000);
    block->SetOnes();
    size_t count = block->CountOnes();
    EXPECT_EQ(1000UL, count);
    EXPECT_EQ(1000UL, block->num());
    delete block;
}

TEST_F(BitVectorBlockTest, SetWordUnit){
    BitVectorBlock* block = new BitVectorBlock(kNumTuplesPerBlock);
    block->SetZeros();
    block->SetWordUnit(0xff, 23);
    EXPECT_EQ(0xffUL, block->GetWordUnit(23));
    EXPECT_EQ(0x0UL, block->GetWordUnit(22));
    EXPECT_EQ(8UL, block->CountOnes());
    delete block;
}

TEST_F(BitVectorBlockTest, AndOr){
    BitVectorBlock* block1 = new BitVectorBlock(kNumTuplesPerBlock);
    BitVectorBlock* block2 = new BitVectorBlock(kNumTuplesPerBlock);
    block1->SetOnes();
    block2->SetZeros();
    block2->SetWordUnit(0x1ff, 10);
    block1->And(block2);
    EXPECT_EQ(9UL, block1->CountOnes());
    block2->SetZeros();
    block2->Or(block1);
    EXPECT_EQ(9UL, block2->CountOnes());
    delete block1;
    delete block2;
}

TEST_F(BitVectorBlockTest, SetAvxUnit){
    BitVectorBlock* block1 = new BitVectorBlock(kNumTuplesPerBlock);
    block1->SetZeros();
    AvxUnit a1 = _mm256_set_epi64x(0xff, 0x0f, 0x3, 0x1);
    block1->SetAvxUnit(a1, 4);
    EXPECT_EQ(15UL, block1->CountOnes());
    delete block1;
}

}
