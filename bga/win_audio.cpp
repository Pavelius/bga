#include "io_stream.h"
#include "win.h"

#pragma comment(lib, "winmm.lib")

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

static void* hWaveOut;

static void audio_open() {
    if(hWaveOut)
        return;
    WAVEFORMATEX waveFormat = {0};
    waveFormat.cbSize = sizeof(waveFormat);
    waveFormat.wFormatTag = 1;
    waveFormat.nChannels = 1; // Mono
    waveFormat.nSamplesPerSec = 44100;
    waveFormat.wBitsPerSample = 16;
    waveFormat.nBlockAlign = waveFormat.nChannels * waveFormat.wBitsPerSample / 8;
    waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
    waveOutOpen(&hWaveOut, (UINT)-1, &waveFormat, 0, 0, 0);
}

static void audio_close() {
    if(hWaveOut)
        return;
}

static void audio_play(char* block, size_t size) {
    WAVEHDR header = {};
    header.dwBufferLength = size;
    header.lpData = block;
    auto error = waveOutPrepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
    if(error)
        return;
    error = waveOutWrite(hWaveOut, &header, sizeof(WAVEHDR));
    if(error)
        return;
}

void audio_play(const char* url) {
    auto p = loadb(url);
    if(!p)
        return;
    auto ph = (wav*)p;
    auto pb = (unsigned char*)p + sizeof(ph);
    audio_open();
    audio_play((char*)pb, ph->subchunk2Size);
}