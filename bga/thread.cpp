#include "thread.h"

#ifdef _WIN32

#define WINBASEAPI extern "C" __declspec(dllimport)
#define WINAPI _stdcall

#define DWORD unsigned int
#define HANDLE void*

WINBASEAPI int WINAPI CloseHandle(HANDLE hObject);
WINBASEAPI void* WINAPI CreateMutexA(void* lpMutexAttributes, int bInitialOwner, const char* lpName);
WINBASEAPI void* WINAPI CreateThread(void* lpThreadAttributes, int dwStackSize, io::thread::fnroutine lpStartAddress, void* lpParameter, int dwCreationFlags, unsigned* lpThreadId);
WINBASEAPI DWORD WINAPI ResumeThread(HANDLE hThread);
WINBASEAPI DWORD WINAPI SuspendThread(HANDLE hThread);
WINBASEAPI int WINAPI WaitForSingleObject(HANDLE hHandle, int dwMilliseconds);

void io::thread::start(fnroutine proc, void* v) {
	if(s != -1)
		return;
	unsigned id;
	s = (decltype(s))CreateThread(0, 0, proc, v, 0, &id);
}

void io::thread::suspend() {
	if(s == -1)
		return;
	SuspendThread((HANDLE)s);
}

void io::thread::resume() {
	if(s == -1)
		return;
	ResumeThread((HANDLE)s);
}

void io::thread::close() {
	if(s == -1)
		return;
	CloseHandle((HANDLE)s);
	s = -1;
}

void io::thread::join() {
	if(s == -1)
		return;
	WaitForSingleObject((HANDLE)s, -1);
	close();
}

void io::thread::wait(unsigned long milliseconds) {
	if(s == -1)
		return;
	WaitForSingleObject((HANDLE)s, milliseconds);
}

#endif // _WIN32