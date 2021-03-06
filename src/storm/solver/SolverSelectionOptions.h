#ifndef SOLVERSELECTIONOPTIONS_H
#define	SOLVERSELECTIONOPTIONS_H


#include "storm/utility/ExtendSettingEnumWithSelectionField.h"

namespace storm {
    namespace solver {
        ExtendEnumsWithSelectionField(MinMaxMethod, PolicyIteration, ValueIteration, Topological)
        
        ExtendEnumsWithSelectionField(LpSolverType, Gurobi, Glpk)
        ExtendEnumsWithSelectionField(EquationSolverType, Native, Gmmxx, Eigen, Elimination)
        ExtendEnumsWithSelectionField(SmtSolverType, Z3, Mathsat)
    }
} 

#endif	

