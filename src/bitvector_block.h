/*******************************************************************************
 * Copyright (c) 2015
 * The Hong Kong Polytechnic University, Database Group
 *
 * Author: Ziqiang Feng (cszqfeng AT comp DOT polyu.edu.hk)
 *
 * See file LICENSE.md for details.
 *******************************************************************************/
#ifndef _BITVECTOR_BLOCK_H_
#define _BITVECTOR_BLOCK_H_

#include "../src/macros.h"
#include "../src/param.h"
#include "../src/types.h"

namespace byteslice{

/**
    Warning: 
    Pay attention to the bit sequence:
    Every 64 tuples are place in an uint64_t unit.
    Within the word unit, tuple with SMALLER Id is
    placed at LOWER (less significant) bits.
    DO NOT interprent the bit vector with different
    data types, otherwise you may fall into the
    big-/little-endian pitfall.
*/
class BitVectorBlock{
/*
   The bit vector block is guaranteed to be 32-byte aligned
   and the number of WordUnit is guaranteed to be
   a multiple of AVX registers
*/
public:
    BitVectorBlock(size_t num);
    ~BitVectorBlock();
    void SetOnes();
    void SetZeros();
    size_t CountOnes();
    void ClearTail();
    void And(const BitVectorBlock* block);
    void Or(const BitVectorBlock* block);
    void Set(const BitVectorBlock* block);

    //bit manipulation
    bool GetBit(size_t pos);
    void SetBit(size_t pos);
    void UnsetBit(size_t pos);

    //mutators
    void SetWordUnit(WordUnit word, size_t pos);
    void SetAvxUnit(AvxUnit avxunit, size_t start_word_pos);

    //accessors
    WordUnit GetWordUnit(size_t pos) const;
    AvxUnit GetAvxUnit(size_t start_word_pos) const;
    size_t num() const;
    size_t num_word_units() const;


private:
    WordUnit* data_ = NULL;
    size_t num_;
    size_t num_word_units_;

};

//mutators
inline void BitVectorBlock::SetWordUnit(WordUnit word, size_t pos){
    data_[pos] = word;
}
inline void BitVectorBlock::SetAvxUnit(AvxUnit avxunit, size_t start_word_pos){
    _mm256_storeu_si256((__m256i*)(data_+start_word_pos), avxunit);
}

//accessors
inline WordUnit BitVectorBlock::GetWordUnit(size_t pos) const{
    return data_[pos];
}

inline AvxUnit BitVectorBlock::GetAvxUnit(size_t start_word_pos) const{
    return _mm256_lddqu_si256((__m256i*)(data_+start_word_pos));
}

inline size_t BitVectorBlock::num() const{
    return num_;
}

inline size_t BitVectorBlock::num_word_units() const{
    return num_word_units_;
}


}   // namespace

#endif  //BITVECTOR_BLOCK_H
