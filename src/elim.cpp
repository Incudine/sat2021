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
 | Main call to ounded variable elimination (BVE).                         |
/*-------------------------------------------------------------------------*/
void CDCL::elim_call ( ) {

/* Start counter(s) */
  stats.preprocess_start = std::chrono::high_resolution_clock::now ( );

/* Initialize occurrences, counters and 'elim_heap' */
  elim_init_occrs ( );

/* Loop over candidate variables */
  for (int var = 1; var <= param.vars; ++var)
    elim_variable (var);
    //if (block[var])
    //  elim_variable (var);
}

/*-------------------------------------------------------------------------+
 | Initializes the the occurrence list.                                    |
/*-------------------------------------------------------------------------*/
void CDCL::elim_init_occrs ( ) {

/* Use variable markers to select candidate variables
   If a some literal occurs in a clause that is too large
   we mark it as a non-candidate. */
  block = new signed char[param.vars + 1] ( );
  memset (block, 1, (param.vars + 1) * sizeof (signed char));

/* Initialize occurrence list with signatures */
  if (!modes.hash) {
    init_occrs ( );
    for (Clause* c : original) {
      if (c->size ( ) > lims.elim_res_size_max) {
        for (const int lit : *c)
          block[abs (lit)] = 0;
      } else {
        for (const int lit : *c)
          occurs (lit).push_back (c);    // add to occurrences
      }
    }

/* Use normal occurrence list */
  } else {
    init_occrs_hash ( );
    for (Clause* c : original) {
      if (c->size ( ) > lims.elim_res_size_max) {
        for (const int lit : *c)
          block[abs (lit)] = 0;
      } else {
        const OccrsHash tt (c, c->hash, c->unique);
        for (const int lit : *c)
          occhash (lit).push_back (tt);
      }
    }
  }
}

/*-------------------------------------------------------------------------+
 | This is the main check for elimination on 'evar'. We check whether the  |
 | set of resolvents pos x neg is bounded (w.r.t.'param.elim_bound').      |
/*-------------------------------------------------------------------------*/
void CDCL::elim_variable (int evar) {

/* Pure literal or too many occurrences */
  if (modes.hash) {
    if (!occhash (evar).size ( ) || !occhash (-evar).size ( )) return;
    if (occhash (-evar).size ( ) > lims.elim_occrs_max) return;
    if (occhash (evar).size ( ) > lims.elim_occrs_max) return;

  } else {
    if (!occurs (evar).size ( ) || !occurs (-evar).size ( )) return;
    if (occurs (-evar).size ( ) > lims.elim_occrs_max) return;
    if (occurs (evar).size ( ) > lims.elim_occrs_max) return;
  }

/* Eliminate if resolvents pos x neg are bounded (i.e., if
   |pos x neg| < |pos| + |neg| + 'param.elim_bound') */
  if (modes.hash) {
    if (elim_resolvents_bounded_signature (evar))
      ++stats.eliminated;
  } else if (elim_resolvents_bounded (evar))
    ++stats.eliminated;
}

/*-------------------------------------------------------------------------+
 | Check if the resolvents involving 'evar' are bounded (a.k.a less than   |
 | pos.size ( ) + neg.size ( ) + 1 + 'param.elim_bound'.                   |
/*-------------------------------------------------------------------------*/
bool CDCL::elim_resolvents_bounded (const int evar) {

/* Bound the number of resolvents. Clauses in 'pos'/'neg' may have
   been removed, so we use the 'noccs' count instead. */
  std::vector<Clause*>& pos = occurs (evar);
  std::vector<Clause*>& neg = occurs (-evar);
  const size_t bound = pos.size ( ) + neg.size ( ) + param.elim_bound;
  int count = 0;

/* Check if the number of resolvents (pos x neg) is bounded */
  for (Clause* c : pos) {

  /* Mark literals in 'c' */
    mark_sign (c);

  /* Check negative occurrence list for tautological resolvents */
    for (Clause* d : neg) {

    /* Check whether resolvents is tautological in the old fashion way */
      if (elim_check_resolvent (c, d, evar))
        ++count;

      if (count > bound) {
        unmark (c);
        return false;
      }
    }

  /* Unmark literals */
    unmark (c);
  }
  return true;
}

/*-------------------------------------------------------------------------+
 | Check if the resolvents involving 'evar' are bounded (a.k.a less than   |
 | pos.size ( ) + neg.size ( ) + 1 + 'param.elim_bound'.                   |
/*-------------------------------------------------------------------------*/
bool CDCL::elim_resolvents_bounded_signature (const int evar) {

/* Bound the number of resolvents. Clauses in 'pos'/'neg' may have
   been removed, so we use the 'noccs' count instead. */
  std::vector<OccrsHash>& pos = occhash (evar);
  std::vector<OccrsHash>& neg = occhash (-evar);
  const size_t bound = pos.size ( ) + neg.size ( ) + param.elim_bound;
  int count = 0;
  int checks = 0;
  int hash_matches = 0;
  const uint64_t idx = 1ULL << (abs (evar) % 64);

  for (const OccrsHash& chu : pos) {
    const uint64_t hci = chu.hash & ~idx;
    const uint64_t uci = chu.unique & idx;
    for (const OccrsHash& dhu : neg) {
      //++checks;
      if ((hci & dhu.hash) != 0ULL) continue;
      if ((uci & dhu.unique) != 0ULL) continue;
      //++hash_matches;
      if (++count > bound) {
        //stats.elim_hash_matches += hash_matches;
        //stats.elim_checks += checks;
        return false;
      }
    }
  }
  //stats.elim_hash_matches += hash_matches;

/* Check if the number of resolvents (pos x neg) is bounded */
  for (const OccrsHash& chu : pos) {
    const uint64_t hci = chu.hash & ~idx;
    const uint64_t uci = chu.unique & idx;

  /* Mark literals in 'c' and the negation of the resolution literal */
    bool marked = false;
    for (const OccrsHash& dhu : neg) {

    /* Skip if counted during signature test */
      if ((hci & dhu.hash) == 0ULL) {
        if ((uci & dhu.unique) == 0ULL)
          continue;
      }
      //++checks;

    /* Mark literals (if not already marked) */
      if (!marked) { mark_sign (chu.c); marked = true; }

    /* Check whether resolvents is tautological in the old fashion way */
      if (elim_check_resolvent (chu.c, dhu.c, evar)) {
        if (++count > bound) {
          if (marked) unmark (chu.c);
          //stats.elim_checks += checks;
          return false;
        }
      }
    }
    if (marked) unmark (chu.c);
  }
  //stats.elim_checks += checks;
  return true;
}

/*-------------------------------------------------------------------------+
 | Check if the resolvent between 'c' and 'd' on 'elit' is valid. Assumes  |
 | 'elit' is in 'c' and '-elit' in 'd'. The function fails for tautological|
 | resolvents (x in c and '-x' in d), unit resolvents (which are propagated|
 | in the function 'elim_propagate', or if the resolvent subsumes either   |
 | 'c' or 'd'.                                                             |
/*-------------------------------------------------------------------------*/
bool CDCL::elim_check_resolvent (Clause* c, Clause* d, const int res) {

/* Now check literal marks of 'd', skipping root-falsified literals.
   If 'lit' and '-lit' are marked, cRd is tautological. */
  ++stats.elim_resolvents;
  int subst = 0;
  for (const int lit : *d) {
    if (lit == -res) continue;                // skip resolution variable
    if (sign_marked (lit) < 0)
      return false;                           // tautological resolvent
  }
  return true;
}

/*-------------------------------------------------------------------------+
 | Eager check if the resolvent cRd is non-tautological.                   |
/*-------------------------------------------------------------------------*/
bool CDCL::elim_check_hash (const uint64_t ch, const uint64_t cu,
                             const uint64_t dh, const uint64_t du,
                              const uint64_t idx) {
  if ((ch & dh) != idx) return false;
  if ((cu & du & idx) != 0ULL) return false;
  return true;
}

} //End namespace sat