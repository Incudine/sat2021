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
 | Initialize empty occurence list.                                        |
/*-------------------------------------------------------------------------*/
void CDCL::init_occrs ( ) {

  assert (occrs.empty ( ));
  occrs.resize (param.lits + 2, std::vector<Clause*> ( ));
  assert (occrs.size ( ) == param.lits + 2);
}

/*-------------------------------------------------------------------------+
 | Initialize empty occurence list.                                        |
/*-------------------------------------------------------------------------*/
void CDCL::init_occrs_hash ( ) {

  assert (occrshash.empty ( ));
  occrshash.resize (param.lits + 2, std::vector<OccrsHash> ( ));
  assert (occrshash.size ( ) == param.lits + 2);
}

/*-------------------------------------------------------------------------+
 | Initialize empty occurence list.                                        |
/*-------------------------------------------------------------------------*/
void CDCL::init_ohu ( ) {

  assert (ohu.empty ( ));
  ohu.resize (param.lits + 2, std::vector<std::pair<uint64_t,uint64_t>> ( ));
  assert (ohu.size ( ) == param.lits + 2);
}

} //End namespace sat