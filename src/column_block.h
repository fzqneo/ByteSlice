/*******************************************************************************
 * Copyright (c) 2015
 * The Hong Kong Polytechnic University, Database Group
 *
 * Author: Ziqiang Feng (cszqfeng AT comp DOT polyu.edu.hk)
 *
 * See file LICENSE.md for details.
 *******************************************************************************/
#ifndef     COLUMN_BLOCK_H
#define     COLUMN_BLOCK_H

#include "../src/bitvector_block.h"
#include "../src/macros.h"
#include "../src/param.h"
#include "../src/sequential_binary_file.h"
#include "../src/types.h"

namespace byteslice{

class ColumnBlock{
public:
    virtual ~ColumnBlock(){
    }

    virtual WordUnit GetTuple(size_t pos_in_block) const = 0;
    virtual void SetTuple(size_t pos_in_block, WordUnit value) = 0;
    virtual void Scan(Comparator comparator, WordUnit literal, BitVectorBlock* bv_block, Bitwise bit_opt=Bitwise::kSet) const = 0;
    virtual void Scan(Comparator comparator, const ColumnBlock* column_block, BitVectorBlock* bv_block, Bitwise bit_opti=Bitwise::kSet) const = 0;
    virtual void BulkLoadArray(const WordUnit* codes, size_t num, size_t start_pos=0) = 0;
    virtual void SerToFile(SequentialWriteBinaryFile &file) const = 0;
    virtual void DeserFromFile(const SequentialReadBinaryFile &file) = 0;
    virtual bool Resize(size_t size) = 0;

    //accessors
    ColumnType type() const;
    size_t bit_width() const;
    size_t num_tuples() const;


protected:
    ColumnBlock(ColumnType type, size_t bit_width, size_t num):
        type_(type), bit_width_(bit_width), num_tuples_(num){
    }
    const ColumnType type_;
    const size_t bit_width_;
    size_t num_tuples_;
    

};

inline ColumnType ColumnBlock::type() const{
    return type_;
}

inline size_t ColumnBlock::bit_width() const{
    return bit_width_;
}

inline size_t ColumnBlock::num_tuples() const{
    return num_tuples_;
}

}

#endif  //COLUMN_BLOCK_H
