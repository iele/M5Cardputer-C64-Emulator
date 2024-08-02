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
#include "sid.h"

#include <stdint.h>

#ifndef play_sid_h_
#define play_sid_h_
//#define SAMPLERATE 11100
#define SAMPLERATE 22050
#define AUDIO_BLOCK_SAMPLES 512
#define AUDIO_BLOCK_TIME ( AUDIO_BLOCK_SAMPLES / (SAMPLERATE / 1000) )
//#define SAMPLERATE 6500
#define CLOCKFREQ 985248


class AudioPlaySID
{
public:
	AudioPlaySID(void) { begin(); }
	void begin(void);
	void setSampleParameters(float clockfreq, float samplerate);
	inline void setreg(int ofs, int val) { sid.write(ofs, val); }
	inline uint8_t getreg(int ofs) { return sid.read(ofs); }
	void reset(void);
	void stop(void);
	void update(void * stream, int len);
	inline bool isPlaying(void) { return playing; }
  cycle_count csdelta;
  SID* sidptr;
private:

	volatile bool playing;
	SID sid;

};


#endif
