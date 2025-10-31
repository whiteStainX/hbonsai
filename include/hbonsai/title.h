#ifndef HBONSAI_TITLE_H
#define HBONSAI_TITLE_H

#include <string>

// Forward-declare notcurses type
struct ncplane;

namespace hbonsai {

class Title {
public:
    Title(const std::string& text);

    void render(ncplane* plane);

private:
    std::string text_;
};

} // namespace hbonsai

#endif // HBONSAI_TITLE_H
