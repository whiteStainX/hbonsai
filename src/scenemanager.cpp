#include "hbonsai/scenemanager.h"

#include <algorithm>
#include <chrono>
#include <thread>

#include "hbonsai/renderer.h"

namespace hbonsai {

void SceneManager::addScene(std::unique_ptr<Scene> scene) {
    if (scene) {
        scenes_.push_back(std::move(scene));
    }
}

void SceneManager::run(Renderer& renderer, const AppConfig& appConfig) {
    if (scenes_.empty()) {
        return;
    }

    auto current = scenes_.front().get();
    current->onEnter(renderer);

    using Clock = std::chrono::steady_clock;
    auto previous = Clock::now();

    while (!scenes_.empty()) {
        current = scenes_.front().get();

        auto now = Clock::now();
        double dt = std::chrono::duration<double>(now - previous).count();
        previous = now;

        current->update(dt);
        current->draw(renderer);
        renderer.render();

        if (current->isFinished()) {
            scenes_.pop_front();
            if (!scenes_.empty()) {
                current = scenes_.front().get();
                current->onEnter(renderer);
                previous = Clock::now();
            }
            continue;
        }

        double delay = appConfig.timeStep > 0.0f ? std::min<double>(appConfig.timeStep, 0.05) : 0.01;
        std::this_thread::sleep_for(std::chrono::duration<double>(delay));
    }
}

} // namespace hbonsai
