#pragma once

#include <memory>

namespace custom {
class player {
public:
    player() noexcept;
    player(int argc, const char* const* argv);

    player(player&&) noexcept;
    player& operator=(player) noexcept;

    ~player();

public:
    void add_tracks_to_queue(const std::string&);

    void start();

protected:
    void play();

    void parse_array(const std::string&);

protected:
    class core;
    std::unique_ptr<core> core_;
};
} // namespace custom
