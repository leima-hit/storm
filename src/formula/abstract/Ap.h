/*
 * Ap.h
 *
 *  Created on: 19.10.2012
 *      Author: Thomas Heinemann
 */

#ifndef STORM_FORMULA_ABSTRACT_AP_H_
#define STORM_FORMULA_ABSTRACT_AP_H_

#include "src/formula/abstract/AbstractFormula.h"
#include "src/formula/AbstractFormulaChecker.h"

namespace storm {
namespace property {
namespace abstract {

/*!
 * @brief
 * Logic-abstract Class for an abstract formula tree with atomic proposition as root.
 *
 * This class represents the leaves in the formula tree.
 *
 * @see AbstractFormula
 */
template <class T>
class Ap : public virtual AbstractFormula<T> {

public:
	/*!
	 * Constructor
	 *
	 * Creates a new atomic proposition leaf, with the label Ap
	 *
	 * @param ap The string representing the atomic proposition
	 */
	Ap(std::string ap) {
		this->ap = ap;
	}

	/*!
	 * Destructor.
	 * At this time, empty...
	 */
	virtual ~Ap() { }

	/*!
	 * @returns the name of the atomic proposition
	 */
	const std::string& getAp() const {
		return ap;
	}

	/*!
	 * @returns a string representation of the leaf.
	 *
	 */
	virtual std::string toString() const {
		return getAp();
	}
	
	/*!
     *  @brief Checks if all subtrees conform to some logic.
     *	
     *	As atomic propositions have no subformulas, we return true here.
     * 
     *  @param checker Formula checker object.
     *  @return true
     */
	virtual bool validate(const AbstractFormulaChecker<T>& checker) const {
		return true;
	}

private:
	std::string ap;
};

} //namespace abstract

} //namespace property

} //namespace storm

#endif /* STORM_FORMULA_ABSTRACT_ABSTRCT_AP_H_ */