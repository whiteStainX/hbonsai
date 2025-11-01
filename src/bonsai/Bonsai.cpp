#include "hbonsai/bonsai.h"

#include <algorithm>
#include <cwchar>
#include <string>

namespace hbonsai {
namespace {

std::wstring utf8_to_wstring(const std::string& input) {
    std::wstring output;
    output.reserve(input.size());

    size_t i = 0;
    while (i < input.size()) {
        unsigned char byte = static_cast<unsigned char>(input[i]);
        char32_t codepoint = 0;
        size_t continuation_count = 0;

        if ((byte & 0x80u) == 0) {
            codepoint = byte;
        } else if ((byte & 0xE0u) == 0xC0u) {
            codepoint = byte & 0x1Fu;
            continuation_count = 1;
        } else if ((byte & 0xF0u) == 0xE0u) {
            codepoint = byte & 0x0Fu;
            continuation_count = 2;
        } else if ((byte & 0xF8u) == 0xF0u) {
            codepoint = byte & 0x07u;
            continuation_count = 3;
        } else {
            ++i;
            continue;
        }

        if (i + continuation_count >= input.size()) {
            break;
        }

        bool valid = true;
        for (size_t j = 1; j <= continuation_count; ++j) {
            unsigned char follow = static_cast<unsigned char>(input[i + j]);
            if ((follow & 0xC0u) != 0x80u) {
                valid = false;
                break;
            }
            codepoint = (codepoint << 6) | (follow & 0x3Fu);
        }

        if (!valid) {
            ++i;
            continue;
        }

        output.push_back(static_cast<wchar_t>(codepoint));
        i += continuation_count + 1;
    }

    return output;
}

} // namespace

Bonsai::Bonsai(const BonsaiConfig& config)
    : config_(config), rng_(config.seed == 0 ? std::random_device{}()
                                             : static_cast<unsigned int>(config.seed)) {
    if (config_.leaves.empty()) {
        leaves_.push_back(L"&");
    } else {
        for (const auto& leaf : config_.leaves) {
            if (!leaf.empty()) {
                leaves_.push_back(utf8_to_wstring(leaf));
            }
        }
        if (leaves_.empty()) {
            leaves_.push_back(L"&");
        }
    }
}

std::vector<TreePart> Bonsai::generate(int height, int width) {
    std::vector<TreePart> parts;
    if (height <= 0 || width <= 0) {
        return parts;
    }

    treeHeight_ = height;
    treeWidth_ = width;

    Counters counters;
    counters.shootCounter = roll(1000);

    int startY = treeHeight_ - 1;
    int startX = treeWidth_ / 2;

    branch(startY, startX, config_.lifeStart, BranchType::Trunk, counters, parts);

    return parts;
}

int Bonsai::roll(int max) {
    if (max <= 0) {
        return 0;
    }
    std::uniform_int_distribution<int> dist(0, max - 1);
    return dist(rng_);
}

std::pair<int, int> Bonsai::setDeltas(BranchType type, int life, int age, int multiplier) {
    int dx = 0;
    int dy = 0;
    int dice = 0;
    int safeMultiplier = std::max(1, multiplier);

    switch (type) {
    case BranchType::Trunk: {
        if (age <= 2 || life < 4) {
            dy = 0;
            dx = roll(3) - 1;
        } else if (age < safeMultiplier * 3) {
            int step = std::max(1, static_cast<int>(safeMultiplier * 0.5f));
            if (step > 0 && age % step == 0) {
                dy = -1;
            } else {
                dy = 0;
            }

            dice = roll(10);
            if (dice == 0) {
                dx = -2;
            } else if (dice >= 1 && dice <= 3) {
                dx = -1;
            } else if (dice >= 4 && dice <= 5) {
                dx = 0;
            } else if (dice >= 6 && dice <= 8) {
                dx = 1;
            } else {
                dx = 2;
            }
        } else {
            dice = roll(10);
            dy = (dice > 2) ? -1 : 0;
            dx = roll(3) - 1;
        }
        break;
    }
    case BranchType::ShootLeft: {
        dice = roll(10);
        if (dice <= 1) {
            dy = -1;
        } else if (dice <= 7) {
            dy = 0;
        } else {
            dy = 1;
        }

        dice = roll(10);
        if (dice <= 1) {
            dx = -2;
        } else if (dice <= 5) {
            dx = -1;
        } else if (dice <= 8) {
            dx = 0;
        } else {
            dx = 1;
        }
        break;
    }
    case BranchType::ShootRight: {
        dice = roll(10);
        if (dice <= 1) {
            dy = -1;
        } else if (dice <= 7) {
            dy = 0;
        } else {
            dy = 1;
        }

        dice = roll(10);
        if (dice <= 1) {
            dx = 2;
        } else if (dice <= 5) {
            dx = 1;
        } else if (dice <= 8) {
            dx = 0;
        } else {
            dx = -1;
        }
        break;
    }
    case BranchType::Dying: {
        dice = roll(10);
        if (dice <= 1) {
            dy = -1;
        } else if (dice <= 8) {
            dy = 0;
        } else {
            dy = 1;
        }

        dice = roll(15);
        if (dice == 0) {
            dx = -3;
        } else if (dice <= 2) {
            dx = -2;
        } else if (dice <= 5) {
            dx = -1;
        } else if (dice <= 8) {
            dx = 0;
        } else if (dice <= 11) {
            dx = 1;
        } else if (dice <= 13) {
            dx = 2;
        } else {
            dx = 3;
        }
        break;
    }
    case BranchType::Dead: {
        dice = roll(10);
        if (dice <= 2) {
            dy = -1;
        } else if (dice <= 6) {
            dy = 0;
        } else {
            dy = 1;
        }
        dx = roll(3) - 1;
        break;
    }
    }

    return {dx, dy};
}

std::wstring Bonsai::chooseString(BranchType type, int life, int dx, int dy) {
    if (life < 4) {
        type = BranchType::Dying;
    }

    switch (type) {
    case BranchType::Trunk:
        if (dy == 0) {
            return L"/~";
        } else if (dx < 0) {
            return L"\\|";
        } else if (dx == 0) {
            return L"/|\\";
        } else {
            return L"|/";
        }
    case BranchType::ShootLeft:
        if (dy > 0) {
            return L"\\";
        } else if (dy == 0) {
            return L"\\_";
        } else if (dx < 0) {
            return L"\\|";
        } else if (dx == 0) {
            return L"/|";
        }
        return L"/";
    case BranchType::ShootRight:
        if (dy > 0) {
            return L"/";
        } else if (dy == 0) {
            return L"_/";
        } else if (dx < 0) {
            return L"\\|";
        } else if (dx == 0) {
            return L"/|";
        }
        return L"/";
    case BranchType::Dying:
    case BranchType::Dead: {
        const auto& leaf = leaves_[roll(static_cast<int>(leaves_.size()))];
        return leaf;
    }
    }
    return L"?";
}

int Bonsai::chooseColor(BranchType type, bool& bold) {
    bold = false;
    switch (type) {
    case BranchType::Trunk:
    case BranchType::ShootLeft:
    case BranchType::ShootRight:
        if (roll(2) == 0) {
            bold = true;
            return config_.colors[3];
        }
        return config_.colors[1];
    case BranchType::Dying:
        if (roll(10) == 0) {
            bold = true;
        }
        return config_.colors[2];
    case BranchType::Dead:
        if (roll(3) == 0) {
            bold = true;
        }
        return config_.colors[0];
    }
    return config_.colors[1];
}

void Bonsai::emitString(int y, int x, const std::wstring& str, int colorIndex, bool bold,
                       std::vector<TreePart>& parts) {
    int currentX = x;
    for (wchar_t wc : str) {
        int width = wcwidth(wc);
        if (width <= 0) {
            width = 1;
        }

        if (currentX >= 0 && currentX < treeWidth_ && y >= 0 && y < treeHeight_) {
            TreePart part;
            part.x = currentX;
            part.y = y;
            part.ch = wc;
            part.colorIndex = colorIndex;
            part.bold = bold;
            parts.push_back(part);
        }

        currentX += width;
    }
}

void Bonsai::branch(int y, int x, int life, BranchType type, Counters& counters,
                    std::vector<TreePart>& parts) {
    if (life <= 0) {
        return;
    }

    counters.branches++;
    int safeMultiplier = std::max(1, config_.multiplier);
    int shootCooldown = safeMultiplier;

    while (life > 0) {
        life--;
        int age = config_.lifeStart - life;
        auto [dx, dy] = setDeltas(type, life, age, safeMultiplier);

        if (dy > 0 && y > (treeHeight_ - 2)) {
            dy--;
        }

        if (life < 3) {
            branch(y, x, life, BranchType::Dead, counters, parts);
        } else if (type == BranchType::Trunk && life < (safeMultiplier + 2)) {
            branch(y, x, life, BranchType::Dying, counters, parts);
        } else if ((type == BranchType::ShootLeft || type == BranchType::ShootRight) &&
                   life < (safeMultiplier + 2)) {
            branch(y, x, life, BranchType::Dying, counters, parts);
        } else if (type == BranchType::Trunk &&
                   ((roll(3) == 0) || (safeMultiplier > 0 && life > 0 && life % safeMultiplier == 0))) {
            if (roll(8) == 0 && life > 7) {
                shootCooldown = safeMultiplier * 2;
                int extraLife = roll(5) - 2;
                branch(y, x, life + extraLife, BranchType::Trunk, counters, parts);
            } else if (shootCooldown <= 0) {
                shootCooldown = safeMultiplier * 2;
                int shootLife = life + safeMultiplier;
                counters.shoots++;
                counters.shootCounter++;
                BranchType branchType = (counters.shootCounter % 2 == 0) ? BranchType::ShootRight
                                                                        : BranchType::ShootLeft;
                branch(y, x, shootLife, branchType, counters, parts);
            }
        }

        shootCooldown--;

        x += dx;
        y += dy;

        if (treeWidth_ > 0) {
            x = std::clamp(x, 0, treeWidth_ - 1);
        }
        if (treeHeight_ > 0) {
            y = std::clamp(y, 0, treeHeight_ - 1);
        }

        bool bold = false;
        int color = chooseColor(type, bold);
        std::wstring glyph = chooseString(type, life, dx, dy);
        emitString(y, x, glyph, color, bold, parts);
    }
}

} // namespace hbonsai
