// Solves the diet problem using EasySCIP.
// by Ricardo Bittencourt 2013

#include <iostream>
#include "easyscip.h"

using namespace std;
using namespace easyscip;

int main() {
  // Create a MIPSolver using EasySCIP.
  MIPSolver solver;

  // Create one variable for each food type.
  // Each variable range from 0 to 1000, and the last coefficient is the price.
  Variable corn = solver.integer_variable(0, 1000, 0.08);
  Variable milk = solver.integer_variable(0, 1000, 0.23);
  Variable bread = solver.integer_variable(0, 1000, 0.05);

  // Add a constraint for the calories.
  Constraint calories = solver.constraint();
  calories.add_variable(corn, 72);
  calories.add_variable(milk, 121);
  calories.add_variable(bread, 65);
  calories.commit(2000, 200000);

  // Add a constraint for the vitamin A.
  Constraint vitamin_a = solver.constraint();
  vitamin_a.add_variable(corn, 107);
  vitamin_a.add_variable(milk, 500);
  vitamin_a.add_variable(bread, 0);
  vitamin_a.commit(5000, 500000);

  // Solve the MIP model.
  Solution sol = solver.solve();

  // Print solution.
  cout << "Corn: " << sol.value(corn) << "\n";
  cout << "Milk: " << sol.value(milk) << "\n";
  cout << "Bread: " << sol.value(bread) << "\n";

  // No need to worry about memory management, everything is released 
  // when the objects are deleted after falling out of scope.
  return 0;
}
