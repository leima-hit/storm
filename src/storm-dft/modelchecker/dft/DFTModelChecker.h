#pragma  once

#include "storm/logic/Formula.h"
#include "storm/modelchecker/results/CheckResult.h"
#include "storm/utility/storm.h" // TODO this should not be included here.
#include "storm/utility/Stopwatch.h"

#include "storm-dft/storage/dft/DFT.h"


namespace storm {
    namespace modelchecker {

        /*!
         * Analyser for DFTs.
         */
        template<typename ValueType>
        class DFTModelChecker {

            typedef std::pair<ValueType, ValueType> approximation_result;
            typedef boost::variant<ValueType, approximation_result> dft_result;

        public:

            /*!
             * Constructor.
             */
            DFTModelChecker();

            /*!
             * Main method for checking DFTs.
             *
             * @param origDft             Original DFT
             * @param formula             Formula to check for
             * @param symred              Flag indicating if symmetry reduction should be used
             * @param allowModularisation Flag indication if modularisation is allowed
             * @param enableDC            Flag indicating if dont care propagation should be used
             * @param approximationError  Error allowed for approximation. Value 0 indicates no approximation
             */
            void check(storm::storage::DFT<ValueType> const& origDft, std::shared_ptr<const storm::logic::Formula> const& formula, bool symred = true, bool allowModularisation = true, bool enableDC = true, double approximationError = 0.0);

            /*!
             * Print timings of all operations to stream.
             *
             * @param os Output stream to write to.
             */
            void printTimings(std::ostream& os = std::cout);

            /*!
             * Print result to stream.
             *
             * @param os Output stream to write to.
             */
            void printResult(std::ostream& os = std::cout);

        private:

            // Timing values
            storm::utility::Stopwatch buildingTimer;
            storm::utility::Stopwatch explorationTimer;
            storm::utility::Stopwatch bisimulationTimer;
            storm::utility::Stopwatch modelCheckingTimer;
            storm::utility::Stopwatch totalTimer;

            // Model checking result
            dft_result checkResult;

            // Allowed error bound for approximation
            double approximationError;

            /*!
             * Internal helper for model checking a DFT.
             *
             * @param dft                 DFT
             * @param formula             Formula to check for
             * @param symred              Flag indicating if symmetry reduction should be used
             * @param allowModularisation Flag indication if modularisation is allowed
             * @param enableDC            Flag indicating if dont care propagation should be used
             * @param approximationError  Error allowed for approximation. Value 0 indicates no approximation
             *
             * @return Model checking result (or in case of approximation two results for lower and upper bound)
             */
            dft_result checkHelper(storm::storage::DFT<ValueType> const& dft, std::shared_ptr<const storm::logic::Formula> const& formula, bool symred, bool allowModularisation, bool enableDC, double approximationError);

            /*!
             * Internal helper for building a CTMC from a DFT via parallel composition.
             *
             * @param dft                 DFT
             * @param formula             Formula to check for
             * @param symred              Flag indicating if symmetry reduction should be used
             * @param allowModularisation Flag indication if modularisation is allowed
             * @param enableDC            Flag indicating if dont care propagation should be used
             * @param approximationError  Error allowed for approximation. Value 0 indicates no approximation
             *
             * @return CTMC representing the DFT
             */
            std::shared_ptr<storm::models::sparse::Ctmc<ValueType>> buildModelComposition(storm::storage::DFT<ValueType> const& dft, std::shared_ptr<const storm::logic::Formula> const& formula, bool symred, bool allowModularisation, bool enableDC);

            /*!
             * Check model generated from DFT.
             *
             * @param dft                The DFT
             * @param formula            Formula to check for
             * @param symred             Flag indicating if symmetry reduction should be used
             * @param enableDC           Flag indicating if dont care propagation should be used
             * @param approximationError Error allowed for approximation. Value 0 indicates no approximation
             *
             * @return Model checking result
             */
            dft_result checkDFT(storm::storage::DFT<ValueType> const& dft, std::shared_ptr<const storm::logic::Formula> const& formula, bool symred, bool enableDC, double approximationError = 0.0);

            /*!
             * Check the given markov model for the given property.
             *
             * @param model   Model to check
             * @param formula Formula to check for
             *
             * @return Model checking result
             */
            std::unique_ptr<storm::modelchecker::CheckResult> checkModel(std::shared_ptr<storm::models::sparse::Model<ValueType>>& model, std::shared_ptr<const storm::logic::Formula> const& formula);

            /*!
             * Checks if the computed approximation is sufficient, i.e.
             * upperBound - lowerBound <= approximationError * mean(lowerBound, upperBound).
             *
             * @param lowerBound         The lower bound on the result.
             * @param upperBound         The upper bound on the result.
             * @param approximationError The allowed error for approximating.
             * @param relative           Flag indicating if the error should be relative to 1 or
                                         to the mean of lower and upper bound.
             *
             * @return True, if the approximation is sufficient.
             */
            bool isApproximationSufficient(ValueType lowerBound, ValueType upperBound, double approximationError, bool relative);

        };
    }
}
