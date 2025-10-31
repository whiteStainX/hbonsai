#include <algorithm>
#include <chrono>
#include <clocale>
#include <iostream>
#include <thread>
#include <vector>
#include <notcurses/notcurses.h>
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

    // Determine Y position for the title. Place it near the top portion of the
    // screen, but keep it within the drawable tree area to avoid rendering
    // issues on very small terminals.
    int title_y = std::clamp(static_cast<int>(rows / 6), 1, std::max(0, treeHeight - 1));

    if (config.live) {
        renderer.prepareFrame(config);
        if (!config.title.empty()) {
            renderer.drawTitle(config.title, true, title_y);
        }
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
        if (!config.title.empty()) {
            renderer.drawTitle(config.title, true, title_y);
        }
        renderer.render();
    }

    // New loop for blinking cursor and waiting for exit
    if (!config.title.empty()) {
        // Flush any pending input before starting the loop.
        ncinput input;
        while (notcurses_get_nblock(renderer.getNc(), &input) != (char32_t)-1) {
        }

        bool cursor_visible = true;
        auto last_blink_time = std::chrono::steady_clock::now();

        while (true) {
            char32_t key = notcurses_get_nblock(renderer.getNc(), &input);
            if (key != (char32_t)-1) {
                // Allow a few common keys to exit the application, matching the
                // previous behaviour of quitting with 'q'. Any key will exit,
                // but we explicitly check for 'q' to ensure it remains
                // functional.
                if (key == 'q' || key == 'Q' || key == NCKEY_ENTER || key == NCKEY_ESC || key == NCKEY_SPACE) {
                    break;
                }

                break;
            }

            auto now = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_blink_time);

            if (elapsed.count() >= 500) { // Blink every 500ms
                cursor_visible = !cursor_visible;
                renderer.drawTitle(config.title, cursor_visible, title_y);
                renderer.render();
                last_blink_time = now;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(50)); // Small sleep to prevent busy-waiting
        }
    } else {
        renderer.wait(); // Original behavior if no title
    }

    return 0;
}
