#include "inputs.h"
#include <array>
#include <numeric>
#include "random"
#include "parlay/primitives.h"

using namespace std;
using namespace parlay;

long* seqToArray(sequence<long> A_) {
    long n = A_.size();
    long *A = new long[n];
    parallel_for(0, n, [&] (size_t i){
        A[i] = A_[i];
    });
    return A;
}

long* returnSortedArray(long n){
    sequence<long> A(n);
    iota(A.begin(), A.end(), 1);
    return seqToArray(A);
}

long* returnMergeArray(long n){
    sequence<long> A = tabulate(n, [&](size_t i) {
        if (i < floor(n/2)) return (long)(2 * i);
        else return (long)(2 * (n - i - 1) + 1);
    });
    return seqToArray(A);
}

long* returnShuffledMergeArray(long n){
    random_device rd1;
    mt19937 gen1(rd1());
    uniform_int_distribution<long> dis1(0, INT_MAX);

    random_generator gen(dis1(gen1));
    uniform_int_distribution<long> dis(0, 1);

    long * A = returnMergeArray(n);

    for (int i = 0; i < n; i = i + 2) {
        auto r = gen[i];
        if (dis(r) == 1) swap(A[i], A[i + 1]);
    }
    return A;
}

long* returnRandomArray(long n) {
    random_device rd1;
    mt19937 gen1(rd1());
    uniform_int_distribution<long> dis1(0, INT_MAX);

    random_generator gen(dis1(gen1));
    uniform_int_distribution<long> dis(0, INT_MAX);
    sequence<long> A = tabulate(ceil(1.2 * n), [&](size_t i) {
        auto r = gen[i];
        return dis(r);
    });

    A = remove_duplicates(A);
    A = A.subseq(0, n);
    assert(A.size() == n);
    return seqToArray(A);
}