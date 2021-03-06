#include "storm/settings/modules/MultiObjectiveSettings.h"

#include "storm/settings/Option.h"
#include "storm/settings/OptionBuilder.h"
#include "storm/settings/ArgumentBuilder.h"
#include "storm/settings/Argument.h"
#include "storm/settings/ArgumentValidators.h"


namespace storm {
    namespace settings {
        namespace modules {
            
            const std::string MultiObjectiveSettings::moduleName = "multiobjective";
            const std::string MultiObjectiveSettings::exportPlotOptionName = "exportplot";
            const std::string MultiObjectiveSettings::precisionOptionName = "precision";
            const std::string MultiObjectiveSettings::maxStepsOptionName = "maxsteps";
            
            MultiObjectiveSettings::MultiObjectiveSettings() : ModuleSettings(moduleName) {
                this->addOption(storm::settings::OptionBuilder(moduleName, exportPlotOptionName, true, "Saves data for plotting of pareto curves and achievable values.")
                                .addArgument(storm::settings::ArgumentBuilder::createStringArgument("directory", "A path to a directory in which the results will be saved.").build()).build());
                this->addOption(storm::settings::OptionBuilder(moduleName, precisionOptionName, true, "The precision used for the approximation of numerical- and pareto queries.")
                                .addArgument(storm::settings::ArgumentBuilder::createDoubleArgument("value", "The precision.").setDefaultValueDouble(1e-04).addValidatorDouble(ArgumentValidatorFactory::createDoubleRangeValidatorExcluding(0.0, 1.0)).build()).build());
                this->addOption(storm::settings::OptionBuilder(moduleName, maxStepsOptionName, true, "Aborts the computation after the given number of refinement steps (= computed pareto optimal points).")
                                .addArgument(storm::settings::ArgumentBuilder::createUnsignedIntegerArgument("value", "the threshold for the number of refinement steps to be performed.").build()).build());
            }
            
            bool MultiObjectiveSettings::isExportPlotSet() const {
                return this->getOption(exportPlotOptionName).getHasOptionBeenSet();
            }
            
            std::string MultiObjectiveSettings::getExportPlotDirectory() const {
                return this->getOption(exportPlotOptionName).getArgumentByName("directory").getValueAsString();
            }
            
            double MultiObjectiveSettings::getPrecision() const {
                return this->getOption(precisionOptionName).getArgumentByName("value").getValueAsDouble();
            }
            
            bool MultiObjectiveSettings::isMaxStepsSet() const {
                return this->getOption(maxStepsOptionName).getHasOptionBeenSet();
            }
            
            uint_fast64_t MultiObjectiveSettings::getMaxSteps() const {
                return this->getOption(maxStepsOptionName).getArgumentByName("value").getValueAsUnsignedInteger();
            }
            
            bool MultiObjectiveSettings::check() const {
                std::shared_ptr<storm::settings::ArgumentValidator<std::string>> validator = ArgumentValidatorFactory::createWritableFileValidator();
                
                return !isExportPlotSet()
                    || (validator->isValid(getExportPlotDirectory() + "boundaries.csv")
                        && validator->isValid(getExportPlotDirectory() + "overapproximation.csv")
                        && validator->isValid(getExportPlotDirectory() + "underapproximation.csv")
                        && validator->isValid(getExportPlotDirectory() + "paretopoints.csv"));
            }

        } // namespace modules
    } // namespace settings
} // namespace storm
