#include "hbonsai/config.h"

#include <algorithm>
#include <array>
#include <cstdlib>
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

} // namespace

Config parse_args(int argc, char* argv[]) {
    Config config;

    auto require_value = [&](int index, const std::string& option) -> std::string {
        if (index + 1 >= argc) {
            std::cerr << "Missing value for option " << option << std::endl;
            return {};
        }
        return argv[index + 1];
    };

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "-l" || arg == "--live") {
            config.live = true;
        } else if (arg == "-t" || arg == "--time") {
            std::string value = require_value(i, arg);
            if (!value.empty()) {
                ++i;
                float parsed = config.timeStep;
                if (parse_float(value, parsed) && parsed >= 0.0f) {
                    config.timeStep = parsed;
                } else {
                    std::cerr << "Invalid value for " << arg << ": " << value << std::endl;
                }
            }
        } else if (arg == "-i" || arg == "--infinite") {
            config.infinite = true;
        } else if (arg == "-w" || arg == "--wait") {
            std::string value = require_value(i, arg);
            if (!value.empty()) {
                ++i;
                double parsed = config.timeWait;
                if (parse_double(value, parsed) && parsed >= 0.0) {
                    config.timeWait = parsed;
                } else {
                    std::cerr << "Invalid value for " << arg << ": " << value << std::endl;
                }
            }
        } else if (arg == "-S" || arg == "--screensaver") {
            config.screensaver = true;
            config.live = true;
            config.infinite = true;
        } else if (arg == "-m" || arg == "--message") {
            std::string value = require_value(i, arg);
            if (!value.empty()) {
                ++i;
                config.message = value;
            }
        } else if (arg == "-b" || arg == "--base") {
            std::string value = require_value(i, arg);
            if (!value.empty()) {
                ++i;
                int parsed = config.baseType;
                if (parse_int(value, parsed) && parsed >= 0) {
                    config.baseType = parsed;
                } else {
                    std::cerr << "Invalid value for " << arg << ": " << value << std::endl;
                }
            }
        } else if (arg == "-c" || arg == "--leaf") {
            std::string value = require_value(i, arg);
            if (!value.empty()) {
                ++i;
                auto leaves = split_list(value);
                if (!leaves.empty()) {
                    config.leaves = leaves;
                }
            }
        } else if (arg == "-k" || arg == "--color" || arg == "--colors") {
            std::string value = require_value(i, arg);
            if (!value.empty()) {
                ++i;
                auto parts = split_list(value);
                if (parts.size() == config.colors.size()) {
                    std::array<int, 4> parsedColors = config.colors;
                    bool ok = true;
                    for (size_t idx = 0; idx < parts.size(); ++idx) {
                        int parsed = parsedColors[idx];
                        if (parse_int(parts[idx], parsed)) {
                            parsedColors[idx] = parsed;
                        } else {
                            ok = false;
                            break;
                        }
                    }
                    if (ok) {
                        config.colors = parsedColors;
                    } else {
                        std::cerr << "Invalid color list: " << value << std::endl;
                    }
                } else {
                    std::cerr << "Color list must contain exactly 4 values." << std::endl;
                }
            }
        } else if (arg == "-M" || arg == "--multiplier") {
            std::string value = require_value(i, arg);
            if (!value.empty()) {
                ++i;
                int parsed = config.multiplier;
                if (parse_int(value, parsed) && parsed >= 0) {
                    config.multiplier = parsed;
                } else {
                    std::cerr << "Invalid value for " << arg << ": " << value << std::endl;
                }
            }
        } else if (arg == "-L" || arg == "--life") {
            std::string value = require_value(i, arg);
            if (!value.empty()) {
                ++i;
                int parsed = config.lifeStart;
                if (parse_int(value, parsed) && parsed >= 0) {
                    config.lifeStart = parsed;
                } else {
                    std::cerr << "Invalid value for " << arg << ": " << value << std::endl;
                }
            }
        } else if (arg == "-p" || arg == "--print") {
            config.printTree = true;
        } else if (arg == "-s" || arg == "--seed") {
            std::string value = require_value(i, arg);
            if (!value.empty()) {
                ++i;
                int parsed = config.seed;
                if (parse_int(value, parsed)) {
                    config.seed = parsed;
                } else {
                    std::cerr << "Invalid seed value: " << value << std::endl;
                }
            }
        } else if (arg == "-v" || arg == "--verbose") {
            config.verbosity += 1;
        }
    }

    return config;
}

} // namespace hbonsai
