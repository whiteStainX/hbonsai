#include "hbonsai/config.h"

#include <algorithm>
#include <array>
#include <cstdlib>
#include <fstream>
#include <getopt.h>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace hbonsai {

namespace {

std::vector<std::string> split_list(const std::string& input) {
    std::vector<std::string> result;
    std::stringstream ss(input);
    std::string item;
    while (std::getline(ss, item, ',')) {
        if (!item.empty()) {
            result.push_back(item);
        }
    }
    return result;
}

bool parse_int(const std::string& value, int& out) {
    try {
        size_t idx = 0;
        int parsed = std::stoi(value, &idx, 10);
        if (idx != value.size()) {
            return false;
        }
        out = parsed;
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

bool parse_float(const std::string& value, float& out) {
    try {
        size_t idx = 0;
        float parsed = std::stof(value, &idx);
        if (idx != value.size()) {
            return false;
        }
        out = parsed;
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

bool parse_double(const std::string& value, double& out) {
    try {
        size_t idx = 0;
        double parsed = std::stod(value, &idx);
        if (idx != value.size()) {
            return false;
        }
        out = parsed;
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

std::string default_cache_path() {
    const char* xdg_cache = std::getenv("XDG_CACHE_HOME");
    if (xdg_cache && *xdg_cache != '\0') {
        return std::string(xdg_cache) + "/cbonsai";
    }

    const char* home = std::getenv("HOME");
    if (home && *home != '\0') {
        return std::string(home) + "/.cache/cbonsai";
    }

    return "cbonsai";
}

bool load_progress(Config& config) {
    std::ifstream file(config.loadFile);
    if (!file.is_open()) {
        std::cerr << "error: file was not opened properly for reading: " << config.loadFile << std::endl;
        return false;
    }

    int seed = 0;
    int target = 0;
    if (!(file >> seed >> target)) {
        std::cerr << "error: save file could not be read" << std::endl;
        return false;
    }

    config.seed = seed;
    config.targetBranchCount = target;
    return true;
}

void set_error(Config& config, int code, bool show_help) {
    config.exitRequested = true;
    config.exitCode = code;
    config.showHelp = show_help;
}

bool validate_color_indices(const std::vector<std::string>& parts, std::array<int, 4>& out) {
    if (parts.size() != out.size()) {
        std::cerr << "error: color list must contain exactly 4 values" << std::endl;
        return false;
    }

    for (size_t idx = 0; idx < parts.size(); ++idx) {
        int parsed = out[idx];
        if (!parse_int(parts[idx], parsed) || parsed < 0 || parsed >= 256) {
            std::cerr << "error: invalid color index: '" << parts[idx] << "'" << std::endl;
            return false;
        }
        out[idx] = parsed;
    }
    return true;
}

} // namespace

Config parse_args(int argc, char* argv[]) {
    Config config;
    config.saveFile = default_cache_path();
    config.loadFile = default_cache_path();

    std::string leavesInput = "&";
    std::string colorsInput = "2,3,10,11";

    opterr = 0;
    optind = 1;

    const option long_options[] = {
        {"live", no_argument, nullptr, 'l'},
        {"time", required_argument, nullptr, 't'},
        {"infinite", no_argument, nullptr, 'i'},
        {"wait", required_argument, nullptr, 'w'},
        {"screensaver", no_argument, nullptr, 'S'},
        {"message", required_argument, nullptr, 'm'},
        {"base", required_argument, nullptr, 'b'},
        {"leaf", required_argument, nullptr, 'c'},
        {"color", required_argument, nullptr, 'k'},
        {"colors", required_argument, nullptr, 'k'},
        {"multiplier", required_argument, nullptr, 'M'},
        {"life", required_argument, nullptr, 'L'},
        {"print", no_argument, nullptr, 'p'},
        {"seed", required_argument, nullptr, 's'},
        {"save", optional_argument, nullptr, 'W'},
        {"load", optional_argument, nullptr, 'C'},
        {"verbose", no_argument, nullptr, 'v'},
        {"help", no_argument, nullptr, 'h'},
        {nullptr, 0, nullptr, 0}
    };

    const char* short_opts = ":lt:iw:Sm:b:c:k:M:L:ps:W::C::vh";

    bool has_error = false;

    while (true) {
        int option_index = 0;
        int c = getopt_long(argc, argv, short_opts, long_options, &option_index);
        if (c == -1) {
            break;
        }

        switch (c) {
        case 'l':
            config.live = true;
            break;
        case 't': {
            if (!optarg) {
                std::cerr << "error: option requires an argument -- 't'" << std::endl;
                has_error = true;
                break;
            }
            float parsed = config.timeStep;
            if (parse_float(optarg, parsed) && parsed > 0.0f) {
                config.timeStep = parsed;
            } else {
                std::cerr << "error: invalid step time: '" << optarg << "'" << std::endl;
                has_error = true;
            }
            break;
        }
        case 'i':
            config.infinite = true;
            break;
        case 'w': {
            if (!optarg) {
                std::cerr << "error: option requires an argument -- 'w'" << std::endl;
                has_error = true;
                break;
            }
            double parsed = config.timeWait;
            if (parse_double(optarg, parsed) && parsed > 0.0) {
                config.timeWait = parsed;
            } else {
                std::cerr << "error: invalid wait time: '" << optarg << "'" << std::endl;
                has_error = true;
            }
            break;
        }
        case 'S':
            config.live = true;
            config.infinite = true;
            config.screensaver = true;
            config.save = true;
            config.load = true;
            break;
        case 'm':
            if (!optarg) {
                std::cerr << "error: option requires an argument -- 'm'" << std::endl;
                has_error = true;
            } else {
                config.message = optarg;
            }
            break;
        case 'b': {
            if (!optarg) {
                std::cerr << "error: option requires an argument -- 'b'" << std::endl;
                has_error = true;
                break;
            }
            int parsed = config.baseType;
            if (parse_int(optarg, parsed) && parsed >= 0) {
                config.baseType = parsed;
            } else {
                std::cerr << "error: invalid base index: '" << optarg << "'" << std::endl;
                has_error = true;
            }
            break;
        }
        case 'c':
            if (!optarg) {
                std::cerr << "error: option requires an argument -- 'c'" << std::endl;
                has_error = true;
            } else {
                leavesInput = optarg;
            }
            break;
        case 'k':
            if (!optarg) {
                std::cerr << "error: option requires an argument -- 'k'" << std::endl;
                has_error = true;
            } else {
                colorsInput = optarg;
            }
            break;
        case 'M': {
            if (!optarg) {
                std::cerr << "error: option requires an argument -- 'M'" << std::endl;
                has_error = true;
                break;
            }
            int parsed = config.multiplier;
            if (parse_int(optarg, parsed) && parsed > 0) {
                config.multiplier = parsed;
            } else {
                std::cerr << "error: invalid multiplier: '" << optarg << "'" << std::endl;
                has_error = true;
            }
            break;
        }
        case 'L': {
            if (!optarg) {
                std::cerr << "error: option requires an argument -- 'L'" << std::endl;
                has_error = true;
                break;
            }
            int parsed = config.lifeStart;
            if (parse_int(optarg, parsed) && parsed > 0) {
                config.lifeStart = parsed;
            } else {
                std::cerr << "error: invalid initial life: '" << optarg << "'" << std::endl;
                has_error = true;
            }
            break;
        }
        case 'p':
            config.printTree = true;
            break;
        case 's': {
            if (!optarg) {
                std::cerr << "error: option requires an argument -- 's'" << std::endl;
                has_error = true;
                break;
            }
            int parsed = config.seed;
            if (parse_int(optarg, parsed) && parsed > 0) {
                config.seed = parsed;
            } else {
                std::cerr << "error: invalid seed: '" << optarg << "'" << std::endl;
                has_error = true;
            }
            break;
        }
        case 'W':
            config.save = true;
            if (optarg) {
                config.saveFile = optarg;
            }
            break;
        case 'C':
            config.load = true;
            if (optarg) {
                config.loadFile = optarg;
            }
            break;
        case 'v':
            config.verbosity += 1;
            break;
        case 'h':
            config.showHelp = true;
            config.exitRequested = true;
            config.exitCode = 0;
            break;
        case ':':
            if (optopt == 'W') {
                config.save = true;
            } else if (optopt == 'C') {
                config.load = true;
            } else {
                std::cerr << "error: option requires an argument -- '" << static_cast<char>(optopt) << "'" << std::endl;
                has_error = true;
            }
            break;
        case '?':
        default: {
            const char* unknown = (optind > 0 && optind - 1 < argc) ? argv[optind - 1] : nullptr;
            if (unknown) {
                std::cerr << "error: invalid option -- '" << unknown << "'" << std::endl;
            } else {
                std::cerr << "error: invalid option" << std::endl;
            }
            has_error = true;
            break;
        }
        }

        if (config.exitRequested || has_error) {
            break;
        }
    }

    if (config.exitRequested) {
        return config;
    }

    if (has_error) {
        set_error(config, 1, true);
        return config;
    }

    auto leaves = split_list(leavesInput);
    if (!leaves.empty()) {
        config.leaves = leaves;
    } else {
        config.leaves = {"&"};
    }

    std::array<int, 4> parsedColors = config.colors;
    auto colorParts = split_list(colorsInput);
    if (!validate_color_indices(colorParts, parsedColors)) {
        set_error(config, 1, true);
        return config;
    }
    config.colors = parsedColors;

    if (config.save && config.saveFile.empty()) {
        config.saveFile = default_cache_path();
    }
    if (config.load && config.loadFile.empty()) {
        config.loadFile = default_cache_path();
    }

    if (config.load) {
        if (!load_progress(config)) {
            set_error(config, 1, false);
            return config;
        }
    }

    return config;
}

void print_help(std::ostream& os) {
    os << "Usage: cbonsai [OPTION]...\n"
       << "\n"
       << "cbonsai is a beautifully random bonsai tree generator.\n"
       << "\n"
       << "Options:\n"
       << "  -l, --live             live mode: show each step of growth\n"
       << "  -t, --time=TIME        in live mode, wait TIME secs between\n"
       << "                           steps of growth (must be larger than 0) [default: 0.03]\n"
       << "  -i, --infinite         infinite mode: keep growing trees\n"
       << "  -w, --wait=TIME        in infinite mode, wait TIME between each tree\n"
       << "                           generation [default: 4.00]\n"
       << "  -S, --screensaver      screensaver mode; equivalent to -li and\n"
       << "                           quit on any keypress\n"
       << "  -m, --message=STR      attach message next to the tree\n"
       << "  -b, --base=INT         ascii-art plant base to use, 0 is none\n"
       << "  -c, --leaf=LIST        list of comma-delimited strings randomly chosen\n"
       << "                           for leaves [default: &]\n"
       << "  -k, --color=LIST       list of 4 comma-delimited color indices (0-255) for\n"
       << "                           each of dark leaves, dark wood, light leaves, and\n"
       << "                           light wood, in that order [default: 2,3,10,11]\n"
       << "  -M, --multiplier=INT   branch multiplier; higher -> more\n"
       << "                           branching (0-20) [default: 5]\n"
       << "  -L, --life=INT         life; higher -> more growth (0-200) [default: 32]\n"
       << "  -p, --print            print tree to terminal when finished\n"
       << "  -s, --seed=INT         seed random number generator\n"
       << "  -W, --save[=FILE]      save progress to file [default: $XDG_CACHE_HOME/cbonsai or $HOME/.cache/cbonsai]\n"
       << "  -C, --load[=FILE]      load progress from file [default: $XDG_CACHE_HOME/cbonsai]\n"
       << "  -v, --verbose          increase output verbosity\n"
       << "  -h, --help             show help\n";
}

} // namespace hbonsai
