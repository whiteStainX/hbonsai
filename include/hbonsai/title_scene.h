#ifndef HBONSAI_TITLE_SCENE_H
#define HBONSAI_TITLE_SCENE_H

#include "hbonsai/config.h"
#include "hbonsai/scene.h"

namespace hbonsai {

class TitleScene : public Scene {
public:
    explicit TitleScene(const TitleConfig& config);

    void onEnter(Renderer& renderer) override;
    void update(double dt) override;
    void draw(Renderer& renderer) override;
    bool isFinished() const override;

private:
    const TitleConfig& config_;
    double elapsed_ = 0.0;
    bool finished_ = false;
    bool hasDrawn_ = false;
};

} // namespace hbonsai

#endif // HBONSAI_TITLE_SCENE_H
