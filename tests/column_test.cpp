/*******************************************************************************
 * Copyright (c) 2015
 * The Hong Kong Polytechnic University, Database Group
 *
 * Author: Ziqiang Feng (cszqfeng AT comp.polyu.edu.hk)
 *
 * See file LICENSE.md for details.
 *******************************************************************************/

#include    <cstdlib>
#include    <fstream>
#include    <string>

#include    "gtest/gtest.h"

#include 	"src/column.h"


namespace byteslice{

class ColumnTest: public ::testing::Test{
public:
    virtual void SetUp(){
        data_ = new WordUnit[num_];
        std::srand(std::time(0));
        for(size_t i=0; i < num_; i++){
            data_[i] = std::rand() & mask_;
        }
    }

    virtual void TearDown(){
        delete[] data_;
    }

protected:
    WordUnit* data_;
    const size_t num_ = 2.45*kNumTuplesPerBlock;
    const size_t bit_width_ = 21;
    const WordUnit mask_ = (1ULL << bit_width_) - 1 ;
};

TEST_F(ColumnTest, LoadTextFile){
    std::string filename(std::tmpnam(nullptr));
    
    // write data to a text file
    std::ofstream outfile(filename, std::ofstream::out);
    for(size_t i=0; i < num_; i++){
        outfile << data_[i] << std::endl;
    }
    outfile.close();

    // Verify
    Column* column = new Column(ColumnType::kByteSlicePadRight, bit_width_, num_);
    column->LoadTextFile(filename);
    for(size_t i=0; i < num_; i++){
        EXPECT_EQ(data_[i], column->GetTuple(i));
    }
    delete column;
}

TEST_F(ColumnTest, NaiveSetTuple){
    Column* column = new Column(ColumnType::kNaive, bit_width_, num_);
    for(size_t i=0; i < num_; i++){
        column->SetTuple(i, data_[i]);
    }
    for(size_t i=0; i < num_; i++){
        EXPECT_EQ(data_[i], column->GetTuple(i));
    }
    delete column;
}


TEST_F(ColumnTest, NaiveBulkLoadAndScanLiteral){
    WordUnit literal = std::rand() & mask_;
    Column* column = new Column(ColumnType::kNaive, bit_width_, num_);
    BitVector* bitvector = new BitVector(column);

    column->BulkLoadArray(data_, num_);
    column->Scan(Comparator::kLess, literal, bitvector, Bitwise::kSet);
    size_t bvcount = bitvector->CountOnes();
    size_t count = 0;
    for(size_t i=0; i < num_; i++){
        EXPECT_EQ(data_[i], column->GetTuple(i));
        count += (data_[i] < literal);
        EXPECT_EQ((data_[i] < literal), bitvector->GetBit(i));
    }
    EXPECT_EQ(count, bvcount);
    delete bitvector;
    delete column;
}



}   // namespace
