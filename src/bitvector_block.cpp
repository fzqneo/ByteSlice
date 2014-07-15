/*******************************************************************************
 * Copyright (c) 2015
 * The Hong Kong Polytechnic University, Database Group
 *
 * Author: Ziqiang Feng (cszqfeng AT comp DOT polyu.edu.hk)
 *
 * See file LICENSE.md for details.
 *******************************************************************************/
#include "bitvector_block.h"

#include	<cassert>
#include    <cstdlib>
#include    <cstring>

namespace byteslice{

BitVectorBlock::BitVectorBlock(size_t num):
    num_(num), num_word_units_(CEIL(num, kNumAvxBits)*(kNumAvxBits/kNumWordBits)){
    assert(num_ <= kNumTuplesPerBlock);
    // always allocate a full-block's storage
    size_t count = posix_memalign((void**)&data_, 32, sizeof(WordUnit)*CEIL(kNumTuplesPerBlock, kNumWordBits));
    (void)count;
    SetOnes();
}

BitVectorBlock::~BitVectorBlock(){
    free(data_);
}

bool BitVectorBlock::GetBit(size_t pos){
    size_t word_id = pos / kNumWordBits;
    size_t offset = pos % kNumWordBits;
    WordUnit mask = 1ULL << offset;
    return (data_[word_id] & mask);
}

void BitVectorBlock::SetBit(size_t pos){
    size_t word_id = pos / kNumWordBits;
    size_t offset = pos % kNumWordBits;
    WordUnit mask = 1ULL << offset;
    data_[word_id] |= mask;
}

void BitVectorBlock::UnsetBit(size_t pos){
    size_t word_id = pos / kNumWordBits;
    size_t offset = pos % kNumWordBits;
    WordUnit mask = 1ULL << offset;
    data_[word_id] &= ~mask;
}

void BitVectorBlock::SetOnes(){
    memset(data_, 0xff, sizeof(WordUnit)*num_word_units_);
    ClearTail();
}

void BitVectorBlock::SetZeros(){
    memset(data_, 0x0, sizeof(WordUnit)*num_word_units_);
}

size_t BitVectorBlock::CountOnes(){
    size_t count = 0;
    for(size_t i=0; i<num_word_units_; i++){
        //count += _mm_popcnt_u64(data_[i]);
        count += POPCNT64(data_[i]);
    }
    return count;
}

void BitVectorBlock::And(const BitVectorBlock* block){
    for(size_t i=0; i<num_word_units_; i++){
        data_[i] &= block->GetWordUnit(i);
    }
    ClearTail();
}

void BitVectorBlock::Or(const BitVectorBlock* block){
    for(size_t i=0; i<num_word_units_; i++){
        data_[i] |= block->GetWordUnit(i);
    }
    ClearTail();
}

void BitVectorBlock::Set(const BitVectorBlock* block){
    for(size_t i=0; i<num_word_units_; i++){
        data_[i] = block->GetWordUnit(i);
    }
    ClearTail();
} 


void BitVectorBlock::ClearTail(){
    //I may have to clear up to 4 WordUnit
    size_t num_empty = kNumAvxBits - (num_ % kNumAvxBits);
    if(kNumAvxBits != num_empty){
        size_t i = num_word_units_ - 1;
        while(num_empty >= kNumWordBits){
            data_[i] = 0;
            num_empty -= kNumWordBits;
            i--;
        }
        data_[i] &= (-1ULL >> num_empty);
    }

}


}
