#ifndef STORM_UTILITY_CONSTANTS_H_
#define STORM_UTILITY_CONSTANTS_H_

#ifdef max
#	undef max
#endif

#ifdef min
#	undef min
#endif

#include <limits>
#include <cstdint>
#include <string>

#include <vector>
#include <map>

namespace storm {

    // Forward-declare MatrixEntry class.
    namespace storage {
        template<typename IndexType, typename ValueType> class MatrixEntry;
    }

    namespace utility {

        template<typename ValueType>
        ValueType one();

        template<typename ValueType>
        ValueType zero();

        template<typename ValueType>
        ValueType infinity();

        template<typename ValueType>
        bool isOne(ValueType const& a);

        template<typename ValueType>
        bool isZero(ValueType const& a);

        template<typename ValueType>
        bool isConstant(ValueType const& a);

        template<typename ValueType>
        bool isInfinity(ValueType const& a);

        template<typename ValueType>
        ValueType pow(ValueType const& value, uint_fast64_t exponent);

        template<typename ValueType>
        ValueType simplify(ValueType value);

        template<typename ValueType>
        std::pair<ValueType, ValueType> minmax(std::vector<ValueType> const& values);

        template<typename ValueType>
        ValueType minimum(std::vector<ValueType> const& values);
        
        template<typename ValueType>
        ValueType maximum(std::vector<ValueType> const& values);

        template< typename K, typename ValueType>
        std::pair<ValueType, ValueType> minmax(std::map<K, ValueType> const& values);

        template< typename K, typename ValueType>
        ValueType minimum(std::map<K, ValueType> const& values);
        
        template<typename K, typename ValueType>
        ValueType maximum(std::map<K, ValueType> const& values);
        
        
        template<typename IndexType, typename ValueType>
        storm::storage::MatrixEntry<IndexType, ValueType>& simplify(storm::storage::MatrixEntry<IndexType, ValueType>& matrixEntry);

        template<typename IndexType, typename ValueType>
        storm::storage::MatrixEntry<IndexType, ValueType>&& simplify(storm::storage::MatrixEntry<IndexType, ValueType>&& matrixEntry);

        template<typename TargetType, typename SourceType>
        TargetType convertNumber(SourceType const& number);

        template<typename ValueType>
        ValueType sqrt(ValueType const& number);
        
        template<typename ValueType>
        ValueType abs(ValueType const& number);

        template<typename ValueType>
        ValueType floor(ValueType const& number);

        template<typename ValueType>
        ValueType ceil(ValueType const& number);

        template<typename ValueType>
        bool isInteger(ValueType const& number);
        
        template<typename ValueType>
        std::string to_string(ValueType const& value);
    }
}

#endif /* STORM_UTILITY_CONSTANTS_H_ */
