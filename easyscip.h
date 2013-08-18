// EasySCIP 0.1
// A C++ interface to SCIP that is easy to use.
// by Ricardo Bittencourt 2013

// Please check sendmoremoney_binary.cc for a sample usage.

#include <vector>
#include "objscip/objscip.h"
#include "objscip/objscipdefplugins.h"

namespace easyscip {

class Constraint;
class Solution;
class MIPSolver;

class Variable {
 protected:
  Variable() : var_(NULL) {
  }
  SCIP_VAR *var_;
  friend Constraint;
  friend Solution;
  friend MIPSolver;
};

class BinaryVariable : public Variable {
 protected:
  BinaryVariable(SCIP *scip, double objective) {
    SCIPcreateVarBasic(
        scip, &var_, "variable", 0, 1, objective, SCIP_VARTYPE_BINARY);
    SCIPaddVar(scip, var_);
  }
  friend MIPSolver;
};

class Constraint {
 public:
  void add_variable(Variable& var, double val) {
    vars_.push_back(var.var_);
    vals_.push_back(val);
  }
  void commit(double lower_bound, double upper_bound) {
    SCIP_VAR **vars = new SCIP_VAR*[vars_.size()];
    SCIP_Real *vals = new SCIP_Real[vals_.size()];
    copy(vars_.begin(), vars_.end(), vars);
    copy(vals_.begin(), vals_.end(), vals);
    SCIP_CONS *cons;
    SCIPcreateConsLinear(scip_, &cons, "constraint", vars_.size(), vars, vals, 
        lower_bound, upper_bound, TRUE, TRUE, TRUE, TRUE, TRUE,
        FALSE, FALSE, FALSE, FALSE, FALSE);
    SCIPaddCons(scip_, cons);
    SCIPreleaseCons(scip_, &cons);
    delete[] vars;
    delete[] vals;
  }
 protected:
  SCIP *scip_;
  std::vector<SCIP_VAR*> vars_;
  std::vector<SCIP_Real> vals_;
  Constraint(SCIP *scip) : scip_(scip) {
  }
  friend MIPSolver;
};

class Solution {
 public:
  double objective() {
    return SCIPgetSolOrigObj(scip_, sol_);
  }
  double value(Variable& var) {
    return SCIPgetSolVal(scip_, sol_, var.var_);
  }
  bool is_optimal() {
    return SCIPgetStatus(scip_) == SCIP_STATUS_OPTIMAL;
  }
  bool is_feasible() {
    return SCIPgetStatus(scip_) != SCIP_STATUS_INFEASIBLE;
  }
 protected:
  Solution(SCIP *scip, SCIP_Sol *sol) : scip_(scip), sol_(sol) {
  }
  SCIP *scip_;
  SCIP_Sol *sol_;
  friend MIPSolver;
};

class MIPSolver {
 public:
  MIPSolver() {
    SCIPcreate (&scip_);
    SCIPsetMessagehdlrLogfile(scip_, "log.txt");
    SCIPprintVersion(scip_, NULL);
    SCIPsetEmphasis(scip_, SCIP_PARAMEMPHASIS_FEASIBILITY, FALSE);
    SCIPincludeDefaultPlugins(scip_);
    SCIPcreateProbBasic(scip_, "MIP");
  }
  ~MIPSolver() {
    SCIPfree(&scip_);
  }
  Variable binary_variable(double objective) {
    return BinaryVariable(scip_, objective);
  }
  Constraint constraint() {
    return Constraint(scip_);
  }
  Solution solve() {
    SCIPsolve(scip_);
    return Solution(scip_, SCIPgetBestSol(scip_));
  }
  void set_time_limit(int seconds) {
    SCIPsetRealParam(scip_, "limits/time", seconds);
  }
 private:
  SCIP *scip_;
};

}  // namespace easyscip
