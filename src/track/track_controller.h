#pragma once

#include <string>

namespace audio {
class track_controller {
public:
    virtual ~track_controller() = default;

public:
    virtual void play() = 0;
	virtual void stop() = 0;
	virtual bool is_playing() = 0;
    virtual void close() = 0;
    virtual std::string get_name() = 0;
};
} // namespace audio
