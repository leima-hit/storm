#ifndef STORM_STORAGE_SPARSE_STATEVALUATIONS_H_
#define STORM_STORAGE_SPARSE_STATEVALUATIONS_H_

#include <cstdint>
#include <string>

#include "storm/storage/sparse/StateType.h"
#include "storm/storage/expressions/SimpleValuation.h"

#include "storm/models/sparse/StateAnnotation.h"

namespace storm {
    namespace storage {
        namespace sparse {
            
            // A structure holding information about the reachable state space that can be retrieved from the outside.
            struct StateValuations : public storm::models::sparse::StateAnnotation {
                /*!
                 * Constructs a state information object for the given number of states.
                 */
                StateValuations(state_type const& numberOfStates);
                
                virtual ~StateValuations() = default;
                
                // A mapping from state indices to their variable valuations.
                std::vector<storm::expressions::SimpleValuation> valuations;
                
                virtual std::string stateInfo(state_type const& state) const override;
            };
            
        }
    }
}

#endif /* STORM_STORAGE_SPARSE_STATEVALUATIONS_H_ */
