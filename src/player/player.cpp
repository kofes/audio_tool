#include "player.h"

#include "../scope_guard.h"

#include <array>
#include <chrono>

#include <string>
#include <sstream>

#include <queue>
#include <iterator>

using namespace custom;

namespace {
    constexpr auto count_tracks = 5;
}

namespace {
    enum class track_state: char {
        NONE,
        PLAY
    };
}

class player::core {
public:
    std::array<std::chrono::milliseconds, count_tracks> periods_;
    std::queue<std::vector<track_state>> tracks_states_;
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

namespace {
    class duplicated_arguments: public std::logic_error {
        using std::logic_error::logic_error;
    };
}

void player::parse_array(const std::string& src) {
    std::ostringstream osstream;

    std::istringstream isstream(src);
    constexpr auto delimiter = ' ';

    std::vector<std::string> tokens;
    std::copy(
        std::istream_iterator<std::string>(isstream),
        std::istream_iterator<std::string>(),
        std::back_inserter(tokens)
    );

    if (tokens.size() > count_tracks) {
        osstream << "the sequence must have no more than " << count_tracks << " elements.";
        throw std::range_error(osstream.str());
    }

    std::vector<track_state> result(count_tracks, track_state::NONE);

    try {
        auto i = 0u;

        custom::scope_guard guard([&i, &osstream] {
            osstream << "element #" << i;
        });

        for (; i < tokens.size(); ++i) {
            size_t index;
            auto value = std::stoi(tokens[i], &index);

            if (index < tokens[i].size()) {
                throw std::invalid_argument("");
            }

            if (value < 1) {
                throw std::underflow_error("");
            }

            if (value > count_tracks) {
                throw std::overflow_error("");
            }

            if (result[value] != track_state::NONE) {
                throw duplicated_arguments("");
            }

            result[value] = track_state::PLAY;
        }
    } catch(const std::invalid_argument& ex) {
        osstream << " is invalid (invalid argument).";
        throw std::invalid_argument(osstream.str());
    } catch(const std::out_of_range& ex) {
        osstream << " is too huge (out of range).";
        throw std::invalid_argument(osstream.str());
    } catch(const std::overflow_error& ex) {
        osstream << " is greater than the limit. The limit eq. " << count_tracks << " (overflow).";
        throw std::invalid_argument(osstream.str());
    } catch(const std::underflow_error& ex) {
        osstream << " is less than 1 (underflow).";
        throw std::invalid_argument(osstream.str());
    } catch(const duplicated_arguments& ex) {
        osstream << " is a duplicate (duplicated arguments).";
        throw std::invalid_argument(osstream.str());
    }

    core_->tracks_states_.push(std::move(result));
}
