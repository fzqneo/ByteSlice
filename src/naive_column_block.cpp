/*******************************************************************************
 * Copyright (c) 2015
 * The Hong Kong Polytechnic University, Database Group
 *
 * Author: Ziqiang Feng (cszqfeng AT comp DOT polyu.edu.hk)
 *
 * See file LICENSE.md for details.
 *******************************************************************************/
#include "naive_column_block.h"

#include	<cassert>
#include    <cstring>

namespace byteslice{

template <typename DTYPE>
NaiveColumnBlock<DTYPE>::NaiveColumnBlock(size_t num):
    ColumnBlock(ColumnType::kNaive, sizeof(DTYPE)*8, num){
        data_ = new DTYPE[kNumTuplesPerBlock];
        memset(data_, 0x0, sizeof(DTYPE)*kNumTuplesPerBlock);
}

template <typename DTYPE>
NaiveColumnBlock<DTYPE>::~NaiveColumnBlock(){
    delete[] data_;
}

template <typename DTYPE>
bool NaiveColumnBlock<DTYPE>::Resize(size_t num){
    num_tuples_ = num;
    return true;
}

template <typename DTYPE>
void NaiveColumnBlock<DTYPE>::SerToFile(SequentialWriteBinaryFile &file) const{
    file.Append(&num_tuples_, sizeof(num_tuples_));
    file.Append(data_, sizeof(DTYPE)*kNumTuplesPerBlock);
}

template <typename DTYPE>
void NaiveColumnBlock<DTYPE>::DeserFromFile(const SequentialReadBinaryFile &file){
    file.Read(&num_tuples_, sizeof(num_tuples_));
    file.Read(data_, sizeof(DTYPE)*kNumTuplesPerBlock);
}

//Scan against a literal
template <typename DTYPE>
void NaiveColumnBlock<DTYPE>::Scan(Comparator comparator, WordUnit literal, 
        BitVectorBlock* bv_block, Bitwise bit_opt) const{
    assert(bv_block->num() == num_tuples_);
    switch(comparator){
        case Comparator::kLess:
            return ScanHelper1<Comparator::kLess>(literal, bv_block, bit_opt);
        case Comparator::kGreater:
            return ScanHelper1<Comparator::kGreater>(literal, bv_block, bit_opt);
        case Comparator::kLessEqual:
            return ScanHelper1<Comparator::kLessEqual>(literal, bv_block, bit_opt);
        case Comparator::kGreaterEqual:
            return ScanHelper1<Comparator::kGreaterEqual>(literal, bv_block, bit_opt);
        case Comparator::kEqual:
            return ScanHelper1<Comparator::kEqual>(literal, bv_block, bit_opt);
        case Comparator::kInequal:
            return ScanHelper1<Comparator::kInequal>(literal, bv_block, bit_opt);
    }

}

template <typename DTYPE>
template <Comparator CMP>
void NaiveColumnBlock<DTYPE>::ScanHelper1(WordUnit literal, BitVectorBlock* bv_block, 
        Bitwise bit_opt) const{
    switch(bit_opt){
        case Bitwise::kSet:
            return ScanHelper2<CMP, Bitwise::kSet>(literal, bv_block);
        case Bitwise::kAnd:
            return ScanHelper2<CMP, Bitwise::kAnd>(literal, bv_block);
        case Bitwise::kOr:
            return ScanHelper2<CMP, Bitwise::kOr>(literal, bv_block);
    }
}

template <typename DTYPE>
template <Comparator CMP, Bitwise OPT>
void NaiveColumnBlock<DTYPE>::ScanHelper2(WordUnit literal, BitVectorBlock* bv_block) const{
    //Do the real work here
    DTYPE lit = static_cast<DTYPE>(literal);
    for(size_t offset = 0; offset < num_tuples_; offset += kNumWordBits){
        WordUnit word = 0;
        for(size_t i = 0; i < kNumWordBits; i++){
            size_t pos = offset + i;
            if(pos >= num_tuples_){
                break;
            }

            WordUnit bit;
            switch(CMP){
                case Comparator::kLess:
                    bit = (data_[pos] < lit);
                    break;
                case Comparator::kGreater:
                    bit = (data_[pos] > lit);
                    break;
                case Comparator::kLessEqual:
                    bit = (data_[pos] <= lit);
                    break;
                case Comparator::kGreaterEqual:
                    bit = (data_[pos] >= lit);
                    break;
                case Comparator::kEqual:
                    bit = (data_[pos] == lit);
                    break;
                case Comparator::kInequal:
                    bit = (data_[pos] != lit);
                    break;
            }

            //word |= (bit << (kNumWordBits -1 - i));
            word |= (bit << i);
        }
        size_t bv_word_id = offset / kNumWordBits;
        WordUnit x;
        switch(OPT){
            case Bitwise::kSet:
                x = word;
                break;
            case Bitwise::kAnd:
                x = bv_block->GetWordUnit(bv_word_id);
                x &= word;
                break;
            case Bitwise::kOr:
                x = bv_block->GetWordUnit(bv_word_id);
                x |= word;
                break;
        }
        bv_block->SetWordUnit(x, bv_word_id);
    }

}

//Scan against another column block
template <typename DTYPE>
void NaiveColumnBlock<DTYPE>::Scan(Comparator comparator, const ColumnBlock* column_block, 
        BitVectorBlock* bv_block, Bitwise bit_opt) const{
    assert(column_block->type() == type_);
    assert(column_block->num_tuples() == num_tuples_);
    assert(column_block->bit_width() == bit_width_);

    switch(comparator){
        case Comparator::kLess:
            return ScanHelper1<Comparator::kLess>(column_block, bv_block, bit_opt);
        case Comparator::kGreater:
            return ScanHelper1<Comparator::kGreater>(column_block, bv_block, bit_opt);
        case Comparator::kLessEqual:
            return ScanHelper1<Comparator::kLessEqual>(column_block, bv_block, bit_opt);
        case Comparator::kGreaterEqual:
            return ScanHelper1<Comparator::kGreaterEqual>(column_block, bv_block, bit_opt);
        case Comparator::kEqual:
            return ScanHelper1<Comparator::kEqual>(column_block, bv_block, bit_opt);
        case Comparator::kInequal:
            return ScanHelper1<Comparator::kInequal>(column_block, bv_block, bit_opt);
    }
}

template <typename DTYPE>
template <Comparator CMP>
void NaiveColumnBlock<DTYPE>::ScanHelper1(const ColumnBlock* colblock,
        BitVectorBlock* bvblock, Bitwise bit_opt) const{
    switch(bit_opt){
        case Bitwise::kSet:
            return ScanHelper2<CMP, Bitwise::kSet>(colblock, bvblock);
        case Bitwise::kAnd:
            return ScanHelper2<CMP, Bitwise::kAnd>(colblock, bvblock);
        case Bitwise::kOr:
            return ScanHelper2<CMP, Bitwise::kOr>(colblock, bvblock);
    }
}

template <typename DTYPE>
template <Comparator CMP, Bitwise OPT>
void NaiveColumnBlock<DTYPE>::ScanHelper2(const ColumnBlock* colblock, 
                                        BitVectorBlock* bvblock) const{
    //DO the real work real
    for(size_t offset = 0; offset < num_tuples_; offset += kNumWordBits){
        WordUnit word = 0;
        for(size_t i = 0; i < kNumWordBits; i++){
            size_t pos = offset + i;
            if(pos >= num_tuples_){
                break;
            }

            WordUnit bit;
            DTYPE lit = static_cast<DTYPE>(colblock->GetTuple(pos));
            switch(CMP){
                case Comparator::kLess:
                    bit = (data_[pos] < lit);
                    break;
                case Comparator::kGreater:
                    bit = (data_[pos] > lit);
                    break;
                case Comparator::kLessEqual:
                    bit = (data_[pos] <= lit);
                    break;
                case Comparator::kGreaterEqual:
                    bit = (data_[pos] >= lit);
                    break;
                case Comparator::kEqual:
                    bit = (data_[pos] == lit);
                    break;
                case Comparator::kInequal:
                    bit = (data_[pos] != lit);
                    break;
            }

            //word |= (bit << (kNumWordBits -1 - i));
            word |= (bit << i);
        }
        size_t bv_word_id = offset / kNumWordBits;
        WordUnit x;
        switch(OPT){
            case Bitwise::kSet:
                x = word;
                break;
            case Bitwise::kAnd:
                x = bvblock->GetWordUnit(bv_word_id);
                x &= word;
                break;
            case Bitwise::kOr:
                x = bvblock->GetWordUnit(bv_word_id);
                x |= word;
                break;
        }
        bvblock->SetWordUnit(x, bv_word_id);
    }
}

template <typename DTYPE>
void NaiveColumnBlock<DTYPE>::BulkLoadArray(const WordUnit* codes, size_t num, 
        size_t start_pos){
    assert(start_pos + num <= num_tuples_);
    for(size_t i = 0; i < num; i++){
        data_[start_pos+i] = static_cast<DTYPE>(codes[i]);
    }
}


template class NaiveColumnBlock<uint8_t>;
template class NaiveColumnBlock<uint16_t>;
template class NaiveColumnBlock<uint32_t>;
template class NaiveColumnBlock<uint64_t>;

}   // namespace byteslice
