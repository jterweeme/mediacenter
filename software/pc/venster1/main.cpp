#include <Windows.h>
#include <stdint.h>
#include <iostream>
#include "resource.h"

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

class MyWinClass : public WinClass
{
public:
	MyWinClass(WNDPROC wp, HINSTANCE hinst) : WinClass(wp, L"dinges", hinst)
	{
		wclass.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
	}
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
};

class MainWindow : public GenericWindow
{
public:
	MainWindow(WinClass *wclass) : GenericWindow(wclass) { }
};

class MainClass
{
public:
	MainClass(HINSTANCE h, HINSTANCE p, char *c, int m);
	int mainLoop();
private:
	static LRESULT CALLBACK WindowProcedure(HWND h, unsigned int msg, WPARAM w, LPARAM l);
};

GenericWindow::GenericWindow(WinClass *wclass)
{
	this->wclass = wclass;

	hwnd = ::CreateWindow(
				wclass->getClassName(),
				L"Generic Window",
				WS_OVERLAPPEDWINDOW,
				CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 0, 0, wclass->getHinst(), 0);
}

WinClass::WinClass(WNDPROC wp, const wchar_t *className, HINSTANCE hinst)
{
	this->className = className;
	this->hinst = hinst;
	wclass.style = 0;
	wclass.lpfnWndProc = wp;
	wclass.cbClsExtra = 0;
	wclass.cbWndExtra = 0;
	wclass.hInstance = hinst;
	wclass.hIcon = 0;
	wclass.hCursor = ::LoadCursor(0, IDC_ARROW);
	wclass.lpszClassName = className;
	wclass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wclass.lpszMenuName = 0;
}

MainClass::MainClass(HINSTANCE h, HINSTANCE, char *c, int m)
{
	MyWinClass wclass(WindowProcedure, h);
	wclass.registerClass();
	MainWindow mw(&wclass);
	mw.Show(m);
}

void GenericWindow::Show(int cmdShow)
{
	::ShowWindow(hwnd, cmdShow);
	::UpdateWindow(hwnd);
}

int MainClass::mainLoop()
{
	MSG msg;
	int status;

	while ((status = ::GetMessage(&msg, 0, 0, 0)) != 0)
	{
		if (status == -1)
			return -1;

		::DispatchMessage(&msg);
	}

	return msg.wParam;
}

int WINAPI 
WinMain(HINSTANCE h, HINSTANCE p, char *c, int m)
{
	MainClass mc(h, p, c, m);
	return mc.mainLoop();
}

LRESULT CALLBACK 
MainClass::WindowProcedure(HWND h, unsigned int msg, WPARAM w, LPARAM l)
{
	switch (msg)
	{
	case WM_DESTROY:
		::PostQuitMessage(0);
		return 0;
	}
	return ::DefWindowProc(h, msg, w, l);
}
