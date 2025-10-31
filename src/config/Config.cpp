#include "hbonsai/config.h"
#include <iostream>
#include <string>

// For a real implementation, consider using a dedicated library
// for command-line parsing, such as CLI11 or cxxopts.

namespace hbonsai {

Config parse_args(int argc, char* argv[]) {
    Config config;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-l" || arg == "--live") {
            config.live = true;
        } else if (arg == "-i" || arg == "--infinite") {
            config.infinite = true;
        } else if (arg == "-S" || arg == "--screensaver") {
            config.screensaver = true;
            config.live = true;
            config.infinite = true;
        } else if ((arg == "-m" || arg == "--message") && i + 1 < argc) {
            config.message = argv[++i];
        }
        // TODO: Add parsing for all the other arguments from ref.c
    }

    return config;
}

} // namespace hbonsai
