#include <track.h>

#include <SDL2/SDL.h>

#include <exception>
#include <stdexcept>
#include <iostream>

namespace audio {

	track::track(std::string track_name) : track_name_(track_name) {
		if (SDL_LoadWAV(track_name.c_str(), &wav_spec_, &wav_buffer_, &wav_length_) == NULL) {
			throw std::invalid_argument(std::string("can't load track: ") + track_name);
		}
	}

	void track::play() {
		deviceId_ = SDL_OpenAudioDevice(NULL, 0, &wav_spec_, NULL, 0);
		if (SDL_QueueAudio(deviceId_, wav_buffer_, wav_length_) < 0) {
			throw std::runtime_error(std::string("can't open audio ") + track_name_);
		}
		SDL_PauseAudioDevice(deviceId_, 0);
	}

	std::string track::get_name() {
		return track_name_;
	}

	void track::stop() {
		if (deviceId_) {
			SDL_PauseAudioDevice(deviceId_, 1);
			close();
		}
		deviceId_ = 0;
	}

	bool track::is_playing() {
		return SDL_GetQueuedAudioSize(deviceId_) > 0 && SDL_GetAudioDeviceStatus(deviceId_) == SDL_AUDIO_PLAYING;
	}

	track::~track() {
		SDL_FreeWAV(wav_buffer_);
	}

	void track::close() {
		SDL_ClearQueuedAudio(deviceId_);
		SDL_CloseAudioDevice(deviceId_);
	}

}
