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
 | Initialization of solver containers and parameters.                     |
/*-------------------------------------------------------------------------*/
void CDCL::init_solver ( ) {

  srand (0);
  init_statistics ( );
  init_parameters ( );
  init_states ( );
  init_model ( );
  init_block ( );
}

/*-------------------------------------------------------------------------+
 | Initialize parameters container (see 'params.h').                       |
/*-------------------------------------------------------------------------*/
void CDCL::init_parameters ( ) {

  param.block_lits_fixed  =       0;
  param.eliminating       =   false;
  param.elim_bound        =       0;
  param.lits              =       2*param.vars;
}

/*-------------------------------------------------------------------------+
 | Initialize schedule container (see 'schedule.h').                       |
/*-------------------------------------------------------------------------*/
void CDCL::init_states ( ) {

  state.occurring       =  false;
}

/*-------------------------------------------------------------------------+
 | Initialize model, trail, and mark containers.                           |
/*-------------------------------------------------------------------------*/
void CDCL::init_model ( ) {

  std::fill (&model[-param.vars], &model[param.vars] + 1, 0);
}

/*-------------------------------------------------------------------------+
 | Initialize blocked clause elimination containers.                       |
/*-------------------------------------------------------------------------*/
void CDCL::init_block ( ) {
  
/* Initialize bmark and bskip to [0,...,0] */
  bmark = new signed char[param.lits + 1] ( );
  bmark += param.vars;

/* Initialize bmark to [1,...,1] */
  block = new signed char[param.lits + 1];
  std::fill (block, block + param.lits + 1, 1);
  block += param.vars;
}

} //End namespace sat