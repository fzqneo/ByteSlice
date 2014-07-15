/*******************************************************************************
 * Copyright (c) 2015
 * The Hong Kong Polytechnic University, Database Group
 *
 * Author: Ziqiang Feng (cszqfeng AT comp DOT polyu.edu.hk)
 *
 * See file LICENSE.md for details.
 *******************************************************************************/
#ifndef BYTESLICE_COLUMN_BLOCK_H
#define BYTESLICE_COLUMN_BLOCK_H

#include "../src/avx-utility.h"
#include "../src/column_block.h"

namespace byteslice{


/**
Warning:
    Bytes are FLIPPED in internal storage to preserve order.
*/

static constexpr size_t kMemSizePerByteSlice = 
    sizeof(ByteUnit)*CEIL(kNumTuplesPerBlock, kNumAvxBits/8)*(kNumAvxBits/8);

template <size_t BIT_WIDTH, Direction PDIRECTION = Direction::kRight>
class ByteSliceColumnBlock: public ColumnBlock{
public:
    ByteSliceColumnBlock(size_t num=kNumTuplesPerBlock);
    virtual ~ByteSliceColumnBlock();

    WordUnit GetTuple(size_t pos) const override;
    void SetTuple(size_t pos, WordUnit value) override;

    void Scan(Comparator comparator, WordUnit literal, BitVectorBlock* bvblock,
            Bitwise bit_opt = Bitwise::kSet) const override;
    void Scan(Comparator comparator, const ColumnBlock* other_block,
            BitVectorBlock* bvblock, Bitwise bit_opt = Bitwise::kSet) const override;

    void BulkLoadArray(const WordUnit* codes, size_t num, size_t start_pos = 0) override;

    void SerToFile(SequentialWriteBinaryFile &file) const override;
    void DeserFromFile(const SequentialReadBinaryFile &file) override;
    bool Resize(size_t size) override;

    Direction GetPadDirection();
    
private:
    //Scan Helper: literal
    template <Comparator CMP>
    void ScanHelper1(WordUnit literal, BitVectorBlock* bvblock, Bitwise bit_opt) const;
    template <Comparator CMP, Bitwise OPT>
    void ScanHelper2(WordUnit literal, BitVectorBlock* bvblock) const;

    //Scan Helper: other block
    template <Comparator CMP>
    void ScanHelper1(const ByteSliceColumnBlock<BIT_WIDTH, PDIRECTION>* other_block,
                            BitVectorBlock* bvblock, Bitwise bit_opt) const;
    template <Comparator CMP, Bitwise OPT>
    void ScanHelper2(const ByteSliceColumnBlock<BIT_WIDTH, PDIRECTION>* other_block,
                            BitVectorBlock* bvblock) const;


    //Scan Kernel
    template <Comparator CMP>
    inline void ScanKernel(const AvxUnit &byteslice1, const AvxUnit &byteslice2,
            AvxUnit &mask_less, AvxUnit &mask_greater, AvxUnit &mask_equal) const;
    template <Comparator CMP, size_t BYTE_ID>
    inline void ScanKernel2(const AvxUnit &byteslice1, const AvxUnit &byteslice2,
            AvxUnit &mask_less, AvxUnit &mask_greater, AvxUnit &mask_equal) const;

    static constexpr size_t kNumBytesPerCode = CEIL(BIT_WIDTH, 8);
    static constexpr size_t kNumPaddingBits = kNumBytesPerCode * 8 - BIT_WIDTH;
    static constexpr Direction kPadDirection = PDIRECTION;
    static constexpr WordUnit kCodeMask = (1ULL << BIT_WIDTH) - 1;

    ByteUnit* data_[4];


};

template <size_t BIT_WIDTH, Direction PDIRECTION>
inline WordUnit ByteSliceColumnBlock<BIT_WIDTH, PDIRECTION>::GetTuple(size_t pos) const{
    WordUnit ret = 0ULL;
    switch(kNumBytesPerCode){
        case 4:
            ret = (static_cast<WordUnit>(FLIP(data_[0][pos])) << 24) |
                    (static_cast<WordUnit>(FLIP(data_[1][pos])) << 16) |
                    (static_cast<WordUnit>(FLIP(data_[2][pos])) << 8) |
                    static_cast<WordUnit>(FLIP(data_[3][pos]));
            break;
        case 3:
            ret = (static_cast<WordUnit>(FLIP(data_[0][pos])) << 16) |
                    (static_cast<WordUnit>(FLIP(data_[1][pos])) << 8) |
                    static_cast<WordUnit>(FLIP(data_[2][pos]));
            break;
        case 2:
            ret = (static_cast<WordUnit>(FLIP(data_[0][pos])) << 8) |
                    static_cast<WordUnit>(FLIP(data_[1][pos]));
            break;
        case 1:
            ret = static_cast<WordUnit>(FLIP(data_[0][pos]));
            break;
    }
    switch(PDIRECTION){
        case Direction::kRight:
            ret >>= kNumPaddingBits;
            break;
        case Direction::kLeft:
            break;
    }
    return ret;
}

template <size_t BIT_WIDTH, Direction PDIRECTION>
inline void ByteSliceColumnBlock<BIT_WIDTH, PDIRECTION>::SetTuple(size_t pos, WordUnit value){
    switch(PDIRECTION){
        case Direction::kRight:
            value <<= kNumPaddingBits;
            break;
        case Direction::kLeft:
            break;
    }

    switch(kNumBytesPerCode){
        case 4:
            data_[0][pos] = FLIP(static_cast<ByteUnit>(value >> 24));
            data_[1][pos] = FLIP(static_cast<ByteUnit>(value >> 16));
            data_[2][pos] = FLIP(static_cast<ByteUnit>(value >> 8));
            data_[3][pos] = FLIP(static_cast<ByteUnit>(value));
            break;
        case 3:
            data_[0][pos] = FLIP(static_cast<ByteUnit>(value >> 16));
            data_[1][pos] = FLIP(static_cast<ByteUnit>(value >> 8));
            data_[2][pos] = FLIP(static_cast<ByteUnit>(value));
            break;
        case 2:
            data_[0][pos] = FLIP(static_cast<ByteUnit>(value >> 8));
            data_[1][pos] = FLIP(static_cast<ByteUnit>(value));
            break;
        case 1:
            data_[0][pos] = FLIP(static_cast<ByteUnit>(value));
            break;
    }
}

}   // namespace
#endif
