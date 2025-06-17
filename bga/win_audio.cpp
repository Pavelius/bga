#include "adat.h"
#include "audio.h"
#include "io_stream.h"
#include "mtevent.h"
#include "slice.h"
#include "thread.h"

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

#pragma comment(lib, "winmm.lib")

#define WINBASEAPI extern "C"
#define WINAPI __stdcall
#define CALLBACK __stdcall
#define WORD short unsigned
#define DWORD unsigned

/* wave callback messages */
#define MM_WOM_OPEN         0x3BB           /* waveform output */
#define MM_WOM_CLOSE        0x3BC
#define MM_WOM_DONE         0x3BD
#define WOM_OPEN    MM_WOM_OPEN
#define WOM_CLOSE   MM_WOM_CLOSE
#define WOM_DONE    MM_WOM_DONE

#define CALLBACK_TYPEMASK   0x00070000l    /* callback type mask */
#define CALLBACK_NULL       0x00000000l    /* no callback */
#define CALLBACK_WINDOW     0x00010000l    /* dwCallback is a HWND */
#define CALLBACK_TASK       0x00020000l    /* dwCallback is a HTASK */
#define CALLBACK_FUNCTION   0x00030000l    /* dwCallback is a FARPROC */

struct WAVEFORMATEX {
    WORD		wFormatTag;
    WORD		nChannels;
    DWORD		nSamplesPerSec;
    DWORD		nAvgBytesPerSec;
    WORD		nBlockAlign;
    WORD		wBitsPerSample;
    WORD		cbSize;
};
struct WAVEHDR {
    char*       lpData;
    unsigned    dwBufferLength;
    unsigned    dwBytesRecorded;
    unsigned*   dwUser;
    unsigned    dwFlags;
    unsigned    dwLoops;
    WAVEHDR*	lpNext;
    unsigned*   reserved;
};

WINBASEAPI int WINAPI waveOutReset(void* hwo);
WINBASEAPI int WINAPI waveOutClose(void* hwo);
WINBASEAPI int WINAPI waveOutGetVolume(void* hwo, unsigned* pdwVolume);
WINBASEAPI int WINAPI waveOutOpen(void** phwo, unsigned uDeviceID, const struct WAVEFORMATEX* pwfx, void* dwCallback, void* dwInstance, unsigned fdwOpen);
WINBASEAPI int WINAPI waveOutPrepareHeader(void* hwo, WAVEHDR* pwh, unsigned int cbwh);
WINBASEAPI int WINAPI waveOutUnprepareHeader(void* hwo, WAVEHDR* pwh, unsigned int cbwh);
WINBASEAPI int WINAPI waveOutWrite(void* hwo, WAVEHDR* pwh, unsigned int cbwh);

struct channelinfo {
    WAVEHDR         header;
    void*           handle;
    bool            playing;
    short unsigned  volume;
    fnaudiocb       stopping;
    void*           object; // user defined object
    explicit operator bool() const { return handle != 0; }
    int getindex() const;
    void clear();
    void reset();
};
static channelinfo channels[16];

int channelinfo::getindex() const {
    return this - channels;
}

void channelinfo::clear() {
    memset(this, 0, sizeof(*this));
    volume = 0xFFFF;
}

void channelinfo::reset() {
    if(handle) {
        if(waveOutReset(handle) == 0)
            waveOutClose(handle);
    }
    handle = 0;
    playing = false;
}

void audio_update_channels() {
    for(auto& e : channels) {
        if(e.handle && !e.playing) {
            e.reset();
            if(e.stopping)
                e.stopping(e.getindex());
        }
    }
}

static channelinfo* find_channel() {
    audio_update_channels();
    for(auto& e : channels) {
        if(e.handle)
            continue;
        return &e;
    }
    return 0;
}

static void CALLBACK audio_callback(void* hWaveOut, unsigned int uMsg, unsigned int dwInstance, unsigned int dwParam1, unsigned int dwParam2) {
    if(uMsg == WOM_DONE) {
        waveOutUnprepareHeader(hWaveOut, (WAVEHDR*)dwParam1, sizeof(WAVEHDR));
        auto p = (channelinfo*)dwInstance;
        p->playing = false;
    }
}

short unsigned audio_get_volume(int channel) {
    unsigned value = 0xFFFF;
    if(waveOutGetVolume(channels[channel].handle, &value)==0)
        return (short unsigned)(value & 0xFFFF);
    return 0;
}

void audio_set_volume(int channel, short unsigned volume) {
    auto p = channels + channel;
    if(p->volume == volume)
        return;
}

void* audio_get_object(int channel) {
    return channels[channel].object;
}

void audio_play_memory(int channel, void* object, fnaudiocb callback, void* callback_object) {
    auto ph = (wav*)object;
    auto p = (channel==-1) ? find_channel() : channels + channel;
    WAVEFORMATEX wfx = {0};
    wfx.cbSize = 0;
    wfx.wFormatTag = 1;
    wfx.nChannels = ph->numChannels;
    wfx.nSamplesPerSec = ph->sampleRate;
    wfx.wBitsPerSample = ph->bitsPerSample;
    wfx.nBlockAlign = wfx.nChannels * wfx.wBitsPerSample / 8;
    wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;
    p->clear();
    p->playing = true;
    p->stopping = callback;
    p->object = callback_object;
    waveOutOpen(&p->handle, (DWORD)-1, &wfx, (void*)audio_callback, p, CALLBACK_FUNCTION);
    if(p->handle) {
        p->header = {};
        p->header.dwBufferLength = ph->subchunk2Size;
        p->header.lpData = (char*)ph + sizeof(*ph);
        waveOutPrepareHeader(p->handle, &p->header, sizeof(WAVEHDR));
        waveOutWrite(p->handle, &p->header, sizeof(WAVEHDR));
    }
}