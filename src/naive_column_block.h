/*******************************************************************************
 * Copyright (c) 2015
 * The Hong Kong Polytechnic University, Database Group
 *
 * Author: Ziqiang Feng (cszqfeng AT comp DOT polyu.edu.hk)
 *
 * See file LICENSE.md for details.
 *******************************************************************************/
#ifndef NAIVE_COLUMN_BLOCK_H
#define NAIVE_COLUMN_BLOCK_H

#include "../src/column_block.h"

namespace byteslice{

/**
  Store data in a naive array.
*/
template <typename DTYPE>
class NaiveColumnBlock: public ColumnBlock{
public:
    NaiveColumnBlock(size_t num=kNumTuplesPerBlock);
    virtual ~NaiveColumnBlock();

    WordUnit GetTuple(size_t pos_in_block) const override;
    void SetTuple(size_t pos_in_block, WordUnit value) override;
    
    void Scan(Comparator comparator, WordUnit literal, BitVectorBlock* bv_block,
            Bitwise bit_opt=Bitwise::kSet) const override;
    void Scan(Comparator comparator, const ColumnBlock* column_block,
            BitVectorBlock* bv_block, Bitwise bit_opti=Bitwise::kSet) const override;
    void BulkLoadArray(const WordUnit* codes, size_t num, size_t start_pos=0) override;

    void SerToFile(SequentialWriteBinaryFile &file) const override;
    void DeserFromFile(const SequentialReadBinaryFile &file) override;
    bool Resize(size_t size) override;

private:
    DTYPE* data_;
    //scan helper: against a given literal
    template <Comparator CMP>
    void ScanHelper1(WordUnit literal, BitVectorBlock* bv_block, Bitwise bit_opt) const;
    template <Comparator CMP, Bitwise OPT>
    void ScanHelper2(WordUnit literal, BitVectorBlock* bv_block) const;
    //scan helper: against another column_block
    template <Comparator CMP>
    void ScanHelper1(const ColumnBlock* colblock, BitVectorBlock* bvblock, Bitwise bit_opt) const;
    template <Comparator CMP, Bitwise OPT>
    void ScanHelper2(const ColumnBlock* colblock, BitVectorBlock* bvblock) const;

};

template <typename DTYPE>
inline WordUnit NaiveColumnBlock<DTYPE>::GetTuple(size_t pos_in_block) const{
    return static_cast<WordUnit>(data_[pos_in_block]);
}

template <typename DTYPE>
inline void NaiveColumnBlock<DTYPE>::SetTuple(size_t pos_in_block, WordUnit value){
    data_[pos_in_block] = static_cast<DTYPE>(value);
}


}   // namespace
#endif  //NAIVE_COLUMN_BLOCK_H
