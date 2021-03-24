#pragma once

#include <string>
#include <memory>

namespace audio {
class track_controller;

enum class track_type {
    WAV,
    MP3
};

class fabric {
public:
    fabric() = delete;
    fabric(const fabric&) = delete;
    fabric(fabric&&) = delete;

    fabric& operator=(const fabric&) = delete;
    fabric& operator=(fabric&&) = delete;

public:
    static std::unique_ptr<track_controller> get_track_controller(track_type type, const std::string& name);

};
} // namespace audio
