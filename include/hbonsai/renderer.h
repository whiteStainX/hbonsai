#ifndef HBONSAI_RENDERER_H
#define HBONSAI_RENDERER_H

#include "bonsai.h"
#include "config.h"

#include <utility>

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
    void draw(const Bonsai& bonsai, const Config& config);
    void wait(); // Wait for input

    static int baseHeightForType(int baseType);

private:
    struct notcurses* nc_;
    struct ncplane* stdplane_;
    bool initialized_ = false;

    void setPlaneColor(int colorIndex, bool bold);
    void drawTree(const Bonsai& bonsai, const Config& config, int rows, int cols);
    void drawBase(const Config& config, int rows, int cols);
    void drawMessage(const Config& config, int rows, int cols);
    std::pair<int, int> baseDimensions(int baseType) const;
};

} // namespace hbonsai

#endif // HBONSAI_RENDERER_H
