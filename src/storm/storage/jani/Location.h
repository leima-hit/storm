#pragma once

#include <string>

#include "src/storage/jani/OrderedAssignments.h"

namespace storm {
    namespace jani {
        
        /**
         * Jani Location:
         * 
         * Whereas Jani Locations also support invariants, we do not have support for them (as we do not support any of the allowed model types).
         */
        class Location {
        public:
            /*!
             * Creates a new location.
             */
            Location(std::string const& name, std::vector<Assignment> const& transientAssignments = {});
            
            /*!
             * Retrieves the name of the location.
             */
            std::string const& getName() const;
            
            /*!
             * Retrieves the assignments of this location.
             */
            OrderedAssignments const& getAssignments() const;
            
            /*!
             * Adds the given transient assignment to this location.
             */
            void addTransientAssignment(storm::jani::Assignment const& assignment);
  
            /*!
             * Substitutes all variables in all expressions according to the given substitution.
             */
            void substitute(std::map<storm::expressions::Variable, storm::expressions::Expression> const& substitution);
            
            /*!
             * Checks whether the location is valid, that is, whether the assignments are indeed all transient assignments.
             */
            void checkValid() const;
            
        private:
            /// The name of the location.
            std::string name;
            
            /// The transient assignments made in this location.
            OrderedAssignments assignments;
        };
        
    }
}