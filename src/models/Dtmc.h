/*
 * dtmc.h
 *
 *  Created on: 14.11.2012
 *      Author: Christian Dehnert
 */

#ifndef DTMC_H_
#define DTMC_H_

#include <ostream>

#include "AtomicPropositionsLabeling.h"
#include "GraphTransitions.h"
#include "src/storage/SquareSparseMatrix.h"

namespace mrmc {

namespace models {

/*!
 * This class represents a discrete-time Markov chain (DTMC) whose states are
 * labeled with atomic propositions.
 */
template <class T>
class Dtmc {

public:
	//! Constructor
	/*!
	 * Constructs a DTMC object from the given transition probability matrix and
	 * the given labeling of the states.
	 * @param probabilityMatrix The transition probability function of the
	 * DTMC given by a matrix.
	 * @param stateLabeling The labeling that assigns a set of atomic
	 * propositions to each state.
	 */
	Dtmc(mrmc::storage::SquareSparseMatrix<T>* probabilityMatrix, mrmc::models::AtomicPropositionsLabeling* stateLabeling)
			: backwardTransitions(probabilityMatrix, false) {
		this->probabilityMatrix = probabilityMatrix;
		this->stateLabeling = stateLabeling;
	}

	//! Copy Constructor
	/*!
	 * Copy Constructor. Performs a deep copy of the given DTMC.
	 * @param dtmc A reference to the DTMC that is to be copied.
	 */
	Dtmc(const Dtmc<T> &dtmc) : probabilityMatrix(dtmc.probabilityMatrix),
			stateLabeling(dtmc.stateLabeling) { }

	//! Destructor
	/*!
	 * Destructor. Frees the matrix and labeling associated with this DTMC.
	 */
	~Dtmc() {
		if (this->probabilityMatrix != nullptr) {
			delete this->probabilityMatrix;
		}
		if (this->stateLabeling != nullptr) {
			delete this->stateLabeling;
		}
	}

	/*!
	 * Returns the state space size of the DTMC.
	 * @return The size of the state space of the DTMC.
	 */
	uint_fast64_t getStateSpaceSize() {
		return this->probabilityMatrix->getRowCount();
	}

	/*!
	 * Returns the number of (non-zero) transitions of the DTMC.
	 * @return The number of (non-zero) transitions of the DTMC.
	 */
	uint_fast64_t getNumberOfTransitions() {
		return this->probabilityMatrix->getNonZeroEntryCount();
	}

	/*!
	 * Returns a pointer to the matrix representing the transition probability
	 * function.
	 * @return A pointer to the matrix representing the transition probability
	 * function.
	 */
	mrmc::storage::SquareSparseMatrix<T>* getTransitionProbabilityMatrix() {
		return this->probabilityMatrix;
	}

	/*!
	 * Prints information about the model to the specified stream.
	 * @param out The stream the information is to be printed to.
	 */
	void printModelInformationToStream(std::ostream& out) {
		out << "-------------------------------------------------------------- "
			<< std::endl;
		out << "Model type: \t\tDTMC" << std::endl;
		out << "States: \t\t" << this->getStateSpaceSize() << std::endl;
		out << "Transitions: \t\t" << this->getNumberOfTransitions() << std::endl;
		this->stateLabeling->printAtomicPropositionsInformationToStream(out);
		out << "Size in memory: \t"
			<< (this->probabilityMatrix->getSizeInMemory() +
				this->stateLabeling->getSizeInMemory() +
				sizeof(*this))/1024 << " kbytes" << std::endl;
		out << "-------------------------------------------------------------- "
			<< std::endl;
	}

private:

	/*! A matrix representing the transition probability function of the DTMC. */
	mrmc::storage::SquareSparseMatrix<T>* probabilityMatrix;

	/*! The labeling of the states of the DTMC. */
	mrmc::models::AtomicPropositionsLabeling* stateLabeling;

	/*!
	 * A data structure that stores the predecessors for all states. This is
	 * needed for backwards directed searches.
	 */
	mrmc::models::GraphTransitions<T> backwardTransitions;
};

} // namespace models

} // namespace mrmc

#endif /* DTMC_H_ */