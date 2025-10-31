#include <algorithm>
#include <clocale>
#include <iostream>
#include "hbonsai/config.h"
#include "hbonsai/renderer.h"
#include "hbonsai/bonsai.h"

int main(int argc, char* argv[]) {
    std::setlocale(LC_ALL, "");

    // 1. Parse configuration from command line arguments
    hbonsai::Config config = hbonsai::parse_args(argc, argv);

    // 2. Initialize the renderer
    hbonsai::Renderer renderer;
    if (!renderer.isInitialized()) {
        return 1; // Renderer failed to initialize
    }

    hbonsai::Bonsai bonsai(config);
    auto [rows, cols] = renderer.dimensions();
    int baseHeight = hbonsai::Renderer::baseHeightForType(config.baseType);
    int treeHeight = std::max(1, rows - baseHeight);

    bonsai.grow(treeHeight, cols);
    renderer.draw(bonsai, config);

    renderer.wait();

    return 0;
}
