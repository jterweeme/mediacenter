#ifndef _MISC_H_
#define _MISC_H_
#include <Windows.h>
#include <stdint.h>

class Message
{
public:
	static const uint16_t CLOSE = WM_CLOSE;
	static const uint16_t COMMAND = WM_COMMAND;
    static const uint16_t CREATE = WM_CREATE;
	static const uint16_t DESTROY = WM_DESTROY;
    static const uint16_t SIZE = WM_SIZE;
};

class WindowHandle
{
protected:
    HWND handle;
public:
    HWND getHandle() { return handle; }

    static const uint32_t VIS = WS_VISIBLE;
    static const uint32_t CHILD = WS_CHILD;
};

class WinClassEx
{
protected:
	WNDCLASSEX wclass;
	const wchar_t *className;
	HINSTANCE hinst;
public:
	WinClassEx(WNDPROC wp, const wchar_t *className, HINSTANCE hinst);
	void registerClass() { ::RegisterClassEx(&wclass); }
	const wchar_t *getClassName() { return className; }
	HINSTANCE getHinst() { return hinst; }
};

class MenuBar
{
private:
	HMENU menu;
public:
	MenuBar(HINSTANCE hinst, int id) { menu = ::LoadMenu(hinst, MAKEINTRESOURCE(id)); }
	void setMenu(HWND hwnd) { ::SetMenu(hwnd, menu); }
};

class GenericWindow : public WindowHandle
{
protected:
	WinClassEx *wclass;
public:
    GenericWindow(WinClassEx *wclass, const wchar_t *caption);
    GenericWindow(WinClassEx *wclass) : GenericWindow(wclass, L"Generic Window") { }
	void show(int cmdShow);
    void update() { ::UpdateWindow(handle); }
	void setMenuBar(MenuBar &menuBar) { menuBar.setMenu(handle); }
};

class MDIClient : public WindowHandle
{
public:
    MDIClient(WinClassEx *wclass, HWND parent, CLIENTCREATESTRUCT &css)
    {
        handle = ::CreateWindowEx(WS_EX_CLIENTEDGE, wclass->getClassName(),
            NULL, CHILD | WS_CLIPCHILDREN | WS_VSCROLL | WS_HSCROLL,
            CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
            parent, 0, wclass->getHinst(), (LPVOID)&css);
    }

    void show() { ::ShowWindow(handle, SW_SHOW); }
};

class FileBrowser
{
private:
	OPENFILENAME ofn;
public:
	FileBrowser(GenericWindow gw);
	void browse();
};

#endif

