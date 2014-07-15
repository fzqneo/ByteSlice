/*******************************************************************************
 * Copyright (c) 2015
 * The Hong Kong Polytechnic University, Database Group
 *
 * Author: Ziqiang Feng (cszqfeng AT comp DOT polyu.edu.hk)
 *
 * See file LICENSE.md for details.
 *******************************************************************************/
#ifndef BITVECTOR_ITERATOR_H
#define BITVECTOR_ITERATOR_H

#include "../src/bitvector.h"

namespace byteslice{

/**
  Given a BitVector, extract the positions corresponding to the 1's
*/

class BitVector;
class BitVectorBlock;

class BitVectorIterator{
public:
    BitVectorIterator(const BitVector *bitvector);
    ~BitVectorIterator();
    bool Next();    //Move the cursor to the next 1, return true if next exists
    size_t GetPosition();   //Return the position of the cursor

private:
    const BitVector *bitvector_;
    size_t stack_[kNumWordBits];
    size_t stack_top_ = 1;  //*top* is the available position to push in new item

    //These cursors mark the word unit that is TO BE CONSIDERED, i.e., NOT considered yet.
    size_t cur_block_id_ = 0;
    size_t cur_word_id_ = 0;
    BitVectorBlock* cur_block_ = NULL;
    size_t block_offset_ = 0;

};

inline size_t BitVectorIterator::GetPosition(){
    return stack_[stack_top_ - 1];
}

inline bool BitVectorIterator::Next(){
    stack_top_--;
    //Need to do heavy work only when stack is empty
    if(0 == stack_top_){
        WordUnit word = 0;
        do{
            //first, make sure the cursor is valid
            //advance the cursor if appropriate
            if(cur_word_id_ >= cur_block_->num_word_units()){
                //all words in this block are exhausted, proceed to next block
                cur_word_id_ = 0;
                cur_block_id_++;
                block_offset_ += cur_block_->num();
                //it's possible that we pass the last block
                if(cur_block_id_ >= bitvector_->GetNumBlocks()){ //all BV blocks are exhausted
                    return false;
                }
                cur_block_ = bitvector_->GetBVBlock(cur_block_id_);
            }
            word = cur_block_->GetWordUnit(cur_word_id_);
            cur_word_id_++;
        }
        while(0 == word);

        //Extract positions from this non-zero bit-vector word
        //Attention: because we are using a *stack*,
        //we must push larger positions first
        size_t offset = block_offset_ + (cur_word_id_-1)*kNumWordBits;
        for(size_t bit = kNumWordBits - 1; bit < kNumWordBits; bit--){
            //standard technique to reduce branch miss
            stack_[stack_top_] = offset + bit;
            stack_top_ += ((word >> bit) & 1ULL);
        }

    }

    return true;
}


}   // namespace

#endif //BITVECTOR_ITERATOR_H
