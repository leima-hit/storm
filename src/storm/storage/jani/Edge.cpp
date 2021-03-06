#include "storm/storage/jani/Edge.h"

#include "storm/storage/jani/Model.h"

#include "storm/utility/macros.h"
#include "storm/exceptions/InvalidArgumentException.h"

namespace storm {
    namespace jani {
        
        Edge::Edge(uint64_t sourceLocationIndex, uint64_t actionIndex, boost::optional<storm::expressions::Expression> const& rate, std::shared_ptr<TemplateEdge const> const& templateEdge, std::vector<std::pair<uint64_t, storm::expressions::Expression>> const& destinationTargetLocationsAndProbabilities) : sourceLocationIndex(sourceLocationIndex), actionIndex(actionIndex), rate(rate), templateEdge(templateEdge) {
            
            // Create the concrete destinations from the template edge.
            STORM_LOG_THROW(templateEdge->getNumberOfDestinations() == destinationTargetLocationsAndProbabilities.size(), storm::exceptions::InvalidArgumentException, "Sizes of template edge destinations and target locations mismatch.");
            for (uint64_t i = 0; i < templateEdge->getNumberOfDestinations(); ++i) {
                auto const& templateDestination = templateEdge->getDestination(i);
                destinations.emplace_back(destinationTargetLocationsAndProbabilities[i].first, destinationTargetLocationsAndProbabilities[i].second, templateDestination);
            }
        }
        
        Edge::Edge(uint64_t sourceLocationIndex, uint64_t actionIndex, boost::optional<storm::expressions::Expression> const& rate, std::shared_ptr<TemplateEdge const> const& templateEdge, std::vector<uint64_t> const& destinationLocations, std::vector<storm::expressions::Expression> const& destinationProbabilities) : sourceLocationIndex(sourceLocationIndex), actionIndex(actionIndex), rate(rate), templateEdge(templateEdge) {

            // Create the concrete destinations from the template edge.
            STORM_LOG_THROW(templateEdge->getNumberOfDestinations() == destinationLocations.size() && destinationLocations.size() == destinationProbabilities.size(), storm::exceptions::InvalidArgumentException, "Sizes of template edge destinations and target locations mismatch.");
            for (uint64_t i = 0; i < templateEdge->getNumberOfDestinations(); ++i) {
                auto const& templateDestination = templateEdge->getDestination(i);
                destinations.emplace_back(destinationLocations[i], destinationProbabilities[i], templateDestination);
            }
        }

        uint64_t Edge::getSourceLocationIndex() const {
            return sourceLocationIndex;
        }
        
        uint64_t Edge::getActionIndex() const {
            return actionIndex;
        }
        
        bool Edge::hasRate() const {
            return static_cast<bool>(rate);
        }
        
        storm::expressions::Expression const& Edge::getRate() const {
            return rate.get();
        }
        
        boost::optional<storm::expressions::Expression> const& Edge::getOptionalRate() const {
            return rate;
        }
        
        void Edge::setRate(storm::expressions::Expression const& rate) {
            this->rate = rate;
        }
        
        storm::expressions::Expression const& Edge::getGuard() const {
            return templateEdge->getGuard();
        }
        
        EdgeDestination const& Edge::getDestination(uint64_t index) const {
            return destinations[index];
        }
        
        std::vector<EdgeDestination> const& Edge::getDestinations() const {
            return destinations;
        }
        
        std::size_t Edge::getNumberOfDestinations() const {
            return destinations.size();
        }
        
        OrderedAssignments const& Edge::getAssignments() const {
            return templateEdge->getAssignments();
        }
        
        void Edge::substitute(std::map<storm::expressions::Variable, storm::expressions::Expression> const& substitution) {
            if (this->hasRate()) {
                this->setRate(this->getRate().substitute(substitution));
            }
            for (auto& destination : destinations) {
                destination.substitute(substitution);
            }
        }
                
        bool Edge::hasSilentAction() const {
            return actionIndex == Model::SILENT_ACTION_INDEX;
        }
        
        boost::container::flat_set<storm::expressions::Variable> const& Edge::getWrittenGlobalVariables() const {
            return templateEdge->getWrittenGlobalVariables();
        }
        
        bool Edge::usesVariablesInNonTransientAssignments(std::set<storm::expressions::Variable> const& variables) const {
            return templateEdge->usesVariablesInNonTransientAssignments(variables);
        }
        
        bool Edge::hasTransientEdgeDestinationAssignments() const {
            return templateEdge->hasTransientEdgeDestinationAssignments();
        }
        
        bool Edge::usesAssignmentLevels() const {
            return templateEdge->usesAssignmentLevels();
        }
    }
}
