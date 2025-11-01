#include "hbonsai/title.h"
#include <algorithm>
#include <notcurses/notcurses.h>

namespace hbonsai {

Title::Title(const std::string& text) : text_(text) {}

void Title::render(ncplane* plane) {
    if (!text_.empty()) {
        // Basic title rendering. This can be expanded with effects later.
        unsigned rows = 0;
        unsigned cols = 0;
        ncplane_dim_yx(plane, &rows, &cols);

        int x_pos = static_cast<int>(cols) - static_cast<int>(text_.length());
        x_pos = x_pos > 0 ? x_pos / 2 : 0;

        int y_pos = 0;
        if (rows > 0) {
            int suggested = static_cast<int>(rows) / 6;
            suggested = std::max(1, suggested);
            y_pos = std::min(static_cast<int>(rows) - 1, suggested);
        }

        ncplane_putstr_yx(plane, y_pos, x_pos, text_.c_str());
    }
}

} // namespace hbonsai
