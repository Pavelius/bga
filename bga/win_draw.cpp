#include "draw.h"
#include "win.h"
#include "slice.h"

using namespace draw;

#pragma pack(push)
#pragma pack(1)
static struct video_8t {
	BITMAPINFO		bmp;
	unsigned char	bmp_pallette[256 * 4];
} video_descriptor;
#pragma pack(pop)

static HWND		hwnd;
static point	minimum;

static struct sys_key_mapping {
	unsigned key;
	unsigned id;
} sys_key_mapping_data[] = {{VK_CONTROL, Ctrl},
	{VK_MENU, Alt},
	{VK_SHIFT, Shift},
	{VK_LEFT, KeyLeft},
	{VK_RIGHT, KeyRight},
	{VK_UP, KeyUp},
	{VK_DOWN, KeyDown},
	{VK_PRIOR, KeyPageUp},
	{VK_NEXT, KeyPageDown},
	{VK_HOME, KeyHome},
	{VK_END, KeyEnd},
	{VK_BACK, KeyBackspace},
	{VK_DELETE, KeyDelete},
	{VK_RETURN, KeyEnter},
	{VK_ESCAPE, KeyEscape},
	{VK_SPACE, KeySpace},
	{VK_TAB, KeyTab},
	{VK_F1, F1},
	{VK_F2, F2},
	{VK_F3, F3},
	{VK_F4, F4},
	{VK_F5, F5},
	{VK_F6, F6},
	{VK_F7, F7},
	{VK_F8, F8},
	{VK_F9, F9},
	{VK_F10, F10},
	{VK_F11, F11},
	{VK_F12, F12},
	{VK_MULTIPLY, (unsigned)'*'},
	{VK_DIVIDE, (unsigned)'/'},
	{VK_ADD, (unsigned)'+'},
	{VK_SUBTRACT, (unsigned)'-'},
	{VK_OEM_COMMA, (unsigned)','},
	{VK_OEM_PERIOD, (unsigned)'.'},
};

static int tokey(unsigned key) {
	for(auto& e : sys_key_mapping_data) {
		if(e.key == key)
			return e.id;
	}
	return key;
}

static void set_cursor(cursor e) {
	static void* data[] = {
		LoadCursorA(0, (char*)32512),//IDC_ARROW
		LoadCursorA(0, (char*)32649),//IDC_HAND
		LoadCursorA(0, (char*)32644),//IDC_SIZEWE
		LoadCursorA(0, (char*)32645),//IDC_SIZENS
		LoadCursorA(0, (char*)32646),//IDC_SIZEALL
		LoadCursorA(0, (char*)32648),//IDC_NO
		LoadCursorA(0, (char*)32513),//IDC_IBEAM
		LoadCursorA(0, (char*)32514),//IDC_WAIT
	};
	SetCursor(data[static_cast<int>(e)]);
}

static int handle(MSG& msg) {
	switch(msg.message) {
	case WM_MOUSEMOVE:
		if(msg.hwnd != hwnd)
			break;
		hot.mouse.x = LOWORD(msg.lParam);
		hot.mouse.y = HIWORD(msg.lParam);
		return MouseMove;
	case WM_LBUTTONDOWN:
		if(msg.hwnd != hwnd)
			break;
		left_mouse_press = hot.mouse;
		hot.pressed = true;
		return MouseLeft;
	case WM_LBUTTONDBLCLK:
		if(msg.hwnd != hwnd)
			break;
		hot.pressed = true;
		return MouseLeftDBL;
	case WM_LBUTTONUP:
		if(msg.hwnd != hwnd)
			break;
		hot.pressed = false;
		return MouseLeft;
	case WM_RBUTTONDOWN:
		right_mouse_press = hot.mouse;
		hot.pressed = true;
		return MouseRight;
	case WM_RBUTTONUP:
		hot.pressed = false;
		return MouseRight;
	case WM_MOUSEWHEEL:
		if(msg.wParam & 0x80000000)
			return MouseWheelDown;
		else
			return MouseWheelUp;
		break;
	case WM_MOUSEHOVER:
		return InputIdle;
	case WM_TIMER:
		if(msg.hwnd != hwnd)
			break;
		if(msg.wParam == InputTimer)
			return InputTimer;
		break;
	case WM_KEYDOWN:
		return tokey(msg.wParam);
	case WM_KEYUP:
		return InputKeyUp;
	case WM_CHAR:
		hot.param = msg.wParam;
		return InputSymbol;
	case WM_MY_SIZE:
	case WM_SIZE:
		return InputUpdate;
	}
	return 0;
}

static LRESULT CALLBACK WndProc(HWND hwnd, unsigned uMsg, WPARAM wParam, LPARAM lParam) {
	MSG msg;
	switch(uMsg) {
	case WM_ERASEBKGND:
		if(draw::canvas) {
			RECT rc; GetClientRect(hwnd, &rc);
			video_descriptor.bmp.bmiHeader.biSize = sizeof(video_descriptor.bmp.bmiHeader);
			video_descriptor.bmp.bmiHeader.biWidth = draw::canvas->width;
			video_descriptor.bmp.bmiHeader.biHeight = -draw::canvas->height;
			video_descriptor.bmp.bmiHeader.biBitCount = draw::canvas->bpp;
			video_descriptor.bmp.bmiHeader.biPlanes = 1;
			if(rc.right != draw::canvas->width || rc.bottom != draw::canvas->height)
				StretchDIBits((void*)wParam,
					0, 0, rc.right, rc.bottom,
					0, 0, draw::canvas->width, draw::canvas->height,
					draw::canvas->bits, &video_descriptor.bmp, DIB_RGB_COLORS, SRCCOPY);
			else
				SetDIBitsToDevice((void*)wParam,
					0, 0, rc.right, rc.bottom,
					0, 0, 0, draw::canvas->height,
					draw::canvas->bits, &video_descriptor.bmp, DIB_RGB_COLORS);
		}
		return 1;
	case WM_CLOSE:
		PostQuitMessage(-1);
		return 0;
	case WM_EXITSIZEMOVE:
	case WM_SIZE:
		if(!PeekMessageA(&msg, hwnd, WM_MY_SIZE, WM_MY_SIZE, 0))
			PostMessageA(hwnd, WM_MY_SIZE, 0, 0);
		return 0;
	case WM_GETMINMAXINFO:
		((MINMAXINFO*)lParam)->ptMinTrackSize.x = minimum.x;
		((MINMAXINFO*)lParam)->ptMinTrackSize.y = minimum.y;
		return 0;
	}
	return DefWindowProcA(hwnd, uMsg, wParam, lParam);
}

static const char* register_class(const char* class_name) {
	WNDCLASS wc;
	if(!GetClassInfoA(GetModuleHandleA(0), class_name, &wc)) {
		memset(&wc, 0, sizeof(wc));
		wc.style = CS_OWNDC | CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW; // Own DC For Window.
		wc.lpfnWndProc = WndProc;	// WndProc Handles Messages
		wc.hInstance = GetModuleHandleA(0);	// Set The Instance
		wc.hIcon = (void*)LoadIconA(wc.hInstance, (const char*)1); // WndProc Handles Messages
		wc.lpszClassName = class_name; // Set The Class Name
		RegisterClassA(&wc); // Attempt To Register The Window Class
	}
	return class_name;
}

void draw::getwindowpos(point& pos, point& size, unsigned* flags) {
	RECT rc;
	GetClientRect(hwnd, &rc);
	size.x = (short)(rc.right - rc.left);
	size.y = (short)(rc.bottom - rc.top);
	GetWindowRect(hwnd, &rc);
	pos.x = (short)rc.left;
	pos.y = (short)rc.top;
	if(flags) {
		WINDOWPLACEMENT wp;
		GetWindowPlacement(hwnd, &wp);
		*flags = 0;
		auto wf = GetWindowLongA(hwnd, GWL_STYLE);
		if(wp.showCmd == SW_SHOWMAXIMIZED)
			*flags |= WFMaximized;
		if(wf & WS_THICKFRAME)
			*flags |= WFResize;
		if(wf & WS_MINIMIZEBOX)
			*flags |= WFMinmax;
	}
}

void draw::updatewindow() {
	if(!hwnd)
		return;
	if(!IsWindowVisible(hwnd))
		ShowWindow(hwnd, SW_SHOW);
	InvalidateRect(hwnd, 0, 1);
	UpdateWindow(hwnd);
}

void draw::syscursor(bool enable) {
	ShowCursor(enable ? 1 : 0);
}

void draw::create(int x, int y, int width, int height, unsigned flags, int bpp) {
	if(!bpp)
		bpp = draw::canvas->bpp;
	auto screen_w = GetSystemMetrics(SM_CXFULLSCREEN);
	auto screen_h = GetSystemMetrics(SM_CYFULLSCREEN);
	if(!width)
		width = (screen_w / 3) * 2;
	if(!height)
		height = (screen_h / 3) * 2;
	// custom
	unsigned dwStyle = WS_CAPTION | WS_SYSMENU; // Windows Style;
	RECT MinimumRect = {0, 0, width, height};
	AdjustWindowRectEx(&MinimumRect, dwStyle, 0, 0);
	minimum.x = 800;
	if(minimum.x > width)
		minimum.x = width;
	minimum.y = 600;
	if(minimum.y > height)
		minimum.y = height;
	if(x == -1)
		x = (screen_w - minimum.x) / 2;
	if(y == -1)
		y = (screen_h - minimum.y) / 2;
	// Update current surface
	if(draw::canvas)
		draw::canvas->resize(width, height, bpp, true);
	setclip();
	// Create The Window
	hwnd = CreateWindowExA(0, register_class("CFaceWindow"), 0, dwStyle,
		x, y,
		MinimumRect.right - MinimumRect.left,
		MinimumRect.bottom - MinimumRect.top,
		0, 0, GetModuleHandleA(0), 0);
	if(!hwnd)
		return;
	ShowWindow(hwnd, SW_SHOWNORMAL);
	// Update mouse coordinates
	POINT pt; GetCursorPos(&pt);
	ScreenToClient(hwnd, &pt);
	hot.mouse.x = (short)pt.x;
	hot.mouse.y = (short)pt.y;
}

static unsigned handle_event(unsigned m) {
	if(m < InputSymbol || m > InputNoUpdate) {
		if(GetKeyState(VK_SHIFT) < 0)
			m |= Shift;
		if(GetKeyState(VK_MENU) < 0)
			m |= Alt;
		if(GetKeyState(VK_CONTROL) < 0)
			m |= Ctrl;
	} else if(m == InputUpdate) {
		if(canvas) {
			RECT rc; GetClientRect(hwnd, &rc);
			canvas->resize(rc.right - rc.left, rc.bottom - rc.top, 32, true);
			setclip();
		}
	}
	return m;
}

void draw::doredraw() {
	MSG	msg;
	updatewindow();
	if(!hwnd)
		return;
	while(PeekMessageA(&msg, 0, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessageA(&msg);
		handle_event(handle(msg));
	}
}

int draw::rawinput() {
	MSG	msg;
	updatewindow();
	if(!hwnd)
		return 0;
	while(GetMessageA(&msg, 0, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessageA(&msg);
		unsigned m = handle(msg);
		if(m == InputNoUpdate || m == MouseMove)
			continue;
		if(m) {
			m = handle_event(m);
			return m;
		}
	}
	return 0;
}

void draw::setcaption(const char* string) {
	SetWindowTextA(hwnd, string);
}

void draw::settimer(unsigned milleseconds) {
	if(milleseconds)
		SetTimer(hwnd, InputTimer, milleseconds, 0);
	else
		KillTimer(hwnd, InputTimer);
}