#include "mtevent.h"

// #include "windows.h"

#define WINBASEAPI extern "C" __declspec(dllimport)
#define WINAPI _stdcall

#define INFINITE 0xFFFFFFFF  // Infinite timeout

WINBASEAPI int WINAPI CloseHandle(void* hObject);
WINBASEAPI void* WINAPI CreateEventA(void* lpEventAttributes, int bManualReset, int bInitialState, const char* lpName);
WINBASEAPI int WINAPI SetEvent(void* hEvent);
WINBASEAPI unsigned long WINAPI WaitForSingleObject(void* hHandle, unsigned long dwMilliseconds);

io::event::event() : data(CreateEventA(0, false, false, 0)) {
}

io::event::~event() {
	CloseHandle(data);
}

void io::event::post() {
	SetEvent(data);
}

void io::event::wait() {
	WaitForSingleObject(data, INFINITE);
}