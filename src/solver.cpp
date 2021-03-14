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
 |  Main call to solve.                                             |
/*-------------------------------------------------------------------------*/
int CDCL::solve (const std::string dimacs, const std::vector<int>& asmt) {

/* read cnf dimacs */
  if (!read_dimacs (dimacs)) return exit_with (-1);

/* Initialize model stack and watched literal list */
  init_solver ( );

/* Time subsumption / BCE / BVE calls */
  if      (modes.subsume) subsume_forward ( );
  else if (modes.block)   block_call ( );
  else if (modes.elim)    elim_call ( );
  stats.preprocess_end = std::chrono::high_resolution_clock::now ( );

/* Exit */
  return exit_with (0);
}

/*-------------------------------------------------------------------------+
 | This is called upon confirming satisfiability/unsatisfiability of the   |
 | problem.                                                                |
/*-------------------------------------------------------------------------*/
int CDCL::exit_with (const int code) {

/* Delete allocated clauses */
  while (!original.empty ( )) {
    delete_clause (original.back ( ));
    original.pop_back ( );
  }
  model -= param.vars;
  delete [] model;
  bmark -= param.vars;
  delete [] bmark;
  if (modes.elim || modes.block)
    delete [] block;
  return 0;
}

/*-------------------------------------------------------------------------+
 | Delete an allocated clause.                                             |
/*-------------------------------------------------------------------------*/
void CDCL::delete_clause (Clause* c) {

  char* d = (char*) c;
  delete [] d;
}

} // End namespace sat