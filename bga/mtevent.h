#pragma once

namespace io {
struct event {
	void* data;
	event();
	~event();
	void post();
	void wait();
};
}
