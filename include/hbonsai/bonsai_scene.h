#ifndef HBONSAI_BONSAI_SCENE_H
#define HBONSAI_BONSAI_SCENE_H

#include <cstddef>
#include <vector>

#include "hbonsai/bonsai.h"
#include "hbonsai/config.h"
#include "hbonsai/scene.h"

namespace hbonsai {

class BonsaiScene : public Scene {
public:
    BonsaiScene(const AppConfig& appConfig, const BonsaiConfig& bonsaiConfig);

    void onEnter(Renderer& renderer) override;
    void update(double dt) override;
    void draw(Renderer& renderer) override;
    bool isFinished() const override;

private:
    void resetState();

    const AppConfig& appConfig_;
    const BonsaiConfig& bonsaiConfig_;
    Bonsai bonsai_;
    std::vector<TreePart> parts_;
    std::vector<std::size_t> pendingParts_;
    int treeHeight_ = 0;
    int treeWidth_ = 0;
    std::size_t nextIndex_ = 0;
    double accumulator_ = 0.0;
    bool started_ = false;
    bool finished_ = false;
    bool framePrepared_ = false;
    bool staticDrawn_ = false;
};

} // namespace hbonsai

#endif // HBONSAI_BONSAI_SCENE_H
