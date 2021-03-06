#ifndef STORM_H
#define	STORM_H

#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstdio>
#include <sstream>
#include <memory>
#include "storm/storage/ModelFormulasPair.h"

#include "initialize.h"

#include "storm-config.h"

// Headers that provide auxiliary functionality.
#include "storm/settings/SettingsManager.h"

#include "storm/settings/modules/CoreSettings.h"
#include "storm/settings/modules/IOSettings.h"
#include "storm/settings/modules/BisimulationSettings.h"
#include "storm/settings/modules/ParametricSettings.h"
#include "storm/settings/modules/RegionSettings.h"
#include "storm/settings/modules/EliminationSettings.h"
#include "storm/settings/modules/JitBuilderSettings.h"
#include "storm/settings/modules/JaniExportSettings.h"

// Formula headers.
#include "storm/logic/Formulas.h"
#include "storm/logic/FragmentSpecification.h"

// Model headers.
#include "storm/models/ModelBase.h"
#include "storm/models/sparse/Model.h"
#include "storm/models/sparse/StandardRewardModel.h"
#include "storm/models/sparse/MarkovAutomaton.h"
#include "storm/models/symbolic/Model.h"
#include "storm/models/symbolic/StandardRewardModel.h"

#include "storm/storage/dd/Add.h"
#include "storm/storage/dd/Bdd.h"

#include "storm/parser/AutoParser.h"

#include "storm/storage/jani/Model.h"
#include "storm/storage/jani/Property.h"

// Headers of builders.
#include "storm/builder/ExplicitModelBuilder.h"
#include "storm/builder/jit/ExplicitJitJaniModelBuilder.h"
#include "storm/builder/DdPrismModelBuilder.h"
#include "storm/builder/DdJaniModelBuilder.h"

// Headers for model processing.
#include "storm/storage/bisimulation/DeterministicModelBisimulationDecomposition.h"
#include "storm/storage/bisimulation/NondeterministicModelBisimulationDecomposition.h"
#include "storm/storage/ModelFormulasPair.h"
#include "storm/storage/SymbolicModelDescription.h"
#include "storm/storage/jani/JSONExporter.h"

// Headers for model checking.
#include "storm/modelchecker/prctl/SparseDtmcPrctlModelChecker.h"
#include "storm/modelchecker/prctl/SparseMdpPrctlModelChecker.h"
#include "storm/modelchecker/prctl/HybridDtmcPrctlModelChecker.h"
#include "storm/modelchecker/prctl/HybridMdpPrctlModelChecker.h"
#include "storm/modelchecker/prctl/SymbolicDtmcPrctlModelChecker.h"
#include "storm/modelchecker/prctl/SymbolicMdpPrctlModelChecker.h"
#include "storm/modelchecker/reachability/SparseDtmcEliminationModelChecker.h"
#include "storm/modelchecker/abstraction/GameBasedMdpModelChecker.h"
#include "storm/modelchecker/region/SparseDtmcRegionModelChecker.h"
#include "storm/modelchecker/region/SparseMdpRegionModelChecker.h"
#include "storm/modelchecker/region/ParameterRegion.h"
#include "storm/modelchecker/exploration/SparseExplorationModelChecker.h"

#include "storm/modelchecker/csl/SparseCtmcCslModelChecker.h"
#include "storm/modelchecker/csl/helper/SparseCtmcCslHelper.h"
#include "storm/modelchecker/csl/SparseMarkovAutomatonCslModelChecker.h"
#include "storm/modelchecker/csl/HybridCtmcCslModelChecker.h"
#include "storm/modelchecker/csl/SparseMarkovAutomatonCslModelChecker.h"
#include "storm/modelchecker/results/ExplicitQualitativeCheckResult.h"
#include "storm/modelchecker/results/SymbolicQualitativeCheckResult.h"

// Headers for counterexample generation.
#include "storm/counterexamples/MILPMinimalLabelSetGenerator.h"
#include "storm/counterexamples/SMTMinimalCommandSetGenerator.h"

// Headers related to model building.
#include "storm/generator/PrismNextStateGenerator.h"
#include "storm/generator/JaniNextStateGenerator.h"

// Headers related to exception handling.
#include "storm/exceptions/InvalidStateException.h"
#include "storm/exceptions/InvalidArgumentException.h"
#include "storm/exceptions/InvalidSettingsException.h"
#include "storm/exceptions/InvalidTypeException.h"
#include "storm/exceptions/NotImplementedException.h"
#include "storm/exceptions/NotSupportedException.h"

#include "storm/utility/Stopwatch.h"
#include "storm/utility/file.h"

namespace storm {

    namespace parser {
        class FormulaParser;
    }

    template<typename ValueType>
    std::shared_ptr<storm::models::sparse::Model<ValueType>> buildExplicitModel(std::string const& transitionsFile, std::string const& labelingFile, boost::optional<std::string> const& stateRewardsFile = boost::none, boost::optional<std::string> const& transitionRewardsFile = boost::none, boost::optional<std::string> const& choiceLabelingFile = boost::none) {
        return storm::parser::AutoParser<>::parseModel(transitionsFile, labelingFile, stateRewardsFile ? stateRewardsFile.get() : "", transitionRewardsFile ? transitionRewardsFile.get() : "", choiceLabelingFile ? choiceLabelingFile.get() : "" );
    }

    std::vector<std::shared_ptr<storm::logic::Formula const>> extractFormulasFromProperties(std::vector<storm::jani::Property> const& properties);
    std::pair<storm::jani::Model, std::map<std::string, storm::jani::Property>> parseJaniModel(std::string const& path);
    storm::prism::Program parseProgram(std::string const& path);
    std::vector<storm::jani::Property> substituteConstantsInProperties(std::vector<storm::jani::Property> const& properties, std::map<storm::expressions::Variable, storm::expressions::Expression> const& substitution);
    std::vector<storm::jani::Property> parseProperties(storm::parser::FormulaParser& formulaParser, std::string const& inputString, boost::optional<std::set<std::string>> const& propertyFilter = boost::none);
    std::vector<storm::jani::Property> parsePropertiesForExplicit(std::string const& inputString, boost::optional<std::set<std::string>> const& propertyFilter = boost::none);
    std::vector<storm::jani::Property> parsePropertiesForPrismProgram(std::string const& inputString, storm::prism::Program const& program, boost::optional<std::set<std::string>> const& propertyFilter = boost::none);
    std::vector<storm::jani::Property> parsePropertiesForJaniModel(std::string const& inputString, storm::jani::Model const& model, boost::optional<std::set<std::string>> const& propertyFilter = boost::none);
    boost::optional<std::set<std::string>> parsePropertyFilter(boost::optional<std::string> const& propertyFilter);
    std::vector<storm::jani::Property> filterProperties(std::vector<storm::jani::Property> const& properties, boost::optional<std::set<std::string>> const& propertyFilter);

    template<typename ValueType>
    std::shared_ptr<storm::models::sparse::Model<ValueType>> buildSparseModel(storm::storage::SymbolicModelDescription const& model, std::vector<std::shared_ptr<storm::logic::Formula const>> const& formulas) {
        storm::builder::BuilderOptions options(formulas);
        
        if (storm::settings::getModule<storm::settings::modules::IOSettings>().isBuildFullModelSet()) {
            options.setBuildAllLabels();
            options.setBuildAllRewardModels();
            options.clearTerminalStates();
        }

        // Generate command labels if we are going to build a counterexample later.
        if (storm::settings::getModule<storm::settings::modules::CounterexampleGeneratorSettings>().isMinimalCommandSetGenerationSet()) {
            options.setBuildChoiceLabels(true);
        }

        if (storm::settings::getModule<storm::settings::modules::IOSettings>().isJitSet()) {
            STORM_LOG_THROW(model.isJaniModel(), storm::exceptions::NotSupportedException, "Cannot use JIT-based model builder for non-JANI model.");

            storm::builder::jit::ExplicitJitJaniModelBuilder<ValueType> builder(model.asJaniModel(), options);

            if (storm::settings::getModule<storm::settings::modules::JitBuilderSettings>().isDoctorSet()) {
                bool result = builder.doctor();
                STORM_LOG_THROW(result, storm::exceptions::InvalidSettingsException, "The JIT-based model builder cannot be used on your system.");
                STORM_LOG_INFO("The JIT-based model builder seems to be working.");
            }
            
            return builder.build();
        } else {
            std::shared_ptr<storm::generator::NextStateGenerator<ValueType, uint32_t>> generator;
            if (model.isPrismProgram()) {
                generator = std::make_shared<storm::generator::PrismNextStateGenerator<ValueType, uint32_t>>(model.asPrismProgram(), options);
            } else if (model.isJaniModel()) {
                generator = std::make_shared<storm::generator::JaniNextStateGenerator<ValueType, uint32_t>>(model.asJaniModel(), options);
            } else {
                STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Cannot build sparse model from this symbolic model description.");
            }
            storm::builder::ExplicitModelBuilder<ValueType> builder(generator);
            return builder.build();
        }
    }

    template<typename ValueType, storm::dd::DdType LibraryType = storm::dd::DdType::CUDD>
    std::shared_ptr<storm::models::symbolic::Model<LibraryType, ValueType>> buildSymbolicModel(storm::storage::SymbolicModelDescription const& model, std::vector<std::shared_ptr<storm::logic::Formula const>> const& formulas) {
        if (model.isPrismProgram()) {
            typename storm::builder::DdPrismModelBuilder<LibraryType, ValueType>::Options options;
            options = typename storm::builder::DdPrismModelBuilder<LibraryType, ValueType>::Options(formulas);
            
            storm::builder::DdPrismModelBuilder<LibraryType, ValueType> builder;
            return builder.build(model.asPrismProgram(), options);
        } else {
            STORM_LOG_THROW(model.isJaniModel(), storm::exceptions::InvalidArgumentException, "Cannot build symbolic model for the given symbolic model description.");
            typename storm::builder::DdJaniModelBuilder<LibraryType, ValueType>::Options options;
            options = typename storm::builder::DdJaniModelBuilder<LibraryType, ValueType>::Options(formulas);
            
            storm::builder::DdJaniModelBuilder<LibraryType, ValueType> builder;
            return builder.build(model.asJaniModel(), options);
        }
    }
    
    template<typename ModelType>
    std::shared_ptr<ModelType> performDeterministicSparseBisimulationMinimization(std::shared_ptr<ModelType> model, std::vector<std::shared_ptr<storm::logic::Formula const>> const& formulas, storm::storage::BisimulationType type) {
        STORM_LOG_INFO("Performing bisimulation minimization... ");
        typename storm::storage::DeterministicModelBisimulationDecomposition<ModelType>::Options options;
        if (!formulas.empty()) {
            options = typename storm::storage::DeterministicModelBisimulationDecomposition<ModelType>::Options(*model, formulas);
        }
        options.setType(type);
        
        storm::storage::DeterministicModelBisimulationDecomposition<ModelType> bisimulationDecomposition(*model, options);
        bisimulationDecomposition.computeBisimulationDecomposition();
        model = bisimulationDecomposition.getQuotient();
        STORM_LOG_INFO("Bisimulation done. ");
        return model;
    }
    
    template<typename ModelType>
    std::shared_ptr<ModelType> performNondeterministicSparseBisimulationMinimization(std::shared_ptr<ModelType> model, std::vector<std::shared_ptr<storm::logic::Formula const>> const& formulas, storm::storage::BisimulationType type) {
        STORM_LOG_INFO("Performing bisimulation minimization... ");
        typename storm::storage::DeterministicModelBisimulationDecomposition<ModelType>::Options options;
        if (!formulas.empty()) {
            options = typename storm::storage::NondeterministicModelBisimulationDecomposition<ModelType>::Options(*model, formulas);
        }
        options.setType(type);

        storm::storage::NondeterministicModelBisimulationDecomposition<ModelType> bisimulationDecomposition(*model, options);
        bisimulationDecomposition.computeBisimulationDecomposition();
        model = bisimulationDecomposition.getQuotient();
        STORM_LOG_INFO("Bisimulation done.");
        return model;
    }
    
    template<typename ModelType>
    std::shared_ptr<storm::models::sparse::Model<typename ModelType::ValueType>> performBisimulationMinimization(std::shared_ptr<storm::models::sparse::Model<typename ModelType::ValueType>> const& model, std::vector<std::shared_ptr<storm::logic::Formula const>> const& formulas, storm::storage::BisimulationType type) {
        using ValueType = typename ModelType::ValueType;
        
        STORM_LOG_THROW(model->isOfType(storm::models::ModelType::Dtmc) || model->isOfType(storm::models::ModelType::Ctmc) || model->isOfType(storm::models::ModelType::Mdp), storm::exceptions::InvalidSettingsException, "Bisimulation minimization is currently only available for DTMCs, CTMCs and MDPs.");
        model->reduceToStateBasedRewards();

        if (model->isOfType(storm::models::ModelType::Dtmc)) {
            return performDeterministicSparseBisimulationMinimization<storm::models::sparse::Dtmc<ValueType>>(model->template as<storm::models::sparse::Dtmc<ValueType>>(), formulas, type);
        } else if (model->isOfType(storm::models::ModelType::Ctmc)) {
            return performDeterministicSparseBisimulationMinimization<storm::models::sparse::Ctmc<ValueType>>(model->template as<storm::models::sparse::Ctmc<ValueType>>(), formulas, type);
        } else {
            return performNondeterministicSparseBisimulationMinimization<storm::models::sparse::Mdp<ValueType>>(model->template as<storm::models::sparse::Mdp<ValueType>>(), formulas, type);
        }       
    }
        
    template<typename ModelType>
    std::shared_ptr<storm::models::sparse::Model<typename ModelType::ValueType>> performBisimulationMinimization(std::shared_ptr<storm::models::sparse::Model<typename ModelType::ValueType>> const& model, std::shared_ptr<storm::logic::Formula const> const& formula, storm::storage::BisimulationType type) {
        std::vector<std::shared_ptr<storm::logic::Formula const>> formulas = { formula };
        return performBisimulationMinimization<ModelType>(model, formulas , type);
    }
    
    
    template<typename ModelType>
    std::shared_ptr<storm::models::ModelBase> preprocessModel(std::shared_ptr<storm::models::ModelBase> model, std::vector<std::shared_ptr<storm::logic::Formula const>> const& formulas) {
        storm::utility::Stopwatch preprocessingWatch(true);
        
        bool operationPerformed = false;
        if (model->getType() == storm::models::ModelType::MarkovAutomaton && model->isSparseModel()) {
            operationPerformed = true;
            std::shared_ptr<storm::models::sparse::MarkovAutomaton<typename ModelType::ValueType>> ma = model->template as<storm::models::sparse::MarkovAutomaton<typename ModelType::ValueType>>();
            ma->close();
            if (ma->hasOnlyTrivialNondeterminism()) {
                // Markov automaton can be converted into CTMC.
                model = ma->convertToCTMC();
            }
        }

        if (model->isSparseModel() && storm::settings::getModule<storm::settings::modules::GeneralSettings>().isBisimulationSet()) {
            operationPerformed = true;
            storm::storage::BisimulationType bisimType = storm::storage::BisimulationType::Strong;
            if (storm::settings::getModule<storm::settings::modules::BisimulationSettings>().isWeakBisimulationSet()) {
                bisimType = storm::storage::BisimulationType::Weak;
            }
            
            STORM_LOG_THROW(model->isSparseModel(), storm::exceptions::InvalidSettingsException, "Bisimulation minimization is currently only available for sparse models.");
            return performBisimulationMinimization<ModelType>(model->template as<storm::models::sparse::Model<typename ModelType::ValueType>>(), formulas, bisimType);
        }

        preprocessingWatch.stop();
        if (operationPerformed) {
            STORM_PRINT_AND_LOG(std::endl << "Time for model preprocessing: " << preprocessingWatch << "." << std::endl << std::endl);
        }
        
        return model;
    }

    template<typename ValueType>
    void generateCounterexample(storm::storage::SymbolicModelDescription const& model, std::shared_ptr<storm::models::sparse::Model<ValueType>> markovModel, std::shared_ptr<storm::logic::Formula const> const& formula) {
        if (storm::settings::getModule<storm::settings::modules::CounterexampleGeneratorSettings>().isMinimalCommandSetGenerationSet()) {
            STORM_LOG_THROW(model.isPrismProgram(), storm::exceptions::InvalidTypeException, "Minimal command set generation is only available for PRISM models.");
            STORM_LOG_THROW(markovModel->getType() == storm::models::ModelType::Mdp, storm::exceptions::InvalidTypeException, "Minimal command set generation is only available for MDPs.");
            storm::prism::Program const& program = model.asPrismProgram();
            
            std::shared_ptr<storm::models::sparse::Mdp<ValueType>> mdp = markovModel->template as<storm::models::sparse::Mdp<ValueType>>();

            // Determine whether we are required to use the MILP-version or the SAT-version.
            bool useMILP = storm::settings::getModule<storm::settings::modules::CounterexampleGeneratorSettings>().isUseMilpBasedMinimalCommandSetGenerationSet();

            if (useMILP) {
                storm::counterexamples::MILPMinimalLabelSetGenerator<ValueType>::computeCounterexample(program, *mdp, formula);
            } else {
                storm::counterexamples::SMTMinimalCommandSetGenerator<ValueType>::computeCounterexample(program, storm::settings::getModule<storm::settings::modules::IOSettings>().getConstantDefinitionString(), *mdp, formula);
            }

        } else {
            STORM_LOG_THROW(false, storm::exceptions::InvalidSettingsException, "No suitable counterexample representation selected.");
        }
    }

#ifdef STORM_HAVE_CARL
    template<>
    inline void generateCounterexample(storm::storage::SymbolicModelDescription const&, std::shared_ptr<storm::models::sparse::Model<storm::RationalNumber>> , std::shared_ptr<storm::logic::Formula const> const& ) {
        STORM_LOG_THROW(false, storm::exceptions::InvalidSettingsException, "Unable to generate counterexample for exact arithmetic model.");
    }

    template<>
    inline void generateCounterexample(storm::storage::SymbolicModelDescription const&, std::shared_ptr<storm::models::sparse::Model<storm::RationalFunction>> , std::shared_ptr<storm::logic::Formula const> const& ) {
        STORM_LOG_THROW(false, storm::exceptions::InvalidSettingsException, "Unable to generate counterexample for parametric model.");
    }
#endif

    template<typename ValueType>
    void generateCounterexamples(storm::storage::SymbolicModelDescription const& model, std::shared_ptr<storm::models::sparse::Model<ValueType>> markovModel, std::vector<std::shared_ptr<storm::logic::Formula const>> const& formulas) {
        for (auto const& formula : formulas) {
            generateCounterexample(model, markovModel, formula);
        }
    }

    template<typename ValueType, storm::dd::DdType DdType>
    std::unique_ptr<storm::modelchecker::CheckResult> verifyModel(std::shared_ptr<storm::models::ModelBase> model, std::shared_ptr<storm::logic::Formula const> const& formula, bool onlyInitialStatesRelevant) {
        switch(storm::settings::getModule<storm::settings::modules::CoreSettings>().getEngine()) {
            case storm::settings::modules::CoreSettings::Engine::Sparse: {
                std::shared_ptr<storm::models::sparse::Model<ValueType>> sparseModel = model->template as<storm::models::sparse::Model<ValueType>>();
                STORM_LOG_THROW(sparseModel != nullptr, storm::exceptions::InvalidArgumentException, "Sparse engine requires a sparse input model.");
                return (sparseModel, formula, onlyInitialStatesRelevant);
            }
            case storm::settings::modules::CoreSettings::Engine::Hybrid: {
                std::shared_ptr<storm::models::symbolic::Model<DdType>> ddModel = model->template as<storm::models::symbolic::Model<DdType>>();
                STORM_LOG_THROW(ddModel != nullptr, storm::exceptions::InvalidArgumentException, "Hybrid engine requires a DD-based input model.");
                return verifySymbolicModelWithHybridEngine(ddModel, formula, onlyInitialStatesRelevant);
            }
            case storm::settings::modules::CoreSettings::Engine::Dd: {
                std::shared_ptr<storm::models::symbolic::Model<DdType>> ddModel = model->template as<storm::models::symbolic::Model<DdType>>();
                STORM_LOG_THROW(ddModel != nullptr, storm::exceptions::InvalidArgumentException, "Dd engine requires a DD-based input model.");
                return verifySymbolicModelWithDdEngine(ddModel, formula, onlyInitialStatesRelevant);
            }
            default: {
                STORM_LOG_ASSERT(false, "This position should not be reached, as at this point no model has been built.");
            }
        }
    }

    template<typename ValueType>
    std::unique_ptr<storm::modelchecker::CheckResult> verifySparseDtmc(std::shared_ptr<storm::models::sparse::Dtmc<ValueType>> dtmc, storm::modelchecker::CheckTask<storm::logic::Formula, ValueType> const& task) {
        std::unique_ptr<storm::modelchecker::CheckResult> result;
        if (storm::settings::getModule<storm::settings::modules::CoreSettings>().getEquationSolver() == storm::solver::EquationSolverType::Elimination && storm::settings::getModule<storm::settings::modules::EliminationSettings>().isUseDedicatedModelCheckerSet()) {
            storm::modelchecker::SparseDtmcEliminationModelChecker<storm::models::sparse::Dtmc<ValueType>> modelchecker(*dtmc);
            if (modelchecker.canHandle(task)) {
                result = modelchecker.check(task);
            } else {
                STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "The property " << task.getFormula() << " is not supported by the dedicated elimination model checker.");
            }
        } else {
            storm::modelchecker::SparseDtmcPrctlModelChecker<storm::models::sparse::Dtmc<ValueType>> modelchecker(*dtmc);
            if (modelchecker.canHandle(task)) {
                result = modelchecker.check(task);
            } else {
                STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "The property " << task.getFormula() << " is not supported.");
            }
        }
        return result;
    }

    template<typename ValueType>
    std::unique_ptr<storm::modelchecker::CheckResult> verifySparseCtmc(std::shared_ptr<storm::models::sparse::Ctmc<ValueType>> ctmc, storm::modelchecker::CheckTask<storm::logic::Formula, ValueType> const& task) {
        std::unique_ptr<storm::modelchecker::CheckResult> result;
        storm::modelchecker::SparseCtmcCslModelChecker<storm::models::sparse::Ctmc<ValueType>> modelchecker(*ctmc);
        if (modelchecker.canHandle(task)) {
            result = modelchecker.check(task);
        } else {
            STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "The property " << task.getFormula() << " is not supported.");
        }
        return result;
    }

    template<typename ValueType>
    std::unique_ptr<storm::modelchecker::CheckResult> verifySparseMdp(std::shared_ptr<storm::models::sparse::Mdp<ValueType>> mdp, storm::modelchecker::CheckTask<storm::logic::Formula, ValueType> const& task) {
        std::unique_ptr<storm::modelchecker::CheckResult> result;
        storm::modelchecker::SparseMdpPrctlModelChecker<storm::models::sparse::Mdp<ValueType>> modelchecker(*mdp);
        if (modelchecker.canHandle(task)) {
            result = modelchecker.check(task);
        } else {
            STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "The property " << task.getFormula() << " is not supported.");
        }
        return result;
    }

    template<typename ValueType>
    std::unique_ptr<storm::modelchecker::CheckResult> verifySparseMarkovAutomaton(std::shared_ptr<storm::models::sparse::MarkovAutomaton<ValueType>> ma, storm::modelchecker::CheckTask<storm::logic::Formula> const& task) {
        std::unique_ptr<storm::modelchecker::CheckResult> result;
        // Close the MA, if it is not already closed.
        if (!ma->isClosed()) {
            ma->close();
        }
        storm::modelchecker::SparseMarkovAutomatonCslModelChecker<storm::models::sparse::MarkovAutomaton<ValueType>> modelchecker(*ma);
        if (modelchecker.canHandle(task)) {
            result = modelchecker.check(task);
        } else {
            STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "The property " << task.getFormula() << " is not supported.");
        }
        return result;
    }

    template<typename ValueType>
    std::unique_ptr<storm::modelchecker::CheckResult> verifySparseModel(std::shared_ptr<storm::models::sparse::Model<ValueType>> model, std::shared_ptr<storm::logic::Formula const> const& formula, bool onlyInitialStatesRelevant = false) {
        storm::modelchecker::CheckTask<storm::logic::Formula, ValueType> task(*formula, onlyInitialStatesRelevant);

        std::unique_ptr<storm::modelchecker::CheckResult> result;
        if (model->getType() == storm::models::ModelType::Dtmc) {
            result = verifySparseDtmc(model->template as<storm::models::sparse::Dtmc<ValueType>>(), task);
        } else if (model->getType() == storm::models::ModelType::Mdp) {
            result = verifySparseMdp(model->template as<storm::models::sparse::Mdp<ValueType>>(), task);
        } else if (model->getType() == storm::models::ModelType::Ctmc) {
            result = verifySparseCtmc(model->template as<storm::models::sparse::Ctmc<ValueType>>(), task);
        } else if (model->getType() == storm::models::ModelType::MarkovAutomaton) {
            result = verifySparseMarkovAutomaton(model->template as<storm::models::sparse::MarkovAutomaton<ValueType>>(), task);
        } else {
            STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "The model type " << model->getType() << " is not supported.");
        }
        return result;
    }

#ifdef STORM_HAVE_CARL
    template<>
    inline std::unique_ptr<storm::modelchecker::CheckResult> verifySparseModel(std::shared_ptr<storm::models::sparse::Model<storm::RationalNumber>> model, std::shared_ptr<storm::logic::Formula const> const& formula, bool onlyInitialStatesRelevant) {
        storm::modelchecker::CheckTask<storm::logic::Formula, RationalNumber> task(*formula, onlyInitialStatesRelevant);

        std::unique_ptr<storm::modelchecker::CheckResult> result;
        if (model->getType() == storm::models::ModelType::Dtmc) {
            result = verifySparseDtmc(model->template as<storm::models::sparse::Dtmc<storm::RationalNumber>>(), task);
        } else if (model->getType() == storm::models::ModelType::Ctmc) {
            result = verifySparseCtmc(model->template as<storm::models::sparse::Ctmc<storm::RationalNumber>>(), task);
        } else if (model->getType() == storm::models::ModelType::Mdp) {
            result = verifySparseMdp(model->template as<storm::models::sparse::Mdp<storm::RationalNumber>>(), task);
        } else {
            STORM_LOG_ASSERT(false, "Illegal model type.");
        }
        return result;
    }

    inline void exportParametricResultToFile(storm::RationalFunction const& result, storm::models::sparse::Dtmc<storm::RationalFunction>::ConstraintCollector const& constraintCollector, std::string const& path) {
        std::ofstream filestream;
        storm::utility::openFile(path, filestream);
        // TODO: add checks.
        filestream << "!Parameters: ";
        std::set<storm::RationalFunctionVariable> vars = result.gatherVariables();
        std::copy(vars.begin(), vars.end(), std::ostream_iterator<storm::RationalFunctionVariable>(filestream, "; "));
        filestream << std::endl;
        filestream << "!Result: " << result << std::endl;
        filestream << "!Well-formed Constraints: " << std::endl;
        std::copy(constraintCollector.getWellformedConstraints().begin(), constraintCollector.getWellformedConstraints().end(), std::ostream_iterator<storm::ArithConstraint<storm::RationalFunction>>(filestream, "\n"));
        filestream << "!Graph-preserving Constraints: " << std::endl;
        std::copy(constraintCollector.getGraphPreservingConstraints().begin(), constraintCollector.getGraphPreservingConstraints().end(), std::ostream_iterator<storm::ArithConstraint<storm::RationalFunction>>(filestream, "\n"));
        storm::utility::closeFile(filestream);
    }

    template<>
    inline std::unique_ptr<storm::modelchecker::CheckResult> verifySparseModel(std::shared_ptr<storm::models::sparse::Model<storm::RationalFunction>> model, std::shared_ptr<storm::logic::Formula const> const& formula, bool onlyInitialStatesRelevant) {
        storm::modelchecker::CheckTask<storm::logic::Formula, storm::RationalFunction> task(*formula, onlyInitialStatesRelevant);

        std::unique_ptr<storm::modelchecker::CheckResult> result;
        if (model->getType() == storm::models::ModelType::Dtmc) {
            result = verifySparseDtmc(model->template as<storm::models::sparse::Dtmc<storm::RationalFunction>>(), task);
        } else if (model->getType() == storm::models::ModelType::Mdp) {
            //std::shared_ptr<storm::models::sparse::Mdp<storm::RationalFunction>> mdp = model->template as<storm::models::sparse::Mdp<storm::RationalFunction>>();
            STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "The parametric engine currently does not support MDPs.");
        } else if (model->getType() == storm::models::ModelType::Ctmc) {
            result = verifySparseCtmc(model->template as<storm::models::sparse::Ctmc<storm::RationalFunction>>(), task);
        } else {
            STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "The parametric engine currently does not support " << model->getType());
        }
        return result;
    }

    /*!
     * Initializes a region model checker.
     *
     * @param regionModelChecker the resulting model checker object
     * @param programFilePath a path to the prism program file
     * @param formulaString The considered formula (as path to the file or directly as string.) Should be exactly one formula.
     * @param constantsString can be used to specify constants for certain parameters, e.g., "p=0.9,R=42"
     * @return true when initialization was successful
     */
    inline bool initializeRegionModelChecker(std::shared_ptr<storm::modelchecker::region::AbstractSparseRegionModelChecker<storm::RationalFunction, double>>& regionModelChecker,
                                      std::string const& programFilePath,
                                      std::string const& formulaString,
                                      std::string const& constantsString=""){
        regionModelChecker.reset();
        // Program and formula
        storm::prism::Program program = parseProgram(programFilePath);
        program = storm::utility::prism::preprocess(program, constantsString);
        std::vector<std::shared_ptr<const storm::logic::Formula>> formulas = extractFormulasFromProperties(parsePropertiesForPrismProgram(formulaString, program));
        if(formulas.size()!=1) {
            STORM_LOG_ERROR("The given formulaString does not specify exactly one formula");
            return false;
        }
        std::shared_ptr<storm::models::sparse::Model<storm::RationalFunction>> model = buildSparseModel<storm::RationalFunction>(program, formulas);
        auto const& regionSettings = storm::settings::getModule<storm::settings::modules::RegionSettings>();
        storm::modelchecker::region::SparseRegionModelCheckerSettings settings(regionSettings.getSampleMode(), regionSettings.getApproxMode(), regionSettings.getSmtMode());
        // Preprocessing and ModelChecker
        if (model->isOfType(storm::models::ModelType::Dtmc)){
            preprocessModel<storm::models::sparse::Dtmc<storm::RationalFunction>>(model,formulas);
            regionModelChecker = std::make_shared<storm::modelchecker::region::SparseDtmcRegionModelChecker<storm::models::sparse::Dtmc<storm::RationalFunction>, double>>(model->as<storm::models::sparse::Dtmc<storm::RationalFunction>>(), settings);
        } else if (model->isOfType(storm::models::ModelType::Mdp)){
            preprocessModel<storm::models::sparse::Mdp<storm::RationalFunction>>(model,formulas);
            regionModelChecker = std::make_shared<storm::modelchecker::region::SparseMdpRegionModelChecker<storm::models::sparse::Mdp<storm::RationalFunction>, double>>(model->as<storm::models::sparse::Mdp<storm::RationalFunction>>(),  settings);
        } else {
            STORM_LOG_ERROR("The type of the given model is not supported (only Dtmcs or Mdps are supported");
            return false;
        }
        // Specify the formula
        if(!regionModelChecker->canHandle(*formulas[0])){
            STORM_LOG_ERROR("The given formula is not supported.");
            return false;
        }
        regionModelChecker->specifyFormula(formulas[0]);
        return true;
    }

    /*!
     * Computes the reachability value at the given point by instantiating the model.
     *
     * @param regionModelChecker the model checker object that is to be used
     * @param point the valuation of the different variables
     * @return true iff the specified formula is satisfied (i.e., iff the reachability value is within the bound of the formula)
     */
    inline bool checkSamplingPoint(std::shared_ptr<storm::modelchecker::region::AbstractSparseRegionModelChecker<storm::RationalFunction, double>> regionModelChecker,
                                   std::map<storm::RationalFunctionVariable, storm::RationalNumber> const& point){
        return regionModelChecker->valueIsInBoundOfFormula(regionModelChecker->getReachabilityValue(point));
    }

    /*!
     * Does an approximation of the reachability value for all parameters in the given region.
     * @param regionModelChecker the model checker object that is to be used
     * @param lowerBoundaries maps every variable to its lowest possible value within the region. (corresponds to the bottom left corner point in the 2D case)
     * @param upperBoundaries maps every variable to its highest possible value within the region. (corresponds to the top right corner point in the 2D case)
     * @param proveAllSat if set to true, it is checked whether the property is satisfied for all parameters in the given region. Otherwise, it is checked
     *                    whether the property is violated for all parameters.
     * @return true iff the objective (given by the proveAllSat flag) was accomplished.
     *
     * So there are the following cases:
     * proveAllSat=true,  return=true  ==> the property is SATISFIED for all parameters in the given region
     * proveAllSat=true,  return=false ==> the approximative value is NOT within the bound of the formula (either the approximation is too bad or there are points in the region that violate the property)
     * proveAllSat=false, return=true  ==> the property is VIOLATED for all parameters in the given region
     * proveAllSat=false, return=false ==> the approximative value IS within the bound of the formula (either the approximation is too bad or there are points in the region that satisfy the property)
     */
    inline bool checkRegionApproximation(std::shared_ptr<storm::modelchecker::region::AbstractSparseRegionModelChecker<storm::RationalFunction, double>> regionModelChecker,
                                         std::map<storm::RationalFunctionVariable, storm::RationalNumber> const& lowerBoundaries,
                                         std::map<storm::RationalFunctionVariable, storm::RationalNumber> const& upperBoundaries,
                                         bool proveAllSat){
        storm::modelchecker::region::ParameterRegion<storm::RationalFunction> region(lowerBoundaries, upperBoundaries);
        return regionModelChecker->checkRegionWithApproximation(region, proveAllSat);
    }


#endif

    template<storm::dd::DdType DdType>
    std::unique_ptr<storm::modelchecker::CheckResult> verifySymbolicModelWithHybridEngine(std::shared_ptr<storm::models::symbolic::Model<DdType>> model, std::shared_ptr<storm::logic::Formula const> const& formula, bool onlyInitialStatesRelevant = false) {
        std::unique_ptr<storm::modelchecker::CheckResult> result;
        storm::modelchecker::CheckTask<storm::logic::Formula> task(*formula, onlyInitialStatesRelevant);
        if (model->getType() == storm::models::ModelType::Dtmc) {
            std::shared_ptr<storm::models::symbolic::Dtmc<DdType>> dtmc = model->template as<storm::models::symbolic::Dtmc<DdType>>();
            storm::modelchecker::HybridDtmcPrctlModelChecker<storm::models::symbolic::Dtmc<DdType>> modelchecker(*dtmc);
            if (modelchecker.canHandle(task)) {
                result = modelchecker.check(task);
            }
        } else if (model->getType() == storm::models::ModelType::Ctmc) {
            std::shared_ptr<storm::models::symbolic::Ctmc<DdType>> ctmc = model->template as<storm::models::symbolic::Ctmc<DdType>>();
            storm::modelchecker::HybridCtmcCslModelChecker<storm::models::symbolic::Ctmc<DdType>> modelchecker(*ctmc);
            if (modelchecker.canHandle(task)) {
                result = modelchecker.check(task);
            }
        } else if (model->getType() == storm::models::ModelType::Mdp) {
            std::shared_ptr<storm::models::symbolic::Mdp<DdType>> mdp = model->template as<storm::models::symbolic::Mdp<DdType>>();
            storm::modelchecker::HybridMdpPrctlModelChecker<storm::models::symbolic::Mdp<DdType>> modelchecker(*mdp);
            if (modelchecker.canHandle(task)) {
                result = modelchecker.check(task);
            }
        } else {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "This functionality is not yet implemented.");
        }
        return result;
    }

    template<storm::dd::DdType DdType>
    std::unique_ptr<storm::modelchecker::CheckResult> verifySymbolicModelWithDdEngine(std::shared_ptr<storm::models::symbolic::Model<DdType>> model, std::shared_ptr<storm::logic::Formula const> const& formula, bool onlyInitialStatesRelevant) {
        std::unique_ptr<storm::modelchecker::CheckResult> result;
        storm::modelchecker::CheckTask<storm::logic::Formula> task(*formula, onlyInitialStatesRelevant);
        if (model->getType() == storm::models::ModelType::Dtmc) {
            std::shared_ptr<storm::models::symbolic::Dtmc<DdType>> dtmc = model->template as<storm::models::symbolic::Dtmc<DdType>>();
            storm::modelchecker::SymbolicDtmcPrctlModelChecker<storm::models::symbolic::Dtmc<DdType>> modelchecker(*dtmc);
            if (modelchecker.canHandle(task)) {
                result = modelchecker.check(task);
            }
        } else if (model->getType() == storm::models::ModelType::Mdp) {
            std::shared_ptr<storm::models::symbolic::Mdp<DdType>> mdp = model->template as<storm::models::symbolic::Mdp<DdType>>();
            storm::modelchecker::SymbolicMdpPrctlModelChecker<storm::models::symbolic::Mdp<DdType>> modelchecker(*mdp);
            if (modelchecker.canHandle(task)) {
                result = modelchecker.check(task);
            }
        } else {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "This functionality is not yet implemented.");
        }
        return result;
    }
    
    template<storm::dd::DdType DdType, typename ValueType>
    std::unique_ptr<storm::modelchecker::CheckResult> verifySymbolicModelWithAbstractionRefinementEngine(storm::storage::SymbolicModelDescription const& model, std::shared_ptr<const storm::logic::Formula> const& formula, bool onlyInitialStatesRelevant = false) {
        
        STORM_LOG_THROW(model.getModelType() == storm::storage::SymbolicModelDescription::ModelType::DTMC || model.getModelType() == storm::storage::SymbolicModelDescription::ModelType::MDP, storm::exceptions::InvalidSettingsException, "Can only treat DTMCs/MDPs using the abstraction refinement engine.");
        
        if (model.getModelType() == storm::storage::SymbolicModelDescription::ModelType::DTMC) {
            storm::modelchecker::GameBasedMdpModelChecker<DdType, storm::models::symbolic::Dtmc<DdType, ValueType>> modelchecker(model);
            storm::modelchecker::CheckTask<storm::logic::Formula> task(*formula, onlyInitialStatesRelevant);
            return modelchecker.check(task);
        } else {
            storm::modelchecker::GameBasedMdpModelChecker<DdType, storm::models::symbolic::Mdp<DdType, ValueType>> modelchecker(model);
            storm::modelchecker::CheckTask<storm::logic::Formula> task(*formula, onlyInitialStatesRelevant);
            return modelchecker.check(task);
        }
    }
    
    /**
     *
     */
    void exportJaniModel(storm::jani::Model const& model, std::vector<storm::jani::Property> const& properties, std::string const& filepath);
    
    template<typename ValueType>
    void exportMatrixToFile(std::shared_ptr<storm::models::sparse::Model<ValueType>> model, std::string const& filepath) {
        STORM_LOG_THROW(model->getType() != storm::models::ModelType::Ctmc, storm::exceptions::NotImplementedException, "This functionality is not yet implemented." );
        std::ofstream stream;
        storm::utility::openFile(filepath, stream);
        model->getTransitionMatrix().printAsMatlabMatrix(stream);
        storm::utility::closeFile(stream);
    }
        
}

#endif	/* STORM_H */
