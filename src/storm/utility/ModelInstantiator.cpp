/* 
 * File:   ModelInstantiator.cpp
 * Author: Tim Quatmann
 * 
 * Created on February 23, 2016
 */

#include "storm/utility/ModelInstantiator.h"
#include "storm/models/sparse/StandardRewardModel.h"

namespace storm {
    namespace utility {
        
            template<typename ParametricSparseModelType, typename ConstantSparseModelType>
            ModelInstantiator<ParametricSparseModelType, ConstantSparseModelType>::ModelInstantiator(ParametricSparseModelType const& parametricModel){
                //Now pre-compute the information for the equation system.
                initializeModelSpecificData(parametricModel);
                initializeMatrixMapping(this->instantiatedModel->getTransitionMatrix(), this->functions, this->matrixMapping, parametricModel.getTransitionMatrix());
                
                for(auto& rewModel : this->instantiatedModel->getRewardModels()) {
                    if(rewModel.second.hasStateRewards()){
                        initializeVectorMapping(rewModel.second.getStateRewardVector(), this->functions, this->vectorMapping, parametricModel.getRewardModel(rewModel.first).getStateRewardVector());
                    }
                    if(rewModel.second.hasStateActionRewards()){
                        initializeVectorMapping(rewModel.second.getStateActionRewardVector(), this->functions, this->vectorMapping, parametricModel.getRewardModel(rewModel.first).getStateActionRewardVector());
                    }
                    if(rewModel.second.hasTransitionRewards()){
                        initializeMatrixMapping(rewModel.second.getTransitionRewardMatrix(), this->functions, this->matrixMapping, parametricModel.getRewardModel(rewModel.first).getTransitionRewardMatrix());
                    }
                }
            }
            
            template<typename ParametricSparseModelType, typename ConstantType>
            ModelInstantiator<ParametricSparseModelType, ConstantType>::~ModelInstantiator() {
                //Intentionally left empty
            }
            
            template<typename ParametricSparseModelType, typename ConstantSparseModelType>
            storm::storage::SparseMatrix<typename ConstantSparseModelType::ValueType> ModelInstantiator<ParametricSparseModelType, ConstantSparseModelType>::buildDummyMatrix(storm::storage::SparseMatrix<ParametricType> const& parametricMatrix) const{
                storm::storage::SparseMatrixBuilder<ConstantType> matrixBuilder(parametricMatrix.getRowCount(),
                                                                                parametricMatrix.getColumnCount(),
                                                                                parametricMatrix.getEntryCount(),
                                                                                true, // no force dimensions
                                                                                true, //Custom row grouping
                                                                                parametricMatrix.getRowGroupCount());
                for(std::size_t rowGroup = 0; rowGroup < parametricMatrix.getRowGroupCount(); ++rowGroup){
                    matrixBuilder.newRowGroup(parametricMatrix.getRowGroupIndices()[rowGroup]);
                    for(std::size_t row = parametricMatrix.getRowGroupIndices()[rowGroup]; row < parametricMatrix.getRowGroupIndices()[rowGroup+1]; ++row){
                        ConstantType dummyValue = storm::utility::one<ConstantType>();
                        for(auto const& paramEntry : parametricMatrix.getRow(row)){
                            matrixBuilder.addNextValue(row, paramEntry.getColumn(), dummyValue);
                            dummyValue = storm::utility::zero<ConstantType>();
                        }
                    }
                }
                return matrixBuilder.build();
            }
            
            template<typename ParametricSparseModelType, typename ConstantSparseModelType>
            std::unordered_map<std::string, typename ConstantSparseModelType::RewardModelType> ModelInstantiator<ParametricSparseModelType, ConstantSparseModelType>::buildDummyRewardModels(std::unordered_map<std::string, typename ParametricSparseModelType::RewardModelType> const& parametricRewardModel) const {
                std::unordered_map<std::string, typename ConstantSparseModelType::RewardModelType> result;
                for(auto const& paramRewardModel : parametricRewardModel){
                    auto const& rewModel = paramRewardModel.second;
                    boost::optional<std::vector<ConstantType>> optionalStateRewardVector;
                    if(rewModel.hasStateRewards()) {
                        optionalStateRewardVector = std::vector<ConstantType>(rewModel.getStateRewardVector().size());
                    }
                    boost::optional<std::vector<ConstantType>> optionalStateActionRewardVector;
                    if(rewModel.hasStateActionRewards()) {
                        optionalStateActionRewardVector = std::vector<ConstantType>(rewModel.getStateActionRewardVector().size());
                    }
                    boost::optional<storm::storage::SparseMatrix<ConstantType>> optionalTransitionRewardMatrix;
                    if(rewModel.hasTransitionRewards()) {
                        optionalTransitionRewardMatrix = buildDummyMatrix(rewModel.getTransitionRewardMatrix());
                    }
                    result.insert(std::make_pair(paramRewardModel.first, 
                                                 storm::models::sparse::StandardRewardModel<ConstantType>(std::move(optionalStateRewardVector), std::move(optionalStateActionRewardVector), std::move(optionalTransitionRewardMatrix))
                                                ));
                }
                return result;
            }
            
            template<typename ParametricSparseModelType, typename ConstantSparseModelType>
            void ModelInstantiator<ParametricSparseModelType, ConstantSparseModelType>::initializeMatrixMapping(storm::storage::SparseMatrix<ConstantType>& constantMatrix,
                                             std::unordered_map<ParametricType, ConstantType>& functions,
                                             std::vector<std::pair<typename storm::storage::SparseMatrix<ConstantType>::iterator, ConstantType*>>& mapping,
                                             storm::storage::SparseMatrix<ParametricType> const& parametricMatrix) const{
                ConstantType dummyValue = storm::utility::one<ConstantType>();
                auto constantEntryIt = constantMatrix.begin();
                auto parametricEntryIt = parametricMatrix.begin();
                while(parametricEntryIt != parametricMatrix.end()){
                    STORM_LOG_ASSERT(parametricEntryIt->getColumn() == constantEntryIt->getColumn(), "Entries of parametric and constant matrix are not at the same position");
                    if(storm::utility::isConstant(parametricEntryIt->getValue())){
                        //Constant entries can be inserted directly
                        constantEntryIt->setValue(storm::utility::convertNumber<ConstantType>(storm::utility::parametric::getConstantPart(parametricEntryIt->getValue())));
                    } else {
                        //insert the new function and store that the current constantMatrix entry needs to be set to the value of this function
                        auto functionsIt = functions.insert(std::make_pair(parametricEntryIt->getValue(), dummyValue)).first;
                        mapping.emplace_back(std::make_pair(constantEntryIt, &(functionsIt->second)));
                        //Note that references to elements of an unordered map remain valid after calling unordered_map::insert.
                    }
                    ++constantEntryIt;
                    ++parametricEntryIt;
                }
                STORM_LOG_ASSERT(constantEntryIt == constantMatrix.end(), "Parametric matrix seems to have more or less entries then the constant matrix");
                //TODO: is this necessary?
                constantMatrix.updateNonzeroEntryCount();
            }
            
            template<typename ParametricSparseModelType, typename ConstantSparseModelType>
            void ModelInstantiator<ParametricSparseModelType, ConstantSparseModelType>::initializeVectorMapping(std::vector<ConstantType>& constantVector,
                                             std::unordered_map<ParametricType, ConstantType>& functions,
                                             std::vector<std::pair<typename std::vector<ConstantType>::iterator, ConstantType*>>& mapping,
                                             std::vector<ParametricType> const& parametricVector) const{
                ConstantType dummyValue = storm::utility::one<ConstantType>();
                auto constantEntryIt = constantVector.begin();
                auto parametricEntryIt = parametricVector.begin();
                while(parametricEntryIt != parametricVector.end()){
                    if(storm::utility::isConstant(storm::utility::simplify(*parametricEntryIt))){
                        //Constant entries can be inserted directly
                        *constantEntryIt = storm::utility::convertNumber<ConstantType>(storm::utility::parametric::getConstantPart(*parametricEntryIt));
                    } else {
                        //insert the new function and store that the current constantVector entry needs to be set to the value of this function
                        auto functionsIt = functions.insert(std::make_pair(*parametricEntryIt, dummyValue)).first;
                        mapping.emplace_back(std::make_pair(constantEntryIt, &(functionsIt->second)));
                        //Note that references to elements of an unordered map remain valid after calling unordered_map::insert.
                    }
                    ++constantEntryIt;
                    ++parametricEntryIt;
                }
                STORM_LOG_ASSERT(constantEntryIt == constantVector.end(), "Parametric vector seems to have more or less entries then the constant vector");
            }
            
            template<typename ParametricSparseModelType, typename ConstantSparseModelType>
            ConstantSparseModelType const& ModelInstantiator<ParametricSparseModelType, ConstantSparseModelType>::instantiate(std::map<VariableType, CoefficientType>const& valuation){
                //Write results into the placeholders
                for(auto& functionResult : this->functions){
                    functionResult.second=storm::utility::convertNumber<ConstantType>(
                            storm::utility::parametric::evaluate(functionResult.first, valuation));
                }
                
                //Write the instantiated values to the matrices and vectors according to the stored mappings
                for(auto& entryValuePair : this->matrixMapping){
                    entryValuePair.first->setValue(*(entryValuePair.second));
                }
                for(auto& entryValuePair : this->vectorMapping){
                    *(entryValuePair.first)=*(entryValuePair.second);
                }
                
                return *this->instantiatedModel;
            }
        
        template<typename ParametricSparseModelType, typename ConstantSparseModelType>
        void ModelInstantiator<ParametricSparseModelType, ConstantSparseModelType>::checkValid() const {
            // TODO write some checks
        }
        
#ifdef STORM_HAVE_CARL
            template class ModelInstantiator<storm::models::sparse::Dtmc<storm::RationalFunction>, storm::models::sparse::Dtmc<double>>;
            template class ModelInstantiator<storm::models::sparse::Mdp<storm::RationalFunction>, storm::models::sparse::Mdp<double>>;
            template class ModelInstantiator<storm::models::sparse::Ctmc<storm::RationalFunction>, storm::models::sparse::Ctmc<double>>;
            template class ModelInstantiator<storm::models::sparse::MarkovAutomaton<storm::RationalFunction>, storm::models::sparse::MarkovAutomaton<double>>;
            template class ModelInstantiator<storm::models::sparse::StochasticTwoPlayerGame<storm::RationalFunction>, storm::models::sparse::StochasticTwoPlayerGame<double>>;
#endif
    } //namespace utility
} //namespace storm
