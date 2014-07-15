/*******************************************************************************
 * Copyright (c) 2015
 * The Hong Kong Polytechnic University, Database Group
 *
 * Author: Ziqiang Feng (cszqfeng AT comp DOT polyu.edu.hk)
 *
 * See file LICENSE.md for details.
 *******************************************************************************/
#ifndef     PARAM_H
#define     PARAM_H

namespace byteslice{

constexpr size_t kNumTuplesPerBlock = 1024*1024;    // each block contains 1M tuples

}   // namespace

#endif  // PARAM_H
