#ifndef HBONSAI_RENDERER_H
#define HBONSAI_RENDERER_H

#include "bonsai.h"

// Forward-declare notcurses types to keep the header clean
struct notcurses;
struct ncplane;

namespace hbonsai {

class Renderer {
public:
    Renderer();
    ~Renderer();

    bool isInitialized() const;
    void draw(const Bonsai& bonsai);
    void wait(); // Wait for input

private:
    struct notcurses* nc_;
    struct ncplane* stdplane_;
    bool initialized_ = false;
};

} // namespace hbonsai

#endif // HBONSAI_RENDERER_H
