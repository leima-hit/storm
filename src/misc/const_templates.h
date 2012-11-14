/*
 * const_templates.h
 *
 *  Created on: 11.10.2012
 *      Author: Thomas Heinemann
 */

#ifndef CONST_TEMPLATES_H_
#define CONST_TEMPLATES_H_

namespace mrmc {

namespace misc {

/*!
 * Returns a constant value of 0 that is fit to the type it is being written to.
 * As (at least) gcc has problems to use the correct template by the return value
 * only, the function gets a pointer as a parameter to infer the return type.
 *
 * <b>Parameter</b>
 *
 * The parameter is a pointer which is used to infer the return type (So, if you want
 * the return value to be of type double, the parameter has to be of type double*).
 * In most cases, it is a good choice to use the address of the variable that is to be
 * set.
 */
template<typename _Scalar>
static inline _Scalar constGetZero(_Scalar*) {
   return _Scalar(0);
}

/*! @cond TEMPLATE_SPECIALIZATION
 * (exclude the specializations from the documentation) */
template <>
inline int_fast32_t constGetZero(int_fast32_t*) {
   return 0;
}

/*! @internal
 * Specialization of constGetZero for int_fast32_t
 */
template <>
inline double constGetZero(double*) {
   return 0.0;
}
/*! @endcond */

/*!
 * Returns a constant value of 0 that is fit to the type it is being written to.
 * As (at least) gcc has problems to use the correct template by the return value
 * only, the function gets a pointer as a parameter to infer the return type.
 *
 * <b>Parameter</b>
 *
 * The parameter is a pointer which is used to infer the return type (So, if you want
 * the return value to be of type double, the parameter has to be of type double*).
 * In most cases, it is a good choice to use the address of the variable that is to be
 * set.
 */
template<typename _Scalar>
static inline _Scalar constGetOne(_Scalar*) {
   return _Scalar(1);
}

/*! @cond TEMPLATE_SPECIALIZATION
 * (exclude the specializations from the documentation) */
template<>
inline int_fast32_t constGetOne(int_fast32_t*) {
   return 1;
}

template<>
inline double constGetOne(double*) {
   return 1.0;
}
/*! @endcond */

} //namespace misc

} //namespace mrmc


#endif /* CONST_TEMPLATES_H_ */