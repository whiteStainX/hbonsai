#ifndef HBONSAI_CONFIG_H
#define HBONSAI_CONFIG_H

#include <array>
#include <ostream>
#include <string>
#include <vector>

namespace hbonsai {

struct Config {
    bool live = false;
    bool infinite = false;
    bool screensaver = false;
    bool printTree = false;
    int verbosity = 0;
    int lifeStart = 32;
    int multiplier = 5;
    int baseType = 1;
    int seed = 0;
    int targetBranchCount = 0;
    double timeWait = 4.0;
    float timeStep = 0.03f;
    // Display
    std::string message;
    std::string title;
    std::vector<std::string> leaves = {"&"};
    
    std::array<int, 4> colors = {2, 3, 10, 11};
    bool save = false;
    bool load = false;
    std::string saveFile;
    std::string loadFile;

    bool showHelp = false;
    bool exitRequested = false;
    int exitCode = 0;
};

Config parse_args(int argc, char* argv[]);
void print_help(std::ostream& os);

} // namespace hbonsai

#endif // HBONSAI_CONFIG_H
