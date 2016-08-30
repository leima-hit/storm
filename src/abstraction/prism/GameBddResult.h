#pragma once

#include "src/storage/dd/Bdd.h"

namespace storm {
    namespace abstraction {
        namespace prism {
            
            template <storm::dd::DdType DdType>
            struct GameBddResult {
                GameBddResult();
                GameBddResult(storm::dd::Bdd<DdType> const& gameBdd, uint_fast64_t numberOfPlayer2Variables);
                
                storm::dd::Bdd<DdType> bdd;
                uint_fast64_t numberOfPlayer2Variables;
            };
            
        }
    }
}