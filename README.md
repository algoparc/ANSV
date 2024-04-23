A Parallel Implementation of an Optimal ANSV Algorithm
==========================================

Overview:
---------
A parallel implementation of the All Nearest Smaller Values (ANSV) algorithm by Berkman, Schieber, and Vishkin for comparison to the algorithm by Blelloch, Shun, and Zhao.

Features:
---------
Three implementations of algorithms the ANSV problem:
1. Sequential array-based solution with O(n) complexity.
2. The Berkman, Schieber, and Vishkin Optimal solution. 
3. The Shun and Zhao heuristic approach.

Dependencies:
-------------
- CMake (minimum required version 3.16).
- C++20 Standard for modern C++ features.
- ParlayLib (header-only library) for efficient parallel computations.
- Pthread library for multithreading support.

Building the Project:
---------------------
1. Ensure CMake (version 3.16 or higher) and a C++20 compliant compiler are installed.
2. Clone this repository to your system.
3. Ensure ParlayLib is cloned into your project. It's available on [GitHub](https://cmuparlay.github.io/parlaylib/).
3. Navigate to the project directory and run the following commands:
   cmake .
   make
4. This will compile the ANSV2 executable.

Usage:
------
- Execute the ANSV2 executable to run the algorithms.
- The main.cpp file serves as the entry point and can be modified to test different algorithms or inputs.

Applications of ANSV:
---------------------
The ANSV problem has numerous applications in various domains, including but not limited to:
1. Finding the min/max among n elements.
2. Merging sorted lists.
3. Constructing Cartesian trees.
4. Monotone polygon triangulation.
5. Range minimum queries.
6. Parenthesis matching.
7. Binary tree reconstruction.
(For more details, see berkman1993)

References:
-----------
- [berkman1993] **O. Berkman, B. Schieber, and U. Vishkin. 1993.** "Optimal Doubly Logarithmic Parallel Algorithms Based On Finding All Nearest Smaller Values." *Journal of Algorithms* 14, no. 3 (1993): 344-370. [DOI: 10.1006/jagm.1993.1018](https://doi.org/10.1006/jagm.1993.1018)

- [zhao2013] Julian Shun and Fuyao Zhao. 2013.** "Practical Parallel Lempel-Ziv Factorization." In *2013 Data Compression Conference*. IEEE, 123–132. [DOI: 10.1109/DCC.2013.20](https://doi.org/10.1109/DCC.2013.20)

