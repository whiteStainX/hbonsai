#ifndef HBONSAI_CONFIG_H
#define HBONSAI_CONFIG_H

#include <array>
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
    double timeWait = 4.0;
    float timeStep = 0.03f;
    std::string message;
    std::vector<std::string> leaves = {"&"};
    std::array<int, 4> colors = {2, 3, 10, 11};
};

Config parse_args(int argc, char* argv[]);

} // namespace hbonsai

#endif // HBONSAI_CONFIG_H
