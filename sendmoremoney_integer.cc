// Solves the classic puzzle SEND+MORE=MONEY using EasySCIP.
// by Ricardo Bittencourt 2013

#include <iostream>
#include <limits>
#include <vector>
#include "easyscip.h"

using namespace std;
using namespace easyscip;

int main() {
  // Create a MIPSolver using EasySCIP.
  MIPSolver solver;

  // Add one binary variable for each assignment between letter and digit.
  string unknowns = "sendmory";
  vector< vector<Variable> > var(unknowns.size());
  for (int i = 0; i < int(unknowns.size()); i++) {
    for (int j = 0; j <= 9; j++) {
      var[i].push_back(solver.binary_variable(0));
    }
  }

  // Add one integer variable for the result.
  Variable result = solver.integer_variable(0, 1e6, 1);

  // Build the result as the sum of all signatures.
  int signatures[] = {1000, 100+1-10, 10-100, 1, 1000-10000, 100-1000, 10, -1};
  Constraint cons = solver.constraint();
  for (int i = 0; i < int(unknowns.size()); i++) {
    for (int j = 0; j <= 9; j++) {
      cons.add_variable(var[i][j], j * signatures[i]);
    }
  }
  cons.add_variable(result, -1);
  cons.commit(0, 0);

  // Each letter must be assigned to exactly one digit.
  // We model this as one linear constraint by letter, of the form:
  // L_0 + L_1 + ... + L_9 = 1
  for (int i = 0; i < int(unknowns.size()); i++) {
    Constraint letter = solver.constraint();
    for (int j = 0; j <= 9; j++) {
      letter.add_variable(var[i][j], 1);
    }
    letter.commit(1, 1);
  }

  // Each digit must be assigned to no more than one letter.
  // We model this as one linear constraint by letter, of the form:
  // 0 <= D_0 + D_1 + ... + D_9 <= 1
  // This is an inequality since there may be digits without a letter assigned.
  for (int j = 0; j <= 9; j++) {
    Constraint digit = solver.constraint();
    for (int i = 0; i < int(unknowns.size()); i++) {
      digit.add_variable(var[i][j], 1);
    }
    digit.commit(0, 1);
  }

  Constraint s_first = solver.constraint();
  s_first.add_variable(var[0][0], 1);
  s_first.commit(0, 0);

  Constraint m_first = solver.constraint();
  m_first.add_variable(var[4][0], 1);
  m_first.commit(0, 0);

  // Solve the MIP model.
  Solution sol = solver.solve();

  // Print solution.
  for (int i = 0; i < int(unknowns.size()); i++) {
    for (int j = 0; j <= 9; j++) {
      if (sol.value(var[i][j]) > 0.5) {
        cout << unknowns[i] << " = " << j << "\n";
      }
    }
  }

  // No need to worry about memory management, everything is released 
  // when the objects are deleted after falling out of scope.
  return 0;
}
