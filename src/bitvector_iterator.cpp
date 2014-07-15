/*******************************************************************************
 * Copyright (c) 2015
 * The Hong Kong Polytechnic University, Database Group
 *
 * Author: Ziqiang Feng (cszqfeng AT comp DOT polyu.edu.hk)
 *
 * See file LICENSE.md for details.
 *******************************************************************************/
#include "bitvector_iterator.h"

namespace byteslice{

BitVectorIterator::BitVectorIterator(const BitVector *bitvector):
    bitvector_(bitvector),
    cur_block_(bitvector_->GetBVBlock(0)){
}


BitVectorIterator::~BitVectorIterator(){
}


}   // namespace
