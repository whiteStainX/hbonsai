#include <clocale>
#include <iostream>
#include <memory>
#include "hbonsai/config.h"
#include "hbonsai/bonsai_scene.h"
#include "hbonsai/renderer.h"
#include "hbonsai/scenemanager.h"

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

    hbonsai::SceneManager sceneManager;
    sceneManager.addScene(std::make_unique<hbonsai::BonsaiScene>(config.app, config.bonsai, config.title));

    sceneManager.run(renderer, config.app);

    renderer.wait();

    return 0;
}
