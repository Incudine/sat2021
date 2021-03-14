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
 | Read clauses from DIMACS file.                                          |
/*-------------------------------------------------------------------------*/
bool CDCL::read_dimacs (const std::string file_name) {

  assert (unit_clauses.empty ( ));
  assert (clause_lits.empty ( ));

  if (modes.print) std::cout << "[INPUT] reading from dimacs file '" << file_name << "'" << std::endl;
  auto read_start = std::chrono::high_resolution_clock::now ( );
  std::string line;
  std::ifstream dimacs;
  dimacs.open (file_name.c_str ( ), std::ios::binary);

  if (dimacs.is_open ( )) {

  /* Skip comment lines at the beginning of file */
    while (std::getline (dimacs, line) && line[0] == 'c');

  /* Check whether header format is PWCNF */
    if (modes.print) printf ("[INPUT] header '%s'\n", line.c_str ( ));
    if (line[0] == 'p' && line[2] == 'c' && line[3] == 'n' && line[4] == 'f') {

    /* Read header parameters */
      line.erase (0, 6);
      std::istringstream stream (line);
      stream >> param.vars >> param.n_cls_start;
      assert (param.vars > 0);
      assert (param.n_cls_start > 0);
      param.n_cls_start;

    /* Allocate clause database */
      assert (original.empty ( ));
      std::vector<int> lits;
      int ci = 0;
      int tautologies = 0;

    /* Initialize model for tautology spotting */
      model = new int[2*param.vars + 1] ( );
      model += param.vars;

    /* Read literals from file */
      char ch;
      int lit;
      bool sign;
      dimacs.get (ch);
      while (ch == ' ') dimacs.get (ch);
      assert (ch == '-' || std::isdigit (ch));
      assert (!dimacs.eof ( ));
      for ( ; !dimacs.eof ( ); ++ci) {
        lits.clear ( );
        assert (ch != '0');
        for ( ; ch != '0'; ) {
          if (ch == '-') { sign = true; dimacs.get (ch); }
          else sign = false;
          assert (std::isdigit (ch));
          lit = 0;
          for ( ; std::isdigit (ch); dimacs.get (ch))
            lit = 10 * lit + static_cast<int> (ch - '0');
          if (sign) lit = -lit;
          lits.push_back (lit);
          while (ch == ' ') {
            dimacs.get (ch);
            assert (!dimacs.eof ( ));
          }
        }
        assert (ch == '0');
        dimacs.get (ch);
        while (!dimacs.eof ( ) && !(isdigit (ch) || ch == '-'))
          dimacs.get (ch);

      /* allocate new clause */
        if (lits.empty ( )) break;
        const bool taut = tautology (lits);
        if (!taut) {
          if (lits.size ( ) == 1) {
            unit_clauses.push_back (lits[0]);
          } else {
            Clause* c = new_clause (lits);
            original.push_back (c);
            assert (!lits.empty ( ));
          }
        }
      }

    /* Handle insufficient / excess clauses */
      if (ci < param.n_cls_start) {
        std::cout << "[INPUT] WARNING: Found " << ci << "clauses: Expected " << param.n_cls_start << std::endl;
        assert (false);
      } else if (ci > param.n_cls_start) {
        //int clause_count = param.n_cls_start;
        //while (std::getline (dimacs, line)) ++clause_count;
        std::cout << "[INPUT] WARNING: Found " << ci << " clauses. Truncating to " << param.n_cls_start << " clauses." << std::endl;
        assert (false);
      }
      dimacs.close ( );
      auto read_end = std::chrono::high_resolution_clock::now ( );
      //std::cout << "TIME (read) : " <<  std::chrono::duration<double>(read_end - read_start).count ( ) << std::endl;
      return true;
    }
  /* Error in format */
    std::cout << "WARNING : Error in CNF format header. Could not read pwcnf header." << std::endl;
    dimacs.close ( );
    return false;
  }
/* Return failure */
  std::cout << "WARNING: Failed to extract formula! File (" << file_name << ") could not be opened." << std::endl;
  return false;
}

/*-------------------------------------------------------------------------+
 | Check for tautotlogical clauses and remove redundant literals.          |
/*-------------------------------------------------------------------------*/
bool CDCL::tautology (std::vector<int>& lits) {
  
  bool taut = false;
  for (int l = 0; l < lits.size ( ); ++l) {
    const int lit = lits[l];
    if (!value (lit)) set_model (lit);          // set model
    else if (value (lit) > 0) {                 // redundant literal
      lits[l] = lits.back ( );
      lits.pop_back ( );
      --l;
    } else { taut = true; break; }              // tautology
  }
  for (const int lit : lits)
    unset_model (lit);
  return taut;
}

} //End namespace sat