/* 
 * File:   RegionBoundary.h
 * Author: Tim Quatmann
 *
 * Created on October 29, 2015, 2:57 PM
 */

#include "storm/modelchecker/region/RegionBoundary.h"
#include "storm/utility/macros.h"
#include "storm/exceptions/NotImplementedException.h"

namespace storm {
    namespace modelchecker {
        namespace region {
            std::ostream& operator<<(std::ostream& os, RegionBoundary const& regionBoundary) {
                switch (regionBoundary) {
                    case RegionBoundary::LOWER:
                        os << "LowerBoundary";
                        break;
                    case RegionBoundary::UPPER:
                        os << "UpperBoundary";
                        break;
                    case RegionBoundary::UNSPECIFIED:
                        os << "UnspecifiedBoundary";
                        break;
                    default:
                        STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Could not get a string from the region boundary. The case has not been implemented");
                }
                return os;
            }
        }
    }
}
