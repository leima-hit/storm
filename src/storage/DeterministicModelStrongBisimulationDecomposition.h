#ifndef STORM_STORAGE_DETERMINISTICMODELSTRONGBISIMULATIONDECOMPOSITION_H_
#define STORM_STORAGE_DETERMINISTICMODELSTRONGBISIMULATIONDECOMPOSITION_H_

#include <queue>
#include <deque>

#include "src/storage/sparse/StateType.h"
#include "src/storage/Decomposition.h"
#include "src/models/Dtmc.h"
#include "src/models/Ctmc.h"
#include "src/storage/Distribution.h"
#include "src/utility/ConstantsComparator.h"
#include "src/utility/OsDetection.h"

namespace storm {
    namespace storage {
        
        /*!
         * This class represents the decomposition model into its (strong) bisimulation quotient.
         */
        template <typename ValueType>
        class DeterministicModelStrongBisimulationDecomposition : public Decomposition<StateBlock> {
        public:
            /*!
             * Decomposes the given DTMC into equivalence classes under strong bisimulation.
             *
             * @param model The model to decompose.
             * @param buildQuotient Sets whether or not the quotient model is to be built.
             */
            DeterministicModelStrongBisimulationDecomposition(storm::models::Dtmc<ValueType> const& model, bool buildQuotient = false);

            /*!
             * Decomposes the given CTMC into equivalence classes under strong bisimulation.
             *
             * @param model The model to decompose.
             * @param buildQuotient Sets whether or not the quotient model is to be built.
             */
            DeterministicModelStrongBisimulationDecomposition(storm::models::Ctmc<ValueType> const& model, bool buildQuotient = false);
            
            /*!
             * Decomposes the given DTMC into equivalence classes under strong bisimulation in a way that onle safely
             * preserves formulas of the form phi until psi.
             *
             * @param model The model to decompose.
             * @param phiLabel The label that all phi states carry in the model.
             * @param psiLabel The label that all psi states carry in the model.
             * @param bounded If set to true, also bounded until formulas are preserved.
             * @param buildQuotient Sets whether or not the quotient model is to be built.
             */
            DeterministicModelStrongBisimulationDecomposition(storm::models::Dtmc<ValueType> const& model, std::string const& phiLabel, std::string const& psiLabel, bool bounded, bool buildQuotient = false);
            
            /*!
             * Decomposes the given CTMC into equivalence classes under strong bisimulation in a way that onle safely
             * preserves formulas of the form phi until psi.
             *
             * @param model The model to decompose.
             * @param phiLabel The label that all phi states carry in the model.
             * @param psiLabel The label that all psi states carry in the model.
             * @param bounded If set to true, also bounded until formulas are preserved.
             * @param buildQuotient Sets whether or not the quotient model is to be built.
             */
            DeterministicModelStrongBisimulationDecomposition(storm::models::Ctmc<ValueType> const& model, std::string const& phiLabel, std::string const& psiLabel, bool bounded, bool buildQuotient = false);
            
            /*!
             * Retrieves the quotient of the model under the previously computed bisimulation.
             *
             * @return The quotient model.
             */
            std::shared_ptr<storm::models::AbstractDeterministicModel<ValueType>> getQuotient() const;
            
        private:
            class Partition;
            
            class Block {
            public:
                typedef typename std::list<Block>::iterator iterator;
                typedef typename std::list<Block>::const_iterator const_iterator;
                
                // Creates a new block with the given begin and end.
                Block(storm::storage::sparse::state_type begin, storm::storage::sparse::state_type end, Block* prev, Block* next, std::shared_ptr<std::string> const& label = nullptr);
                
                // Prints the block.
                void print(Partition const& partition) const;

                // Sets the beginning index of the block.
                void setBegin(storm::storage::sparse::state_type begin);

                // Moves the beginning index of the block one step further.
                void incrementBegin();
                
                // Sets the end index of the block.
                void setEnd(storm::storage::sparse::state_type end);

                // Moves the end index of the block one step to the front.
                void decrementEnd();
                
                // Returns the beginning index of the block.
                storm::storage::sparse::state_type getBegin() const;
                
                // Returns the beginning index of the block.
                storm::storage::sparse::state_type getEnd() const;
                
                // Retrieves the original beginning index of the block in case the begin index has been moved.
                storm::storage::sparse::state_type getOriginalBegin() const;
                
                // Returns the iterator the block in the list of overall blocks.
                iterator getIterator() const;

                // Returns the iterator the block in the list of overall blocks.
                void setIterator(iterator it);
                
                // Returns the iterator the next block in the list of overall blocks if it exists.
                iterator getNextIterator() const;

                // Returns the iterator the next block in the list of overall blocks if it exists.
                iterator getPreviousIterator() const;

                // Gets the next block (if there is one).
                Block& getNextBlock();

                // Gets the next block (if there is one).
                Block const& getNextBlock() const;
                
                // Gets a pointer to the next block (if there is one).
                Block* getNextBlockPointer();

                // Retrieves whether the block as a successor block.
                bool hasNextBlock() const;

                // Gets the previous block (if there is one).
                Block& getPreviousBlock();

                // Gets a pointer to the previous block (if there is one).
                Block* getPreviousBlockPointer();
                
                // Gets the next block (if there is one).
                Block const& getPreviousBlock() const;

                // Retrieves whether the block as a successor block.
                bool hasPreviousBlock() const;
                
                // Checks consistency of the information in the block.
                bool check() const;
                
                // Retrieves the number of states in this block.
                std::size_t getNumberOfStates() const;
                
                // Checks whether the block is marked as a splitter.
                bool isMarkedAsSplitter() const;
                
                // Marks the block as being a splitter.
                void markAsSplitter();
                
                // Removes the mark.
                void unmarkAsSplitter();
                
                // Retrieves the ID of the block.
                std::size_t getId() const;
                
                // Retrieves the marked position in the block.
                storm::storage::sparse::state_type getMarkedPosition() const;

                // Sets the marked position to the given value..
                void setMarkedPosition(storm::storage::sparse::state_type position);

                // Increases the marked position by one.
                void incrementMarkedPosition();
                
                // Resets the marked position to the begin of the block.
                void resetMarkedPosition();
                
                // Retrieves whether the block is marked as a predecessor.
                bool isMarkedAsPredecessor() const;
                
                // Marks the block as being a predecessor block.
                void markAsPredecessorBlock();
                
                // Removes the marking.
                void unmarkAsPredecessorBlock();
                
                // Sets whether or not the block is to be interpreted as absorbing.
                void setAbsorbing(bool absorbing);
                
                // Retrieves whether the block is to be interpreted as absorbing.
                bool isAbsorbing() const;
                
                // Retrieves whether the block has a special label.
                bool hasLabel() const;
                
                // Retrieves the special label of the block if it has one.
                std::string const& getLabel() const;
                
                // Retrieves a pointer to the label of the block (which is the nullptr if there is none).
                std::shared_ptr<std::string> const& getLabelPtr() const;
                
            private:
                // An iterator to itself. This is needed to conveniently insert elements in the overall list of blocks
                // kept by the partition.
                iterator selfIt;
                
                // Pointers to the next and previous block.
                Block* next;
                Block* prev;
                
                // The begin and end indices of the block in terms of the state vector of the partition.
                storm::storage::sparse::state_type begin;
                storm::storage::sparse::state_type end;
                
                // A field that can be used for marking the block.
                bool markedAsSplitter;
                
                // A field that can be used for marking the block as a predecessor block.
                bool markedAsPredecessorBlock;
                
                // A position that can be used to store a certain position within the block.
                storm::storage::sparse::state_type markedPosition;
                
                // A flag indicating whether the block is to be interpreted as absorbing or not.
                bool absorbing;
                
                // The ID of the block. This is only used for debugging purposes.
                std::size_t id;
                
                // The label of the block or nullptr if it has none.
                std::shared_ptr<std::string> label;
                
                // A counter for the IDs of the blocks.
                static std::size_t blockId;
            };
            
            class Partition {
            public:
                friend class Block;
                
                /*!
                 * Creates a partition with one block consisting of all the states.
                 *
                 * @param numberOfStates The number of states the partition holds.
                 */
                Partition(std::size_t numberOfStates);

                /*!
                 * Creates a partition with three blocks: one with all phi states, one with all psi states and one with
                 * all other states. The former two blocks are marked as being absorbing, because their outgoing
                 * transitions shall not be taken into account for future refinement.
                 *
                 * @param numberOfStates The number of states the partition holds.
                 * @param prob0States The states which have probability 0 of satisfying phi until psi.
                 * @param prob1States The states which have probability 1 of satisfying phi until psi.
                 * @param otherLabel The label that is to be attached to all other states.
                 * @param prob1Label The label that is to be attached to all states with probability 1.
                 */
                Partition(std::size_t numberOfStates, storm::storage::BitVector const& prob0States, storm::storage::BitVector const& prob1States, std::string const& otherLabel, std::string const& prob1Label);
                
                Partition() = default;
                Partition(Partition const& other) = default;
                Partition& operator=(Partition const& other) = default;
#ifndef WINDOWS
                Partition(Partition&& other) = default;
                Partition& operator=(Partition&& other) = default;
#endif
                
                /*!
                 * Splits all blocks of the partition such that afterwards all blocks contain only states with the label
                 * or no labeled state at all.
                 */
                void splitLabel(storm::storage::BitVector const& statesWithLabel);
                
                // Retrieves the size of the partition, i.e. the number of blocks.
                std::size_t size() const;
                
                // Prints the partition to the standard output.
                void print() const;

                // Splits the block at the given position and inserts a new block after the current one holding the rest
                // of the states.
                Block& splitBlock(Block& block, storm::storage::sparse::state_type position);
                
                // Inserts a block before the given block. The new block will cover all states between the beginning
                // of the given block and the end of the previous block.
                Block& insertBlock(Block& block);
                
                // Retrieves the blocks of the partition.
                std::list<Block> const& getBlocks() const;

                // Retrieves the blocks of the partition.
                std::list<Block>& getBlocks();

                // Checks the partition for internal consistency.
                bool check() const;
                
                // Returns an iterator to the beginning of the states of the given block.
                typename std::vector<std::pair<storm::storage::sparse::state_type, ValueType>>::iterator getBegin(Block const& block);
                
                // Returns an iterator to the beginning of the states of the given block.
                typename std::vector<std::pair<storm::storage::sparse::state_type, ValueType>>::iterator getEnd(Block const& block);

                // Returns an iterator to the beginning of the states of the given block.
                typename std::vector<std::pair<storm::storage::sparse::state_type, ValueType>>::const_iterator getBegin(Block const& block) const;
                
                // Returns an iterator to the beginning of the states of the given block.
                typename std::vector<std::pair<storm::storage::sparse::state_type, ValueType>>::const_iterator getEnd(Block const& block) const;

                // Swaps the positions of the two given states.
                void swapStates(storm::storage::sparse::state_type state1, storm::storage::sparse::state_type state2);

                // Swaps the positions of the two states given by their positions.
                void swapStatesAtPositions(storm::storage::sparse::state_type position1, storm::storage::sparse::state_type position2);

                // Retrieves the block of the given state.
                Block& getBlock(storm::storage::sparse::state_type state);

                // Retrieves the block of the given state.
                Block const& getBlock(storm::storage::sparse::state_type state) const;

                // Retrieves the position of the given state.
                storm::storage::sparse::state_type const& getPosition(storm::storage::sparse::state_type state) const;

                // Retrieves the position of the given state.
                void setPosition(storm::storage::sparse::state_type state, storm::storage::sparse::state_type position);
                
                // Sets the position of the state to the given position.
                storm::storage::sparse::state_type const& getState(storm::storage::sparse::state_type position) const;

                // Retrieves the value for the given state.
                ValueType const& getValue(storm::storage::sparse::state_type state) const;
                
                // Retrieves the value at the given position.
                ValueType const& getValueAtPosition(storm::storage::sparse::state_type position) const;
                
                // Sets the given value for the given state.
                void setValue(storm::storage::sparse::state_type state, ValueType value);
                
                // Retrieves the vector with the probabilities going into the current splitter.
                std::vector<std::pair<storm::storage::sparse::state_type, ValueType>>& getStatesAndValues();

                // Increases the value for the given state by the specified amount.
                void increaseValue(storm::storage::sparse::state_type state, ValueType value);
                
                // Updates the block mapping for the given range of states to the specified block.
                void updateBlockMapping(Block& block, typename std::vector<std::pair<storm::storage::sparse::state_type, ValueType>>::iterator first, typename std::vector<std::pair<storm::storage::sparse::state_type, ValueType>>::iterator end);
                
                // Retrieves the first block of the partition.
                Block& getFirstBlock();
            private:
                // The list of blocks in the partition.
                std::list<Block> blocks;
                
                // A mapping of states to their blocks.
                std::vector<Block*> stateToBlockMapping;
                
                // A vector containing all the states and their values. It is ordered in a special way such that the
                // blocks only need to define their start/end indices.
                std::vector<std::pair<storm::storage::sparse::state_type, ValueType>> statesAndValues;
                
                // This vector keeps track of the position of each state in the state vector.
                std::vector<storm::storage::sparse::state_type> positions;
            };
            
            /*!
             * Performs the partition refinement on the model and thereby computes the equivalence classes under strong
             * bisimulation equivalence. If required, the quotient model is built and may be retrieved using
             * getQuotient().
             *
             * @param model The model on whose state space to compute the coarses strong bisimulation relation.
             * @param backwardTransitions The backward transitions of the model.
             * @param The initial partition.
             * @param buildQuotient If set, the quotient model is built and may be retrieved using the getQuotient()
             * method.
             */
            template<typename ModelType>
            void partitionRefinement(ModelType const& model, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, Partition& partition, bool buildQuotient);
            
            /*!
             * Refines the partition based on the provided splitter. After calling this method all blocks are stable
             * with respect to the splitter.
             *
             * @param backwardTransitions A matrix that can be used to retrieve the predecessors (and their
             * probabilities).
             * @param splitter The splitter to use.
             * @param partition The partition to split.
             * @param splitterQueue A queue into which all blocks that were split are inserted so they can be treated
             * as splitters in the future.
             */
            void refinePartition(storm::storage::SparseMatrix<ValueType> const& backwardTransitions, Block& splitter, Partition& partition, std::deque<Block*>& splitterQueue);
            
            /*!
             * Refines the block based on their probability values (leading into the splitter).
             *
             * @param block The block to refine.
             * @param partition The partition that contains the block.
             * @param splitterQueue A queue into which all blocks that were split are inserted so they can be treated
             * as splitters in the future.
             */
            void refineBlockProbabilities(Block& block, Partition& partition, std::deque<Block*>& splitterQueue);
            
            /*!
             * Builds the quotient model based on the previously computed equivalence classes (stored in the blocks
             * of the decomposition.
             *
             * @param model The model whose state space was used for computing the equivalence classes. This is used for
             * determining the transitions of each equivalence class.
             * @param partition The previously computed partition. This is used for quickly retrieving the block of a
             * state.
             */
            template<typename ModelType>
            void buildQuotient(ModelType const& model, Partition const& partition);

            /*!
             * Creates the measure-driven initial partition for reaching psi states from phi states.
             *
             * @param model The model whose state space is partitioned based on reachability of psi states from phi
             * states.
             * @param backwardTransitions The backward transitions of the model.
             * @param phiLabel The label that all phi states carry in the model.
             * @param psiLabel The label that all psi states carry in the model.
             * @param bounded If set to true, the initial partition will be chosen in such a way that preserves bounded
             * reachability queries.
             * @return The resulting partition.
             */
            template<typename ModelType>
            Partition getMeasureDrivenInitialPartition(ModelType const& model, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, std::string const& phiLabel, std::string const& psiLabel, bool bounded = false);
            
            /*!
             * Creates the initial partition based on all the labels in the given model.
             *
             * @param model The model whose state space is partitioned based on its labels.
             * @return The resulting partition.
             */
            template<typename ModelType>
            Partition getLabelBasedInitialPartition(ModelType const& model);
            
            // If required, a quotient model is built and stored in this member.
            std::shared_ptr<storm::models::AbstractDeterministicModel<ValueType>> quotient;
            
            // A comparator that is used for determining whether two probabilities are considered to be equal.
            storm::utility::ConstantsComparator<ValueType> comparator;
        };
    }
}

#endif /* STORM_STORAGE_DETERMINISTICMODELSTRONGBISIMULATIONDECOMPOSITION_H_ */