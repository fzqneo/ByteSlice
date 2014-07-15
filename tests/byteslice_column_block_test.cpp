/*******************************************************************************
 * Copyright (c) 2015
 * The Hong Kong Polytechnic University, Database Group
 *
 * Author: Ziqiang Feng (cszqfeng AT comp.polyu.edu.hk)
 *
 * See file LICENSE.md for details.
 *******************************************************************************/

#include	<cstdio>
#include    <cstdlib>

#include 	"gtest/gtest.h"
#include 	"src/byteslice_column_block.h"
#include 	"src/bitvector_block.h"

namespace byteslice{

class ByteSliceColumnBlockTest: public ::testing::Test{
public:
    virtual void SetUp(){
        num_ = kNumTuplesPerBlock*0.8;
        block_ = new ByteSliceColumnBlock<20>(num_);

        WordUnit* codes = new WordUnit[num_];
        for(size_t i=0; i < num_; i++){
            codes[i] = i;
        }

        block_->BulkLoadArray(codes, num_);
        delete[] codes;
    }

    virtual void TearDown(){
        delete block_;
    }

protected:
    ByteSliceColumnBlock<20>* block_;
    size_t num_;
};

TEST_F(ByteSliceColumnBlockTest, SerDeser){
    std::string filename(std::tmpnam(nullptr));
    std::cout << "temp file: " << filename << "\n";
    // Serialize this block
    SequentialWriteBinaryFile outfile;
    outfile.Open(filename);
    block_->SerToFile(outfile);
    outfile.Close();

    // Deserialize from file
    ColumnBlock* block2 = new ByteSliceColumnBlock<20>(num_);
    SequentialReadBinaryFile infile;
    infile.Open(filename);
    block2->DeserFromFile(infile);
    infile.Close();

    // Verify
    EXPECT_EQ(block_->num_tuples(), block2->num_tuples());
    for(size_t i=0; i<num_; i++){
        EXPECT_EQ(block_->GetTuple(i), block2->GetTuple(i));
    }

    delete block2;
    std::remove(filename.c_str());
}

TEST_F(ByteSliceColumnBlockTest, BulkLoadAndGetTuple){
    for(size_t i=0; i<num_; i++){
        EXPECT_EQ(i, block_->GetTuple(i));
    }
}

TEST_F(ByteSliceColumnBlockTest, ScanLiteral){
    BitVectorBlock* bvblock = new BitVectorBlock(num_);

    std::srand(std::time(0));
    const WordUnit lit = std::rand() % num_;
    block_->Scan(Comparator::kLess, lit, bvblock, Bitwise::kSet);
    EXPECT_EQ(lit, bvblock->CountOnes());

    delete bvblock;
}

TEST_F(ByteSliceColumnBlockTest, ScanOtherBlock){
    BitVectorBlock* bvblock = new BitVectorBlock(num_);
    ByteSliceColumnBlock<20>* block2 = new ByteSliceColumnBlock<20>(num_);

    std::srand(std::time(0));
    for(size_t i=0; i < num_; i++){
        block2->SetTuple(i, std::rand() % num_);
    }

    block_->Scan(Comparator::kGreaterEqual, block2, bvblock, Bitwise::kSet);
    for(size_t i=0; i < num_; i++){
        EXPECT_EQ(bvblock->GetBit(i), block_->GetTuple(i) >= block2->GetTuple(i));
    }

    delete block2;
    delete bvblock;
}

}   // namespace
