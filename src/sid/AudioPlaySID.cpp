/*
	Arduino SID
	Copyright (c) 2015 Frank Bösing
	This library is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.
	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.
	You should have received a copy of the GNU General Public License
	along with this library.  If not, see <http://www.gnu.org/licenses/>.

	Diese Bibliothek ist freie Software: Sie können es unter den Bedingungen
	der GNU General Public License, wie von der Free Software Foundation,
	Version 3 der Lizenz oder (nach Ihrer Wahl) jeder neueren
	veröffentlichten Version, weiterverbreiten und/oder modifizieren.
	Diese Bibliothek wird in der Hoffnung, dass es nützlich sein wird, aber
	OHNE JEDE GEWÄHRLEISTUNG, bereitgestellt; sogar ohne die implizite
	Gewährleistung der MARKTFÄHIGKEIT oder EIGNUNG FÜR EINEN BESTIMMTEN ZWECK.
	Siehe die GNU General Public License für weitere Details.
	Sie sollten eine Kopie der GNU General Public License zusammen mit diesem
	Programm erhalten haben. Wenn nicht, siehe <http://www.gnu.org/licenses/>.

 */
#include "AudioPlaySID.h"
#include "sid.h"
#include <math.h>

// #include <esp32-hal-ledc.h>

void AudioPlaySID::begin(void)
{
	//    hw_timer_t * timer = NULL;
	sidptr = &sid;
	this->reset();
	setSampleParameters(CLOCKFREQ, SAMPLERATE);
	playing = true;
}

void AudioPlaySID::setSampleParameters(float clockfreq, float samplerate)
{
	sid.set_sampling_parameters(clockfreq, SAMPLE_FAST, samplerate);
	AudioPlaySID::csdelta = round((float)clockfreq / ((float)samplerate));
}

void AudioPlaySID::reset(void)
{
	sid.reset();
}

void AudioPlaySID::stop(void)
{
	playing = false;
}

void AudioPlaySID::update(void *stream, int len)
{
	// only update if we're playing
	//	if (!playing) return;
	//
	//	cycle_count delta_t = csdelta;
	//
	//	sidptr->clock(delta_t);
}
