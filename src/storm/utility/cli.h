#pragma once

#include <map>

#include "storm/storage/expressions/ExpressionManager.h"
#include "storm/storage/expressions/Expression.h"

namespace storm {
    namespace utility {
        namespace cli {
        
            std::map<storm::expressions::Variable, storm::expressions::Expression> parseConstantDefinitionString(storm::expressions::ExpressionManager const& manager, std::string const& constantDefinitionString);
            
            std::vector<std::string> parseCommaSeparatedStrings(std::string const& input);
        }
    }
}
