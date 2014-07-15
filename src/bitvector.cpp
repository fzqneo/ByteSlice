/*******************************************************************************
 * Copyright (c) 2015
 * The Hong Kong Polytechnic University, Database Group
 *
 * Author: Ziqiang Feng (cszqfeng AT comp DOT polyu.edu.hk)
 *
 * See file LICENSE.md for details.
 *******************************************************************************/
#include "bitvector.h"

#include    <algorithm>
#include    <omp.h>

namespace byteslice{

BitVector::BitVector(const Column* column):
    BitVector(column->GetNumTuples()){
}

BitVector::BitVector(size_t num):
    num_(num){

    for(size_t count=0; count < num_; count += kNumTuplesPerBlock){
        BitVectorBlock* new_block = 
            new BitVectorBlock(std::min(kNumTuplesPerBlock, num_ - count));
        blocks_.push_back(new_block);
    }
    SetOnes();
}


BitVector::~BitVector(){
    while(!blocks_.empty()){
        delete blocks_.back();
        blocks_.pop_back();
    }
}

void BitVector::And(const BitVector* bitvector){
    assert(num_ == bitvector->num_);

#   pragma omp parallel for schedule(dynamic)
    for(size_t i=0; i < blocks_.size(); i++){
        blocks_[i]->And(bitvector->GetBVBlock(i));
    }
}

void BitVector::Or(const BitVector* bitvector){
    assert(num_ == bitvector->num_);

#   pragma omp parallel for schedule(dynamic)
    for(size_t i=0; i < blocks_.size(); i++){
        blocks_[i]->Or(bitvector->GetBVBlock(i));
    }
}


void BitVector::SetOnes(){
#   pragma omp parallel for schedule(dynamic)
    for(size_t i=0; i < blocks_.size(); i++){
        blocks_[i]->SetOnes();
    }
}

void BitVector::SetZeros(){
#   pragma omp parallel for schedule(dynamic)
    for(size_t i=0; i < blocks_.size(); i++){
        blocks_[i]->SetZeros();
    }
}

size_t BitVector::CountOnes() const{
    size_t count = 0;
#   pragma omp parallel for schedule(dynamic) reduction(+: count)
    for(size_t i=0; i < blocks_.size(); i++){
        count += blocks_[i]->CountOnes();
    }
    return count;
}

bool BitVector::GetBit(size_t pos){
    size_t block_id = pos / kNumTuplesPerBlock;
    size_t pos_in_block = pos % kNumTuplesPerBlock;
    return blocks_[block_id]->GetBit(pos_in_block);
}

void BitVector::SetBit(size_t pos){
    size_t block_id = pos / kNumTuplesPerBlock;
    size_t pos_in_block = pos % kNumTuplesPerBlock;
    blocks_[block_id]->SetBit(pos_in_block);
}

void BitVector::UnsetBit(size_t pos){
    size_t block_id = pos / kNumTuplesPerBlock;
    size_t pos_in_block = pos % kNumTuplesPerBlock;
    blocks_[block_id]->UnsetBit(pos_in_block);
}

}   // namespace
