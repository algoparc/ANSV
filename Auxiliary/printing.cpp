#include "parlay/io.h"
#include "printing.h"

void printArray(parlay::sequence<long> A) {
    std::cout << to_chars(A)  << std::endl;
}

void printArrayStd(long *A_, long n) {
    parlay::sequence<long> A = parlay::tabulate(n, [&] (long i) {
        return A_[i];
    });
    printArray(A);
}