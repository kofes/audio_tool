#pragma once

#include "track_controller.h"

#include <SDL2/SDL.h>

#include <string>

namespace audio {
	class track: public track_controller {
	public:
		track(std::string track_name);
		~track();
	public:
		void play() override;
		void stop() override;
		bool is_playing() override;
		void close() override;
		std::string get_name() override;
	private:
		unsigned int wav_length_;
		unsigned char* wav_buffer_;
		SDL_AudioSpec wav_spec_;
		const std::string track_name_;
		SDL_AudioDeviceID deviceId_;
	};

}
