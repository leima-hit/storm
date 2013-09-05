#ifndef STORM_SETTINGS_ARGUMENT_H_
#define STORM_SETTINGS_ARGUMENT_H_

#include <iostream>
#include <sstream>
#include <string>
#include <list>
#include <utility>
#include <functional>
#include <unordered_map>
#include <vector>
#include <memory>

#include "src/settings/ArgumentBase.h"
#include "src/settings/ArgumentType.h"
#include "src/settings/ArgumentTypeInferationHelper.h"
#include "src/utility/StringHelper.h"
#include "src/exceptions/ArgumentUnificationException.h"
#include "src/exceptions/IllegalArgumentValueException.h"
#include "src/exceptions/IllegalFunctionCallException.h"

namespace storm {
	namespace settings {
		template<typename T>
		class Argument : public ArgumentBase {
		public:
			typedef std::function<bool (const T, std::string&)> userValidationFunction_t;

			/*
			T argumentValue;
			ArgumentType argumentType;

			std::vector<userValidationFunction_t> userValidationFunction;

			T defaultValue;
			bool hasDefaultValue;
			*/

			Argument(std::string const& argumentName, std::string const& argumentDescription, std::vector<userValidationFunction_t> const& validationFunctions, bool isOptional): ArgumentBase(argumentName, argumentDescription, isOptional), argumentType(ArgumentTypeInferation::inferToEnumType<T>()), userValidationFunction(validationFunctions), hasDefaultValue(false) {
				if (isOptional) {
					throw storm::exceptions::IllegalArgumentException() << "Error: The Argument \"" << argumentName << "\" is flaged as optional but no default value was given!";
				}
			}

			Argument(std::string const& argumentName, std::string const& argumentDescription, std::vector<userValidationFunction_t> const& validationFunctions, bool isOptional, T defaultValue): ArgumentBase(argumentName, argumentDescription, isOptional), argumentType(ArgumentTypeInferation::inferToEnumType<T>()), userValidationFunction(validationFunctions), hasDefaultValue(true), defaultValue(defaultValue) {
			
			}

			Argument(Argument const& other) : ArgumentBase(other.argumentName, other.argumentDescription, other.isOptional), argumentType(other.argumentType), hasDefaultValue(other.hasDefaultValue), defaultValue(other.defaultValue) {
				this->userValidationFunction.reserve(other.userValidationFunction.size());
				for (auto i = 0; i < other.userValidationFunction.size(); ++i) {
					this->userValidationFunction.push_back(userValidationFunction_t(other.userValidationFunction.at(i)));
				}
			}

			virtual ~Argument() {
				std::cout << "Destructing an Argument: " << this->getArgumentName() << " of Type " << ArgumentTypeHelper::toString(this->getArgumentType()) << std::endl;

				this->userValidationFunction.clear();
				this->argumentType = ArgumentType::Invalid;
			}

			virtual ArgumentBase* clone() const override {
				return new Argument(*this);
			}

			assignmentResult_t fromStringValue(std::string const& fromStringValue) override {
				bool conversionOk = false;
				T newValue = this->convertFromString(fromStringValue, &conversionOk);
				if (!conversionOk) {
					std::string message("Could not convert the given String into ArgumentType Format (\"");
					message.append(ArgumentTypeHelper::toString(this->argumentType));
					message.append("\")!");
					return std::make_pair(false, message);
				}
				return this->fromTypeValue(newValue);
			}

			assignmentResult_t fromTypeValue(T const& newValue) {
				std::string errorText = "";
				if (!this->validateForEach(newValue, errorText)) {
					// The Conversion failed or a user defined Validation Function was given and it rejected the Input.
					return std::make_pair(false, errorText);
				}
				this->argumentValue = newValue;
				this->hasBeenSet = true;
				return std::make_pair(true, "Success");
			}

			virtual ArgumentType getArgumentType() const override {
				return this->argumentType;
			}

			template <typename S>
			void unify(Argument<S> &rhs) {
				if (this->getArgumentType() != rhs.getArgumentType()) {
					// LOG
					throw storm::exceptions::ArgumentUnificationException() << "Error while unifying Argument \"" << this->getArgumentName() << "\" and Argument \"" << rhs.getArgumentName() << "\": Type Missmatch: \"" << ArgumentTypeHelper::toString(this->getArgumentType()) << "\" against \"" << ArgumentTypeHelper::toString(rhs.getArgumentType()) << "\"";
				}

				if (this->getIsOptional() != rhs.getIsOptional()) {
					// LOG
					throw storm::exceptions::ArgumentUnificationException() << "Error while unifying Argument \"" << this->getArgumentName() << "\" and Argument \"" << rhs.getArgumentName() << "\": IsOptional Missmatch!";
				}

				if (this->getHasDefaultValue() != rhs.getHasDefaultValue()) {
					// LOG
					throw storm::exceptions::ArgumentUnificationException() << "Error while unifying Argument \"" << this->getArgumentName() << "\" and Argument \"" << rhs.getArgumentName() << "\": defaultValue Missmatch!";
				}

				if (this->getArgumentDescription().compare(rhs.getArgumentDescription()) != 0) {
					// LOG Warning: Descriptions of unified arguments do not match
				}

				if (this->getArgumentName().compare(rhs.getArgumentName()) != 0) {
					// LOG Warning: Names of unified arguments do not match
				}

				// Add Validation functions
				userValidationFunction.insert(userValidationFunction.end(), rhs.userValidationFunction.begin(), rhs.userValidationFunction.end());

				// Re-Add the Default Value to check for errors
				this->setDefaultValue(this->defaultValue);
			}

			T getArgumentValue() const {
				if (!this->getHasBeenSet()) {
					// LOG
					throw storm::exceptions::IllegalFunctionCallException() << "Error: Called getArgumentValue() on Argument \"" << this->getArgumentName() << "\", but it was never set and does not contain a default value.";
				}
				return this->argumentValue;
			}

			virtual bool getHasDefaultValue() const override {
				return this->hasDefaultValue;
			}

			std::string const& getDefaultValue() {
				return this->defaultValue;
			}

			void setFromDefaultValue() override {
				if (!this->hasDefaultValue) {
					throw storm::exceptions::IllegalFunctionCallException() << "Error: The Argument \"" << this->getArgumentName() << "\" (" << this->getArgumentDescription() << ") was asked to set its default value but none was set!";
				}
				// this call also sets the hasBeenSet flag
				assignmentResult_t result = this->fromTypeValue(this->defaultValue);
				if (!result.first) {
					throw storm::exceptions::IllegalArgumentValueException() << "Error: While parsing a given configuration the Argument \"" << this->getArgumentName() << "\" (" << this->getArgumentDescription() << ") could not receive its Default Value as it was rejected by its Validation Functions with message: " << result.second;
				}
			}

			virtual std::string getValueAsString() const override {
				switch (this->argumentType) {
					case ArgumentType::String:
						return ArgumentTypeInferation::inferToString(ArgumentType::String, this->argumentValue);
					case ArgumentType::Boolean: {
						bool iValue = ArgumentTypeInferation::inferToBoolean(ArgumentType::Boolean, this->argumentValue);
						if (iValue) {
							return "true";
						} else {
							return "false";
						}

					}
					default:
						return this->convertToString(this->argumentValue);

				}
			}
			
			virtual int_fast64_t getValueAsInteger() const override {
				switch (this->argumentType) {
					case ArgumentType::Integer:
						return ArgumentTypeInferation::inferToInteger(ArgumentType::Integer, this->argumentValue);
					default:
						throw storm::exceptions::IllegalFunctionCallException() << "Error: getValueAsInteger() was called on Argument \"" << getArgumentName() << "\" of Type \"" << ArgumentTypeHelper::toString(getArgumentType()) << "\"!";

				}
			}
			
			virtual uint_fast64_t getValueAsUnsignedInteger() const override {
				switch (this->argumentType) {
					case ArgumentType::UnsignedInteger:
						return ArgumentTypeInferation::inferToUnsignedInteger(ArgumentType::UnsignedInteger, this->argumentValue);
					default:
						throw storm::exceptions::IllegalFunctionCallException() << "Error: getValueAsUnsignedInteger() was called on Argument \"" << getArgumentName() << "\" of Type \"" << ArgumentTypeHelper::toString(getArgumentType()) << "\"!";

				}
			}
			
			virtual double getValueAsDouble() const override {
				switch (this->argumentType) {
					case ArgumentType::Double:
						return ArgumentTypeInferation::inferToDouble(ArgumentType::Double, this->argumentValue);
					default:
						throw storm::exceptions::IllegalFunctionCallException() << "Error: getValueAsDouble() was called on Argument \"" << getArgumentName() << "\" of Type \"" << ArgumentTypeHelper::toString(getArgumentType()) << "\"!";

				}
			}
			
			virtual bool getValueAsBoolean() const override {
				switch (this->argumentType) {
					case ArgumentType::Boolean:
						return ArgumentTypeInferation::inferToBoolean(ArgumentType::Boolean, this->argumentValue);
					default:
						throw storm::exceptions::IllegalFunctionCallException() << "Error: getValueAsBoolean() was called on Argument \"" << getArgumentName() << "\" of Type \"" << ArgumentTypeHelper::toString(getArgumentType()) << "\"!";

				}
			}
		private:
			T argumentValue;
			ArgumentType argumentType;

			std::vector<userValidationFunction_t> userValidationFunction;

			T defaultValue;
			bool hasDefaultValue;

			void setDefaultValue(T const& newDefault) {
				std::string errorText = "";
				if (!this->validateForEach(newDefault, errorText)) {
					// A user defined Validation Function was given and it rejected the Input.
					// LOG
					throw storm::exceptions::IllegalArgumentValueException() << "Illegal Default Value for Argument \"" << this->getArgumentName() << "\".\nThe Validation Function rejected the Value: " << errorText;
				} 
				this->defaultValue = newDefault;
				this->hasDefaultValue = true;
			}

			void unsetDefaultValue() {
				this->hasDefaultValue = false;
			}

			std::string convertToString(T const& t) const {
					std::ostringstream stream;
					stream << t;
					return stream.str();
			}
			
			T convertFromString(std::string const& s, bool* ok = nullptr) const {
				return storm::settings::ArgumentBase::ArgumentHelper::convertFromString<T>(s, ok);
			}

			bool validateForEach(T const& value, std::string& errorMessageTarget) const {
				bool result = true;

				for (auto lambda: this->userValidationFunction) {
					result = result && lambda(value, errorMessageTarget);
				}
				return result;
			}

			
		};
	}
}

#endif // STORM_SETTINGS_ARGUMENT_H_