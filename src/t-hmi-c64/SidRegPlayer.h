#pragma once

#include "reSID/envelope.h"
#include "reSID/extfilt.h"
#include "reSID/filter.h"
#include "reSID/pot.h"
#include "reSID/version.cc"
#include "reSID/voice.h"
#include "reSID/wave.h"
#include "reSID/sid.h"

typedef struct{
    uint16_t samplerate;
    int sid_model;
    int clockfreq;
		float framerate;
    unsigned char *song_data;
    int song_length;
} SidRegPlayerConfig;

class SidRegPlayer
{
public:
	SidRegPlayer(SID *sid) { this->sid = sid;}
	void setDefaultConfig(SidRegPlayerConfig *cfg);
	void begin(SidRegPlayerConfig *cfg);

	inline void setreg(int ofs, int val) { sid->write(ofs, val); }
	inline uint8_t getreg(int ofs) { return sid->read(ofs); }
	void reset(void);
	void stop(void);
	inline bool isPlaying(void) { return playing; }
	size_t read(uint8_t *buffer, size_t bytes);

	// Provides/sets the current frame period in us
	long getFramePeriod() { return(frame_period_us); }
	void setFramePeriod(long period_us) {
		frame_period_us = period_us;
		samples_per_frame = round((float)config->samplerate / ((float)config->clockfreq/frame_period_us));
	}

	// Provides the number of samples per frame
	long getSamplesPerFrame() { return(samples_per_frame); }

private:
	const int SAMPLERATE = 22050;
	const int SID_MODEL = 6581;
	const float PAL_FRAMERATE = 50.0;
	const int CLOCKFREQ = 985248;

	SidRegPlayerConfig *config;

	cycle_count delta_t;					// ratio between system clk and samplerate, ie CLOCKFREQ / SAMPLERATE
	long frame_period_us = 20000;	// raster line time in ms(PAL = 1000/50Hz = 20000 us)
	int samples_per_frame = 441; 	// samplerate / framerate

	volatile bool playing;
	SID *sid;
};
