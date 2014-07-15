/*******************************************************************************
 * Copyright (c) 2015
 * The Hong Kong Polytechnic University, Database Group
 *
 * Author: Ziqiang Feng (cszqfeng AT comp.polyu.edu.hk)
 *
 * See file LICENSE.md for details.
 *******************************************************************************/
#include "../src/bitvector.h"

#include "../src/macros.h"
#include "../src/param.h"
#include "../src/types.h"
#include    "gtest/gtest.h"

namespace byteslice{

class BitVectorTest: public ::testing::Test{
public:
    virtual void SetUp(){
    }

    virtual void TearDown(){
    }

protected:
    const size_t num_ = 3*kNumTuplesPerBlock + 2000;

};

TEST_F(BitVectorTest, Ctor){
    BitVector *bitvector = new BitVector(num_);

    EXPECT_EQ(4UL, bitvector->GetNumBlocks());
    EXPECT_EQ(num_, bitvector->num());
    EXPECT_EQ(kNumTuplesPerBlock, bitvector->GetBVBlock(0)->num());
    EXPECT_EQ(2000UL, bitvector->GetBVBlock(3)->num());

    delete bitvector;
}


}   // namespace
