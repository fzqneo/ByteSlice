/*******************************************************************************
 * Copyright (c) 2015
 * The Hong Kong Polytechnic University, Database Group
 *
 * Author: Ziqiang Feng (cszqfeng AT comp DOT polyu.edu.hk)
 *
 * See file LICENSE.md for details.
 *******************************************************************************/
#ifndef COLUMN_H
#define COLUMN_H


#include    <string>
#include    <vector>

#include 	"bitvector.h"
#include 	"column_block.h"
#include 	"param.h"
#include 	"sequential_binary_file.h"
#include 	"types.h"

namespace byteslice{

class BitVector;

class Column{
public:
    Column(ColumnType type, size_t bit_width, size_t num=0);
    ~Column();
    void Destroy();    

    WordUnit GetTuple(size_t id) const;
    void SetTuple(size_t id, WordUnit value);
    void Resize(size_t num);

    void SerToFile(SequentialWriteBinaryFile &file) const;
    void DeserFromFile(const SequentialReadBinaryFile &file);

    /**
     * @brief Load the column from a projection file in text format.
     * One integer per line.
     */
    size_t LoadTextFile(std::string filepath);

    /**
     * @brief Load the column from a C-array.
     */
    void BulkLoadArray(const WordUnit* codes, size_t num, size_t pos=0);

    void Scan(Comparator comparator, WordUnit literal,
            BitVector* bitvector, Bitwise bit_opt = Bitwise::kSet) const;
    void Scan(Comparator comparator, const Column* other_column, 
            BitVector* bitvector, Bitwise bit_opt = Bitwise::kSet) const;

    ColumnBlock* CreateNewBlock() const;

    size_t GetNumTuples() const { return num_tuples_;}
    size_t GetBitWidth() const { return bit_width_;}
    ColumnType GetType() const { return type_;}
    size_t GetNumBlocks() const { return blocks_.size();}
    ColumnBlock* GetBlock(size_t block_id) const {return blocks_[block_id];}

private:
    ColumnType type_;
    size_t bit_width_;
    size_t num_tuples_;
    std::vector<ColumnBlock*> blocks_;
};


}   // namespace

#endif  //COLUMN_H
