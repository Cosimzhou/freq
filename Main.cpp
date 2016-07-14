#include "main.h"
#include "control.h"
#include "resource.h"
#include "winex.h"
#include <new.h>

int NewHandler (size_t size);

HINSTANCE TheInstance = 0;

// The main window is a modeless dialog box

int WINAPI WinMain
(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR cmdParam, int cmdShow)
{
	TheInstance = hInst;
	_set_new_handler (&NewHandler);
	
	HWND hDialog = 0;
	
	hDialog = CreateDialog (hInst, MAKEINTRESOURCE (DLG_MAIN), 0, DialogProc);
	
	if (!hDialog)
	{
		char buf [100];
		wsprintf (buf, "Error x%x", GetLastError());
		MessageBox (0, buf, "CreateDialog", MB_ICONEXCLAMATION | MB_OK);
		return 1;
	}
	
	MSG  msg;
	while ( GetMessage (&msg, NULL, 0, 0 ) )
	{
		if (!IsDialogMessage (hDialog, &msg))
		{
			TranslateMessage ( &msg );
			DispatchMessage ( &msg );
		}
	}
	
	return msg.wParam;
}

void _wassert ( char* szExpr, char* szFile, int line )
{
	char buf [100];
	wsprintf (buf, "\"%s\" in file %s, line %d", szExpr, szFile, line);
	MessageBox (0, buf, "Assertion Failed", MB_ICONEXCLAMATION | MB_OK );
	FatalExit (-1);
}

// out of memory handler that throws exception
int NewHandler (size_t size)
{
	throw "Out of memory";
	return 0;
}

