#ifndef STORM_MODELCHECKER_MULTIOBJECTIVE_HELPER_SPARSEMULTIOBJECTIVEHELPERRETURNTYPE_H_
#define STORM_MODELCHECKER_MULTIOBJECTIVE_HELPER_SPARSEMULTIOBJECTIVEHELPERRETURNTYPE_H_

#include <vector>
#include <memory>
#include <boost/optional.hpp>

#include "src/modelchecker/multiobjective/helper/SparseMultiObjectiveHelperRefinementStep.h"
#include "src/storage/geometry/Polytope.h"
#include "src/utility/macros.h"

#include "src/exceptions/InvalidStateException.h"

namespace storm {
    namespace modelchecker {
        namespace helper {

            template <typename RationalNumberType>
            class SparseMultiObjectiveHelperReturnType {
                
            public:
                std::vector<SparseMultiObjectiveHelperRefinementStep<RationalNumberType>>& refinementSteps() {
                    return steps;
                }
                std::vector<SparseMultiObjectiveHelperRefinementStep<RationalNumberType>> const& refinementSteps() const {
                    return steps;
                }
                
                std::shared_ptr<storm::storage::geometry::Polytope<RationalNumberType>>& overApproximation() {
                    return overApprox;
                }
                std::shared_ptr<storm::storage::geometry::Polytope<RationalNumberType>> const& overApproximation() const {
                    return overApprox;
                }
                
                std::shared_ptr<storm::storage::geometry::Polytope<RationalNumberType>>& underApproximation() {
                    return underApprox;
                }
                std::shared_ptr<storm::storage::geometry::Polytope<RationalNumberType>> const& underApproximation() const {
                    return underApprox;
                }

                void setThresholdsAreAchievable(bool value) {
                    thresholdsAreAchievable = value ? Tribool::TT : Tribool::FF;
                }
                bool isThresholdsAreAchievableSet() const {
                    return thresholdsAreAchievable != Tribool::INDETERMINATE;
                }
                bool getThresholdsAreAchievable() const {
                    STORM_LOG_THROW(isThresholdsAreAchievableSet(), storm::exceptions::InvalidStateException, "Could not retrieve whether thresholds are acheivable: value not set.");
                    return thresholdsAreAchievable == Tribool::TT;
                }

                void setNumericalResult(RationalNumberType value) {
                    numericalResult = value;
                }
                bool isNumericalResultSet() const {
                    return static_cast<bool>(numericalResult);
                }
                template<typename TargetValueType = RationalNumberType>
                TargetValueType getNumericalResult() const {
                    return storm::utility::convertNumber<TargetValueType>(numericalResult.get());
                }
                
                void setOptimumIsAchievable(bool value) {
                    optimumIsAchievable = value ? Tribool::TT : Tribool::FF;
                }
                bool isOptimumIsAchievableSet() const {
                    return optimumIsAchievable != Tribool::INDETERMINATE;
                }
                bool getOptimumIsAchievableAchievable() const {
                    STORM_LOG_THROW(isOptimumIsAchievableSet(), storm::exceptions::InvalidStateException, "Could not retrieve whether the computed optimum is acheivable: value not set.");
                    return optimumIsAchievable == Tribool::TT;
                }
                
                void setPrecisionOfResult(RationalNumberType value) {
                    precisionOfResult = value;
                }
                bool isPrecisionOfResultSet() const {
                    return static_cast<bool>(precisionOfResult);
                }
                template<typename TargetValueType = RationalNumberType>
                TargetValueType getPrecisionOfResult() const {
                    return storm::utility::convertNumber<TargetValueType>(precisionOfResult.get());
                }
                
                void setTargetPrecisionReached(bool value) {
                    targetPrecisionReached = value;
                }
                bool getTargetPrecisionReached() const {
                    return targetPrecisionReached;
                }
                
                void setMaxStepsPerformed(bool value) {
                    maxStepsPerformed = value;
                }
                bool getMaxStepsPerformed() const {
                    return maxStepsPerformed;
                }
                
            private:
                
                enum class Tribool { FF, TT, INDETERMINATE };
                
                //Stores the results for the individual iterations
                std::vector<SparseMultiObjectiveHelperRefinementStep<RationalNumberType>> steps;
                //Stores an overapproximation of the set of achievable values
                std::shared_ptr<storm::storage::geometry::Polytope<RationalNumberType>> overApprox;
                //Stores an underapproximation of the set of achievable values
                std::shared_ptr<storm::storage::geometry::Polytope<RationalNumberType>> underApprox;
                
                // Stores the result of an achievability query (if applicable).
                // For a numerical query, stores whether there is one feasible solution.
                Tribool thresholdsAreAchievable = Tribool::INDETERMINATE;
                
                //Stores the result of a numerical query (if applicable).
                boost::optional<RationalNumberType> numericalResult;
                //For numerical queries, this is true iff there is an actual scheduler that induces the computed supremum (i.e., supremum == maximum)
                Tribool optimumIsAchievable = Tribool::INDETERMINATE;
                
                //Stores the achieved precision for numerical and pareto queries
                boost::optional<RationalNumberType> precisionOfResult;
                
                //Stores whether the precision of the result is sufficient (only applicable to numerical and pareto queries)
                bool targetPrecisionReached;

                //Stores whether the computation was aborted due to performing too many refinement steps
                bool maxStepsPerformed;
            };
        }
    }
}

#endif /* STORM_MODELCHECKER_MULTIOBJECTIVE_HELPER_SPARSEMULTIOBJECTIVEHELPERRETURNTYPE_H_ */
