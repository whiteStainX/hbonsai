#include "hbonsai/renderer.h"

#include <algorithm>
#include <cwchar>
#include <iostream>
#include <notcurses/notcurses.h>
#include <string>

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

void Renderer::wait() {
    notcurses_get_blocking(nc_, nullptr);
}

} // namespace hbonsai
