#include "hbonsai/bonsai.h"
#include <iostream>
#include <cstdlib>
#include <ctime>

namespace hbonsai {

Bonsai::Bonsai(const Config& config) : config_(config), life_(config.lifeStart) {
    // Initialize random seed
    if (config.seed == 0) {
        srand(time(NULL));
    } else {
        srand(config.seed);
    }
}

void Bonsai::grow() {
    std::cout << "Growing the bonsai..." << std::endl;
    // The main logic from `growTree` in ref.c will go here.
    // It will make the initial call to the recursive `branch` function.
    // For example:
    // int start_x = 80 / 2; // Replace with screen width
    // int start_y = 24 - 1; // Replace with screen height
    // branch(start_y, start_x, life_, 0, 0);
}

const std::vector<TreePart>& Bonsai::getParts() const {
    return parts_;
}

void Bonsai::branch(int y, int x, int life, int type, int age) {
    // The recursive logic from the `branch` function in ref.c will be ported here.
    // It will add TreePart objects to the `parts_` vector.
}

} // namespace hbonsai
