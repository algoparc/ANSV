#include "iostream"
#include "Implementations/seq_array_n_work.h"
#include "Implementations/BerkmanOptimal.h"
#include "Implementations/ShunZhaoOriginal.h"
#include "Auxiliary/inputs.h"
#include "Auxiliary/printing.h"


using namespace std;

int main() {
  long n = 10;
  long blockSize = 3;

  long *L = new long[n];
  long *R = new long[n];

  // SELECT INPUT (all elements must be distinct)
//  long *A = new long[n]{2, 5, 8, 7, 3, 0, 6, 4, 9, 1};
  long *A = returnShuffledMergeArray(n);

  // SELECT ALGORITHM
//  string time = ANSV_seq_array(A, n, L, R, blockSize);
//  string time = ANSV_ShunZhao(A, n, L, R, blockSize, true);
  string res = ANSV_Berkman(A, n, L, R, blockSize);

  // PRINT RESULTS
  cout << "A: " << endl;
  printArrayStd(A, n);

  cout << endl << "L:" << endl;
  printArrayStd(L, n);

  cout << endl << "R:" << endl;
  printArrayStd(R, n);

  cout << endl << res << endl;

  delete [] L;
  delete [] R;
  delete [] A;
  return 1;
}
