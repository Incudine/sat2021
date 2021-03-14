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
#ifndef STATS_H
#define STATS_H

/*-------------------------------------------------------------------------+
 | Statistics container for class CDCL.                                    |
/*-------------------------------------------------------------------------*/
struct Stats {

  ~Stats ( ) { }
  Stats ( ) { }

  long block_checks                        = 0; // blocked clauses checked
  long block_clauses                       = 0; //
  long block_hash_match                    = 0; // blocked clause checks mismatched by hash
  long elim_hashes                         = 0; // hash equivalences
  long eliminated                          = 0; //
  long elim_hash_matches                   = 0; //
  long elim_checks                         = 0; //
  long elim_resolvents                     = 0; //
  long strengthened                        = 0; //
  long subsume_checks                      = 0; // number of subsumption checks (i.e. calls to 'subsume_check')
  long subsume_hash_matches                = 0; //
  long subsumed                            = 0; //
  std::chrono::time_point<std::chrono::high_resolution_clock> time_start;                                  // solving time (start)
  std::chrono::time_point<std::chrono::high_resolution_clock> time_end;                                    // solving time (end)
  std::chrono::time_point<std::chrono::high_resolution_clock> preprocess_start;                            // preprocessing time (start)
  std::chrono::time_point<std::chrono::high_resolution_clock> preprocess_end;                              // preprocessing time (end)
};

#endif