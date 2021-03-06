#include "storm/abstraction/prism/PrismMenuGameAbstractor.h"

#include "storm/abstraction/BottomStateResult.h"
#include "storm/abstraction/GameBddResult.h"
#include "storm/abstraction/ExpressionTranslator.h"

#include "storm/storage/BitVector.h"

#include "storm/storage/prism/Program.h"

#include "storm/storage/dd/DdManager.h"
#include "storm/storage/dd/Add.h"

#include "storm/models/symbolic/StandardRewardModel.h"

#include "storm/settings/SettingsManager.h"

#include "storm/utility/dd.h"
#include "storm/utility/macros.h"
#include "storm/utility/solver.h"
#include "storm/exceptions/WrongFormatException.h"
#include "storm/exceptions/InvalidArgumentException.h"
#include "storm/exceptions/NotSupportedException.h"

#include "storm-config.h"
#include "storm/adapters/CarlAdapter.h"

namespace storm {
    namespace abstraction {
        namespace prism {
                        
            using storm::settings::modules::AbstractionSettings;
            
            template <storm::dd::DdType DdType, typename ValueType>
            PrismMenuGameAbstractor<DdType, ValueType>::PrismMenuGameAbstractor(storm::prism::Program const& program,
                                                                std::shared_ptr<storm::utility::solver::SmtSolverFactory> const& smtSolverFactory)
            : program(program), smtSolverFactory(smtSolverFactory), abstractionInformation(program.getManager(), program.getAllExpressionVariables(), smtSolverFactory->create(program.getManager())), modules(), initialStateAbstractor(abstractionInformation, {program.getInitialStatesExpression()}, this->smtSolverFactory), validBlockAbstractor(abstractionInformation, smtSolverFactory), currentGame(nullptr), refinementPerformed(false) {
                
                // For now, we assume that there is a single module. If the program has more than one module, it needs
                // to be flattened before the procedure.
                STORM_LOG_THROW(program.getNumberOfModules() == 1, storm::exceptions::WrongFormatException, "Cannot create abstract program from program containing too many modules.");
                
                // Add all variables range expressions to the information object.
                for (auto const& range : this->program.get().getAllRangeExpressions()) {
                    abstractionInformation.addConstraint(range);
                    initialStateAbstractor.constrain(range);
                }
                
                uint_fast64_t totalNumberOfCommands = 0;
                uint_fast64_t maximalUpdateCount = 0;
                std::vector<storm::expressions::Expression> allGuards;
                for (auto const& module : program.getModules()) {
                    for (auto const& command : module.getCommands()) {
                        maximalUpdateCount = std::max(maximalUpdateCount, static_cast<uint_fast64_t>(command.getNumberOfUpdates()));
                    }
                    
                    totalNumberOfCommands += module.getNumberOfCommands();
                }
                
                // NOTE: currently we assume that 100 player 2 variables suffice, which corresponds to 2^100 possible
                // choices. If for some reason this should not be enough, we could grow this vector dynamically, but
                // odds are that it's impossible to treat such models in any event.
                abstractionInformation.createEncodingVariables(static_cast<uint_fast64_t>(std::ceil(std::log2(totalNumberOfCommands))), 100, static_cast<uint_fast64_t>(std::ceil(std::log2(maximalUpdateCount))));
                
                // For each module of the concrete program, we create an abstract counterpart.
                bool useDecomposition = storm::settings::getModule<storm::settings::modules::AbstractionSettings>().isUseDecompositionSet();
                for (auto const& module : program.getModules()) {
                    this->modules.emplace_back(module, abstractionInformation, this->smtSolverFactory, useDecomposition);
                }
                
                // Retrieve the command-update probability ADD, so we can multiply it with the abstraction BDD later.
                commandUpdateProbabilitiesAdd = modules.front().getCommandUpdateProbabilitiesAdd();
            }
            
            template <storm::dd::DdType DdType, typename ValueType>
            void PrismMenuGameAbstractor<DdType, ValueType>::refine(RefinementCommand const& command) {
                // Add the predicates to the global list of predicates and gather their indices.
                std::vector<uint_fast64_t> predicateIndices;
                for (auto const& predicate : command.getPredicates()) {
                    STORM_LOG_THROW(predicate.hasBooleanType(), storm::exceptions::InvalidArgumentException, "Expecting a predicate of type bool.");
                    predicateIndices.push_back(abstractionInformation.getOrAddPredicate(predicate));
                }
                
                // Refine all abstract modules.
                for (auto& module : modules) {
                    module.refine(predicateIndices);
                }
                
                // Refine initial state abstractor.
                initialStateAbstractor.refine(predicateIndices);
                
                // Refine the valid blocks.
                validBlockAbstractor.refine(predicateIndices);

                refinementPerformed |= !command.getPredicates().empty();
            }
            
            template <storm::dd::DdType DdType, typename ValueType>
            MenuGame<DdType, ValueType> PrismMenuGameAbstractor<DdType, ValueType>::abstract() {
                if (refinementPerformed) {
                    currentGame = buildGame();
                    refinementPerformed = true;
                }
                return *currentGame;
            }
            
            template <storm::dd::DdType DdType, typename ValueType>
            AbstractionInformation<DdType> const& PrismMenuGameAbstractor<DdType, ValueType>::getAbstractionInformation() const {
                return abstractionInformation;
            }
            
            template <storm::dd::DdType DdType, typename ValueType>
            storm::expressions::Expression const& PrismMenuGameAbstractor<DdType, ValueType>::getGuard(uint64_t player1Choice) const {
                return modules.front().getGuard(player1Choice);
            }
            
            template <storm::dd::DdType DdType, typename ValueType>
            std::map<storm::expressions::Variable, storm::expressions::Expression> PrismMenuGameAbstractor<DdType, ValueType>::getVariableUpdates(uint64_t player1Choice, uint64_t auxiliaryChoice) const {
                return modules.front().getVariableUpdates(player1Choice, auxiliaryChoice);
            }
            
            template <storm::dd::DdType DdType, typename ValueType>
            std::pair<uint64_t, uint64_t> PrismMenuGameAbstractor<DdType, ValueType>::getPlayer1ChoiceRange() const {
                return std::make_pair(0, modules.front().getCommands().size());
            }
            
            template <storm::dd::DdType DdType, typename ValueType>
            storm::expressions::Expression PrismMenuGameAbstractor<DdType, ValueType>::getInitialExpression() const {
                return program.get().getInitialStatesExpression();
            }
            
            template <storm::dd::DdType DdType, typename ValueType>
            storm::dd::Bdd<DdType> PrismMenuGameAbstractor<DdType, ValueType>::getStates(storm::expressions::Expression const& expression) {
                storm::abstraction::ExpressionTranslator<DdType> translator(abstractionInformation, smtSolverFactory->create(abstractionInformation.getExpressionManager()));
                return translator.translate(expression);
            }
            
            template <storm::dd::DdType DdType, typename ValueType>
            std::unique_ptr<MenuGame<DdType, ValueType>> PrismMenuGameAbstractor<DdType, ValueType>::buildGame() {
                // As long as there is only one module, we only build its game representation.
                GameBddResult<DdType> game = modules.front().abstract();
                                
                // Construct a set of all unnecessary variables, so we can abstract from it.
                std::set<storm::expressions::Variable> variablesToAbstract(abstractionInformation.getPlayer1VariableSet(abstractionInformation.getPlayer1VariableCount()));
                auto player2Variables = abstractionInformation.getPlayer2VariableSet(game.numberOfPlayer2Variables);
                variablesToAbstract.insert(player2Variables.begin(), player2Variables.end());
                auto auxVariables = abstractionInformation.getAuxVariableSet(0, abstractionInformation.getAuxVariableCount());
                variablesToAbstract.insert(auxVariables.begin(), auxVariables.end());
                
                // Do a reachability analysis on the raw transition relation.
                storm::dd::Bdd<DdType> transitionRelation = game.bdd.existsAbstract(variablesToAbstract);
                storm::dd::Bdd<DdType> initialStates = initialStateAbstractor.getAbstractStates();
                initialStates.addMetaVariables(abstractionInformation.getSourcePredicateVariables());
                storm::dd::Bdd<DdType> reachableStates = storm::utility::dd::computeReachableStates(initialStates, transitionRelation, abstractionInformation.getSourceVariables(), abstractionInformation.getSuccessorVariables());
                
                // Find the deadlock states in the model. Note that this does not find the 'deadlocks' in bottom states,
                // as the bottom states are not contained in the reachable states.
                storm::dd::Bdd<DdType> deadlockStates = transitionRelation.existsAbstract(abstractionInformation.getSuccessorVariables());
                deadlockStates = reachableStates && !deadlockStates;
                
                // If there are deadlock states, we fix them now.
                storm::dd::Add<DdType, ValueType> deadlockTransitions = abstractionInformation.getDdManager().template getAddZero<ValueType>();
                if (!deadlockStates.isZero()) {
                    deadlockTransitions = (deadlockStates && abstractionInformation.getAllPredicateIdentities() && abstractionInformation.encodePlayer1Choice(0, abstractionInformation.getPlayer1VariableCount()) && abstractionInformation.encodePlayer2Choice(0, 0, game.numberOfPlayer2Variables) && abstractionInformation.encodeAux(0, 0, abstractionInformation.getAuxVariableCount())).template toAdd<ValueType>();
                }
                
                // Compute bottom states and the appropriate transitions if necessary.
                BottomStateResult<DdType> bottomStateResult(abstractionInformation.getDdManager().getBddZero(), abstractionInformation.getDdManager().getBddZero());
                bottomStateResult = modules.front().getBottomStateTransitions(reachableStates, game.numberOfPlayer2Variables);
                bool hasBottomStates = !bottomStateResult.states.isZero();
                
                // Construct the transition matrix by cutting away the transitions of unreachable states.
                storm::dd::Add<DdType, ValueType> transitionMatrix = (game.bdd && reachableStates).template toAdd<ValueType>();
                transitionMatrix *= commandUpdateProbabilitiesAdd;
                transitionMatrix += deadlockTransitions;
                
                // Extend the current game information with the 'non-bottom' tag before potentially adding bottom state transitions.
                transitionMatrix *= (abstractionInformation.getBottomStateBdd(true, true) && abstractionInformation.getBottomStateBdd(false, true)).template toAdd<ValueType>();
                reachableStates &= abstractionInformation.getBottomStateBdd(true, true);
                initialStates &= abstractionInformation.getBottomStateBdd(true, true);
                
                // If there are bottom transitions, exnted the transition matrix and reachable states now.
                if (hasBottomStates) {
                    transitionMatrix += bottomStateResult.transitions.template toAdd<ValueType>();
                    reachableStates |= bottomStateResult.states;
                }
                
                std::set<storm::expressions::Variable> usedPlayer2Variables(abstractionInformation.getPlayer2Variables().begin(), abstractionInformation.getPlayer2Variables().begin() + game.numberOfPlayer2Variables);
                
                std::set<storm::expressions::Variable> allNondeterminismVariables = usedPlayer2Variables;
                allNondeterminismVariables.insert(abstractionInformation.getPlayer1Variables().begin(), abstractionInformation.getPlayer1Variables().end());
                
                std::set<storm::expressions::Variable> allSourceVariables(abstractionInformation.getSourceVariables());
                allSourceVariables.insert(abstractionInformation.getBottomStateVariable(true));
                std::set<storm::expressions::Variable> allSuccessorVariables(abstractionInformation.getSuccessorVariables());
                allSuccessorVariables.insert(abstractionInformation.getBottomStateVariable(false));
                
                return std::make_unique<MenuGame<DdType, ValueType>>(abstractionInformation.getDdManagerAsSharedPointer(), reachableStates, initialStates, abstractionInformation.getDdManager().getBddZero(), transitionMatrix, bottomStateResult.states, allSourceVariables, allSuccessorVariables, abstractionInformation.getExtendedSourceSuccessorVariablePairs(), std::set<storm::expressions::Variable>(abstractionInformation.getPlayer1Variables().begin(), abstractionInformation.getPlayer1Variables().end()), usedPlayer2Variables, allNondeterminismVariables, auxVariables, abstractionInformation.getPredicateToBddMap());
            }
            
            template <storm::dd::DdType DdType, typename ValueType>
            void PrismMenuGameAbstractor<DdType, ValueType>::exportToDot(std::string const& filename, storm::dd::Bdd<DdType> const& highlightStates, storm::dd::Bdd<DdType> const& filter) const {
                this->exportToDot(*currentGame, filename, highlightStates, filter);
            }
            
            // Explicitly instantiate the class.
            template class PrismMenuGameAbstractor<storm::dd::DdType::CUDD, double>;
            template class PrismMenuGameAbstractor<storm::dd::DdType::Sylvan, double>;
#ifdef STORM_HAVE_CARL
            template class PrismMenuGameAbstractor<storm::dd::DdType::Sylvan, storm::RationalFunction>;
#endif
        }
    }
}
