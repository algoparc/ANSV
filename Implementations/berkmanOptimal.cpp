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

#include "berkmanOptimal.h"
#include "parlay/parallel.h"
#include "parlay/primitives.h"
#include "shunZhaoOriginal.h"

using namespace std;
using namespace parlay;

long findRepresentative(long *A, long i, long j) {
    long i1 = i;
    for (long k = i + 1; k < j; ++k) {
        if (A[k] < A[i1]) i1 = k;
    }
    return i1;
}

void farAwayBlocks_ANSV_linear(long *A, long a, long b, long c, long d, long *L, long *R) {
    if (a == -1 or d == -1) return;

    // FIND MATCHES BY MERGING
    long i = b - 1;
    long j = c;
    while(i > a or j < d - 1) {
        if (A[i] < A[j]) {
            if (L[j] == -1) L[j] = i;
            j++;
        }
        else {
            if (R[i] == -1) R[i] = j;
            i--;
        }
    }
}


tuple<long, long> findLeftAndRightMatch(long n, long **T, long d, long *L, long *R, int i){
    long iL = getLeft_opt( T, d, n, i, i);
    long iR = getRight_opt(T, d, n, i, i);
    L[i] = iL;
    R[i] = iR;
    return {iL, iR};
}


void adjacentMergeBOTH(long *A, long n, long *L, long *R, long offset) {
    long i, top;
    long *stack = new long[n];

    for (i = offset, top = -1; i < n + offset; i++) {
        while (top > -1 && A[stack[top]] > A[i]) top--;
        if (top != -1) L[i] = stack[top];
        stack[++top] = i;
    }

    for (i = n - 1 + offset, top = -1; i >= offset; i--) {
        while (top > -1 && A[stack[top]] > A[i]) top--;
        if (top != -1) R[i] = stack[top];
        stack[++top] = i;
    }
    delete[] stack;
}


string ANSV_Berkman(long *A, long n, long *L, long *R, long blockSize){
    internal::timer t("BERKMAN");
    auto *times = new string[3];
    t.start();

    // ------------ CREATE BINARY TREE ------------
    // 1) Create a min-binary tree for the input

    auto [T,d] = createBinaryTree(A, n);

    // TIMING 1
    times[0] = "Binary tree: " + to_string(t.next_time()) + "\n";

    // ------------ LOCAL MATCHES & REPRESENTATIVES ------------
    //  For each group:
    //    a) Find local matches in each group of size 'blockSize'
    //    b) Find representative ri, that is, the (index of) smallest value in each group
    //    c) Find matches of ri. That is, left match b1 and right match b2.
    //    d) Mark borders b1 and b2 in B to be matched later.
    //
    // Notes: representatives are stored in an array REPs of size ceil(n / blockSize) [r1, r2, ...]
    // Notes: borders are stored in an array B of size 2 * ceil(n / blockSize) [(i11, i12), (i21, i22), ...]

    long blockCount = (long)ceilf((float)n / (float)blockSize); // really?
    long *REPs = new long[blockCount];
    long *B = new long[2 * blockCount];
    blocked_for(0, n, blockSize, [&] (size_t blockNumber, long i, long j) {
        // LOCAL MATCHES
        ComputeANSV_Linear(A + i, j - i, L + i, R + i, i);

        // REPRESENTATIVES
        auto ri = findRepresentative(A, i, j);
        REPs[blockNumber] = ri;

        // MARK BORDERS
        auto  [b1, b2] = findLeftAndRightMatch(n, T, d, L, R, ri);
        B[2 * blockNumber] =  b1;
        B[2 * blockNumber + 1] = b2;
    });

    // TIMING 2
    times[1] = "Local matches: " + to_string(t.next_time()) + "\n";

    // ------------ NONLOCAL MATCHES BY MERGING ------------
    // 1) For each group BC:
    //    a) Find the blocks (BL, BR) matched by the representatives
    //    b) If BL is adjacent to BC, then find local matches by merging A[b1:r1] (or using a stack)
    //    c) Repeat b) for BR symmetrically
    //    d) Lookup in REPs the index bBR of the left match of rBR
    //       two cases:
    //         i  ) bBR is not in BL. Then do nothing
    //         ii ) bBR is in BL (where rBL <= bBR). Find nonlocal matches in ]BL_r2, iRL[, ]iR, BR_r1[ by merging
    //    f) Repeat d) for bBL symmetrically
    //
    // Notes:
    // b1:  left match of ri of BC
    // bBL: right match of rBL
    // b2:  right match of ri of BG
    // bBR: left match of rBR
    // BL[.rBL....bBR.b1...] BC[...ri..] BR[...bBL.b2.rBR.]

    blocked_for(0, n, blockSize, [&] (size_t BCi, long i, long j) {
        // SETUP
        auto ri = REPs[BCi];
        auto [b1, b2]   = tuple{B[2 * BCi], B[2 * BCi + 1]};
        auto [BLi, BRi] = tuple{b1 / blockSize, b2 / blockSize};
        auto [rBL, rBR] = tuple{REPs[BLi], REPs[BRi]};

        // ADJACENT MERGING
        if (BLi + 1 == BCi and b1 != -1) adjacentMergeBOTH(A, ri - b1 + 1, L, R, b1);
        if (BRi - 1 == BCi and b2 != -1) adjacentMergeBOTH(A, b2 - ri + 1, L, R, ri);

        // FAR-AWAY MERGING
        if (b1 == -1 or b2 == -1) return; // no far-away merging
        auto [bBL, bBR] = tuple{B[2 * BLi + 1], B[2 * BRi]};
        // block BL matched by both BC and BR
        if (BLi == bBR / blockSize) farAwayBlocks_ANSV_linear(A, bBR, b1 + 1, b2, rBR + 1, L, R);
        // block BR matched by both BC and BL
        if (BRi == bBL / blockSize) farAwayBlocks_ANSV_linear(A, rBL, b1 + 1, b2, bBL + 1, L, R);
    });

    delete [] REPs;
    delete [] B;
    delete [] T;

    // TIMING 3
    times[2] = "Merging: " + to_string(t.next_time()) + "\n";

    double totalTime = t.total_time();
    string res = " --- Berkman & Vishkin --- with n, blockSize: "
                 + to_string(n) + ", " + to_string(blockSize)  + "\n"
                 + times[0] + times[1] + times[2] + "Total time: " + to_string(totalTime) + "\n";

    delete [] times;
    return res;
}