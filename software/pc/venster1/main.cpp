#include <Windows.h>
#include <stdint.h>
#include <iostream>
#include "resource.h"
#include "misc.h"

class MyWinClass : public WinClass
{
public:
	MyWinClass(WNDPROC wp, HINSTANCE hinst) : WinClass(wp, L"dinges", hinst)
	{
		wclass.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
	}
};

class MainWindow : public GenericWindow
{
public:
	MainWindow(WinClass *wclass) : GenericWindow(wclass) { }
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

MainClass::MainClass(HINSTANCE h, int m)
{
	MyWinClass wclass(WindowProcedure, h);
	wclass.registerClass();
	MainWindow mw(&wclass);
	mw.Show(m);
	fb = new FileBrowser(mw);
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


