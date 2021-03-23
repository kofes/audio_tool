#include "player.h"

#include "../scope_guard.h"

#include <array>
#include <chrono>

#include <string>
#include <sstream>

using namespace custom;

namespace {
    constexpr auto count_tracks = 5;
}

class player::core {
public:
    std::array<std::chrono::milliseconds, count_tracks> periods_;

};

player::player() noexcept = default;
player::~player() = default;

player::player(player&& src) noexcept: core_(std::move(src.core_)) {}
player& player::operator=(player src) noexcept {
    core_ = std::move(src.core_);
    return *this;
}

player::player(int argc, const char* const* const argv): core_(std::make_unique<player::core>()) {
    std::ostringstream osstream;

    if (argc-1 != count_tracks) {
        osstream << "required " << count_tracks << " input values, " << (argc-1) << " passed.";
        throw std::invalid_argument(osstream.str());
    }

    try {
        auto i = 1;
        custom::scope_guard guard([&i, &osstream] {
            osstream << " argument #" << i;
        });

        for (; i < argc; ++i) {
            std::string argv_str_repr{argv[i]};
            size_t index;
            auto value = std::stoi(argv_str_repr, &index);

            if (index < argv_str_repr.size()) {
                throw std::invalid_argument("");
            }

            if (value < 0) {
                throw std::underflow_error("");
            }

            core_->periods_[i-1] = std::chrono::milliseconds(value);
        }
    } catch(const std::invalid_argument& ex) {
        osstream << " is invalid (invalid argument).";
        throw std::invalid_argument(osstream.str());
    } catch(const std::out_of_range& ex) {
        osstream << " is too huge (out of range).";
        throw std::invalid_argument(osstream.str());
    } catch(const std::underflow_error& ex) {
        osstream << " is less than 0 (underflow).";
        throw std::invalid_argument(osstream.str());
    }
}