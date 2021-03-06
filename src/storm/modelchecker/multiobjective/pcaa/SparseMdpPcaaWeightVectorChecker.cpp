#include "storm/modelchecker/multiobjective/pcaa/SparseMdpPcaaWeightVectorChecker.h"

#include "storm/adapters/CarlAdapter.h"
#include "storm/models/sparse/Mdp.h"
#include "storm/models/sparse/StandardRewardModel.h"
#include "storm/utility/macros.h"
#include "storm/utility/vector.h"


namespace storm {
    namespace modelchecker {
        namespace multiobjective {
            
            template <class SparseMdpModelType>
            SparseMdpPcaaWeightVectorChecker<SparseMdpModelType>::SparseMdpPcaaWeightVectorChecker(SparseMdpModelType const& model,
                                                                                                   std::vector<PcaaObjective<ValueType>> const& objectives,
                                                                                                   storm::storage::BitVector const& actionsWithNegativeReward,
                                                                                                   storm::storage::BitVector const& ecActions,
                                                                                                   storm::storage::BitVector const& possiblyRecurrentStates) :
                SparsePcaaWeightVectorChecker<SparseMdpModelType>(model, objectives, actionsWithNegativeReward, ecActions, possiblyRecurrentStates) {
                // set the state action rewards
                for(uint_fast64_t objIndex = 0; objIndex < this->objectives.size(); ++objIndex) {
                    typename SparseMdpModelType::RewardModelType const& rewModel = this->model.getRewardModel(this->objectives[objIndex].rewardModelName);
                    STORM_LOG_ASSERT(!rewModel.hasTransitionRewards(), "Reward model has transition rewards which is not expected.");
                    this->discreteActionRewards[objIndex] = rewModel.getTotalRewardVector(this->model.getTransitionMatrix());
                }
            }
            
            template <class SparseMdpModelType>
            void SparseMdpPcaaWeightVectorChecker<SparseMdpModelType>::boundedPhase(std::vector<ValueType> const& weightVector, std::vector<ValueType>& weightedRewardVector) {
                // Allocate some memory so this does not need to happen for each time epoch
                std::vector<uint_fast64_t> optimalChoicesInCurrentEpoch(this->model.getNumberOfStates());
                std::vector<ValueType> choiceValues(weightedRewardVector.size());
                std::vector<ValueType> temporaryResult(this->model.getNumberOfStates());
                std::vector<ValueType> zeroReward(weightedRewardVector.size(), storm::utility::zero<ValueType>());
                // Get for each occurring timeBound the indices of the objectives with that bound.
                std::map<uint_fast64_t, storm::storage::BitVector, std::greater<uint_fast64_t>> lowerTimeBounds;
                std::map<uint_fast64_t, storm::storage::BitVector, std::greater<uint_fast64_t>> upperTimeBounds;
                for(uint_fast64_t objIndex = 0; objIndex < this->objectives.size(); ++objIndex) {
                    auto const& obj = this->objectives[objIndex];
                    if(obj.lowerTimeBound) {
                        auto timeBoundIt = lowerTimeBounds.insert(std::make_pair(storm::utility::convertNumber<uint_fast64_t>(*obj.lowerTimeBound), storm::storage::BitVector(this->objectives.size(), false))).first;
                        STORM_LOG_WARN_COND(storm::utility::convertNumber<ValueType>(timeBoundIt->first) == (*obj.lowerTimeBound), "Rounded non-integral bound " << *obj.lowerTimeBound << " to " << timeBoundIt->first  << ".");
                        timeBoundIt->second.set(objIndex);
                    }
                    if(obj.upperTimeBound) {
                        auto timeBoundIt = upperTimeBounds.insert(std::make_pair(storm::utility::convertNumber<uint_fast64_t>(*obj.upperTimeBound), storm::storage::BitVector(this->objectives.size(), false))).first;
                        STORM_LOG_WARN_COND(storm::utility::convertNumber<ValueType>(timeBoundIt->first) == (*obj.upperTimeBound), "Rounded non-integral bound " << *obj.upperTimeBound << " to " << timeBoundIt->first  << ".");
                        timeBoundIt->second.set(objIndex);
                        
                        // There is no error for the values of these objectives.
                        this->offsetsToLowerBound[objIndex] = storm::utility::zero<ValueType>();
                        this->offsetsToUpperBound[objIndex] = storm::utility::zero<ValueType>();
                    }
                }
                
                // Stores the objectives for which we need to compute values in the current time epoch.
                storm::storage::BitVector consideredObjectives = this->objectivesWithNoUpperTimeBound;
                
                // Stores objectives for which the current epoch passed their lower bound
                storm::storage::BitVector lowerBoundViolatedObjectives(consideredObjectives.size(), false);
                
                auto lowerTimeBoundIt = lowerTimeBounds.begin();
                auto upperTimeBoundIt = upperTimeBounds.begin();
                uint_fast64_t currentEpoch = std::max(lowerTimeBounds.empty() ? 0 : lowerTimeBoundIt->first - 1, upperTimeBounds.empty() ? 0 : upperTimeBoundIt->first); // consider lowerBound - 1 since we are interested in the first epoch that passes the bound
                
                while(currentEpoch > 0) {
                    //For lower time bounds we need to react when the currentEpoch passed the bound
                    // Hence, we substract 1 from the lower time bounds.
                    if(lowerTimeBoundIt != lowerTimeBounds.end() && currentEpoch == lowerTimeBoundIt->first - 1) {
                        lowerBoundViolatedObjectives |= lowerTimeBoundIt->second;
                        for(auto objIndex : lowerTimeBoundIt->second) {
                            // No more reward is earned for this objective.
                            storm::utility::vector::addScaledVector(weightedRewardVector, this->discreteActionRewards[objIndex], -weightVector[objIndex]);
                        }
                        ++lowerTimeBoundIt;
                    }
                    
                    if(upperTimeBoundIt != upperTimeBounds.end() && currentEpoch == upperTimeBoundIt->first) {
                        consideredObjectives |= upperTimeBoundIt->second;
                        for(auto objIndex : upperTimeBoundIt->second) {
                            // This objective now plays a role in the weighted sum
                            storm::utility::vector::addScaledVector(weightedRewardVector, this->discreteActionRewards[objIndex], weightVector[objIndex]);
                        }
                        ++upperTimeBoundIt;
                    }
                    
                    // Get values and scheduler for weighted sum of objectives
                    this->model.getTransitionMatrix().multiplyWithVector(this->weightedResult, choiceValues);
                    storm::utility::vector::addVectors(choiceValues, weightedRewardVector, choiceValues);
                    storm::utility::vector::reduceVectorMax(choiceValues, this->weightedResult, this->model.getTransitionMatrix().getRowGroupIndices(), &optimalChoicesInCurrentEpoch);
                    
                    // get values for individual objectives
                    // TODO we could compute the result for one of the objectives from the weighted result, the given weight vector, and the remaining objective results.
                    for(auto objIndex : consideredObjectives) {
                        std::vector<ValueType>& objectiveResult = this->objectiveResults[objIndex];
                        std::vector<ValueType> const& objectiveRewards = lowerBoundViolatedObjectives.get(objIndex) ? zeroReward : this->discreteActionRewards[objIndex];
                        auto rowGroupIndexIt = this->model.getTransitionMatrix().getRowGroupIndices().begin();
                        auto optimalChoiceIt = optimalChoicesInCurrentEpoch.begin();
                        for(ValueType& stateValue : temporaryResult){
                            uint_fast64_t row = (*rowGroupIndexIt) + (*optimalChoiceIt);
                            ++rowGroupIndexIt;
                            ++optimalChoiceIt;
                            stateValue = objectiveRewards[row];
                            for(auto const& entry : this->model.getTransitionMatrix().getRow(row)) {
                                stateValue += entry.getValue() * objectiveResult[entry.getColumn()];
                            }
                        }
                        objectiveResult.swap(temporaryResult);
                    }
                    --currentEpoch;
                }
            }
            
            template class SparseMdpPcaaWeightVectorChecker<storm::models::sparse::Mdp<double>>;
#ifdef STORM_HAVE_CARL
            template class SparseMdpPcaaWeightVectorChecker<storm::models::sparse::Mdp<storm::RationalNumber>>;
#endif
            
        }
    }
}
