#ifndef _MAIN_H_
#define _MAIN_H_
#include <Windows.h>
#include "misc.h"

class MyWinClass : public WinClassEx
{
public:
    MyWinClass(WNDPROC wp, HINSTANCE hinst) : WinClassEx(wp, L"dinges", hinst)
    {
        wclass.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
    }
};

class MDIClientWinClass : public WinClassEx
{
public:
    MDIClientWinClass(WNDPROC wp, HINSTANCE hinst) : WinClassEx(wp, L"MDI", hinst)
    {
        wclass.lpszMenuName = NULL;
        wclass.lpszClassName = L"MDI";
        wclass.hbrBackground = (HBRUSH)(COLOR_3DFACE + 1);
    }

    void Register() { }
};
#endif


