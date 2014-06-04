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

class FileBrowser
{
private:
	OPENFILENAME ofn;
public:
	FileBrowser();
	void browse();
};

class MainClass
{
private:
	MainClass(HINSTANCE h, int m);
	static MainClass *instance;
	static LRESULT CALLBACK WindowProcedure(HWND h, unsigned int msg, WPARAM w, LPARAM l);
	FileBrowser *fb;
public:
	int mainLoop();
	static MainClass *getInstance(HINSTANCE h, int m);
	static MainClass *getInstance();
	LRESULT CALLBACK control(HWND h, unsigned int msg, WPARAM w, LPARAM l);
};

class MyMenuBar : public MenuBar
{
public:
	static const uint16_t FILE_OPEN = ID_FILE_OPEN;
	static const uint16_t FILE_EXIT = ID_FILE_EXIT;
};

class Message
{
public:
	static const uint16_t CLOSE = WM_CLOSE;
	static const uint16_t COMMAND = WM_COMMAND;
	static const uint16_t DESTROY = WM_DESTROY;
};

MainClass *MainClass::getInstance(HINSTANCE h, int m)
{
	if (!instance)
		instance = new MainClass(h, m);

	return instance;
}

MainClass *MainClass::getInstance()
{
	if (instance)
		return instance;

	return NULL;
}

MainClass *MainClass::instance = NULL;

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

MainClass::MainClass(HINSTANCE h, int m)
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

FileBrowser::FileBrowser()
{
	::ZeroMemory(&ofn, sizeof(ofn));
	/*
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hwnd;
	ofn.lpstrFilter = "Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0\0";
	ofn.lpstrFile = szFileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrDefExt = "txt";*/
}

void FileBrowser::browse()
{
	::GetOpenFileName(&ofn);
}

LRESULT CALLBACK MainClass::control(HWND h, unsigned int msg, WPARAM w, LPARAM l)
{
	switch (msg)
	{
	case Message::COMMAND:
		switch (w)
		{
		case MyMenuBar::FILE_OPEN:
			fb->browse();
			break;
		case MyMenuBar::FILE_EXIT:
			::SendMessage(h, Message::CLOSE, 0, 0);
			break;
		}
		break;
	case Message::DESTROY:
		::PostQuitMessage(0);
		return 0;
	}

	return ::DefWindowProc(h, msg, w, l);
}

LRESULT CALLBACK
MainClass::WindowProcedure(HWND h, unsigned int msg, WPARAM w, LPARAM l)
{
	return getInstance()->control(h, msg, w, l);
}

int WINAPI 
WinMain(HINSTANCE h, HINSTANCE p, char *c, int m)
{
	MainClass *mc = MainClass::getInstance(h, m);
	return mc->mainLoop();
}


