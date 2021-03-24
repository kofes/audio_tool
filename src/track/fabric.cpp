#include "fabric.h"

#include "music.h"
#include "track.h"

#include <SDL2/SDL.h>

using namespace audio;

std::unique_ptr<track_controller> fabric::get_track_controller(track_type type, const std::string& name) {
    switch (type) {
        case track_type::MP3:
            return std::make_unique<music>(name);
        case track_type::WAV:
            return std::make_unique<track>(name);
        default:
            throw std::runtime_error("can't create track controller");
        break;
    }

    return nullptr;
}