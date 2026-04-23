#include "adat.h"
#include "audio.h"
#include "io_stream.h"
#include "slice.h"

namespace {
// WAV file header structure
struct wav {
	// RIFF chunk descriptor
	char            chunkID[4]; // "RIFF"
	unsigned        chunkSize;  // Size of the entire file in bytes minus 8 bytes
	char            format[4];  // "WAVE"
	// format sub-chunk
	char            subchunk1ID[4]; // "fmt "
	unsigned        subchunk1Size; // Size of the fmt chunk (16 for PCM)
	unsigned short  audioFormat;   // Audio format (1 for PCM)
	unsigned short  numChannels;   // Number of channels (1 = mono, 2 = stereo)
	unsigned        sampleRate;    // Sampling rate (e.g., 44100 Hz)
	unsigned        byteRate;      // Byte rate = SampleRate * NumChannels * BitsPerSample / 8
	unsigned short  blockAlign;    // Block align = NumChannels * BitsPerSample / 8
	unsigned short  bitsPerSample; // Bits per sample (e.g., 8, 16, 24, 32)
	// data sub-chunk
	char            subchunk2ID[4]; // "data"
	unsigned        subchunk2Size; // Size of the data chunk (number of bytes of audio data)
};
}

enum channelplayn : unsigned char {
	ChannelReady, ChannelPlayed, ChannelDone,
};

struct channelinfo {
	void*		object; // wav data playing
	volatile channelplayn mode;
	void*       handle;
	explicit operator bool() const { return handle != 0; }
	int getindex() const;
};
static channelinfo music_channel;
static channelinfo channels[16];

int channelinfo::getindex() const {
	return this - channels;
}

void audio_update_channels() {
}

void audio_reset() {
}

void play_music_raw(void* object) {
}

void audio_play(void* object) {
}

bool audio_played(const void* object) {
	if(!object)
		return false;
	for(auto& e : channels) {
		if(e.object == object)
			return true;
	}
	return false;
}

int audio_lenght(const void* object) {
	auto ph = (wav*)object;
	if(!ph || !ph->byteRate)
		return 0;
	auto r = (unsigned long long)ph->subchunk2Size * 1000;
	return (int)(r / ph->byteRate);
}