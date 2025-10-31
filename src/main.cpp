#include <iostream>
#include "hbonsai/config.h"
#include "hbonsai/renderer.h"
#include "hbonsai/bonsai.h"

int main(int argc, char* argv[]) {
    // 1. Parse configuration from command line arguments
    hbonsai::Config config = hbonsai::parse_args(argc, argv);

    // 2. Initialize the renderer
    hbonsai::Renderer renderer;
    if (!renderer.isInitialized()) {
        return 1; // Renderer failed to initialize
    }

    // 3. Create and grow the bonsai tree
    // hbonsai::Bonsai bonsai(config);
    // bonsai.grow();

    // 4. Render the final tree
    // renderer.draw(bonsai);

    std::cout << "hbonsai initialized successfully!" << std::endl;
    renderer.wait(); // Wait for user input before exiting

    return 0;
}
