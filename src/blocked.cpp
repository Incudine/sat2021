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
 | This is the main routine for Blocked Clause Elimination (BCE) as intro- |
 | duced in the original publication "Blocked Clause Elimination, 2013".   |
 | I have added to it the signature-based check published in my paper      |
 | "Hash-based preprocessing and inprocessing techniques in SAT solvers".  |
/*-------------------------------------------------------------------------*/
void CDCL::block_call ( ) {

/* Start counter(s) */
  stats.preprocess_start = std::chrono::high_resolution_clock::now ( );

/* Construct occurence lists and occurrences counters */
  block_init_occrs ( );

/* Loop over literals */
  for (int lit = -param.vars; lit <= param.vars; ++lit)
    if (lit && block[abs (lit)])
      block_literal (lit);
}

/*-------------------------------------------------------------------------+
 | Initialize occurence list during blocked clause elimination. The list   |
 | is populated by all original clauses.                                   |
/*-------------------------------------------------------------------------*/
void CDCL::block_init_occrs ( ) {

/* Use variable markers to select candidate variables
   If a some literal occurs in a clause that is too large
   we mark it as a non-candidate. */
  block = new signed char[param.vars + 1] ( );
  memset (block, 1, (param.vars + 1) * sizeof (signed char));

/* Initialize model for tautology spotting */
  if (modes.hash) {
    init_occrs_hash ( );
    for (Clause* c : original) {
      if (c->size ( ) > lims.block_max_cls_size) {
        for (const int lit : *c)
          block[abs (lit)] = 0;
      } else {
        const OccrsHash ohc (c, c->hash, c->unique);
        for (const int lit : *c)
          occhash (lit).push_back (ohc);
      }
    }

/* Initialize regular occurrence list */
  } else {
    init_occrs ( );
    for (Clause* c : original) {
      if (c->size ( ) > lims.block_max_cls_size) {
        for (const int lit : *c)
          block[abs (lit)] = 0;
      } else {
        for (const int lit : *c)
          occurs (lit).push_back (c);
      }
    }
  }
}

/*-------------------------------------------------------------------------+
 | Check if 'bvar' is pure or if there are blocked clauses in the occur-   |
 | rence list of 'bvar'. We also skip too large occurrence lists.          |
/*-------------------------------------------------------------------------*/
void CDCL::block_literal (const int bvar) {

/* Enable hash-based check */
  int blocked = 0;
  if (modes.hash) {
    const uint64_t idx = 1ULL << (abs (bvar) % 64);
    if (occhash (-bvar).size ( ) > lims.block_max_noccs) return;
    if (!occhash (bvar).size ( )) return;         // pure literal
    for (const OccrsHash& chu : occhash (bvar))
      if (block_check_hash (chu, bvar, idx))
        ++blocked;
    //stats.block_checks += occhash (bvar).size ( );

/* Regular check */
  } else {
    if (occurs (-bvar).size ( ) > lims.block_max_noccs) return;
    if (!occurs (bvar).size ( )) return;          // pure literal
    for (const Clause* c : occurs (bvar))
      if (block_check (c, bvar))
        ++blocked;
  }
  stats.block_clauses += blocked;
}

/*-------------------------------------------------------------------------+
 | Check if 'c' is a blocked clause under 'bvar' by checking clause lit-   |
 | eral marks against clauses in the negative occurrence list of 'bvar'.   |
 | If enabled, we also include a signature-based preheck.                  |
/*-------------------------------------------------------------------------*/
bool CDCL::block_check (const Clause* c, const int bvar) {

/* If clause hashes did not provide a certificate then verify blockedness
   by marking literals 'lit' in 'c' and checking the negative occurrences
   of 'bvar for a clause with no negation of 'lit' in 'c' marked. */
  bool blocked = true;
  mark_sign (c);

  for (const Clause* d : occurs (-bvar)) {
    //assert (d->size ( ) <= lims.block_max_cls_size);
    //if (d->size ( ) > lims.block_max_cls_size) { blocked = false; break; }
    for (const int& lit : *d) {
      if (lit == -bvar) continue;                // skip blocker
      if (sign_marked (lit) < 0) goto next;       // marked
    }
    blocked = false;                              // unmarked clause
    break;                                        // failed
    next:;
  }
  unmark (c);                                     // unmark literals!
  return blocked;
}

/*-------------------------------------------------------------------------+
 | Check if 'c' is a blocked clause under 'bvar' by checking clause lit-   |
 | eral marks against clauses in the negative occurrence list of 'bvar'.   |
 | We also include a signature-based preheck.                              |
/*-------------------------------------------------------------------------*/
bool CDCL::block_check_hash (const OccrsHash& chu, const int bvar, const uint64_t idx) {

/* Mark literals in 'c' */
  Clause* c = chu.c;
  const uint64_t hci = chu.hash & ~idx;
  const uint64_t uci = chu.unique & idx;

/* Eager check hashes (abs (lit) mod 64) of clauses in the negative
   occurrenceces of 'bvar'. Such a clause cerifies that 'c' cannot
   be a blocked clause and we may skip a more expensive literal check. */
  if (modes.hash) {
    for (const OccrsHash& dhu : occhash (-bvar)) {
      if ((hci & dhu.hash) != 0ULL) continue;
      if ((uci & dhu.unique) != 0ULL) continue;
      //++stats.block_hash_match;                   // statistics
      return false;                               // not a blocker!
    }
  }

/* If clause hashes did not provide a certificate then verify blockedness
   by marking literals 'lit' in 'c' and checking the negative occurrences
   of 'bvar for a clause with no negation of 'lit' in 'c' marked. */
  bool blocked = true;
  mark_sign (c);
  for (const OccrsHash& dhu : occhash (-bvar)) {
    Clause* d = dhu.c;
    for (const int& lit : *d) {
      if (lit == -bvar) continue;                 // skip blocker
      if (sign_marked (lit) < 0) goto next;       // marked
    }
    blocked = false;                              // unmarked clause
    break;                                        // failed
    next:;
  }
  unmark (c);                                     // unmark literals!
  return blocked;
}

} //End namespace sat