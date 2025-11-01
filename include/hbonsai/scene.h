#ifndef HBONSAI_SCENE_H
#define HBONSAI_SCENE_H

namespace hbonsai {

class Renderer;

class Scene {
public:
    virtual ~Scene() = default;

    virtual void onEnter(Renderer& renderer) { (void)renderer; }
    virtual void update(double dt) = 0;
    virtual void draw(Renderer& renderer) = 0;
    virtual bool isFinished() const = 0;
};

} // namespace hbonsai

#endif // HBONSAI_SCENE_H
