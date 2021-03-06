#include "storm/modelchecker/results/CheckResult.h"

#include "storm-config.h"
#include "storm/adapters/CarlAdapter.h"

#include "storm/modelchecker/results/ExplicitQualitativeCheckResult.h"
#include "storm/modelchecker/results/ExplicitQuantitativeCheckResult.h"
#include "storm/modelchecker/results/SymbolicQualitativeCheckResult.h"
#include "storm/modelchecker/results/SymbolicQuantitativeCheckResult.h"
#include "storm/modelchecker/results/HybridQuantitativeCheckResult.h"
#include "storm/modelchecker/results/ParetoCurveCheckResult.h"

#include "storm/utility/macros.h"
#include "storm/exceptions/InvalidOperationException.h"

namespace storm {
    namespace modelchecker {        
        bool CheckResult::isExplicit() const {
            return false;
        }
        
        bool CheckResult::isSymbolic() const {
            return false;
        }
        
        bool CheckResult::isHybrid() const {
            return false;
        }
        
        bool CheckResult::isQuantitative() const {
            return false;
        }
        
        bool CheckResult::isQualitative() const {
            return false;
        }
        
        bool CheckResult::isResultForAllStates() const {
            return false;
        }
        
        std::ostream& operator<<(std::ostream& out, CheckResult const& checkResult) {
            checkResult.writeToStream(out);
            return out;
        }
        
        bool CheckResult::isExplicitQualitativeCheckResult() const {
            return false;
        }
        
        bool CheckResult::isExplicitQuantitativeCheckResult() const {
            return false;
        }
        
        bool CheckResult::isSymbolicQualitativeCheckResult() const {
            return false;
        }
        
        bool CheckResult::isSymbolicQuantitativeCheckResult() const {
            return false;
        }
        
        bool CheckResult::isHybridQuantitativeCheckResult() const {
            return false;
        }
        
        bool CheckResult::isParetoCurveCheckResult() const {
            return false;
        }
        
        ExplicitQualitativeCheckResult& CheckResult::asExplicitQualitativeCheckResult() {
            return dynamic_cast<ExplicitQualitativeCheckResult&>(*this);
        }
        
        ExplicitQualitativeCheckResult const& CheckResult::asExplicitQualitativeCheckResult() const {
            return dynamic_cast<ExplicitQualitativeCheckResult const&>(*this);
        }
        
        template<typename ValueType>
        ExplicitQuantitativeCheckResult<ValueType>& CheckResult::asExplicitQuantitativeCheckResult() {
            return dynamic_cast<ExplicitQuantitativeCheckResult<ValueType>&>(*this);
        }
        
        template<typename ValueType>
        ExplicitQuantitativeCheckResult<ValueType> const& CheckResult::asExplicitQuantitativeCheckResult() const {
            return dynamic_cast<ExplicitQuantitativeCheckResult<ValueType> const&>(*this);
        }
        
        QualitativeCheckResult& CheckResult::asQualitativeCheckResult() {
            return dynamic_cast<QualitativeCheckResult&>(*this);
        }
        
        QualitativeCheckResult const& CheckResult::asQualitativeCheckResult() const {
            return dynamic_cast<QualitativeCheckResult const&>(*this);
        }


        template <storm::dd::DdType Type>
        SymbolicQualitativeCheckResult<Type>& CheckResult::asSymbolicQualitativeCheckResult() {
            return dynamic_cast<SymbolicQualitativeCheckResult<Type>&>(*this);
        }
        
        template <storm::dd::DdType Type>
        SymbolicQualitativeCheckResult<Type> const& CheckResult::asSymbolicQualitativeCheckResult() const {
            return dynamic_cast<SymbolicQualitativeCheckResult<Type> const&>(*this);
        }
        
        template <storm::dd::DdType Type, typename ValueType>
        SymbolicQuantitativeCheckResult<Type, ValueType>& CheckResult::asSymbolicQuantitativeCheckResult() {
            return dynamic_cast<SymbolicQuantitativeCheckResult<Type, ValueType>&>(*this);
        }
        
        template <storm::dd::DdType Type, typename ValueType>
        SymbolicQuantitativeCheckResult<Type, ValueType> const& CheckResult::asSymbolicQuantitativeCheckResult() const {
            return dynamic_cast<SymbolicQuantitativeCheckResult<Type, ValueType> const&>(*this);
        }

        template <storm::dd::DdType Type, typename ValueType>
        HybridQuantitativeCheckResult<Type, ValueType>& CheckResult::asHybridQuantitativeCheckResult() {
            return dynamic_cast<HybridQuantitativeCheckResult<Type, ValueType>&>(*this);
        }
        
        template <storm::dd::DdType Type, typename ValueType>
        HybridQuantitativeCheckResult<Type, ValueType> const& CheckResult::asHybridQuantitativeCheckResult() const {
            return dynamic_cast<HybridQuantitativeCheckResult<Type, ValueType> const&>(*this);
        }
        
        template<typename ValueType>
        ParetoCurveCheckResult<ValueType>& CheckResult::asParetoCurveCheckResult() {
            return dynamic_cast<ParetoCurveCheckResult<ValueType>&>(*this);
        }
        
        template<typename ValueType>
        ParetoCurveCheckResult<ValueType> const& CheckResult::asParetoCurveCheckResult() const {
            return dynamic_cast<ParetoCurveCheckResult<ValueType> const&>(*this);
        }
        
        // Explicitly instantiate the template functions.
        template ExplicitQuantitativeCheckResult<double>& CheckResult::asExplicitQuantitativeCheckResult();
        template ExplicitQuantitativeCheckResult<double> const& CheckResult::asExplicitQuantitativeCheckResult() const;

        template SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>& CheckResult::asSymbolicQualitativeCheckResult();
        template SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD> const& CheckResult::asSymbolicQualitativeCheckResult() const;
        template SymbolicQuantitativeCheckResult<storm::dd::DdType::CUDD, double>& CheckResult::asSymbolicQuantitativeCheckResult();
        template SymbolicQuantitativeCheckResult<storm::dd::DdType::CUDD, double> const& CheckResult::asSymbolicQuantitativeCheckResult() const;
        template HybridQuantitativeCheckResult<storm::dd::DdType::CUDD, double>& CheckResult::asHybridQuantitativeCheckResult();
        template HybridQuantitativeCheckResult<storm::dd::DdType::CUDD, double> const& CheckResult::asHybridQuantitativeCheckResult() const;

        template SymbolicQualitativeCheckResult<storm::dd::DdType::Sylvan>& CheckResult::asSymbolicQualitativeCheckResult();
        template SymbolicQualitativeCheckResult<storm::dd::DdType::Sylvan> const& CheckResult::asSymbolicQualitativeCheckResult() const;
        template SymbolicQuantitativeCheckResult<storm::dd::DdType::Sylvan, double>& CheckResult::asSymbolicQuantitativeCheckResult();
        template SymbolicQuantitativeCheckResult<storm::dd::DdType::Sylvan, double> const& CheckResult::asSymbolicQuantitativeCheckResult() const;
        template HybridQuantitativeCheckResult<storm::dd::DdType::Sylvan, double>& CheckResult::asHybridQuantitativeCheckResult();
        template HybridQuantitativeCheckResult<storm::dd::DdType::Sylvan, double> const& CheckResult::asHybridQuantitativeCheckResult() const;
        
        template ParetoCurveCheckResult<double>& CheckResult::asParetoCurveCheckResult();
        template ParetoCurveCheckResult<double> const& CheckResult::asParetoCurveCheckResult() const;

#ifdef STORM_HAVE_CARL
        template ExplicitQuantitativeCheckResult<storm::RationalNumber>& CheckResult::asExplicitQuantitativeCheckResult();
        template ExplicitQuantitativeCheckResult<storm::RationalNumber> const& CheckResult::asExplicitQuantitativeCheckResult() const;

        template ExplicitQuantitativeCheckResult<storm::RationalFunction>& CheckResult::asExplicitQuantitativeCheckResult();
        template ExplicitQuantitativeCheckResult<storm::RationalFunction> const& CheckResult::asExplicitQuantitativeCheckResult() const;

        template ParetoCurveCheckResult<storm::RationalNumber>& CheckResult::asParetoCurveCheckResult();
        template ParetoCurveCheckResult<storm::RationalNumber> const& CheckResult::asParetoCurveCheckResult() const;
#endif
    }
}
