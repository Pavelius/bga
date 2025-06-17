#pragma once

namespace io {
class thread {
	unsigned long s;
public:
	typedef void (*fnroutine)(void* param);
	constexpr thread() : s(-1) {}
	thread(fnroutine proc, void* param = 0) : s(-1) { start(proc, param); }
	~thread() { close(); }
	constexpr explicit operator bool() const { return s != -1; }
	void close();
	void join();
	void resume();
	void start(fnroutine proc, void* param = 0);
	void suspend();
	void wait(unsigned long milliseconds);
};
}
