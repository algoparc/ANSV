//// Copyright (c) 2012 Fuyao Zhao, Julian Shun
//// Copyright (c) 2024 Rolf Svenning
////
//// Permission is hereby granted, free of charge, to any person obtaining a
//// copy of this software and associated documentation files (the
//// "Software"), to deal in the Software without restriction, including
//// without limitation the rights (to use, copy, modify, merge, publish,
//// distribute, sublicense, and/or sell copies of the Software, and to
//// permit persons to whom the Software is furnished to do so, subject to
//// the following conditions:
////
//// The above copyright notice and this permission notice shall be included
//// in all copies or substantial portions of the Software.
////
//// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
//// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
//// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
//// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
//// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
//// OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
//// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#ifndef ANSV2_SHUNZHAOORIGINAL_H
#define ANSV2_SHUNZHAOORIGINAL_H

#include "parlay/parallel.h"
#include "parlay/sequence.h"

long getLeft_opt(long **table, long depth, long n, long index, long start);
long getRight_opt(long **table, long depth, long n, long index, long start);
void ComputeANSV_Linear(long a[], long n, long leftElements[], long rightElements[], long offset);
long cflog2(long i);
std::string ANSV_ShunZhao(long *A, long n, long *left, long *right, long blockSize, bool usingHeuristic);

// Refactored for BSV
std::tuple<long **, long> createBinaryTree(long *A, long n);

#endif
