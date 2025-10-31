#include "hbonsai/title.h"
#include <notcurses/notcurses.h>

namespace hbonsai {

Title::Title(const std::string& text) : text_(text) {}

void Title::render(ncplane* plane) {
    if (!text_.empty()) {
        // Basic title rendering. This can be expanded with effects later.
        int cols = ncplane_dim_x(plane);
        int x_pos = (cols - text_.length()) / 2;
        ncplane_putstr_yx(plane, 0, x_pos, text_.c_str());
    }
}

} // namespace hbonsai
