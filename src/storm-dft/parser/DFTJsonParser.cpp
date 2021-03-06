#include "DFTJsonParser.h"

#include <iostream>
#include <fstream>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/replace.hpp>
#include "storm/storage/expressions/ExpressionManager.h"
#include "storm/exceptions/NotImplementedException.h"
#include "storm/exceptions/FileIoException.h"
#include "storm/exceptions/NotSupportedException.h"
#include "storm/utility/macros.h"
#include "storm/utility/file.h"

namespace storm {
    namespace parser {

        template<typename ValueType>
        storm::storage::DFT<ValueType> DFTJsonParser<ValueType>::parseJson(const std::string& filename) {
            readFile(filename);
            storm::storage::DFT<ValueType> dft = builder.build();
            STORM_LOG_DEBUG("Elements:" << std::endl << dft.getElementsString());
            STORM_LOG_DEBUG("Spare Modules:" << std::endl << dft.getSpareModulesString());
            return dft;
        }

        template<typename ValueType>
        std::string DFTJsonParser<ValueType>::stripQuotsFromName(std::string const& name) {
            size_t firstQuots = name.find("\"");
            size_t secondQuots = name.find("\"", firstQuots+1);
            
            if(firstQuots == std::string::npos) {
                return name;
            } else {
                STORM_LOG_THROW(secondQuots != std::string::npos, storm::exceptions::FileIoException, "No ending quotation mark found in " << name);
                return name.substr(firstQuots+1,secondQuots-1);
            }
        }

        template<typename ValueType>
        std::string DFTJsonParser<ValueType>::getString(json const& structure, std::string const& errorInfo) {
            STORM_LOG_THROW(structure.is_string(), storm::exceptions::FileIoException, "Expected a string in " << errorInfo << ", got '" << structure.dump() << "'");
            return structure.front();
        }

        template<typename ValueType>
        std::string DFTJsonParser<ValueType>::parseNodeIdentifier(std::string const& name) {
            return boost::replace_all_copy(name, "'", "__prime__");
        }

        template<typename ValueType>
        void DFTJsonParser<ValueType>::readFile(const std::string& filename) {
            STORM_LOG_DEBUG("Parsing from JSON");

            std::ifstream file;
            storm::utility::openFile(filename, file);
            json parsedJson;
            parsedJson << file;
            storm::utility::closeFile(file);

            // Start by building mapping from ids to names
            std::map<std::string, std::string> nameMapping;
            for (auto& element: parsedJson) {
                if (element.at("classes") != "") {
                    json data = element.at("data");
                    std::string id = data.at("id");
                    std::string name = data.at("name");
                    nameMapping[id] = name;
                }
            }

            // TODO: avoid hack
            std::string toplevelId = nameMapping["1"];

            for (auto& element : parsedJson) {
                bool success = true;
                if (element.at("classes") == "") {
                    continue;
                }
                json data = element.at("data");
                std::string name = data.at("name");
                std::vector<std::string> childNames;
                if (data.count("children") > 0) {
                    for (auto& child : data.at("children")) {
                        childNames.push_back(nameMapping[child]);
                    }
                }

                std::string type = getString(element.at("classes"), "classes");

                if(type == "and") {
                    success = builder.addAndElement(name, childNames);
                } else if (type == "or") {
                    success = builder.addOrElement(name, childNames);
                } else if (type == "pand") {
                    success = builder.addPandElement(name, childNames);
                } else if (type == "por") {
                    success = builder.addPorElement(name, childNames);
                } else if (type == "spare") {
                    success = builder.addSpareElement(name, childNames);
                } else if (type == "seq") {
                    success = builder.addSequenceEnforcer(name, childNames);
                } else if (type== "fdep") {
                    success = builder.addDepElement(name, childNames, storm::utility::one<ValueType>());
                } else if (type== "pdep") {
                    ValueType probability = parseRationalExpression(data.at("prob"));
                    success = builder.addDepElement(name, childNames, probability);
                } else if (type == "be") {
                    ValueType failureRate = parseRationalExpression(data.at("rate"));
                    ValueType dormancyFactor = parseRationalExpression(data.at("dorm"));
                    success = builder.addBasicElement(name, failureRate, dormancyFactor);
                } else {
                    STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Type name: " << type << "  not recognized.");
                    success = false;
                }

                // Set layout positions
                json position = element.at("position");
                double x = position.at("x");
                double y = position.at("y");
                builder.addLayoutInfo(name, x / 7, y / 7);
                STORM_LOG_THROW(success, storm::exceptions::FileIoException, "Error while adding element '" << element << "'.");
            }

            if(!builder.setTopLevel(toplevelId)) {
                STORM_LOG_THROW(false, storm::exceptions::FileIoException, "Top level id unknown.");
            }
        }

        template<typename ValueType>
        ValueType DFTJsonParser<ValueType>::parseRationalExpression(std::string const& expr) {
            STORM_LOG_ASSERT(false, "Specialized method should be called.");
            return 0;
        }

        template<>
        double DFTJsonParser<double>::parseRationalExpression(std::string const& expr) {
            return boost::lexical_cast<double>(expr);
        }

        // Explicitly instantiate the class.
        template class DFTJsonParser<double>;

#ifdef STORM_HAVE_CARL
        template<>
        storm::RationalFunction DFTJsonParser<storm::RationalFunction>::parseRationalExpression(std::string const& expr) {
            STORM_LOG_TRACE("Translating expression: " << expr);
            storm::expressions::Expression expression = parser.parseFromString(expr);
            STORM_LOG_TRACE("Expression: " << expression);
            storm::RationalFunction rationalFunction = evaluator.asRational(expression);
            STORM_LOG_TRACE("Parsed expression: " << rationalFunction);
            return rationalFunction;
        }

        template class DFTJsonParser<RationalFunction>;
#endif
        
    }
}
