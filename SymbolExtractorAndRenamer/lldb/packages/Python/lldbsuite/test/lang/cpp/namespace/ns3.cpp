//===-- ns3.cpp ------------------------------------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "ns.h"
extern int func();

// Note: the following function must be before the using.
void test_lookup_before_using_directive()
{
    // BP_before_using_directive
    printf("before using directive: func() = %d\n", func()); // eval func(), exp: 1
}
using namespace A;
void test_lookup_after_using_directive()
{
    // BP_after_using_directive
    //printf("func() = %d\n", func()); // eval func(), exp: error, amiguous
    printf("after using directive: func2() = %d\n", func2()); // eval func2(), exp: 3
    printf("after using directive: ::func() = %d\n", ::func()); // eval ::func(), exp: 1
    printf("after using directive: B::func() = %d\n", B::func()); // eval B::func(), exp: 4
}
