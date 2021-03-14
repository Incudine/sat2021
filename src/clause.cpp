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
#include "solver.h"

namespace sat {

/*-------------------------------------------------------------------------+
 | Return a newly allocated Clause containing literals 'lits'.             |
/*-------------------------------------------------------------------------*/
Clause* CDCL::new_clause (const std::vector<int>& lits) {

  const size_t size = lits.size ( );
  const size_t bytes = sizeof (Clause) + (size - 2) * sizeof (int);
  Clause* c = (Clause*) new char[bytes]; // allocate memory
  c->length   = size;                    // set length of clause
  for (int i = 0; i < size; ++i)         // copy literals
    c->lits[i] = lits[i];
  compute_hash (c);
  return c;
}

/*-------------------------------------------------------------------------+
 | Compute the hash for variables in 'c' and uniqueness of hash.           |
/*-------------------------------------------------------------------------*/
void CDCL::compute_hash (Clause* c) {

  uint64_t hash = 0ULL;
  uint64_t unique = 0ULL;
  for (const int lit : *c) {
    const uint64_t x = 1ULL << ((abs (lit)) & 63ULL);
    if (hash & x) unique |= x;            // collision signature - u(C)
    else hash |= x;                       // clause signature    - h(C)
  }
  c->hash = hash;
  c->unique = unique;
}

} // End namespace sat