#ifndef HBONSAI_BONSAI_H
#define HBONSAI_BONSAI_H

#include "config.h"
#include <vector>
#include <cstdint>

namespace hbonsai {

// Represents a single character/cell on the screen for the tree
struct TreePart {
    int x, y;
    uint32_t ch;
    uint64_t channels; // For color
};

class Bonsai {
public:
    explicit Bonsai(const Config& config);

    void grow();
    const std::vector<TreePart>& getParts() const;

private:
    const Config& config_;
    std::vector<TreePart> parts_;
    int life_;

    // This will be the C++ version of the recursive `branch` function
    void branch(int y, int x, int life, int type, int age);
};

} // namespace hbonsai

#endif // HBONSAI_BONSAI_H
