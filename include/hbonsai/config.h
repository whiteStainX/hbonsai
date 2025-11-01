#ifndef HBONSAI_CONFIG_H
#define HBONSAI_CONFIG_H

#include <array>
#include <ostream>
#include <string>
#include <vector>

namespace hbonsai {

struct AppConfig {
    bool live = false;
    bool infinite = false;
    bool screensaver = false;
    bool printTree = false;
    int verbosity = 0;
    float timeStep = 0.03f;
};

struct BonsaiConfig {
    int lifeStart = 32;
    int multiplier = 5;
    int baseType = 1;
    int seed = 0;
    int targetBranchCount = 0;
    std::string message;
    std::vector<std::string> leaves = {"&"};
    std::array<int, 4> colors = {2, 3, 10, 11};
    bool save = false;
    bool load = false;
    std::string saveFile;
    std::string loadFile;
};

struct TitleConfig {
    std::string text = "hbonsai";
    double displaySeconds = 4.0;
};

struct Config {
    AppConfig app;
    BonsaiConfig bonsai;
    TitleConfig title;

    bool showHelp = false;
    bool exitRequested = false;
    int exitCode = 0;
};

Config parse_args(int argc, char* argv[]);
void print_help(std::ostream& os);

} // namespace hbonsai

#endif // HBONSAI_CONFIG_H
