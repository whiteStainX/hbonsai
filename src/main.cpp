#include <algorithm>
#include <chrono>
#include <clocale>
#include <iostream>
#include <thread>
#include <vector>
#include "hbonsai/config.h"
#include "hbonsai/renderer.h"
#include "hbonsai/bonsai.h"

int main(int argc, char* argv[]) {
    std::setlocale(LC_ALL, "");

    // 1. Parse configuration from command line arguments
    hbonsai::Config config = hbonsai::parse_args(argc, argv);

    if (config.exitRequested) {
        if (config.showHelp) {
            hbonsai::print_help(std::cout);
        }
        return config.exitCode;
    }

    // 2. Initialize the renderer
    hbonsai::Renderer renderer;
    if (!renderer.isInitialized()) {
        return 1; // Renderer failed to initialize
    }

    hbonsai::Bonsai bonsai(config);
    auto [rows, cols] = renderer.dimensions();
    int baseHeight = hbonsai::Renderer::baseHeightForType(config.baseType);
    int treeHeight = std::max(1, rows - baseHeight);

    std::vector<hbonsai::TreePart> parts = bonsai.generate(treeHeight, cols);

    if (config.live) {
        renderer.prepareFrame(config);
        renderer.render();
        for (const auto& part : parts) {
            renderer.drawLive(part, config);
            renderer.render();
            if (config.timeStep > 0.0f) {
                std::this_thread::sleep_for(std::chrono::duration<float>(config.timeStep));
            }
        }
    } else {
        renderer.drawStatic(parts, config);
        renderer.render();
    }

    renderer.wait();

    return 0;
}
