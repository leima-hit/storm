#include "storm/solver/GlpkLpSolver.h"

#ifdef STORM_HAVE_GLPK

#include <iostream>

#include "storm/storage/expressions/LinearCoefficientVisitor.h"

#include "storm/settings/SettingsManager.h"
#include "storm/utility/macros.h"
#include "storm/storage/expressions/Expression.h"
#include "storm/storage/expressions/ExpressionManager.h"

#include "storm/exceptions/InvalidAccessException.h"
#include "storm/exceptions/InvalidStateException.h"
#include "storm/exceptions/InvalidArgumentException.h"

#include "storm/settings/modules/DebugSettings.h"
#include "storm/settings/modules/GlpkSettings.h"

#include <cmath>

namespace storm {
    namespace solver {
        GlpkLpSolver::GlpkLpSolver(std::string const& name, OptimizationDirection const& optDir) : LpSolver(optDir), lp(nullptr), variableToIndexMap(), nextVariableIndex(1), nextConstraintIndex(1), modelContainsIntegerVariables(false), isInfeasibleFlag(false), isUnboundedFlag(false), rowIndices(), columnIndices(), coefficientValues() {
            // Create the LP problem for glpk.
            lp = glp_create_prob();
            
            // Set its name and model sense.
            glp_set_prob_name(lp, name.c_str());
            
            // Set whether the glpk output shall be printed to the command line.
            glp_term_out(storm::settings::getModule<storm::settings::modules::DebugSettings>().isDebugSet() || storm::settings::getModule<storm::settings::modules::GlpkSettings>().isOutputSet() ? GLP_ON : GLP_OFF);
            
            // Because glpk uses 1-based indexing (wtf!?), we need to put dummy elements into the matrix vectors.
            rowIndices.push_back(0);
            columnIndices.push_back(0);
            coefficientValues.push_back(0);
        }
        
        GlpkLpSolver::GlpkLpSolver(std::string const& name) : GlpkLpSolver(name, OptimizationDirection::Minimize) {
            // Intentionally left empty.
        }
        
        GlpkLpSolver::GlpkLpSolver() : GlpkLpSolver("", OptimizationDirection::Minimize) {
            // Intentionally left empty.
        }
        
        GlpkLpSolver::GlpkLpSolver(OptimizationDirection const& optDir) : GlpkLpSolver("", optDir) {
            // Intentionally left empty.
        }
        
        GlpkLpSolver::~GlpkLpSolver() {
            // Dispose of all objects allocated dynamically by glpk.
            glp_delete_prob(this->lp);
            glp_free_env();
        }
        
        storm::expressions::Variable GlpkLpSolver::addBoundedContinuousVariable(std::string const& name, double lowerBound, double upperBound, double objectiveFunctionCoefficient) {
            storm::expressions::Variable newVariable = manager->declareRationalVariable(name);
            this->addVariable(newVariable, GLP_CV, GLP_DB, lowerBound, upperBound, objectiveFunctionCoefficient);
            return newVariable;
        }
        
        storm::expressions::Variable GlpkLpSolver::addLowerBoundedContinuousVariable(std::string const& name, double lowerBound, double objectiveFunctionCoefficient) {
            storm::expressions::Variable newVariable = manager->declareRationalVariable(name);
            this->addVariable(newVariable, GLP_CV, GLP_LO, lowerBound, 0, objectiveFunctionCoefficient);
            return newVariable;
        }
        
        storm::expressions::Variable GlpkLpSolver::addUpperBoundedContinuousVariable(std::string const& name, double upperBound, double objectiveFunctionCoefficient) {
            storm::expressions::Variable newVariable = manager->declareRationalVariable(name);
            this->addVariable(newVariable, GLP_CV, GLP_UP, 0, upperBound, objectiveFunctionCoefficient);
            return newVariable;
        }
        
        storm::expressions::Variable GlpkLpSolver::addUnboundedContinuousVariable(std::string const& name, double objectiveFunctionCoefficient) {
            storm::expressions::Variable newVariable = manager->declareRationalVariable(name);
            this->addVariable(newVariable, GLP_CV, GLP_FR, 0, 0, objectiveFunctionCoefficient);
            return newVariable;
        }
        
        storm::expressions::Variable GlpkLpSolver::addBoundedIntegerVariable(std::string const& name, double lowerBound, double upperBound, double objectiveFunctionCoefficient) {
            storm::expressions::Variable newVariable = manager->declareIntegerVariable(name);
            this->addVariable(newVariable, GLP_IV, GLP_DB, lowerBound, upperBound, objectiveFunctionCoefficient);
            this->modelContainsIntegerVariables = true;
            return newVariable;
        }
        
        storm::expressions::Variable GlpkLpSolver::addLowerBoundedIntegerVariable(std::string const& name, double lowerBound, double objectiveFunctionCoefficient) {
            storm::expressions::Variable newVariable = manager->declareIntegerVariable(name);
            this->addVariable(newVariable, GLP_IV, GLP_LO, lowerBound, 0, objectiveFunctionCoefficient);
            this->modelContainsIntegerVariables = true;
            return newVariable;
        }

        storm::expressions::Variable GlpkLpSolver::addUpperBoundedIntegerVariable(std::string const& name, double upperBound, double objectiveFunctionCoefficient) {
            storm::expressions::Variable newVariable = manager->declareIntegerVariable(name);
            this->addVariable(newVariable, GLP_IV, GLP_UP, 0, upperBound, objectiveFunctionCoefficient);
            this->modelContainsIntegerVariables = true;
            return newVariable;
        }
        
        storm::expressions::Variable GlpkLpSolver::addUnboundedIntegerVariable(std::string const& name, double objectiveFunctionCoefficient) {
            storm::expressions::Variable newVariable = manager->declareIntegerVariable(name);
            this->addVariable(newVariable, GLP_IV, GLP_FR, 0, 0, objectiveFunctionCoefficient);
            this->modelContainsIntegerVariables = true;
            return newVariable;
        }
        
        storm::expressions::Variable GlpkLpSolver::addBinaryVariable(std::string const& name, double objectiveFunctionCoefficient) {
            storm::expressions::Variable newVariable = manager->declareIntegerVariable(name);
            this->addVariable(newVariable, GLP_BV, GLP_FR, 0, 0, objectiveFunctionCoefficient);
            this->modelContainsIntegerVariables = true;
            return newVariable;
        }
        
        void GlpkLpSolver::addVariable(storm::expressions::Variable const& variable, int variableType, int boundType, double lowerBound, double upperBound, double objectiveFunctionCoefficient) {
            // Check for valid variable type.
            STORM_LOG_ASSERT(variableType == GLP_CV || variableType == GLP_IV || variableType == GLP_BV, "Illegal type '" << variableType << "' for glpk variable.");
            
            // Check for valid bound type.
            STORM_LOG_ASSERT(boundType == GLP_FR || boundType == GLP_UP || boundType == GLP_LO || boundType == GLP_DB, "Illegal bound type for variable '" << variable.getName() << "'.");
            
            // Finally, create the actual variable.
            glp_add_cols(this->lp, 1);
            glp_set_col_name(this->lp, nextVariableIndex, variable.getName().c_str());
            glp_set_col_bnds(lp, nextVariableIndex, boundType, lowerBound, upperBound);
            glp_set_col_kind(this->lp, nextVariableIndex, variableType);
            glp_set_obj_coef(this->lp, nextVariableIndex, objectiveFunctionCoefficient);
            this->variableToIndexMap.emplace(variable, this->nextVariableIndex);
            ++this->nextVariableIndex;
        }
        
        void GlpkLpSolver::update() const {
            // Intentionally left empty.
        }
        
        void GlpkLpSolver::addConstraint(std::string const& name, storm::expressions::Expression const& constraint) {
            // Add the row that will represent this constraint.
            glp_add_rows(this->lp, 1);
            glp_set_row_name(this->lp, nextConstraintIndex, name.c_str());
            
            STORM_LOG_THROW(constraint.getManager() == this->getManager(), storm::exceptions::InvalidArgumentException, "Constraint was not built over the proper variables.");
            STORM_LOG_THROW(constraint.isRelationalExpression(), storm::exceptions::InvalidArgumentException, "Illegal constraint is not a relational expression.");
            STORM_LOG_THROW(constraint.getOperator() != storm::expressions::OperatorType::NotEqual, storm::exceptions::InvalidArgumentException, "Illegal constraint uses inequality operator.");
            
            storm::expressions::LinearCoefficientVisitor::VariableCoefficients leftCoefficients = storm::expressions::LinearCoefficientVisitor().getLinearCoefficients(constraint.getOperand(0));
            storm::expressions::LinearCoefficientVisitor::VariableCoefficients rightCoefficients = storm::expressions::LinearCoefficientVisitor().getLinearCoefficients(constraint.getOperand(1));
            leftCoefficients.separateVariablesFromConstantPart(rightCoefficients);
            
            // Now we need to transform the coefficients to the vector representation.
            std::vector<int> variables;
            std::vector<double> coefficients;
            for (auto const& variableCoefficientPair : leftCoefficients) {
                auto variableIndexPair = this->variableToIndexMap.find(variableCoefficientPair.first);
                variables.push_back(variableIndexPair->second);
                coefficients.push_back(leftCoefficients.getCoefficient(variableIndexPair->first));
            }
            
            // Determine the type of the constraint and add it properly.
            switch (constraint.getOperator()) {
                case storm::expressions::OperatorType::Less:
                    glp_set_row_bnds(this->lp, nextConstraintIndex, GLP_UP, 0, rightCoefficients.getConstantPart() - storm::settings::getModule<storm::settings::modules::GlpkSettings>().getIntegerTolerance());
                    break;
                case storm::expressions::OperatorType::LessOrEqual:
                    glp_set_row_bnds(this->lp, nextConstraintIndex, GLP_UP, 0, rightCoefficients.getConstantPart());
                    break;
                case storm::expressions::OperatorType::Greater:
                    glp_set_row_bnds(this->lp, nextConstraintIndex, GLP_LO, rightCoefficients.getConstantPart() + storm::settings::getModule<storm::settings::modules::GlpkSettings>().getIntegerTolerance(), 0);
                    break;
                case storm::expressions::OperatorType::GreaterOrEqual:
                    glp_set_row_bnds(this->lp, nextConstraintIndex, GLP_LO, rightCoefficients.getConstantPart(), 0);
                    break;
                case storm::expressions::OperatorType::Equal:
                    glp_set_row_bnds(this->lp, nextConstraintIndex, GLP_FX, rightCoefficients.getConstantPart(), rightCoefficients.getConstantPart());
                    break;
                default:
                    STORM_LOG_ASSERT(false, "Illegal operator in LP solver constraint.");
            }
            
            // Record the variables and coefficients in the coefficient matrix.
            rowIndices.insert(rowIndices.end(), variables.size(), nextConstraintIndex);
            columnIndices.insert(columnIndices.end(), variables.begin(), variables.end());
            coefficientValues.insert(coefficientValues.end(), coefficients.begin(), coefficients.end());
            
            ++nextConstraintIndex;
            this->currentModelHasBeenOptimized = false;
        }
        
        void GlpkLpSolver::optimize() const {
            // First, reset the flags.
            this->isInfeasibleFlag = false;
            this->isUnboundedFlag = false;
            
            // Start by setting the model sense.
            glp_set_obj_dir(this->lp, this->getOptimizationDirection() == OptimizationDirection::Minimize ? GLP_MIN : GLP_MAX);
            
            glp_load_matrix(this->lp, rowIndices.size() - 1, rowIndices.data(), columnIndices.data(), coefficientValues.data());
            
            int error = 0;
            if (this->modelContainsIntegerVariables) {
                glp_iocp* parameters = new glp_iocp();
                glp_init_iocp(parameters);
                parameters->presolve = GLP_ON;
                parameters->tol_int = storm::settings::getModule<storm::settings::modules::GlpkSettings>().getIntegerTolerance();
                error = glp_intopt(this->lp, parameters);
                delete parameters;
                
                // In case the error is caused by an infeasible problem, we do not want to view this as an error and
                // reset the error code.
                if (error == GLP_ENOPFS) {
                    this->isInfeasibleFlag = true;
                    error = 0;
                } else if (error == GLP_ENODFS) {
                    this->isUnboundedFlag = true;
                    error = 0;
                } else if (error == GLP_EBOUND) {
                    throw storm::exceptions::InvalidStateException() << "The bounds of some variables are illegal. Note that glpk only accepts integer bounds for integer variables.";
                }
            } else {
                error = glp_simplex(this->lp, nullptr);
            }
            
            STORM_LOG_THROW(error == 0, storm::exceptions::InvalidStateException, "Unable to optimize glpk model (" << error << ").");
            this->currentModelHasBeenOptimized = true;
        }
        
        bool GlpkLpSolver::isInfeasible() const {
            if (!this->currentModelHasBeenOptimized) {
                throw storm::exceptions::InvalidStateException() << "Illegal call to GlpkLpSolver::isInfeasible: model has not been optimized.";
            }

            if (this->modelContainsIntegerVariables) {
                return isInfeasibleFlag;
            } else {
                return glp_get_status(this->lp) == GLP_INFEAS || glp_get_status(this->lp) == GLP_NOFEAS;
            }
        }
        
        bool GlpkLpSolver::isUnbounded() const {
            if (!this->currentModelHasBeenOptimized) {
                throw storm::exceptions::InvalidStateException() << "Illegal call to GlpkLpSolver::isUnbounded: model has not been optimized.";
            }

            if (this->modelContainsIntegerVariables) {
                return isUnboundedFlag;
            } else {
                return glp_get_status(this->lp) == GLP_UNBND;
            }
        }
        
        bool GlpkLpSolver::isOptimal() const {
            if (!this->currentModelHasBeenOptimized) {
                return false;
            }
            
            int status = 0;
            if (this->modelContainsIntegerVariables) {
                status = glp_mip_status(this->lp);
            } else {
                status = glp_get_status(this->lp);
            }
            return status == GLP_OPT;
        }
        
        double GlpkLpSolver::getContinuousValue(storm::expressions::Variable const& variable) const {
            if (!this->isOptimal()) {
                STORM_LOG_THROW(!this->isInfeasible(), storm::exceptions::InvalidAccessException, "Unable to get glpk solution from infeasible model.");
                STORM_LOG_THROW(!this->isUnbounded(),  storm::exceptions::InvalidAccessException, "Unable to get glpk solution from unbounded model.");
                STORM_LOG_THROW(false, storm::exceptions::InvalidAccessException, "Unable to get glpk solution from unoptimized model.");
            }
            
            auto variableIndexPair = this->variableToIndexMap.find(variable);
            STORM_LOG_THROW(variableIndexPair != this->variableToIndexMap.end(), storm::exceptions::InvalidAccessException, "Accessing value of unknown variable '" << variable.getName() << "'.");
            
            double value = 0;
            if (this->modelContainsIntegerVariables) {
                value = glp_mip_col_val(this->lp, static_cast<int>(variableIndexPair->second));
            } else {
                value = glp_get_col_prim(this->lp, static_cast<int>(variableIndexPair->second));
            }
            return value;
        }
        
        int_fast64_t GlpkLpSolver::getIntegerValue(storm::expressions::Variable const& variable) const {
            if (!this->isOptimal()) {
                STORM_LOG_THROW(!this->isInfeasible(), storm::exceptions::InvalidAccessException, "Unable to get glpk solution from infeasible model.");
                STORM_LOG_THROW(!this->isUnbounded(),  storm::exceptions::InvalidAccessException, "Unable to get glpk solution from unbounded model.");
                STORM_LOG_THROW(false, storm::exceptions::InvalidAccessException, "Unable to get glpk solution from unoptimized model.");
            }
           
            auto variableIndexPair = this->variableToIndexMap.find(variable);
            STORM_LOG_THROW(variableIndexPair != this->variableToIndexMap.end(), storm::exceptions::InvalidAccessException, "Accessing value of unknown variable '" << variable.getName() << "'.");

            double value = 0;
            if (this->modelContainsIntegerVariables) {
                value = glp_mip_col_val(this->lp, static_cast<int>(variableIndexPair->second));
            } else {
                value = glp_get_col_prim(this->lp, static_cast<int>(variableIndexPair->second));
            }

            // Now check the desired precision was actually achieved.
            STORM_LOG_THROW(std::fabs(static_cast<int>(value) - value) <= storm::settings::getModule<storm::settings::modules::GlpkSettings>().getIntegerTolerance(), storm::exceptions::InvalidStateException, "Illegal value for integer variable in glpk solution (" << value << ").");
            
            return static_cast<int_fast64_t>(value);
        }
        
        bool GlpkLpSolver::getBinaryValue(storm::expressions::Variable const& variable) const {
            if (!this->isOptimal()) {
                STORM_LOG_THROW(!this->isInfeasible(), storm::exceptions::InvalidAccessException, "Unable to get glpk solution from infeasible model.");
                STORM_LOG_THROW(!this->isUnbounded(),  storm::exceptions::InvalidAccessException, "Unable to get glpk solution from unbounded model.");
                STORM_LOG_THROW(false, storm::exceptions::InvalidAccessException, "Unable to get glpk solution from unoptimized model.");
            }

            auto variableIndexPair = this->variableToIndexMap.find(variable);
            STORM_LOG_THROW(variableIndexPair != this->variableToIndexMap.end(), storm::exceptions::InvalidAccessException, "Accessing value of unknown variable '" << variable.getName() << "'.");
            
            double value = 0;
            if (this->modelContainsIntegerVariables) {
                value = glp_mip_col_val(this->lp, static_cast<int>(variableIndexPair->second));
            } else {
                value = glp_get_col_prim(this->lp, static_cast<int>(variableIndexPair->second));
            }

            STORM_LOG_THROW(std::fabs(static_cast<int>(value) - value) <= storm::settings::getModule<storm::settings::modules::GlpkSettings>().getIntegerTolerance(), storm::exceptions::InvalidStateException, "Illegal value for binary variable in glpk solution (" << value << ").");
            
            return static_cast<bool>(value);
        }
        
        double GlpkLpSolver::getObjectiveValue() const {
            if (!this->isOptimal()) {
                STORM_LOG_THROW(!this->isInfeasible(), storm::exceptions::InvalidAccessException, "Unable to get glpk solution from infeasible model.");
                STORM_LOG_THROW(!this->isUnbounded(),  storm::exceptions::InvalidAccessException, "Unable to get glpk solution from unbounded model.");
                STORM_LOG_THROW(false, storm::exceptions::InvalidAccessException, "Unable to get glpk solution from unoptimized model.");
            }

            double value = 0;
            if (this->modelContainsIntegerVariables) {
                value = glp_mip_obj_val(this->lp);
            } else {
                value = glp_get_obj_val(this->lp);
            }

            return value;
        }
        
        void GlpkLpSolver::writeModelToFile(std::string const& filename) const {
            glp_load_matrix(this->lp, rowIndices.size() - 1, rowIndices.data(), columnIndices.data(), coefficientValues.data());
            glp_write_lp(this->lp, 0, filename.c_str());
        }
    }
}

#endif
