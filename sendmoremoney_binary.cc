// Solves the classic puzzle SEND+MORE=MONEY using EasySCIP.
// by Ricardo Bittencourt 2013

#include <iostream>
#include "easyscip.h"

using namespace std;
using namespace easyscip;

int main() {
  // Create a MIPSolver using EasySCIP.
  MIPSolver solver;

  // Add one binary variable for each assignment between letter and digit.
  // We're looking only for feasibility, not optimility, so we're
  // setting the objective coefficient for all variables to 0.
  string unknowns = "sendmory";
  vector< vector<Variable> > var(unknowns.size());
  for (int i = 0; i < int(unknowns.size()); i++) {
    for (int j = 0; j <= 9; j++) {
      var[i].push_back(solver.binary_variable(0));
    }
  }

  // Add one binary variable for each carry.
  // Again the coefficient is zero.
  vector<Variable> carry;
  for (int i = 0; i < 4; i++) {
    carry.push_back(solver.binary_variable(0));
  }

  // Add a constraint for the last column: D + E = Y + 10 * carry[0];
  // This is done by a linear equation, where each letter is added ten times:
  //   0*D_0 + 1*D_1 + .... + 9*D_9
  // + 0*E_0 + 1*E_1 + .... + 9*E_9
  // - 0*Y_0 - 1*Y_1 - .... - 9*Y_9
  // - 10*carry[0] == 0

  // Here we're creating the constraint.
  Constraint column0 = solver.constraint();
  // Add all relevant variables to the constraint.
  for (int i = 0; i <= 9; i++) {
    column0.add_variable(var[unknowns.find('d')][i], i);
    column0.add_variable(var[unknowns.find('e')][i], i);
    column0.add_variable(var[unknowns.find('y')][i], -i);
  }
  column0.add_variable(carry[0], -10);
  // Commit the constraint to the solver.
  // The solver uses inequalities for the form lhs <= x <= rhs,
  // but we can turn that into an equality by setting lhs=rhs.
  // The Constraint object can be deleted after it's committed.
  // In this case, it will be deleted when it falls out of the scope.
  column0.commit(0, 0);

  // Constraint: N + R + carry[0] = E + 10 * carry[1];
  Constraint column1 = solver.constraint();
  for (int i = 0; i <= 9; i++) {
    column1.add_variable(var[unknowns.find('n')][i], i);
    column1.add_variable(var[unknowns.find('r')][i], i);
    column1.add_variable(var[unknowns.find('e')][i], -i);
  }
  column1.add_variable(carry[0], 1);
  column1.add_variable(carry[1], -10);
  column1.commit(0, 0);

  // Constraint: E + O + carry[1] = N + 10 * carry[2];
  Constraint column2 = solver.constraint();
  for (int i = 0; i <= 9; i++) {
    column2.add_variable(var[unknowns.find('e')][i], i);
    column2.add_variable(var[unknowns.find('o')][i], i);
    column2.add_variable(var[unknowns.find('n')][i], -i);
  }
  column2.add_variable(carry[1], 1);
  column2.add_variable(carry[2], -10);
  column2.commit(0, 0);

  // Constraint: S + M + carry[2] = O + 10 * carry[3];
  Constraint column3 = solver.constraint();
  for (int i = 0; i <= 9; i++) {
    column3.add_variable(var[unknowns.find('s')][i], i);
    column3.add_variable(var[unknowns.find('m')][i], i);
    column3.add_variable(var[unknowns.find('o')][i], -i);
  }
  column3.add_variable(carry[2], 1);
  column3.add_variable(carry[3], -10);
  column3.commit(0, 0);

  // Constraint: carry[3] = M;
  Constraint column4 = solver.constraint();
  for (int i = 0; i <= 9; i++) {
    column4.add_variable(var[unknowns.find('m')][i], -i);
  }
  column4.add_variable(carry[3], 1);
  column4.commit(0, 0);

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
