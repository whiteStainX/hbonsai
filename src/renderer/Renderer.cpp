#include "hbonsai/renderer.h"
#include <iostream>
#include <notcurses/notcurses.h>

namespace hbonsai {

Renderer::Renderer() {
    struct notcurses_options ncopts = {
        .flags = NCOPTION_SUPPRESS_BANNERS // Don't show startup/shutdown info
    };

    nc_ = notcurses_init(&ncopts, NULL);
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

void Renderer::draw(const Bonsai& bonsai) {
    // The logic to render the `bonsai.getParts()` vector will go here.
    // You will iterate through the parts and use ncplane_putwc_yx() or similar.
    
    // Example:
    // for (const auto& part : bonsai.getParts()) {
    //     ncplane_set_channels(stdplane_, part.channels);
    //     ncplane_putwc_yx(stdplane_, part.y, part.x, part.ch);
    // }

    notcurses_render(nc_);
}

void Renderer::wait() {
    // Blocks until a character is read.
    notcurses_get_blocking(nc_, NULL);
}

} // namespace hbonsai
