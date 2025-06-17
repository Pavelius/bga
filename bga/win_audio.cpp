#include "adat.h"
#include "io_stream.h"
#include "mtevent.h"
#include "slice.h"
#include "thread.h"
#include "win.h"

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

WINBASEAPI int WINAPI waveOutReset(void* hwo);
WINBASEAPI int WINAPI waveOutClose(void* hwo);

struct wavinfo {
    wavinfo();
    ~wavinfo();
    WAVEHDR header;
    void* data;
    bool playing;
};

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

static wavinfo channels[16];

static void cleanup_channels() {
    for(auto& e : channels) {
        if(e.data && !e.playing) {
            if(waveOutReset(e.data)==0)
                waveOutClose(e.data);
            e.data = 0;
            e.playing = false;
        }
    }
}

static wavinfo* find_channel() {
    cleanup_channels();
    for(auto& e : channels) {
        if(e.data)
            continue;
        return &e;
    }
    return 0;
}

wavinfo::wavinfo() : data(0), header{0}, playing(false) {
}

wavinfo::~wavinfo() {
}

static void* audio_open() {
}

static void audio_close() {
}

static void CALLBACK audio_callback(void* hWaveOut, unsigned int uMsg, unsigned int dwInstance, unsigned int dwParam1, unsigned int dwParam2) {
    if(uMsg == WOM_DONE) {
        auto p = (wavinfo*)dwInstance;
        waveOutUnprepareHeader(hWaveOut, (WAVEHDR*)dwParam1, sizeof(WAVEHDR));
        p->playing = false;
    }
}

void audio_play_memory(void* object) {
    auto ph = (wav*)object;
    wavinfo* p = find_channel();
    WAVEFORMATEX wfx = {0};
    wfx.cbSize = 0;
    wfx.wFormatTag = 1;
    wfx.nChannels = ph->numChannels; // Mono
    wfx.nSamplesPerSec = ph->sampleRate;
    wfx.wBitsPerSample = ph->bitsPerSample;
    wfx.nBlockAlign = wfx.nChannels * wfx.wBitsPerSample / 8;
    wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;
    p->playing = true;
    waveOutOpen(&p->data, (UINT)-1, &wfx, (void*)audio_callback, p, CALLBACK_FUNCTION);
    if(p->data) {
        p->header = {};
        p->header.dwBufferLength = ph->subchunk2Size;
        p->header.lpData = (char*)ph + sizeof(*ph);
        waveOutPrepareHeader(p->data, &p->header, sizeof(WAVEHDR));
        waveOutWrite(p->data, &p->header, sizeof(WAVEHDR));
    }
}

void audio_play(const char* url) {
    auto p = loadb(url);
    if(!p)
        return;
    audio_play_memory(p);
}