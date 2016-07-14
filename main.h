#if !defined MAIN_H
#define MAIN_H

#include <windows.h>
#include "winex.h"

// procedures called by Windows

BOOL CALLBACK DialogProc
	(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain
    (HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR cmdParam, int cmdShow);

#endif
