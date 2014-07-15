/*******************************************************************************
 * Copyright (c) 2015
 * The Hong Kong Polytechnic University, Database Group
 *
 * Author: Ziqiang Feng (cszqfeng AT comp DOT polyu.edu.hk)
 *
 * See file LICENSE.md for details.
 *******************************************************************************/
#include "types.h"

#include "column.h"

namespace byteslice{

std::ostream& operator<< (std::ostream &out, ColumnType type){
    switch(type){
        case ColumnType::kNaive:
            out << "Naive";
            break;
       case ColumnType::kByteSlicePadRight:
            out << "ByteSlicePadRight";
            break;
        case ColumnType::kByteSlicePadLeft:
            out << "ByteSlicePadLeft";
            break;
    }
    return out;
}

std::ostream& operator<< (std::ostream &out, Comparator comp){
    switch(comp){
        case Comparator::kEqual:
            out << "Equal";
            break;
        case Comparator::kInequal:
            out << "Inequal";
            break;
        case Comparator::kLess:
            out << "Less";
            break;
        case Comparator::kGreater:
            out << "Greater";
            break;
        case Comparator::kLessEqual:
            out << "LessEqual";
            break;
        case Comparator::kGreaterEqual:
            out << "GreaterEqual";
            break;
    }
    return out;
}


}   // namespace
