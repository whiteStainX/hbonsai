#ifndef HBONSAI_RENDERER_H
#define HBONSAI_RENDERER_H

#include "bonsai.h"
#include "config.h"

#include <utility>
#include <vector>

// Forward-declare notcurses types to keep the header clean
struct notcurses;
struct ncplane;

namespace hbonsai {

class Renderer {
public:
    Renderer();
    ~Renderer();

    bool isInitialized() const;
    std::pair<int, int> dimensions() const;
    void prepareFrame(const BonsaiConfig& config);
    void drawStatic(const std::vector<TreePart>& parts, const BonsaiConfig& config);
    void drawLive(const TreePart& part, const BonsaiConfig& config);
    void renderTitle(const TitleConfig& config);
    void render();
    void wait(); // Wait for input

    static int baseHeightForType(int baseType);

private:
    struct notcurses* nc_;
    struct ncplane* stdplane_;
    bool initialized_ = false;

    void setPlaneColor(int colorIndex, bool bold);
    void drawTree(const std::vector<TreePart>& parts, const BonsaiConfig& config, int rows, int cols);
    void drawBase(const BonsaiConfig& config, int rows, int cols);
    void drawMessage(const BonsaiConfig& config, int rows, int cols);
    std::pair<int, int> baseDimensions(int baseType) const;
};

} // namespace hbonsai

#endif // HBONSAI_RENDERER_H
