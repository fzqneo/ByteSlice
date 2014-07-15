/*******************************************************************************
 * Copyright (c) 2015
 * The Hong Kong Polytechnic University, Database Group
 *
 * Author: Ziqiang Feng (cszqfeng AT comp DOT polyu.edu.hk)
 *
 * See file LICENSE.md for details.
 *******************************************************************************/
#ifndef     MACROS_H
#define     MACROS_H

#include    <x86intrin.h>
#include	<cassert>

#define CEIL(X,Y) (((X)-1) / (Y) + 1)

#define POPCNT64(X) (_mm_popcnt_u64(X))

#endif  // MACROS_H
