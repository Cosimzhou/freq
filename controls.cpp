#include "controls.h"

void ScrollBar::Command (int code, int pos)
{
    int iPos = _iPos; 
    switch (code)
    {
        case SB_TOP:
            iPos = _cPos - 1;
            break;
        case SB_BOTTOM:
            iPos = 0;
            break;
        case SB_LINEUP:
            iPos -= 1;
            if (iPos < 0)
                iPos = 0;
            break;
        case SB_PAGEUP:
            iPos -= _cPage;
            if (iPos < 0)
                iPos = 0;
            break;
        case SB_LINEDOWN:
            iPos += 1;
            if (iPos >= _cPos)
                iPos = _cPos - 1;
            break;
        case SB_PAGEDOWN:
            iPos += _cPage;
            if (iPos >= _cPos)
                iPos = _cPos - 1;
            break;
        case SB_THUMBPOSITION:
        case SB_THUMBTRACK:
            iPos = pos;
            break;
    }
    if (iPos != _iPos)
        SetPos (iPos);
}

LRESULT CALLBACK SubProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );

SubEdit::SubEdit (HWND hwndParent, int id)
        : Edit (hwndParent, id)
{
    _wprocOld = (PWinProc) GetWindowLong (Hwnd (), GWL_WNDPROC);
    
    SetWindowLong (Hwnd (), GWL_USERDATA,  (LONG) this);
    SetWindowLong (Hwnd (), GWL_WNDPROC,  (LONG) SubProc);
}


LRESULT CALLBACK SubProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_GETDLGCODE:
    {
        long code = SubEdit::CallDefault 
            (hWnd, message, wParam, lParam);
        return DLGC_WANTALLKEYS | code;
    }
    case WM_CHAR:
        //Process this message to avoid message beeps.
        if (wParam == VK_RETURN || wParam == VK_TAB)
            return 0;
        break;
    case WM_KEYDOWN:
        if (wParam == VK_RETURN) 
        {
            PostMessage (GetParent(hWnd), WM_COMMAND, (WPARAM)IDOK, 0L);
            return 0;
        }
        break;
    }
    return SubEdit::CallDefault (hWnd, message, wParam, lParam);
}

