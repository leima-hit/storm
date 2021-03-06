#include "storm/models/symbolic/Model.h"

#include <boost/algorithm/string/join.hpp>

#include "storm/exceptions/IllegalArgumentException.h"
#include "storm/exceptions/InvalidOperationException.h"

#include "storm/adapters/AddExpressionAdapter.h"

#include "storm/storage/dd/DdManager.h"
#include "storm/storage/dd/Add.h"
#include "storm/storage/dd/Bdd.h"

#include "storm/models/symbolic/StandardRewardModel.h"

#include "storm-config.h"
#include "storm/adapters/CarlAdapter.h"

namespace storm {
    namespace models {
        namespace symbolic {
            template<storm::dd::DdType Type, typename ValueType>
            Model<Type, ValueType>::Model(storm::models::ModelType const& modelType,
                                          std::shared_ptr<storm::dd::DdManager<Type>> manager,
                                          storm::dd::Bdd<Type> reachableStates,
                                          storm::dd::Bdd<Type> initialStates,
                                          storm::dd::Bdd<Type> deadlockStates,
                                          storm::dd::Add<Type, ValueType> transitionMatrix,
                                          std::set<storm::expressions::Variable> const& rowVariables,
                                          std::shared_ptr<storm::adapters::AddExpressionAdapter<Type, ValueType>> rowExpressionAdapter,
                                          std::set<storm::expressions::Variable> const& columnVariables,
                                          std::shared_ptr<storm::adapters::AddExpressionAdapter<Type, ValueType>> columnExpressionAdapter,
                                          std::vector<std::pair<storm::expressions::Variable, storm::expressions::Variable>> const& rowColumnMetaVariablePairs,
                                          std::map<std::string, storm::expressions::Expression> labelToExpressionMap,
                                          std::unordered_map<std::string, RewardModelType> const& rewardModels)
            : ModelBase(modelType), manager(manager), reachableStates(reachableStates), initialStates(initialStates), deadlockStates(deadlockStates), transitionMatrix(transitionMatrix), rowVariables(rowVariables), rowExpressionAdapter(rowExpressionAdapter), columnVariables(columnVariables), columnExpressionAdapter(columnExpressionAdapter), rowColumnMetaVariablePairs(rowColumnMetaVariablePairs), labelToExpressionMap(labelToExpressionMap), rewardModels(rewardModels) {
                // Intentionally left empty.
            }
            
            template<storm::dd::DdType Type, typename ValueType>
            uint_fast64_t Model<Type, ValueType>::getNumberOfStates() const {
                return reachableStates.getNonZeroCount();
            }
            
            template<storm::dd::DdType Type, typename ValueType>
            uint_fast64_t Model<Type, ValueType>::getNumberOfTransitions() const {
                return transitionMatrix.getNonZeroCount();
            }
            
            template<storm::dd::DdType Type, typename ValueType>
            storm::dd::DdManager<Type> const& Model<Type, ValueType>::getManager() const {
                return *manager;
            }
            
            template<storm::dd::DdType Type, typename ValueType>
            storm::dd::DdManager<Type>& Model<Type, ValueType>::getManager() {
                return *manager;
            }
            
            template<storm::dd::DdType Type, typename ValueType>
            std::shared_ptr<storm::dd::DdManager<Type>> const& Model<Type, ValueType>::getManagerAsSharedPointer() const {
                return manager;
            }
            
            template<storm::dd::DdType Type, typename ValueType>
            storm::dd::Bdd<Type> const& Model<Type, ValueType>::getReachableStates() const {
                return reachableStates;
            }
            
            template<storm::dd::DdType Type, typename ValueType>
            storm::dd::Bdd<Type> const& Model<Type, ValueType>::getInitialStates() const {
                return initialStates;
            }

            template<storm::dd::DdType Type, typename ValueType>
            storm::dd::Bdd<Type> const& Model<Type, ValueType>::getDeadlockStates() const {
                return deadlockStates;
            }
            
            template<storm::dd::DdType Type, typename ValueType>
            storm::dd::Bdd<Type> Model<Type, ValueType>::getStates(std::string const& label) const {
                STORM_LOG_THROW(labelToExpressionMap.find(label) != labelToExpressionMap.end(), storm::exceptions::IllegalArgumentException, "The label " << label << " is invalid for the labeling of the model.");
                return this->getStates(labelToExpressionMap.at(label));
            }
            
            template<storm::dd::DdType Type, typename ValueType>
            storm::dd::Bdd<Type> Model<Type, ValueType>::getStates(storm::expressions::Expression const& expression) const {
                if (expression.isTrue()) {
                    return this->getReachableStates();
                } else if (expression.isFalse()) {
                    return manager->getBddZero();
                }
                STORM_LOG_THROW(rowExpressionAdapter != nullptr, storm::exceptions::InvalidOperationException, "Cannot create BDD for expression without expression adapter.");
                return rowExpressionAdapter->translateExpression(expression).toBdd() && this->reachableStates;
            }
            
            template<storm::dd::DdType Type, typename ValueType>
            bool Model<Type, ValueType>::hasLabel(std::string const& label) const {
                auto labelIt = labelToExpressionMap.find(label);
                if (labelIt != labelToExpressionMap.end()) {
                    return true;
                } else {
                    return label == "init" || label == "deadlock";
                }
            }
            
            template<storm::dd::DdType Type, typename ValueType>
            storm::dd::Add<Type, ValueType> const& Model<Type, ValueType>::getTransitionMatrix() const {
                return transitionMatrix;
            }
            
            template<storm::dd::DdType Type, typename ValueType>
            storm::dd::Add<Type, ValueType>& Model<Type, ValueType>::getTransitionMatrix() {
                return transitionMatrix;
            }
            
            template<storm::dd::DdType Type, typename ValueType>
            storm::dd::Bdd<Type> Model<Type, ValueType>::getQualitativeTransitionMatrix() const {
                return this->getTransitionMatrix().notZero();
            }
            
            template<storm::dd::DdType Type, typename ValueType>
            std::set<storm::expressions::Variable> const& Model<Type, ValueType>::getRowVariables() const {
                return rowVariables;
            }
            
            template<storm::dd::DdType Type, typename ValueType>
            std::set<storm::expressions::Variable> const& Model<Type, ValueType>::getColumnVariables() const {
                return columnVariables;
            }
            
            template<storm::dd::DdType Type, typename ValueType>
            std::vector<std::pair<storm::expressions::Variable, storm::expressions::Variable>> const& Model<Type, ValueType>::getRowColumnMetaVariablePairs() const {
                return rowColumnMetaVariablePairs;
            }
            
            template<storm::dd::DdType Type, typename ValueType>
            void Model<Type, ValueType>::setTransitionMatrix(storm::dd::Add<Type, ValueType> const& transitionMatrix) {
                this->transitionMatrix = transitionMatrix;
            }
            
            template<storm::dd::DdType Type, typename ValueType>
            std::map<std::string, storm::expressions::Expression> const& Model<Type, ValueType>::getLabelToExpressionMap() const {
                return labelToExpressionMap;
            }
            
            template<storm::dd::DdType Type, typename ValueType>
            storm::dd::Add<Type, ValueType> Model<Type, ValueType>::getRowColumnIdentity() const {
                storm::dd::Add<Type, ValueType> result = this->getManager().template getAddOne<ValueType>();
                for (auto const& pair : this->getRowColumnMetaVariablePairs()) {
                    result *= this->getManager().template getIdentity<ValueType>(pair.first).equals(this->getManager().template getIdentity<ValueType>(pair.second)).template toAdd<ValueType>();
                    result *= this->getManager().getRange(pair.first).template toAdd<ValueType>() * this->getManager().getRange(pair.second).template toAdd<ValueType>();
                }
                return result;
            }
            
            template<storm::dd::DdType Type, typename ValueType>
            bool Model<Type, ValueType>::hasRewardModel(std::string const& rewardModelName) const {
                return this->rewardModels.find(rewardModelName) != this->rewardModels.end();
            }
            
            template<storm::dd::DdType Type, typename ValueType>
            typename Model<Type, ValueType>::RewardModelType const& Model<Type, ValueType>::getRewardModel(std::string const& rewardModelName) const {
                auto it = this->rewardModels.find(rewardModelName);
                if (it == this->rewardModels.end()) {
                    if (rewardModelName.empty()) {
                        if (this->hasUniqueRewardModel()) {
                            return this->getUniqueRewardModel();
                        } else {
                            STORM_LOG_THROW(false, storm::exceptions::IllegalArgumentException, "Unable to refer to default reward model, because there is no default model or it is not unique.");
                        }
                    } else {
                        STORM_LOG_THROW(false, storm::exceptions::IllegalArgumentException, "The requested reward model '" << rewardModelName << "' does not exist.");
                    }
                }
                return it->second;
            }
            
            template<storm::dd::DdType Type, typename ValueType>
            typename Model<Type, ValueType>::RewardModelType const& Model<Type, ValueType>::getUniqueRewardModel() const {
                STORM_LOG_THROW(this->hasUniqueRewardModel(), storm::exceptions::InvalidOperationException, "Cannot retrieve unique reward model, because there is no unique one.");
                return this->rewardModels.cbegin()->second;
            }

            template<storm::dd::DdType Type, typename ValueType>
            typename Model<Type, ValueType>::RewardModelType& Model<Type, ValueType>::getUniqueRewardModel() {
                STORM_LOG_THROW(this->hasUniqueRewardModel(), storm::exceptions::InvalidOperationException, "Cannot retrieve unique reward model, because there is no unique one.");
                return this->rewardModels.begin()->second;
            }

            template<storm::dd::DdType Type, typename ValueType>
            bool Model<Type, ValueType>::hasUniqueRewardModel() const {
                return this->rewardModels.size() == 1;
            }
            
            template<storm::dd::DdType Type, typename ValueType>
            bool Model<Type, ValueType>::hasRewardModel() const {
                return !this->rewardModels.empty();
            }

            template<storm::dd::DdType Type, typename ValueType>
            std::unordered_map<std::string, typename Model<Type, ValueType>::RewardModelType> const& Model<Type, ValueType>::getRewardModels() const {
                return this->rewardModels;
            }
            
            template<storm::dd::DdType Type, typename ValueType>
            void Model<Type, ValueType>::printModelInformationToStream(std::ostream& out) const {
                this->printModelInformationHeaderToStream(out);
                this->printModelInformationFooterToStream(out);
            }

            template<storm::dd::DdType Type, typename ValueType>
            std::vector<std::string> Model<Type, ValueType>::getLabels() const {
                std::vector<std::string> labels;
                for(auto const& entry : labelToExpressionMap) {
                    labels.push_back(entry.first);
                }
                return labels;
            }
            
            template<storm::dd::DdType Type, typename ValueType>
            void Model<Type, ValueType>::printModelInformationHeaderToStream(std::ostream& out) const {
                out << "-------------------------------------------------------------- " << std::endl;
                out << "Model type: \t" << this->getType() << " (symbolic)" << std::endl;
                out << "States: \t" << this->getNumberOfStates() << " (" << reachableStates.getNodeCount() << " nodes)" << std::endl;
                out << "Transitions: \t" << this->getNumberOfTransitions() << " (" << transitionMatrix.getNodeCount() << " nodes)" << std::endl;
            }
            
            template<storm::dd::DdType Type, typename ValueType>
            void Model<Type, ValueType>::printModelInformationFooterToStream(std::ostream& out) const {
                this->printRewardModelsInformationToStream(out);
                this->printDdVariableInformationToStream(out);
                out << std::endl;
                out << "Labels: \t" << this->labelToExpressionMap.size() << std::endl;
                for (auto const& label : labelToExpressionMap) {
                    out << "   * " << label.first << std::endl;
                }
                out << "-------------------------------------------------------------- " << std::endl;
            }
            
            template<storm::dd::DdType Type, typename ValueType>
            void Model<Type, ValueType>::printRewardModelsInformationToStream(std::ostream& out) const {
                if (this->rewardModels.size()) {
                    std::vector<std::string> rewardModelNames;
                    std::for_each(this->rewardModels.cbegin(), this->rewardModels.cend(),
                                  [&rewardModelNames] (typename std::pair<std::string, RewardModelType> const& nameRewardModelPair) {
                                      if (nameRewardModelPair.first.empty()) { rewardModelNames.push_back("(default)"); } else { rewardModelNames.push_back(nameRewardModelPair.first); }
                                  });
                    out << "Reward Models:  " << boost::join(rewardModelNames, ", ") << std::endl;
                } else {
                    out << "Reward Models:  none" << std::endl;
                }
            }
            
            template<storm::dd::DdType Type, typename ValueType>
            void Model<Type, ValueType>::printDdVariableInformationToStream(std::ostream& out) const {
                uint_fast64_t rowVariableCount = 0;
                for (auto const& metaVariable : this->rowVariables) {
                    rowVariableCount += this->getManager().getMetaVariable(metaVariable).getNumberOfDdVariables();
                }
                uint_fast64_t columnVariableCount = 0;
                for (auto const& metaVariable : this->columnVariables) {
                    columnVariableCount += this->getManager().getMetaVariable(metaVariable).getNumberOfDdVariables();
                }
                
                out << "Variables: \t" << "rows: " << this->rowVariables.size() << " meta variables (" << rowVariableCount << " DD variables)" << ", columns: " << this->columnVariables.size() << " meta variables (" << columnVariableCount << " DD variables)";
            }
            
            template<storm::dd::DdType Type, typename ValueType>
            bool Model<Type, ValueType>::isSymbolicModel() const {
                return true;
            }
            
            // Explicitly instantiate the template class.
            template class Model<storm::dd::DdType::CUDD, double>;
            template class Model<storm::dd::DdType::Sylvan, double>;
#ifdef STORM_HAVE_CARL
			template class Model<storm::dd::DdType::Sylvan, storm::RationalFunction>;
#endif
        } // namespace symbolic
    } // namespace models
} // namespace storm
