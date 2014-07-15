/*******************************************************************************
 * Copyright (c) 2015
 * The Hong Kong Polytechnic University, Database Group
 *
 * Author: Ziqiang Feng (cszqfeng AT comp DOT polyu.edu.hk)
 *
 * See file LICENSE.md for details.
 *******************************************************************************/
#include 	"column.h"

#include    <algorithm>
#include    <fstream>
#include    <iostream>
#include    <omp.h>

#include 	"byteslice_column_block.h"
#include 	"naive_column_block.h"

namespace byteslice {

Column::Column(ColumnType type, size_t bit_width, size_t num) :
		type_(type), bit_width_(bit_width), num_tuples_(num) {

	for (size_t count = 0; count < num; count += kNumTuplesPerBlock) {
		ColumnBlock* new_block = CreateNewBlock();
		new_block->Resize(std::min(kNumTuplesPerBlock, num - count));
		blocks_.push_back(new_block);
	}
}

Column::~Column() {
	Destroy();
}

void Column::Destroy() {
	while (!blocks_.empty()) {
		delete blocks_.back();
		blocks_.pop_back();
	}
}

WordUnit Column::GetTuple(size_t id) const {
	assert(id < num_tuples_);
	size_t block_id = id / kNumTuplesPerBlock;
	size_t pos_in_block = id % kNumTuplesPerBlock;
	return blocks_[block_id]->GetTuple(pos_in_block);
}

void Column::SetTuple(size_t id, WordUnit value) {
	size_t block_id = id / kNumTuplesPerBlock;
	size_t pos_in_block = id % kNumTuplesPerBlock;
	blocks_[block_id]->SetTuple(pos_in_block, value);
}

size_t Column::LoadTextFile(std::string filepath) {
	std::ifstream infile;
	infile.open(filepath, std::ifstream::in);
	if (!infile.good()) {
		std::cerr << "Can't open file: " << filepath << std::endl;
		return -1;
	}
	WordUnit val;
	size_t id = 0;
	for (id = 0; (id < GetNumTuples()) && (infile >> val); id++) {
		SetTuple(id, val);
	}
	infile.close();
	return id;
}

void Column::Resize(size_t num) {
	num_tuples_ = num;
	const size_t new_num_blocks = CEIL(num, kNumTuplesPerBlock);
	const size_t old_num_blocks = blocks_.size();
	if (new_num_blocks > old_num_blocks) {    // need to add blocks
		// fill up the last block
		blocks_[old_num_blocks - 1]->Resize(kNumTuplesPerBlock);
		// append new blocks
		for (size_t bid = old_num_blocks; bid < new_num_blocks; bid++) {
			ColumnBlock* new_block = CreateNewBlock();
			new_block->Resize(kNumTuplesPerBlock);
			blocks_.push_back(new_block);
		}
	} else if (new_num_blocks < old_num_blocks) {   // need to remove blocks
		for (size_t bid = old_num_blocks - 1; bid > new_num_blocks; bid--) {
			delete blocks_.back();
			blocks_.pop_back();
		}
	}
	// now the number of block is desired
	// correct the size of the last block
	size_t num_tuples_last_block = num % kNumTuplesPerBlock;
	if (0 < num_tuples_last_block) {
		blocks_.back()->Resize(num_tuples_last_block);
	}

	assert(blocks_.size() == new_num_blocks);
}

void Column::SerToFile(SequentialWriteBinaryFile &file) const {
	for (auto block : blocks_) {
		block->SerToFile(file);
	}
}

void Column::DeserFromFile(const SequentialReadBinaryFile &file) {
	for (auto block : blocks_) {
		block->DeserFromFile(file);
	}
}

void Column::BulkLoadArray(const WordUnit* codes, size_t num, size_t pos) {
	assert(pos + num <= num_tuples_);
	size_t block_id = pos / kNumTuplesPerBlock;
	size_t pos_in_block = pos % kNumTuplesPerBlock;
	size_t num_remain_tuples = num;
	const WordUnit* data_ptr = codes;
	while (num_remain_tuples > 0) {
		size_t size = std::min(blocks_[block_id]->num_tuples() - pos_in_block,
				num_remain_tuples);
		blocks_[block_id]->BulkLoadArray(data_ptr, size, pos_in_block);
		data_ptr += size;
		num_remain_tuples -= size;
		pos_in_block = 0;
		block_id++;
	}
}

void Column::Scan(Comparator comparator, WordUnit literal, BitVector* bitvector,
		Bitwise bit_opt) const {

	assert(num_tuples_ == bitvector->num());

#pragma omp parallel for schedule(dynamic)
	for (size_t block_id = 0; block_id < blocks_.size(); block_id++) {

		blocks_[block_id]->Scan(comparator, literal,
				bitvector->GetBVBlock(block_id), bit_opt);
	}
}

void Column::Scan(Comparator comparator, const Column* other_column,
		BitVector* bitvector, Bitwise bit_opt) const {
	assert(num_tuples_ == bitvector->num());
	assert(type_ == other_column->GetType());
	assert(bit_width_ == other_column->GetBitWidth());
	assert(num_tuples_ == other_column->GetNumTuples());

#pragma omp parallel for schedule(dynamic)
	for (size_t block_id = 0; block_id < blocks_.size(); block_id++) {
		blocks_[block_id]->Scan(comparator, other_column->blocks_[block_id],
				bitvector->GetBVBlock(block_id), bit_opt);
	}

}

ColumnBlock* Column::CreateNewBlock() const {
	assert(0 < bit_width_ && 32 >= bit_width_);
	if (!(0 < bit_width_ && 32 >= bit_width_)) {
		std::cerr << "[FATAL] Incorrect bit width: " << bit_width_ << std::endl;
		exit(1);
	}

	switch (type_) {
	case ColumnType::kNaive:
		switch (CEIL(bit_width_, 8)) {
		case 1:
			return new NaiveColumnBlock<uint8_t>();
		case 2:
			return new NaiveColumnBlock<uint16_t>();
		case 3:
		case 4:
			return new NaiveColumnBlock<uint32_t>();
		}
		break;
	case ColumnType::kByteSlicePadRight:
		switch (bit_width_) {
		case 1:
			return new ByteSliceColumnBlock<1>();
		case 2:
			return new ByteSliceColumnBlock<2>();
		case 3:
			return new ByteSliceColumnBlock<3>();
		case 4:
			return new ByteSliceColumnBlock<4>();
		case 5:
			return new ByteSliceColumnBlock<5>();
		case 6:
			return new ByteSliceColumnBlock<6>();
		case 7:
			return new ByteSliceColumnBlock<7>();
		case 8:
			return new ByteSliceColumnBlock<8>();
		case 9:
			return new ByteSliceColumnBlock<9>();
		case 10:
			return new ByteSliceColumnBlock<10>();
		case 11:
			return new ByteSliceColumnBlock<11>();
		case 12:
			return new ByteSliceColumnBlock<12>();
		case 13:
			return new ByteSliceColumnBlock<13>();
		case 14:
			return new ByteSliceColumnBlock<14>();
		case 15:
			return new ByteSliceColumnBlock<15>();
		case 16:
			return new ByteSliceColumnBlock<16>();
		case 17:
			return new ByteSliceColumnBlock<17>();
		case 18:
			return new ByteSliceColumnBlock<18>();
		case 19:
			return new ByteSliceColumnBlock<19>();
		case 20:
			return new ByteSliceColumnBlock<20>();
		case 21:
			return new ByteSliceColumnBlock<21>();
		case 22:
			return new ByteSliceColumnBlock<22>();
		case 23:
			return new ByteSliceColumnBlock<23>();
		case 24:
			return new ByteSliceColumnBlock<24>();
		case 25:
			return new ByteSliceColumnBlock<25>();
		case 26:
			return new ByteSliceColumnBlock<26>();
		case 27:
			return new ByteSliceColumnBlock<27>();
		case 28:
			return new ByteSliceColumnBlock<28>();
		case 29:
			return new ByteSliceColumnBlock<29>();
		case 30:
			return new ByteSliceColumnBlock<30>();
		case 31:
			return new ByteSliceColumnBlock<31>();
		case 32:
			return new ByteSliceColumnBlock<32>();
		}
		break;
	default:
		std::cerr << "[FATAL] Unknown column type." << std::endl;
		exit(1);
	}

	return nullptr;
}

}   // namespace
