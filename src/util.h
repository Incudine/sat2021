/*-------------------------------------------------------------------------+
 | Copyright (c) 2020, Henrik Cao, henrik.cao@aalto.fi, Espoo, Finland.    |
 |                                                                         |
 | Permission is hereby granted, free of charge, to any person obtaining a |
 | copy of this software and associated documentation files, to deal in the|
 | Software without restriction, including without limitation the rights to|
 | use, copy, modify, merge, publish, distribute, sublicense, and/or sell  |
 | copies of the Software, and to permit persons to whom the Software is   |
 | furnished to do so, subject to the following conditions:                |
 |                                                                         |
 | The above copyright notice and this permission notice shall be included |
 | in all copies or substantial portions of the Software.                  |
 |                                                                         |
 | THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS |
 | OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABI- |
 | LITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT |
 | SHALL THE AUTHORS OR COPYRIGHT HOLDERS BELIABLE FOR ANY CLAIM, DAMAGES  |
 | OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,|
 | ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR   |
 | OTHER DEALINGS IN THE SOFTWARE.                                         |
/*-------------------------------------------------------------------------*/
/* General includes */
#include <iostream>
#include <vector>
#include <random>
#include <cmath>
#include <algorithm> 
#include <functional> 
#include <chrono>
#include <map>
#include <queue>
#include <assert.h>
#include <bitset>
#include <bits/stdc++.h>

/* User included libraries */
#include "clause.h"

#ifndef UTIL_H
#define UTIL_H

/*-------------------------------------------------------------------------+
 | Auxiliary print functions.                                              |
/*-------------------------------------------------------------------------*/
template <class T>
void print_vector ( std::vector<T>& vec ) {
  if (vec.empty ( )) { std::cout << "[ ]" << std::endl; return; }
  std::cout << "[";
  for (int i = 0; i < vec.size ( ) - 1; ++i)
    std::cout << vec[i] << ", ";
  std::cout << vec[vec.size ( ) - 1] << "]" << std::endl;
}
template <class T>
void print_vector_2D ( std::vector<T>& vec ) {
  if (vec.empty ( )) { std::cout << "[ ]" << std::endl; return; }
  std::cout << "[";
  for (int i = 0; i < vec.size ( ) - 1; ++i) {
    std::cout << "[";
    for (int j = 0; j < vec[i].size ( ) - 1; ++j)
      std::cout << vec[i][j] << ",";
    std::cout << vec[i][vec[i].size ( ) - 1] << "],";
  }
  std::cout << "[";
  for (int j = 0; j < vec[vec.size ( ) - 1].size ( ) - 1; ++j)
    std::cout << vec[vec.size ( ) - 1][j] << ",";
  std::cout << vec[vec.size ( ) - 1][vec[vec.size ( ) - 1].size ( ) - 1] << "]]" << std::endl;
}

static void print_vec (const std::vector<int> vec) {
    for (int i = 0; i < vec.size ( ); i++)
        std::cout << vec[i] << " ";
    std::cout << std::endl;
}

static void print_vec (Clause* cls) {
    for (const int lit : *cls)
        std::cout << lit << " ";
    std::cout << std::endl;
}

#endif