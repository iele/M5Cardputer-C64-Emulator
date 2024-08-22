#include "SidRegPlayer.h"

void SidRegPlayer::setDefaultConfig(SidRegPlayerConfig *cfg) {
  cfg->samplerate = SAMPLERATE;
  cfg->sid_model = SID_MODEL;
  cfg->clockfreq = CLOCKFREQ;
	cfg->framerate = PAL_FRAMERATE;
}

void SidRegPlayer::begin(SidRegPlayerConfig *cfg)
{
	config = cfg;
	this->reset();
	sid->set_sampling_parameters(config->clockfreq, SAMPLE_FAST, config->samplerate);
	delta_t = config->clockfreq / config->samplerate;

	setFramePeriod(cfg->clockfreq / cfg->framerate);
	// frame_period_us = 1000000 / cfg->framerate;
	// samples_per_frame = round((float)config->samplerate / ((float)cfg->framerate));
	// samples_per_frame = round((float)config->samplerate / ((float)1000000/frame_period_us));

	printf("clockfreq: %d\n", this->config->clockfreq);
	printf("samplerate: %d\n", this->config->samplerate);
	printf("samples per frame: %d\n", getSamplesPerFrame());
	printf("frame period: %ld us\n", getFramePeriod());
	printf("delta_t: %d\n", delta_t);

	playing = true;
}

void SidRegPlayer::reset(void)
{
	sid->reset();
}

void SidRegPlayer::stop(void)
{
	playing = false;
}

/// fill the data with 2 channels
size_t SidRegPlayer::read(uint8_t *buffer, size_t bytes)
{
  size_t result = 0;

  if (!playing)
    return result;

  int16_t *ptr = (int16_t *)buffer;
  for (int j = 0; j < samples_per_frame; j++)
  {
		sid->clock(delta_t);
    int16_t sample = sid->output();
		//ESP_LOGI("SidRegPlayer", "sample: %d", sample);
    *ptr++ = sample;
    *ptr++ = sample;
    result += 4;
  }

  return result;
}
