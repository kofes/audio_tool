#include "player.h"

#include "../scope_guard.h"
#include "../config.h"
#include "../track/track.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#include <array>
#include <chrono>

#include <string>
#include <sstream>

#include <queue>
#include <set>
#include <map>

#include <iterator>

#include <mutex>
#include <thread>
#include <future>

#include <random>

using namespace custom;

namespace {
    constexpr auto count_tracks = 5;
}

namespace {
    enum class track_state {
        DISCARDED = 0,
        AWAITS_FOR_LOOP,
        AWAITS_FOR_ONE_PLAY,
        PLAYED
    };
}

class player::core {
public:
    core(): generator_(random_device_()), distrubution_(0, count_tracks) {
        int result = 0;
        int flags = MIX_INIT_MP3;

        if (SDL_Init(SDL_INIT_AUDIO) < 0) {
            throw std::runtime_error("failed to init SDL");
        }

        if (flags != (result = Mix_Init(flags))) {
            std::ostringstream osstream;
            osstream
                << "could not initialize mixer (result: " << result << ").\n"
                << "Mix_Init: " << Mix_GetError();
            throw std::runtime_error(osstream.str());
        }

        // Mix_OpenAudio(48000, MIX_DEFAULT_FORMAT, 1, 1024
        if (Mix_OpenAudio(48000, MIX_DEFAULT_FORMAT, 1, 1024) == -1) {
            std::ostringstream osstream;
            osstream
                << "could not open audio.\n"
                << "Mix_OpenAudio: " << Mix_GetError();
            throw std::runtime_error(osstream.str());
        }
    }

    ~core() {
        SDL_Quit();
    }

public:
    std::vector<audio::track> tracks_;
    std::array<std::chrono::milliseconds, count_tracks> periods_;
    std::array<std::chrono::time_point<std::chrono::system_clock>, count_tracks> timeouts_;
    std::array<track_state, count_tracks> tracks_to_play_ = { track_state::DISCARDED };

public:
    std::random_device random_device_;
    std::mt19937 generator_;
    std::uniform_int_distribution<> distrubution_;

public:
    std::thread play_thread_;
    std::mutex tracks_list_mutex_;
};

player::player() noexcept = default;
player::~player() {
    if (core_ && core_->play_thread_.joinable()) {
        core_->play_thread_.detach();
    }
}

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

    core_->tracks_.reserve(count_tracks);
    for (auto i = 0; i < count_tracks; ++i) {
        core_->tracks_.emplace_back(RESOURCES_PATH + std::string{"track"} + std::to_string(i+1) + ".mp3");
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

    std::set<unsigned char> result;

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

            if (result.count(static_cast<unsigned char>(value-1))) {
                throw duplicated_arguments("");
            }

            result.emplace(static_cast<unsigned char>(value-1));
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

    std::lock_guard<std::mutex> lock(core_->tracks_list_mutex_);

    // update tracks states
    for (auto i = 0u; i < core_->tracks_to_play_.size(); ++i) {
        switch (core_->tracks_to_play_[i]) {
            case track_state::PLAYED:
                core_->tracks_to_play_[i] = track_state::DISCARDED;
            break;
            case track_state::AWAITS_FOR_LOOP:
                core_->tracks_to_play_[i] = track_state::AWAITS_FOR_ONE_PLAY;
            break;
            default: break;
        }
        if (result.count(i)) {
            core_->tracks_to_play_[i] = track_state::AWAITS_FOR_LOOP;
        }
    }
}

void player::add_tracks_to_queue(const std::string& src) {
    parse_array(src);
}

void player::start() {
    if (!core_) {
        return;
    }
    if (core_->play_thread_.joinable()) {
        return;
    }

    core_->play_thread_ = std::thread(&player::play, this);
}

void player::play() {
    using namespace std::chrono_literals;
    
    while (true) {
        for (auto i = 0; i < count_tracks; ++i) {
            while (audio::track::is_playing()) {} // wait until playing

            std::lock_guard<std::mutex> lock(core_->tracks_list_mutex_);
            if (core_->tracks_to_play_[i] == track_state::DISCARDED) {
                continue;
            }

            auto current_time = std::chrono::system_clock::now();
            auto diff = current_time - core_->timeouts_[i];

            if (diff >= core_->periods_[i]) {
                if (core_->tracks_to_play_[i] == track_state::AWAITS_FOR_ONE_PLAY) {
                    core_->tracks_to_play_[i] = track_state::DISCARDED;
                } else {
                    core_->tracks_to_play_[i] = track_state::PLAYED;
                }
                core_->tracks_[i].play();
                core_->timeouts_[i] = current_time;
            }
        }
    }
}