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
 | Our forward subsumption algorithm is based on the one-watcher literal   |
 | scheme by Zhang (see "On subsumption removal and on-the-fly CNF simpli- |
 | fication", 2005) and the cardinality based sorting suggested by Bayardo |
 | (see ("Fast algorithms for finding extremal sets", 2011).               |
/*-------------------------------------------------------------------------*/
bool CDCL::subsume_forward ( ) {

  assert (candsizes.empty ( ));
  const bool hash = modes.hash;

/* Start counter */
  stats.preprocess_start = std::chrono::high_resolution_clock::now ( );

/* Schedule (sorted) candidates for subsumption */
  subsume_candidates ( );
  if (hash) init_occrs_hash ( );
  else      init_occrs ( );

/* Check candidates in increasing order of size */
  int candsmarked = 0;

  for (std::pair<Clause*,size_t>& cs : candsizes) {
    Clause* c = cs.first;

  /* Find the smallest occurrence list to watch 'c' */
    if (hash) {
      if (cs.second > 2) subsume_check_hash (c);
      const int lit = subsume_min_occrs_hash (c);
      occhash (lit).push_back (OccrsHash (c, c->hash, c->unique));
    } else {
      if (cs.second > 2) subsume_check (c);
      const int lit = subsume_min_occrs (c);
      occurs (lit).push_back (c);
    }
  }
  return true;
}

/*-------------------------------------------------------------------------+
 | Collect candidates clauses for subsumption and sort.                    |
/*-------------------------------------------------------------------------*/
void CDCL::subsume_candidates ( ) {

/* Collect candidate clauses and count literal occurrences.
   We exclude clauses that are too large; root satisfied. */
  subsume_find_candidates (original);

/* Sort candidates by cardinality */
  std::sort (candsizes.begin ( ), candsizes.end ( ), Sort_cls_size_subsume ( ));
}

/*-------------------------------------------------------------------------+
 | Collect candidates clauses for subsumption from 'clauses'.              |
/*-------------------------------------------------------------------------*/
void CDCL::subsume_find_candidates (std::vector<Clause*>& clauses) {

  for (Clause* c : clauses)
    if (c->size ( ) <= lims.subsume_max_cls_size)
      candsizes.push_back (std::pair<Clause*,size_t>(c, c->size ( )));
}

/*-------------------------------------------------------------------------+
 | This is the subsumption check of my forward subsumption algorithm. It   |
 | checks whether 'c' is subsumed by or can be strengthened by some clause |
 | in 'occrs' (which at this point only contains smaller than or equally   |
 | long clauses as 'c').                                                   |
/*-------------------------------------------------------------------------*/
void CDCL::subsume_check (Clause* c) {

/* Mark literals in 'c' */
  mark_sign (c);
  Clause* sub = 0;
  int str = 0;
  int checks = 0;
  int matches = 0;
  
/* Check for subsumption and strengthening candidates in the positive
   occurrences of literals in 'c' */
  for (const int& lit : *c) {
    for (Clause* d : occurs (lit)) {
      str = subsumed (d);                   // check if subsumed
      if (str) { sub = d; goto found; }     // found candidate
    }
  }
/* Further check for strengthening candidates in the negative
   occurrences of literals in 'c' */
  for (const int& lit : *c) {
    for (Clause* d : occurs (-lit)) {
      str = subsumed (d);                   // check if 'd' strengthens 'c'
      if (str) { sub = d; goto found; }     // found candidate
    }
  }
/* Unmark literals in 'c' */
  found:;
  unmark (c);

/* Check if 'c' can be subsumed or strengthened */
  if (str == INT_MIN) ++stats.subsumed;         // 'sub' subsumes 'c'
  else if (str) ++stats.strengthened;           // 'str' strengthens 'c'
}

/*-------------------------------------------------------------------------+
 | This is the subsumption check of my forward subsumption algorithm. It   |
 | checks whether 'c' is subsumed by or can be strengthened by some clause |
 | in 'occrs' (which at this point only contains smaller than or equally   |
 | long clauses as 'c'). I have added the signature-based pre-check from   |
 | my paper "Hash-based preprocessing and inprocessing techniques in SAT   |
 | solvers".                                                               |
/*-------------------------------------------------------------------------*/
void CDCL::subsume_check_hash (Clause* c) {

/* Mark literals in 'c' */
  mark_sign (c);
  Clause* sub = 0;
  int str = 0;
  int checks = 0;
  int matches = 0;
  const uint64_t chash = ~c->hash;
  const uint64_t cunique = ~c->unique;
  
/* Check for subsumption and strengthening candidates in the positive
   occurrences of literals in 'c' */
  for (const int& lit : *c) {
    for (const OccrsHash& chu : occhash (lit)) {
      //++checks;
      if (chu.hash & chash) continue;
      if (chu.unique & cunique) continue;
      //if (chu.hash & chash) { ++matches; continue; }
      //if (chu.unique & cunique) { ++matches; continue; }
      str = subsumed (chu.c);                   // check if subsumed
      if (str) { sub = chu.c; goto found; }     // found candidate
    }
  }
/* Further check for strengthening candidates in the negative
   occurrences of literals in 'c' */
  for (const int& lit : *c) {
    for (const OccrsHash& chu : occhash (-lit)) {
      //++checks;
      if (chu.hash & chash) continue;
      if (chu.unique & cunique) continue;
      //if (chu.hash & chash) { ++matches; continue; }
      //if (chu.unique & cunique) { ++matches; continue; }
      str = subsumed (chu.c);                   // check if 'd' strengthens 'c'
      if (str) { sub = chu.c; goto found; }     // found candidate
    }
  }
/* Unmark literals in 'c' */
  found:;
  unmark (c);
  //stats.subsume_checks += checks;               // statistics
  //stats.subsume_hash_matches += matches;        // statistics

/* Check if 'c' can be subsumed or strengthened */
  if (str == INT_MIN) ++stats.subsumed;         // 'sub' subsumes 'c'
  else if (str) ++stats.strengthened;           // 'str' strengthens 'c'
}

/*-------------------------------------------------------------------------+
 | This is the true hot-spot of the subsumption algorithm. In order to che-|
 | ck whether 'c' subsumes the exterior (marked) clause, we have to compare|
 | the signs of literals in 'c', which involves accessing the mark through |
 | 'sign_marked', which merely returns the saved sign.                     |
 | Fuction :                                                               |
 | Returns INT_MIN if 'c' is subsumed.                                     |
 | Returns 'str' (literal to strengthen) if 'str' strengthens 'c'.         |
 | Otherwise returns 0.                                                    |
/*-------------------------------------------------------------------------*/
int CDCL::subsumed (const Clause* c) {

  int str = 0;
  for (const int& lit : *c) {
    const signed char sign = sign_marked (lit);
    if (!sign) return 0;                      // 'lit' unmarked
    else if (sign > 0) continue;              // 'lit' mark is positive
    else if (str) return 0;                   // two negated literals
    else str = lit;                           // strengthening literal
  }
  if (!str) return INT_MIN;                   // subsumption
  return str;                                 // strengthen
}

/*-------------------------------------------------------------------------+
 | Find the literal in 'c' in the smallest occurrence list.                |
/*-------------------------------------------------------------------------*/
int CDCL::subsume_min_occrs (const Clause* c) {

  int minlit = 0;
  size_t minsize = UINTMAX_MAX;
  for (const int& lit : *c) {
    const size_t size = occurs (lit).size ( );
    if (size >= minsize) continue;
    minlit = lit;
    minsize = size;
  }
  return minlit;
}

/*-------------------------------------------------------------------------+
 | Find the literal in 'c' in the smallest occurrence list.                |
/*-------------------------------------------------------------------------*/
int CDCL::subsume_min_occrs_hash (const Clause* c) {

  int minlit = 0;
  size_t minsize = UINTMAX_MAX;
  for (const int& lit : *c) {
    const size_t size = occhash (lit).size ( );
    if (size >= minsize) continue;
    minlit = lit;
    minsize = size;
  }
  return minlit;
}

} //End namespace sat