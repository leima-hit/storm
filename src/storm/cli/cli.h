#ifndef STORM_UTILITY_CLI_H_
#define STORM_UTILITY_CLI_H_

#include <string>

namespace storm {
    namespace cli {
            
        std::string getCurrentWorkingDirectory();
            
        void printHeader(std::string const& name, const int argc, const char* argv[]);
        
        void printVersion(std::string const& name);
            
        void showTimeAndMemoryStatistics(uint64_t wallclockMilliseconds = 0);
        
        /*!
         * Parses the given command line arguments.
         *
         * @param argc The argc argument of main().
         * @param argv The argv argument of main().
         * @return True iff the program should continue to run after parsing the options.
         */
        bool parseOptions(const int argc, const char* argv[]);
            
        void processOptions();
    }
}

#endif
