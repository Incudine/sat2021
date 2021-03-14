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

/*-------- General includes ----------------------------------------*/
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
#include <climits>

/*-------- Header Includes --------------------------------------*/
#include "clause.h"
#include "limits.h"
#include "modes.h"
#include "params.h"
#include "stats.h"
#include "state.h"
#include "util.h"
#include "watcher.h"

#ifndef SOLVER_H
#define SOLVER_H

namespace sat {

struct Sort_cls_size_subsume : std::binary_function <std::pair<Clause*,size_t>,std::pair<Clause*,size_t>,bool> {
  Sort_cls_size_subsume ( ) { }
  bool operator ( ) (std::pair<Clause*,size_t>& x, std::pair<Clause*,size_t>& y ) { return x.second < y.second; }
};

class CDCL {

  struct OccrsHash {
    OccrsHash (Clause* c_, uint64_t h, uint64_t u) : c {c_}, hash {h}, unique {u} { }
    Clause* c        =    0;        // clause pointer
    uint64_t hash    = 0ULL;        // hash value
    uint64_t unique  = 0ULL;        // uniqueness of 'hash'
  };

  public:

    CDCL ( ) { };
    ~CDCL ( ) { };

/*-------------------------------------------------------------------------+
 | Map a positive/negative literals to positive indices in [0,2*nvars + 1].|
 | used exclusively for ordering occurrences.                                 |
/*-------------------------------------------------------------------------*/
    int widx (const int lit) {
      if (lit > 0) return lit + lit + 1;
      else return -lit - lit;
    }

/*-------------------------------------------------------------------------+
 | Set value of model 'lit'.                                               |
/*-------------------------------------------------------------------------*/
    void set_model (const int lit) {
      assert (!model[lit]);
      assert (!model[-lit]);
      model[lit] = 1, model[-lit] = -1;
    }

/*-------------------------------------------------------------------------+
 | Unset model for the positive and negative literals.                     |
/*-------------------------------------------------------------------------*/
    void unset_model (const int lit) { model[lit] = model[-lit] = 0; }

/*-------------------------------------------------------------------------+
 | Returns the value of lit under the current model. The value is          |
 | 1 if the literal is satisfied by the model, -1 if the literal is        |
 | unsatisfied, and 0 if the variable is unset. The current                |
 | version implements a branchless bit operation to compute whether        |
 | the literal sign and model value match.                                 |
/*-------------------------------------------------------------------------*/
    int value (const int lit) { return model[lit]; }

/*-------------------------------------------------------------------------+
 | Marker functions used in BCE and subsumption routines.                  |
/*-------------------------------------------------------------------------*/
    void mark_sign (const int lit) { bmark[lit] = 1; bmark[-lit] = -1; }
    int sign_marked (const int lit) { return bmark[lit]; }
    void unmark (const int lit) { bmark[lit] = bmark[-lit] = 0; }
    void mark_sign (const Clause* c) { for (const int& lit : *c) mark_sign (lit); }
    void unmark (const Clause* c) { for (const int& lit : *c) unmark (lit); }
    void unmark (const std::vector<int>& lits) { for (const int& lit : lits) unmark (lit); }

/* ----------------------- Regular functions -------------------------*/

  /* blocked.cpp */
    void block_call ( );
    void block_init_occrs ( );
    void block_literal (const int lit);
    bool block_check (const Clause* c, const int block);
    bool block_check_hash (const OccrsHash& chu, const int block, const uint64_t idx);

  /* clause.cpp */
    Clause* new_clause (const std::vector<int>& lits);
    void compute_hash (Clause* c);

  /* elim.cpp */
    void elim_call ( );
    void elim_init_occrs ( );
    void elim_variable (int lit);
    bool elim_check_resolvent (Clause* c, Clause* d, const int lit);
    bool elim_check_hash (const uint64_t ch, const uint64_t cu, const uint64_t dh, const uint64_t du, const uint64_t idx);
    bool elim_resolvents_bounded (const int lit);
    bool elim_resolvents_bounded_signature (const int lit);

  /* init.cpp */
    void init_solver ( );
    void init_parameters ( );
    void init_model ( );
    void init_block ( );
    void init_subsumption ( );
    void init_elimination ( );
    void init_statistics ( ) { stats = Stats ( ); }
    void init_states ( );// { state = State ( ); }
    void set_modes (Modes& modes_) { modes = modes_; }
  
  /* input.cpp */
    bool read_dimacs (const std::string file_name);
    bool tautology (std::vector<int>& lits);

  /* solver.cpp */
    int solve (const std::string dimacs, const std::vector<int>& asmt);
    int exit_with (const int code);
    void delete_clause (Clause* c);

  /* subsume.cpp */
    bool simplify ( );
    void subsumption ( );
    bool subsume_forward ( );
    void subsume_candidates ( );
    void subsume_find_candidates (std::vector<Clause*>& clauses);
    void subsume_check (Clause* c);
    void subsume_check_hash (Clause* c);
    int subsumed (const Clause* c);
    int subsume_min_occrs (const Clause* c);
    int subsume_min_occrs_hash (const Clause* c);

  /* watcher.cpp */
    bool occurring ( ) { return state.occurring; }
    void init_occrs ( );
    void init_occrs_hash ( );
    void init_ohu ( );

/* ------------------------------------------------------------------------ */
    Params param;                            // solver variables
    Limits lims;                             // runtime limits
    Modes modes;                             // input modes
    Stats stats;                             // statistics
    State state;                             // current states
    signed char* bmark;                      // marker container (see 'block.cpp/subsume.cpp')
    signed char* block;                      // marker container (see 'block.cpp')
    std::vector<std::pair<Clause*,size_t>> candsizes;              // container for candidate clauses (see 'subsume.cpp')

  private:

    std::vector<OccrsHash>& occhash (const int lit) { return occrshash[widx (lit)]; }
    std::vector<Clause*>& occurs (const int lit) { return occrs[widx (lit)]; }
    std::vector<std::pair<uint64_t, uint64_t>>& ochash (const int lit) { return ohu[widx (lit)]; }
    std::vector<int> unit_clauses;                 // see 'input.cpp' and 'preprocess.cpp'
    std::vector<std::vector<int>> clause_lits;     // clause buffer 'input.cpp'
    std::vector<Clause*> original;                 // original clauses
    int* model;                                    // model
    std::vector<std::vector<Clause*>> occrs;                    // occurrence list
    std::vector<std::vector<OccrsHash>> occrshash; // container for occurrences/hash pairs (see 'subsume.cpp')
    std::vector<std::vector<std::pair<uint64_t, uint64_t>>> ohu;

    std::vector<std::vector<std::pair<Clause*, std::pair<uint64_t, uint64_t>>>> och_test;
    std::vector<std::pair<Clause*, std::pair<uint64_t, uint64_t>>>& fvar_och_test (const int lit) { return och_test[widx (lit)]; }
};

} //End namespace sat


#endif