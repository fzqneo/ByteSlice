/*******************************************************************************
 * Copyright (c) 2015
 * The Hong Kong Polytechnic University, Database Group
 *
 * Author: Ziqiang Feng (cszqfeng AT comp.polyu.edu.hk)
 *
 * See file LICENSE.md for details.
 *******************************************************************************/
#include "../src/bitvector_iterator.h"

#include    "gtest/gtest.h"
#include    <cstdlib>

namespace byteslice{

class BitVectorIteratorTest: public ::testing::Test{
public:
    virtual void SetUp(){
        std::srand(std::time(0));
        bitvector_ = new BitVector(num_);
    }

    virtual void TearDown(){
        delete bitvector_;
    }

protected:
    BitVector *bitvector_;
    const size_t num_ = 3*kNumTuplesPerBlock + 2000;
};

TEST_F(BitVectorIteratorTest, Simple){
    bitvector_->SetZeros();
    size_t jump = kNumTuplesPerBlock*3/4;
    for(size_t i=1; i <=4; i++){
        bitvector_->SetBit(i*jump);
    }

    //Verify
    BitVectorIterator* itor = new BitVectorIterator(bitvector_);
    for(size_t i=1; i <=4; i++){
        EXPECT_TRUE(itor->Next());
        EXPECT_EQ(i*jump, itor->GetPosition());
    }
    EXPECT_FALSE(itor->Next());
    delete itor;
}

TEST_F(BitVectorIteratorTest, Random){
    bitvector_->SetZeros();
    size_t jump = kNumTuplesPerBlock*3/4;
    size_t answer[4];
    for(size_t i=0; i < 4; i++){
        answer[i] = i*jump + std::rand() % jump;
        bitvector_->SetBit(answer[i]);
    }

    //Verify
    BitVectorIterator* itor = new BitVectorIterator(bitvector_);
    for(size_t i=0; i < 4; i++){
        EXPECT_TRUE(itor->Next());
        EXPECT_EQ(answer[i], itor->GetPosition());
    }
    EXPECT_FALSE(itor->Next());
    delete itor;
}

TEST_F(BitVectorIteratorTest, AllOnes){
    bitvector_->SetOnes();
    //Verify
    BitVectorIterator* itor = new BitVectorIterator(bitvector_);
    for(size_t i=0; i < num_; i++){
        EXPECT_TRUE(itor->Next());
        EXPECT_EQ(i, itor->GetPosition());
    }
    EXPECT_FALSE(itor->Next());
    delete itor;
}

}   // namespace
