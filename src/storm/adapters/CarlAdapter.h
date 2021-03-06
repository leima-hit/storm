#ifndef STORM_ADAPTERS_CARLADAPTER_H_
#define STORM_ADAPTERS_CARLADAPTER_H_

// Include config to know whether CARL is available or not.
#include "storm-config.h"

#include <boost/multiprecision/gmp.hpp>

#ifdef STORM_HAVE_CLN
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmismatched-tags"

#pragma GCC diagnostic push

#include <cln/cln.h>

#pragma GCC diagnostic pop
#pragma clang diagnostic pop

#endif

#ifdef STORM_HAVE_CARL

#include <carl/numbers/numbers.h>
#include <carl/core/MultivariatePolynomial.h>
#include <carl/core/RationalFunction.h>
#include <carl/core/VariablePool.h>
#include <carl/core/FactorizedPolynomial.h>
#include <carl/core/Relation.h>
#include <carl/core/SimpleConstraint.h>
#include <carl/util/stringparser.h>

namespace carl {
    // Define hash values for all polynomials and rational function.
    template<typename C, typename O, typename P>
    inline size_t hash_value(carl::MultivariatePolynomial<C,O,P> const& p) {
        std::hash<carl::MultivariatePolynomial<C,O,P>> h;
        return h(p);
    }
    
    template<typename Pol>
    inline size_t hash_value(carl::FactorizedPolynomial<Pol> const& p) {
        std::hash<FactorizedPolynomial<Pol>> h;
        return h(p);
    }

    template<typename Pol, bool AutoSimplify>
    inline size_t hash_value(carl::RationalFunction<Pol, AutoSimplify> const& f)  {
        std::hash<Pol> h;
        return h(f.nominator()) ^ h(f.denominator());
    }
    
    template<typename Number>
    inline size_t hash_value(carl::Interval<Number> const& i) {
        std::hash<Interval<Number>> h;
        return h(i);
    }

}

inline size_t hash_value(mpq_class const& q) {
    std::hash<mpq_class> h;
    return h(q);
}


#if defined STORM_HAVE_CLN && defined STORM_USE_CLN_NUMBERS
namespace cln {
    inline size_t hash_value(cl_RA const& n) {
        std::hash<cln::cl_RA> h;
        return h(n);
    }
}
#endif


#include "NumberAdapter.h"

namespace storm {
    typedef carl::Variable RationalFunctionVariable;
    typedef carl::MultivariatePolynomial<RationalNumber> RawPolynomial;
    typedef carl::FactorizedPolynomial<RawPolynomial> Polynomial;
	typedef carl::Cache<carl::PolynomialFactorizationPair<RawPolynomial>> RawPolynomialCache;
    typedef carl::Relation CompareRelation;
    
    typedef carl::RationalFunction<Polynomial, true> RationalFunction;
    typedef carl::Interval<double> Interval;
    template<typename T> using ArithConstraint = carl::SimpleConstraint<T>;
}

#endif

#endif /* STORM_ADAPTERS_CARLADAPTER_H_ */
