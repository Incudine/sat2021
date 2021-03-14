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
#ifndef LIMITS_H
#define LIMITS_H

/*-------------------------------------------------------------------------+
 | Limit container for class CDCL.                                         |
/*-------------------------------------------------------------------------*/
struct Limits {

  ~Limits ( ) { }
  Limits ( ) { }

  const int      block_max_cls_size   = 1e3;
  const int      block_max_noccs      = 1e3;
  const int      elim_res_size_max    = 1e4;
  const int      elim_occrs_max       = 1e3;
  const int      subsume_max_cls_size = 1e3;
};

#endif