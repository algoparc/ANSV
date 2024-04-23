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

#include "seq_array_n_work.h"
#include "parlay/primitives.h"

using namespace std;
using namespace parlay;


string ANSV_seq_array(long *A, long n, long *L, long *R, long blockSize) {
    internal::timer t("Time");
    t.start();

    // LEFT SMALLER VALUES
    for (int i = 0; i < n; ++i) {
        L[i] = -1;
        int j = i - 1;
        while (0 <= j) {
            if (A[j] <= A[i]) {
                L[i] = j;
                break;
            } else if (L[j] != -1) j = L[j];
            else break;
        }
    }

    // RIGHT SMALLER VALUES
    for (int i = n - 1; 0 <= i; --i) {
        R[i] = -1;
        int j = i + 1;
        while (j < n) {
            if (A[j] <= A[i]) {
                R[i] = j;
                break;
            } else if (R[j] != -1) j = R[j];
            else break;
        }
    }

    double totalTime = t.total_time();
    string res = " --- Sequential --- with n, blockSize: "
                 + to_string(n) + ", " + to_string(blockSize) + "\n"
                 + "Total time: " + to_string(totalTime) + "\n";
    return res;
}

