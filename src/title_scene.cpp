#include "hbonsai/title_scene.h"

#include "hbonsai/renderer.h"

namespace hbonsai {

TitleScene::TitleScene(const TitleConfig& config)
    : config_(config) {}

void TitleScene::onEnter(Renderer& renderer) {
    (void)renderer;
    elapsed_ = 0.0;
    finished_ = config_.displaySeconds <= 0.0;
    hasDrawn_ = false;
}

void TitleScene::update(double dt) {
    if (finished_) {
        return;
    }

    elapsed_ += dt;
    if (elapsed_ >= config_.displaySeconds) {
        finished_ = true;
    }
}

void TitleScene::draw(Renderer& renderer) {
    if (!hasDrawn_) {
        renderer.renderTitle(config_);
        hasDrawn_ = true;
    }
}

bool TitleScene::isFinished() const {
    return finished_;
}

} // namespace hbonsai
