#include "hbonsai/renderer.h"

#include <algorithm>
#include <chrono>
#include <cwchar>
#include <iostream>
#include <notcurses/notcurses.h>
#include <string>
#include <thread>

namespace hbonsai {
namespace {
constexpr int kTextColor = 7;
}

Renderer::Renderer() {
    struct notcurses_options ncopts = {
        .flags = NCOPTION_SUPPRESS_BANNERS
    };

    nc_ = notcurses_init(&ncopts, nullptr);
    if (nc_ == nullptr) {
        std::cerr << "Error: notcurses_init() failed." << std::endl;
        return;
    }

    stdplane_ = notcurses_stdplane(nc_);
    initialized_ = true;
}

Renderer::~Renderer() {
    if (initialized_) {
        notcurses_stop(nc_);
    }
}

bool Renderer::isInitialized() const {
    return initialized_;
}

std::pair<int, int> Renderer::dimensions() const {
    if (!initialized_) {
        return {0, 0};
    }
    unsigned rows = 0;
    unsigned cols = 0;
    ncplane_dim_yx(stdplane_, &rows, &cols);
    return {static_cast<int>(rows), static_cast<int>(cols)};
}

void Renderer::setPlaneColor(int colorIndex, bool bold) {
    uint64_t channels = 0;
    ncchannels_set_fg_palindex(&channels, colorIndex);
    ncchannels_set_bg_default(&channels);
    ncplane_set_channels(stdplane_, channels);
    ncplane_set_styles(stdplane_, bold ? NCSTYLE_BOLD : NCSTYLE_NONE);
}

std::pair<int, int> Renderer::baseDimensions(int baseType) const {
    switch (baseType) {
    case 1:
        return {4, 31};
    case 2:
        return {3, 15};
    default:
        return {0, 0};
    }
}

int Renderer::baseHeightForType(int baseType) {
    switch (baseType) {
    case 1:
        return 4;
    case 2:
        return 3;
    default:
        return 0;
    }
}

void Renderer::prepareFrame(const Config& config) {
    if (!initialized_) {
        return;
    }

    ncplane_erase(stdplane_);
    unsigned rows = 0;
    unsigned cols = 0;
    ncplane_dim_yx(stdplane_, &rows, &cols);

    drawBase(config, static_cast<int>(rows), static_cast<int>(cols));
    drawMessage(config, static_cast<int>(rows), static_cast<int>(cols));
}

void Renderer::drawStatic(const std::vector<TreePart>& parts, const Config& config) {
    if (!initialized_) {
        return;
    }

    prepareFrame(config);

    unsigned rows = 0;
    unsigned cols = 0;
    ncplane_dim_yx(stdplane_, &rows, &cols);

    drawTree(parts, config, static_cast<int>(rows), static_cast<int>(cols));
    drawMessage(config, static_cast<int>(rows), static_cast<int>(cols));
}

void Renderer::drawLive(const TreePart& part, const Config& config) {
    if (!initialized_) {
        return;
    }

    unsigned rows = 0;
    unsigned cols = 0;
    ncplane_dim_yx(stdplane_, &rows, &cols);

    int baseHeight = baseHeightForType(config.baseType);
    int treeHeight = static_cast<int>(rows) - baseHeight;
    if (treeHeight <= 0) {
        return;
    }

    int y = part.y;
    int x = part.x;
    if (y < 0 || y >= treeHeight || x < 0 || x >= static_cast<int>(cols)) {
        return;
    }

    setPlaneColor(part.colorIndex, part.bold);
    ncplane_putwc_yx(stdplane_, y, x, part.ch);

    drawMessage(config, static_cast<int>(rows), static_cast<int>(cols));
}

void Renderer::render() {
    if (!initialized_) {
        return;
    }

    ncplane_set_styles(stdplane_, NCSTYLE_NONE);
    notcurses_render(nc_);
}

void Renderer::drawTree(const std::vector<TreePart>& parts, const Config& config, int rows, int cols) {
    int baseHeight = baseHeightForType(config.baseType);
    int treeHeight = rows - baseHeight;
    if (treeHeight <= 0) {
        return;
    }

    for (const auto& part : parts) {
        int y = part.y;
        int x = part.x;
        if (y < 0 || y >= treeHeight || x < 0 || x >= cols) {
            continue;
        }

        setPlaneColor(part.colorIndex, part.bold);
        ncplane_putwc_yx(stdplane_, y, x, part.ch);
    }
}

void Renderer::drawBase(const Config& config, int rows, int cols) {
    auto [height, width] = baseDimensions(config.baseType);
    if (height == 0 || width == 0) {
        return;
    }

    int startY = rows - height;
    int startX = std::max(0, (cols - width) / 2);

    auto printSegment = [&](int y, int x, const std::wstring& text, int color, bool bold) {
        setPlaneColor(color, bold);
        ncplane_putwstr_yx(stdplane_, y, x, text.c_str());
        int width = wcswidth(text.c_str(), text.size());
        return width < 0 ? 0 : width;
    };

    if (config.baseType == 1) {
        int x = startX;
        x += printSegment(startY, x, L":", kTextColor, true);
        x += printSegment(startY, x, L"___________", config.colors[2], false);
        x += printSegment(startY, x, L"./~~~\\.", config.colors[3], true);
        x += printSegment(startY, x, L"___________", config.colors[2], false);
        printSegment(startY, x, L":", kTextColor, false);

        setPlaneColor(kTextColor, false);
        ncplane_putwstr_yx(stdplane_, startY + 1, startX, L" \\                           / ");
        ncplane_putwstr_yx(stdplane_, startY + 2, startX, L"  \\_________________________/ ");
        ncplane_putwstr_yx(stdplane_, startY + 3, startX, L"  (_)                     (_)");
    } else if (config.baseType == 2) {
        int x = startX;
        x += printSegment(startY, x, L"(", kTextColor, false);
        x += printSegment(startY, x, L"---", config.colors[2], false);
        x += printSegment(startY, x, L"./~~~\\.", config.colors[3], true);
        x += printSegment(startY, x, L"---", config.colors[2], false);
        printSegment(startY, x, L")", kTextColor, false);

        setPlaneColor(kTextColor, false);
        ncplane_putwstr_yx(stdplane_, startY + 1, startX, L" (           ) ");
        ncplane_putwstr_yx(stdplane_, startY + 2, startX, L"  (_________)  ");
    }
}

void Renderer::drawMessage(const Config& config, int rows, int cols) {
    if (config.message.empty()) {
        return;
    }

    int msgY = std::clamp(static_cast<int>(rows * 0.7), 0, std::max(0, rows - 1));
    int estimatedWidth = static_cast<int>(config.message.size());
    int msgX = std::clamp(static_cast<int>(cols * 0.7), 0, std::max(0, cols - 1));
    if (msgX + estimatedWidth >= cols) {
        msgX = std::max(0, cols - estimatedWidth - 1);
    }

    setPlaneColor(kTextColor, true);
    ncplane_putstr_yx(stdplane_, msgY, msgX, config.message.c_str());
}

void Renderer::drawTitle(const std::string& title, bool cursor_visible, int y_pos) {
    if (!initialized_ || title.empty()) {
        return;
    }
    unsigned rows = 0;
    unsigned cols = 0;
    ncplane_dim_yx(stdplane_, &rows, &cols);

    if (rows == 0 || cols == 0) {
        return;
    }

    if (y_pos < 0) {
        y_pos = 0;
    } else if (y_pos >= static_cast<int>(rows)) {
        y_pos = static_cast<int>(rows) - 1;
    }

    int len = static_cast<int>(title.length());
    int x_pos = std::max(0, (static_cast<int>(cols) - len) / 2);

    // Trim the title if it is wider than the available columns to prevent
    // wrapping or drawing outside the plane bounds.
    int max_title_width = std::max(0, static_cast<int>(cols) - x_pos);
    if (len > max_title_width) {
        len = max_title_width;
    }

    // Clear the line before drawing to prevent artifacts
    setPlaneColor(kTextColor, false);
    std::string blank(static_cast<size_t>(cols), ' ');
    ncplane_putstr_yx(stdplane_, y_pos, 0, blank.c_str());

    // Draw title
    setPlaneColor(kTextColor, true);
    if (len > 0) {
        ncplane_putnstr_yx(stdplane_, y_pos, x_pos, static_cast<size_t>(len), title.c_str());
    }

    // Draw cursor
    if (cursor_visible && x_pos + len < static_cast<int>(cols)) {
        ncplane_putstr_yx(stdplane_, y_pos, x_pos + len, "█");
    }
}

void Renderer::waitForExit(const std::string& title, int title_y) {
    if (!initialized_) {
        return;
    }

    ncinput input{};

    // Flush any pending events to ensure we start fresh.
    while (notcurses_get_nblock(nc_, &input) != static_cast<char32_t>(-1)) {
    }

    const bool hasTitle = !title.empty();

    if (!hasTitle) {
        while (true) {
            char32_t key = notcurses_get_blocking(nc_, &input);
            if (isExitKey(key)) {
                break;
            }
        }
        return;
    }

    bool cursorVisible = true;
    auto lastBlink = std::chrono::steady_clock::now();

    drawTitle(title, cursorVisible, title_y);
    render();

    while (true) {
        char32_t key = notcurses_get_nblock(nc_, &input);
        if (isExitKey(key)) {
            break;
        }

        auto now = std::chrono::steady_clock::now();
        if (std::chrono::duration_cast<std::chrono::milliseconds>(now - lastBlink).count() >= 500) {
            cursorVisible = !cursorVisible;
            drawTitle(title, cursorVisible, title_y);
            render();
            lastBlink = now;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

bool Renderer::isExitKey(char32_t key) const {
    if (key == static_cast<char32_t>(-1) || key == NCKEY_RESIZE) {
        return false;
    }

    if (key == 'q' || key == 'Q' || key == NCKEY_ESC || key == NCKEY_ENTER || key == NCKEY_SPACE) {
        return true;
    }

    return true;
}

} // namespace hbonsai
