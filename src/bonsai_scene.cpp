#include "hbonsai/bonsai_scene.h"

#include <algorithm>

#include "hbonsai/renderer.h"

namespace hbonsai {

BonsaiScene::BonsaiScene(const AppConfig& appConfig, const BonsaiConfig& bonsaiConfig)
    : appConfig_(appConfig), bonsaiConfig_(bonsaiConfig), bonsai_(bonsaiConfig) {}

void BonsaiScene::onEnter(Renderer& renderer) {
    auto [rows, cols] = renderer.dimensions();
    int baseHeight = Renderer::baseHeightForType(bonsaiConfig_.baseType);
    treeHeight_ = std::max(1, rows - baseHeight);
    treeWidth_ = cols;
    resetState();
}

void BonsaiScene::resetState() {
    parts_ = bonsai_.generate(treeHeight_, treeWidth_);
    pendingParts_.clear();
    nextIndex_ = 0;
    accumulator_ = 0.0;
    started_ = false;
    finished_ = parts_.empty();
    framePrepared_ = false;
    staticDrawn_ = false;
}

void BonsaiScene::update(double dt) {
    if (!appConfig_.live) {
        return;
    }

    if (finished_) {
        return;
    }

    if (!started_) {
        if (nextIndex_ < parts_.size()) {
            pendingParts_.push_back(nextIndex_++);
            started_ = true;
        } else {
            finished_ = true;
            return;
        }
    }

    if (appConfig_.timeStep <= 0.0f) {
        while (nextIndex_ < parts_.size()) {
            pendingParts_.push_back(nextIndex_++);
        }
        finished_ = true;
        return;
    }

    accumulator_ += dt;
    while (accumulator_ >= static_cast<double>(appConfig_.timeStep) && nextIndex_ < parts_.size()) {
        pendingParts_.push_back(nextIndex_++);
        accumulator_ -= static_cast<double>(appConfig_.timeStep);
    }

    if (nextIndex_ >= parts_.size()) {
        finished_ = true;
    }
}

void BonsaiScene::draw(Renderer& renderer) {
    if (!appConfig_.live) {
        if (!staticDrawn_) {
            renderer.drawStatic(parts_, bonsaiConfig_);
            staticDrawn_ = true;
            finished_ = true;
        }
        return;
    }

    if (!framePrepared_) {
        renderer.prepareFrame(bonsaiConfig_);
        framePrepared_ = true;
    }

    for (std::size_t index : pendingParts_) {
        if (index < parts_.size()) {
            renderer.drawLive(parts_[index], bonsaiConfig_);
        }
    }
    pendingParts_.clear();
}

bool BonsaiScene::isFinished() const {
    if (!appConfig_.live) {
        return finished_ && staticDrawn_;
    }
    return finished_ && pendingParts_.empty();
}

} // namespace hbonsai
