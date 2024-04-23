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

#include "shunZhaoOriginal.h"
#include "parlay/primitives.h"


using namespace std;
using namespace parlay;

#define LEFT(i) ((i) << 1)
#define RIGHT(i) (((i) << 1) | 1)
#define PARENT(i) ((i) >> 1)


long getLeft_opt(long **table, long depth, long n, long index, long start) {
    long value = table[0][index];
    if (value == table[depth - 1][0]) return -1;

    long cur = PARENT(start), d, dist = 2;
    for (d = 1; d < depth; d++) {
        if ((cur + 1) * dist > start + 1) cur --;
        if (cur < 0) return -1;

        if (table[d][cur] >= value) cur = PARENT(cur);
        else break;

        dist <<= 1;
    }

    for ( ; d > 0; d--) {
        if (table[d - 1][RIGHT(cur)] < value) cur = RIGHT(cur);
        else cur = LEFT(cur);
    }
    return cur;
}

long getRight_opt(long **table, long depth, long n, long index, long start) {
    long value = table[0][index];
    if (value == table[depth - 1][0]) return -1;

    long cur = PARENT(start), d, dist = 2;
    for (d = 1; d < depth; d++) {
        if (cur * dist < start) cur ++;
        if (cur * dist >= n) return -1;

        if (table[d][cur] >= value) cur = PARENT(cur);
        else break;

        dist <<= 1;
    }

    for ( ; d > 0; d--) {
        if (table[d - 1][LEFT(cur)] < value) cur = LEFT(cur);
        else cur = RIGHT(cur);
    }
    return cur;
}


void ComputeANSV_Linear(long A[], long n, long L[], long R[], long offset) {
    long i, top;
    long *stack = new long[n];

    for (i = 0, top = -1; i < n; i++) {
        while (top > -1 && A[stack[top]] > A[i]) top--;
        if (top == -1) L[i] = -1;
        else L[i] = stack[top] + offset;
        stack[++top] = i;
    }

    for (i = n - 1, top = -1; i >= 0; i--) {
        while (top > -1 && A[stack[top]] > A[i]) top--;
        if (top == -1) R[i] = -1;
        else R[i] = stack[top] + offset;
        stack[++top] = i;
    }
    delete[] stack;
}

inline long cflog2(long i) {
    long res = 0;

    i--;
    if (i >> 16) {
        res += 16;
        i >>= 16;
    } else i &= 0xffff;

    for (; i; i >>= 1) res++;
    return res;
}

tuple<long **, long> createBinaryTree(long *A, long n) {
    long l2 = cflog2(n);
    long depth = l2 + 1;
    long **table = new long*[depth];
    table[0] = A;
    long m = n;
    for (long i = 1; i < depth; i++) {
        m = (m + 1) / 2;
        table[i] = new long[m];
    }

    m = n;
    for (long d = 1; d < depth; d++) {
        long m2 = m / 2;

        parallel_for (0, m2, [&] (size_t i) {
            table[d][i] = min(table[d - 1][LEFT(i)], table[d - 1][RIGHT(i)]);
        });

        if (m % 2) table[d][m2] = table[d - 1][LEFT(m2)];
        m = (m + 1) / 2;
    }

    return {table, depth};
}

string ANSV_ShunZhao(long *A, long n, long *left, long *right, long blockSize, bool usingHeuristic) {
    internal::timer t("shunZhao");
    auto *times = new string[3];
    t.start();

    // ------------ CREATE BINARY TREE ------------
    //
    // 1) Create a min-binary tree for the input
    long l2 = cflog2(n);
    long depth = l2 + 1;
    long **table = new long*[depth];

    table[0] = A;
    long m = n;
    for (long i = 1; i < depth; i++) {
        m = (m + 1) / 2;
        table[i] = new long[m];
    }

    m = n;
    for (long d = 1; d < depth; d++) {
        long m2 = m / 2;

        parallel_for(0, m2, [&] (size_t i) {
            table[d][i] = min(table[d - 1][LEFT(i)], table[d - 1][RIGHT(i)]);
        });

        if (m % 2) {
            table[d][m2] = table[d - 1][LEFT(m2)];
        }

        m = (m + 1) / 2;
    }

    // TIMING 1
    times[0] = "Binary tree: " + to_string(t.next_time()) + "\n";

    // ------------ LOCAL MATCHES & REPRESENTATIVES ------------
    //
    blocked_for(0, n, blockSize, [&] (size_t blockNumber, size_t i, size_t j) {
        ComputeANSV_Linear(A + i, j - i, left + i, right + i, i);
    });

    // TIMING 2
    times[1] = "Local matches: " + to_string(t.next_time()) + "\n";

    // ------------ NONLOCAL MATCHES BY TREE TRAVERSAL ------------
    //
    blocked_for(0, n, blockSize, [&] (size_t blockNumber, size_t i, size_t j) {
        long tmp = i;
        for (long k = i; k < j; k++) {
            if (left[k] == -1) {
                if (tmp != -1 && A[tmp] >= A[k]) {
                    tmp = getLeft_opt(table, depth, n, k, usingHeuristic ? tmp : k); // Heuristic ON/OFF
                }
                left[k] = tmp;
            }
        }

        tmp = j - 1;
        for (long k = j - 1; k >= (long)i; k--) {
            if (right[k] == -1) {
                if (tmp != -1 && A[tmp] >= A[k]) {
                    tmp = getRight_opt(table, depth, n, k, usingHeuristic ? tmp : k); // Heuristic ON/OFF
                }
                right[k] = tmp;
            }
        }
    });

    for (long i = 1; i < depth; i++) delete table[i];
    delete[] table;

    // TIMING 3
    times[2] = "Tree traversal: " + to_string(t.next_time()) + "\n";

    double totalTime = t.total_time();
    string res = " --- Shun & Zhao --- with n, blockSize, heuristic: "
                 + to_string(n) + ", " + to_string(blockSize) + ", " + to_string(usingHeuristic) + "\n"
                 + times[0] + times[1] + times[2] + "Total time: " + to_string(totalTime) + "\n";

    delete [] times;
    return res;
}

