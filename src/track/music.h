#pragma once

#include "track_controller.h"

#include <SDL2/SDL.h>

#include <SDL2/SDL_mixer.h>

#include <string>

namespace audio {
	class music: public track_controller {
	public:
		music(std::string track_name);
		~music() override;

	public:
		void play() override;
		void stop() override;
		bool is_playing() override;
		void close() override;
		std::string get_name() override;

	private:
		const std::string track_name_;
		Mix_Music* music_;
	};

}
