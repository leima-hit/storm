/*
 * InstantaneousReward.h
 *
 *  Created on: 26.12.2012
 *      Author: Christian Dehnert
 */

#ifndef STORM_FORMULA_INSTANTANEOUSREWARD_H_
#define STORM_FORMULA_INSTANTANEOUSREWARD_H_

#include "PctlPathFormula.h"
#include "PctlStateFormula.h"
#include "boost/integer/integer_mask.hpp"
#include <string>

namespace storm {

namespace formula {

/*!
 * @brief
 * Class for a PCTL (path) formula tree with a Instantaneous Reward node as root.
 *
 * The subtrees are seen as part of the object and deleted with the object
 * (this behavior can be prevented by setting them to NULL before deletion)
 *
 * @see PctlPathFormula
 * @see PctlFormula
 */
template <class T>
class InstantaneousReward : public PctlPathFormula<T> {

public:
	/*!
	 * Empty constructor
	 */
	InstantaneousReward() {
		bound = 0;
	}

	/*!
	 * Constructor
	 *
	 * @param bound The time instance of the reward formula
	 */
	InstantaneousReward(uint_fast64_t bound) {
		this->bound = bound;
	}

	/*!
	 * Empty destructor.
	 */
	virtual ~InstantaneousReward() {
		// Intentionally left empty.
	}

	/*!
	 * @returns the time instance for the instantaneous reward operator
	 */
	uint_fast64_t getBound() const {
		return bound;
	}

	/*!
	 * Sets the the time instance for the instantaneous reward operator
	 *
	 * @param bound the new bound.
	 */
	void setBound(uint_fast64_t bound) {
		this->bound = bound;
	}

	/*!
	 * @returns a string representation of the formula
	 */
	virtual std::string toString() const {
		std::string result = "I=";
		result += std::to_string(bound);
		return result;
	}

	/*!
	 * Clones the called object.
	 *
	 * Performs a "deep copy", i.e. the subtrees of the new object are clones of the original ones
	 *
	 * @returns a new BoundedUntil-object that is identical the called object.
	 */
	virtual PctlPathFormula<T>* clone() const {
		return new InstantaneousReward(bound);
	}


	/*!
	 * Calls the model checker to check this formula.
	 * Needed to infer the correct type of formula class.
	 *
	 * @note This function should only be called in a generic check function of a model checker class. For other uses,
	 *       the methods of the model checker should be used.
	 *
	 * @returns A vector indicating the probability that the formula holds for each state.
	 */
	virtual std::vector<T> *check(const storm::modelChecker::DtmcPrctlModelChecker<T>& modelChecker) const {
	  return modelChecker.checkInstantaneousReward(*this);
	}

private:
	uint_fast64_t bound;
};

} //namespace formula

} //namespace storm

#endif /* STORM_FORMULA_INSTANTANEOUSREWARD_H_ */