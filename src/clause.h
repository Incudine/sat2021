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
#ifndef CLAUSE_H
#define CLAUSE_H

/*-------------------------------------------------------------------------+
 | Clause container.                                                       |
/*-------------------------------------------------------------------------*/
struct Clause {

  //int id = -1;             // unique clause id (for debugging)
  unsigned short length = 0; // size of lits
  uint64_t hash     =  0ULL; // absolute value hash (mod 64)
  uint64_t unique   =  0ULL; // unique hash markers
  int lits[2];               // literals container

/* Iterators */
  int* begin ( ) { return &lits[0]; }
  int* end ( )   { return &lits[0] + length; }
  const int size   ( ) const { return length; }
  const int* begin ( ) const { return &lits[0]; }
  const int* end ( ) const { return &lits[0] + length; }
};

#endif