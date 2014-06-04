#include <Windows.h>
#include "misc.h"

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

GenericWindow::GenericWindow(WinClass *wclass)
{
	this->wclass = wclass;

	hwnd = ::CreateWindow(
		wclass->getClassName(),
		L"Generic Window",
		WS_OVERLAPPEDWINDOW,
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

void GenericWindow::Show(int cmdShow)
{
	::ShowWindow(hwnd, cmdShow);
	::UpdateWindow(hwnd);
}


