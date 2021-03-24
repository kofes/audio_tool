#include <music.h>

#include <SDL2/SDL.h>

#include <exception>
#include <stdexcept>
#include <iostream>

namespace audio {
	music::music(std::string track_name) : track_name_(track_name) {
		music_ = Mix_LoadMUS(track_name_.c_str());
		if (!music_) {
			throw std::invalid_argument(std::string("can't load track: ") + track_name_);
		}
	}

	void music::play() {
		if (Mix_PlayMusic(music_, 1) == -1) {
			throw std::invalid_argument(std::string("can't play track: ") + track_name_ + ". Error: " + std::string{ Mix_GetError()});
		}
	}

	std::string music::get_name() {
		return track_name_;
	}

	void music::stop() {
		Mix_FreeMusic(music_);
		music_ = nullptr;
	}

	bool music::is_playing() {
		return Mix_PlayingMusic();
	}

	music::~music() {
		Mix_FreeMusic(music_);
	}

	void music::close() {
		Mix_CloseAudio();
	}

}
