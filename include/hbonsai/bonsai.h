#ifndef HBONSAI_BONSAI_H
#define HBONSAI_BONSAI_H

#include "config.h"
#include <vector>
#include <string>
#include <utility>
#include <cstdint>
#include <cwchar>
#include <random>

namespace hbonsai {

// Represents a single character/cell on the screen for the tree
struct TreePart {
    int x = 0;
    int y = 0;
    wchar_t ch = L' ';
    int colorIndex = 0;
    bool bold = false;
};

class Bonsai {
public:
    explicit Bonsai(const Config& config);

    void grow(int height, int width);
    const std::vector<TreePart>& getParts() const;

private:
    const Config& config_;
    std::vector<TreePart> parts_;
    int life_;

    enum class BranchType { Trunk, ShootLeft, ShootRight, Dying, Dead };

    struct Counters {
        int branches = 0;
        int shoots = 0;
        int shootCounter = 0;
    };

    std::vector<std::wstring> leaves_;
    int treeHeight_ = 0;
    int treeWidth_ = 0;

    std::mt19937 rng_;

    int roll(int max);
    std::pair<int, int> setDeltas(BranchType type, int life, int age, int multiplier);
    std::wstring chooseString(BranchType type, int life, int dx, int dy);
    int chooseColor(BranchType type, bool& bold);
    void emitString(int y, int x, const std::wstring& str, int colorIndex, bool bold);

    // Recursive branch growth translated from ref.c
    void branch(int y, int x, int life, BranchType type, Counters& counters);
};

} // namespace hbonsai

#endif // HBONSAI_BONSAI_H
