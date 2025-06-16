#include "win.h"

static void* hWaveOut;

static void initialize_audio() {
}

static void put_audio_block(char* block, size_t size) {
    WAVEHDR header = {};
    header.dwBufferLength = size;
    header.lpData = block;
    waveOutPrepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
    waveOutWrite(hWaveOut, &header, sizeof(WAVEHDR));
    // WaitForSingleObject(waveDone, INFINITE);
    waveOutUnprepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
}