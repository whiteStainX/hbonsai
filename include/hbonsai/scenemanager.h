#ifndef HBONSAI_SCENEMANAGER_H
#define HBONSAI_SCENEMANAGER_H

#include <deque>
#include <memory>

#include "hbonsai/config.h"
#include "hbonsai/scene.h"

namespace hbonsai {

class Renderer;

class SceneManager {
public:
    void addScene(std::unique_ptr<Scene> scene);
    void run(Renderer& renderer, const AppConfig& appConfig);

private:
    std::deque<std::unique_ptr<Scene>> scenes_;
};

} // namespace hbonsai

#endif // HBONSAI_SCENEMANAGER_H
