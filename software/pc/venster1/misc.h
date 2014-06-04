#ifndef _MISC_H_
#define _MISC_H_
#include <Windows.h>
#include <stdint.h>

class Message
{
public:
	static const uint16_t CLOSE = WM_CLOSE;
	static const uint16_t COMMAND = WM_COMMAND;
	static const uint16_t DESTROY = WM_DESTROY;
};

class WinClass
{
protected:
	WNDCLASS wclass;
	const wchar_t *className;
	HINSTANCE hinst;
public:
	WinClass(WNDPROC wp, const wchar_t *className, HINSTANCE hinst);
	void registerClass() { ::RegisterClass(&wclass); }
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

class GenericWindow
{
protected:
	HWND hwnd;
	WinClass *wclass;
public:
	GenericWindow(WinClass *wclass);
	void Show(int cmdShow);
	void setMenuBar(MenuBar &menuBar) { menuBar.setMenu(hwnd); }
	HWND getHandle() { return hwnd; }
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

