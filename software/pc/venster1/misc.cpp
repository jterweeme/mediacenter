#include <Windows.h>
#include "misc.h"

WinClassEx::WinClassEx(WNDPROC wp, const wchar_t *className, HINSTANCE hinst)
{
	this->className = className;
	this->hinst = hinst;
	wclass.style = CS_HREDRAW | CS_VREDRAW;
	wclass.lpfnWndProc = wp;
	wclass.cbClsExtra = 0;
	wclass.cbWndExtra = 0;
	wclass.hInstance = hinst;
	wclass.hIcon = 0;
    wclass.hIconSm = 0;
    wclass.cbSize = sizeof(wclass);
	wclass.hCursor = ::LoadCursor(0, IDC_ARROW);
	wclass.lpszClassName = className;
    wclass.hbrBackground = (HBRUSH)(COLOR_3DSHADOW + 1);
	wclass.lpszMenuName = 0;
}

GenericWindow::GenericWindow(WinClassEx *wclass, const wchar_t *caption)
{
	this->wclass = wclass;

    handle = ::CreateWindowEx(WS_EX_APPWINDOW,
		wclass->getClassName(),
		caption,
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 0, 0, wclass->getHinst(), 0);
}

FileBrowser::FileBrowser(GenericWindow gw)
{
	::ZeroMemory(&ofn, sizeof(ofn));
	ofn.hwndOwner = gw.getHandle();
	ofn.lStructSize = sizeof(ofn);
	ofn.lpstrFilter = L"Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0\0";
	//ofn.lpstrFile = szFileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrDefExt = L"txt";
}

void FileBrowser::browse()
{
	::GetOpenFileName(&ofn);
}

void GenericWindow::show(int cmdShow)
{
	::ShowWindow(handle, cmdShow);
	::UpdateWindow(handle);
}


