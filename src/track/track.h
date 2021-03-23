#pragma once

#include <SDL2/SDL.h>

#include <string>

namespace audio {


	class track {
	public:
		track(std::string track_name);
		~track();
	public:
		void play();
		void stop();
		bool is_playing();
		void close();
		std::string get_name();
	private:
		unsigned int wav_length_;
		unsigned char* wav_buffer_;
		SDL_AudioSpec wav_spec_;
		const std::string track_name_;
		SDL_AudioDeviceID deviceId_;
	};

}
