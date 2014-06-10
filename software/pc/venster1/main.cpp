#include <Windows.h>
#include <stdint.h>
#include <iostream>
#include <CommCtrl.h>
#include "resource.h"
#include "misc.h"
#include "main.h"

HINSTANCE g_hInst;

static HWND mdicclienthandle;

class MyMDIClient : public MDIClient
{
public:
    MyMDIClient(WinClassEx *wclass, HWND parent) : MDIClient(wclass, parent)
    {
        ::mdicclienthandle = this->handle = ::CreateWindowEx(WS_EX_CLIENTEDGE, L"MDI",
            NULL, CHILD | WS_CLIPCHILDREN | WS_VSCROLL | WS_HSCROLL,
            CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
            parent, 0, g_hInst, NULL);
    }
    void show() { ::ShowWindow(mdicclienthandle, SW_SHOW); }
};

class StatusBar : public WindowHandle
{
public:
    StatusBar() { }
    StatusBar(HWND parent)
    {
        handle = ::CreateWindowEx(0, STATUSCLASSNAME, 0, CHILD | VIS | SBARS_SIZEGRIP, 0, 0, 0, 0, parent, 0, g_hInst, 0);
    }
};

StatusBar *gsb;


class ToolBar : public WindowHandle
{
public:
    ToolBar(HWND parent);
    static const uint32_t ID_TOOLBAR = 4998;
    static const uint32_t CM_FILE_NEW = 9070;
    static const uint32_t CM_FILE_OPEN = 9072;
};

ToolBar *tb1;

class MainWindow : public GenericWindow
{
private:
    MainWindow(WinClassEx *wclass) : GenericWindow(wclass, L"Venster1") { fb = new FileBrowser(*this); }
    static MainWindow *instance;
    FileBrowser *fb;
    LRESULT CALLBACK control(HWND h, uint32_t msg, WPARAM w, LPARAM l);
    LRESULT CALLBACK MDIcontrol(HWND h, uint32_t msg, WPARAM w, LPARAM l);
    
public:
    static LRESULT CALLBACK WindowProcedure(HWND h, uint32_t msg, WPARAM w, LPARAM l);
    static LRESULT CALLBACK MDIProc(HWND h, uint32_t msg, WPARAM w, LPARAM l);
    static MainWindow *createInstance(WinClassEx *wc);
    static MainWindow *getInstance();
};



class MainClass
{
private:
	MainClass(HINSTANCE h, int m);
	static MainClass *instance;
public:
	int mainLoop();
	static MainClass *getInstance(HINSTANCE h, int m);
	static MainClass *getInstance();
	
};

class MyMenuBar : public MenuBar
{
public:
    static const uint16_t FILE_NEW = ID_FILE_NEW;
	static const uint16_t FILE_OPEN = ID_FILE_OPEN;
	static const uint16_t FILE_EXIT = ID_FILE_EXIT;
    static const uint16_t HELP_ABOUT = ID_HELP_ABOUT;
};

MainWindow *MainWindow::createInstance(WinClassEx *wc)
{
    if (!instance)
        instance = new MainWindow(wc);

    return instance;
}

MDIClientWinClass *mcwc;
MyMDIClient *mdic;

MainWindow *MainWindow::getInstance()
{
    if (instance)
        return instance;

    return NULL;
}

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
MainWindow *MainWindow::instance = NULL;

MainClass::MainClass(HINSTANCE h, int m)
{
	MyWinClass wclass(MainWindow::WindowProcedure, h);
	wclass.registerClass();
	MainWindow *mw = MainWindow::createInstance(&wclass);
	mw->Show(m);
	
}

ToolBar::ToolBar(HWND parent)
{
    TBADDBITMAP tbab;
    TBBUTTON tbb[9] = { 0 };
    handle = CreateWindowEx(0, TOOLBARCLASSNAME, NULL, WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, parent, (HMENU)ID_TOOLBAR, g_hInst, NULL);
    SendMessage(handle, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);
    tbab.hInst = HINST_COMMCTRL;
    tbab.nID = IDB_STD_SMALL_COLOR;
    SendMessage(handle, TB_ADDBITMAP, 0, (LPARAM)&tbab);
    tbb[0].iBitmap = STD_FILENEW;
    tbb[0].fsState = TBSTATE_ENABLED;
    tbb[0].fsStyle = TBSTYLE_BUTTON;
    tbb[0].idCommand = CM_FILE_NEW;
    tbb[1].iBitmap = STD_FILEOPEN;
    tbb[1].fsState = TBSTATE_ENABLED;
    tbb[1].fsStyle = TBSTYLE_BUTTON;
    tbb[1].idCommand = CM_FILE_OPEN;
    tbb[2].iBitmap = STD_FILESAVE;
    tbb[2].fsStyle = TBSTYLE_BUTTON;
    //tbb[2].idCommand = CM_FILE_SAVE;
    tbb[3].fsStyle = TBSTYLE_SEP;
    tbb[4].iBitmap = STD_CUT;
    tbb[4].fsStyle = TBSTYLE_BUTTON;
    //tbb[4].idCommand = CM_EDIT_CUT;
    tbb[5].iBitmap = STD_COPY;
    tbb[5].fsStyle = TBSTYLE_BUTTON;
    //tbb[5].idCommand = CM_EDIT_COPY;
    tbb[6].iBitmap = STD_PASTE;
    tbb[6].fsStyle = TBSTYLE_BUTTON;
    //tbb[6].idCommand = CM_EDIT_PASTE;
    tbb[7].fsStyle = TBSTYLE_SEP;
    tbb[8].iBitmap = STD_UNDO;
    tbb[8].fsStyle = TBSTYLE_BUTTON;
    //tbb[8].idCommand = CM_EDIT_UNDO;
    ::SendMessage(handle, TB_ADDBUTTONS, 9, (LPARAM)&tbb);
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

LRESULT CALLBACK MainWindow::MDIcontrol(HWND h, uint32_t msg, WPARAM w, LPARAM l)
{
    return ::DefWindowProc(h, msg, w, l);
}

LRESULT CALLBACK MainWindow::control(HWND h, uint32_t msg, WPARAM w, LPARAM l)
{
	switch (msg)
	{
    case Message::CREATE:
    {
        mcwc = new MDIClientWinClass(0, g_hInst);
        gsb = new StatusBar(h);
        tb1 = new ToolBar(h);
        CLIENTCREATESTRUCT css;
        mdic = new MyMDIClient(mcwc, h);
        mdic->show();
    }
        break;
	case Message::COMMAND:
		switch (w)
		{
        case MyMenuBar::FILE_NEW:
        {
            MDICREATESTRUCT mcs;
            HWND hChild;
            mcs.szTitle = L"[Untitled]";
            mcs.szClass = L"MDI";
            mcs.hOwner = g_hInst;
            mcs.x = mcs.cx = CW_USEDEFAULT;
            mcs.y = mcs.cy = CW_USEDEFAULT;
            mcs.style = MDIS_ALLCHILDSTYLES;
            hChild = (HWND)SendMessage(mdicclienthandle, WM_MDICREATE, 0, (LONG)&mcs);

            if (!hChild)
            {
                throw "MDI Child creation failed";
            }
        }
            break;
		case MyMenuBar::FILE_OPEN:
        case ToolBar::CM_FILE_OPEN:
			fb->browse();
			break;
		case MyMenuBar::FILE_EXIT:
			::SendMessage(h, Message::CLOSE, 0, 0);
			break;
        case MyMenuBar::HELP_ABOUT:
            ::MessageBox(h, L"Venster1\ndoorJasper ter Weeme", L"About", 0);
            break;
		}
		break;
    case Message::SIZE:
        ::SendMessage(gsb->getHandle(), Message::SIZE, 0, 0);
        break;
	case Message::DESTROY:
		::PostQuitMessage(0);
		return 0;
	}

	return ::DefWindowProc(h, msg, w, l);
}

LRESULT CALLBACK
MainWindow::WindowProcedure(HWND h, unsigned int msg, WPARAM w, LPARAM l)
{
	return getInstance()->control(h, msg, w, l);
}

LRESULT CALLBACK
MainWindow::MDIProc(HWND h, uint32_t msg, WPARAM w, LPARAM l)
{
    return getInstance()->MDIcontrol(h, msg, w, l);
}

int WINAPI 
WinMain(HINSTANCE h, HINSTANCE p, char *c, int m)
{
    g_hInst = h;
    ::InitCommonControls();
	MainClass *mc = MainClass::getInstance(h, m);
	return mc->mainLoop();
}


