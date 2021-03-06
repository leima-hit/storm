#include "storm/solver/SolverSelectionOptions.h"

namespace storm {
    namespace solver {
        std::string toString(MinMaxMethod m) {
            switch(m) {
                case MinMaxMethod::PolicyIteration:
                    return "policy";
                case MinMaxMethod::ValueIteration:
                    return "value";
                case MinMaxMethod::Topological:
                    return "topological";

            }
            return "invalid";
        }
        
        std::string toString(LpSolverType t) {
            switch(t) {
                case LpSolverType::Gurobi:
                    return "Gurobi";
                case LpSolverType::Glpk:
                    return "Glpk";
            }
            return "invalid";
        }
        
        std::string toString(EquationSolverType t) {
            switch(t) {
                case EquationSolverType::Native:
                    return "Native";
                case EquationSolverType::Gmmxx:
                    return "Gmmxx";
                case EquationSolverType::Eigen:
                    return "Eigen";
                case EquationSolverType::Elimination:
                    return "Elimination";
            }
            return "invalid";
        }
        
        std::string toString(SmtSolverType t) {
            switch(t) {
                case SmtSolverType::Z3:
                    return "Z3";
                case SmtSolverType::Mathsat:
                    return "Mathsat";
            }
            return "invalid";
        }
    }
}
