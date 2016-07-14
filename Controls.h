#if !defined controls_h
#define controls_h
//////////////////////////////////////
//  controls.h
//  Window Controls
//  (c) Cosim Sestal, 2006
//////////////////////////////////////

#include "wassert.h"
#include <windows.h>
#include <stdio.h>
#include <strstrea.h>

typedef LRESULT (CALLBACK *PWinProc)(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );

const int MAX_POS = 32;

class MenuBar
{
public:
	MenuBar (HWND hwnd)
		: _hwnd (hwnd), _hMenu (GetMenu (hwnd))
	{}
	void Enable (int id)
	{
		EnableMenuItem (_hMenu, id, MF_BYCOMMAND | MF_ENABLED);
	}
	void Disable (int id)
	{
		EnableMenuItem (_hMenu, id, MF_BYCOMMAND | MF_GRAYED);
	}
	void Refresh ()
	{
		DrawMenuBar(_hwnd);
	}
	
private:
	HWND    _hwnd;
	HMENU   _hMenu;
};


// This is for controls embedded
// in regular dialog boxes that have
// a dialog procedure. They can be constructed
// in response to WM_INITDIALOG message

class SimpleControl
{
public:
	SimpleControl (HWND hwndParent, int id)
		: _hWnd (GetDlgItem (hwndParent, id))
	{}
	
	void SetFocus ()
	{
		::SetFocus (_hWnd);
	}
	
	HWND Hwnd () const { return _hWnd; }
	
protected:
	HWND _hWnd;
};

class Combo: public SimpleControl
{
public:
	Combo (HWND hwndParent, int id)
		: SimpleControl (hwndParent, id),
		_cPos (0) 
	{}
	
	void AddValue (int val)
	{
		char buf[6];
		wsprintf ( buf, "%d", val );
		AddString ( buf );
		_aVal [_cPos] = val;
		_cPos++;
		Assert (_cPos < MAX_POS);
	}
	
	void SelectPos (int i)
	{
		SendMessage (Hwnd (), CB_SETCURSEL, i, 0);
	}
	
	// code is the HIWORD (wParam)
	static bool IsChanged (int code) 
	{ 
		return code == CBN_SELENDOK;
	}
	
	int GetValue ()
	{
		return _aVal [GetIndex()];
	}
	
	void SetPosByVal(int val)
	{
		for(int i = 0; i < _cPos; ++i){
			if(_aVal[i] == val){
				SelectPos(i);
				break;
			}
		}
	}
private:
	
	void AddString (char* str)
	{
		SendMessage (Hwnd (), CB_ADDSTRING, 0, (LPARAM) str);
	}
	
	int GetIndex()
	{
		return SendMessage (Hwnd (), CB_GETCURSEL, 0, 0);
	}
	
	int _aVal [MAX_POS];
	int _cPos;
};

class EditReadOnly: public SimpleControl
{
public:
	EditReadOnly (HWND hwndParent, int id)
		: SimpleControl (hwndParent, id)
	{}
	
	void SetString (char const * buf)
	{
		SendMessage (Hwnd (), WM_SETTEXT, 0, (LPARAM) buf);
	}
	
	void SetNumber (int n)
	{
		char buf[6];
		wsprintf (buf, "%d", n);
		SetString (buf);
	}
	
	void SetHexNumber (int n)
	{
		char buf[6];
		wsprintf (buf, "%x", n);
		SetString (buf);
	}
	
	void SetDouble (double x)
	{
		char buf [10];
		ostrstream str( buf, 10);
		str << x << ends;
		SetString (buf);
	}
};

class Edit: public SimpleControl
{
public:
	Edit (HWND hwndParent, int id)
		: SimpleControl (hwndParent, id)
	{}
	
	void SetString (char* buf)
	{
		SendMessage (Hwnd (), WM_SETTEXT, 0, (LPARAM) buf);
	}
	void SetVal(int val)
	{
		char buf[256];
		sprintf(buf, "%d", val);
		SendMessage (Hwnd (), WM_SETTEXT, 0, (LPARAM) buf);
	}

	// code is the HIWORD (wParam)
	static bool IsChanged (int code)
	{ 
		return code == EN_CHANGE;
	}
	
	int GetLen ()
	{
		return SendMessage (Hwnd (), WM_GETTEXTLENGTH, 0, 0);
	}
	
	void GetString (char* buf, int len)
	{
		SendMessage (Hwnd (), WM_GETTEXT, (WPARAM) len, (LPARAM) buf);
	}

	int GetVal()
	{
		char buf[256];
		SendMessage (Hwnd (), WM_GETTEXT, (WPARAM) 256, (LPARAM) buf);
		return atoi(buf);
	}

	void Select ()
	{
		SendMessage (Hwnd (), EM_SETSEL, 0, -1);
	}
};

class SubEdit: public Edit
{
public:
	SubEdit (HWND hwndParent, int id);
	
	static LRESULT CallDefault (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
	{
		// Gives compile error if not compiled with STRICT!
		return CallWindowProc (This (hWnd)->_wprocOld, hWnd, message, wParam, lParam);
	}
	
	static SubEdit* This (HWND hwnd)
	{
		return (SubEdit*) GetWindowLong (hwnd, GWL_USERDATA);
	}
	
private:
	PWinProc _wprocOld;
};

class RadioButton: public SimpleControl
{
public:
	RadioButton (HWND hwndParent, int id)
		: SimpleControl (hwndParent, id)
	{}
	
	// code is the HIWORD (wParam)
	static bool IsClicked (int code)
	{
		return code == BN_CLICKED;
	}
	void Check()
	{
		SendMessage (Hwnd (), BM_SETCHECK, (WPARAM)1, 0);
	}
	void UnCheck()
	{
		SendMessage (Hwnd (), BM_SETCHECK, 0, 0);
	}
	void Disable()
	{
		EnableWindow (Hwnd (), FALSE);
	}
	void Enable()
	{
		EnableWindow (Hwnd (), TRUE);
	}
};

class Button: public SimpleControl
{
public:
	Button (HWND hwndParent, int id)
		: SimpleControl (hwndParent, id)
	{}
	
	void Push ()
	{
		SendMessage (Hwnd (), BM_SETSTATE, (WPARAM)1, 0);
	}
	
	void Pop ()
	{
		SendMessage (Hwnd (), BM_SETSTATE, 0, 0);
	}
};

class ScrollBar: public SimpleControl
{
public:
	ScrollBar (HWND hwndParent, int id)
		: SimpleControl (hwndParent, id), _iPos(0), _cPos(0) 
	{}
	
	void Init (int range, int cPage)
	{
		_cPos = range;
		_cPage = cPage;  // page increment
		SetRange (range);
	}
	
	// Usage: Command (LOWORD (wParam), HIWORD (wParam));
	void Command (int code, int pos);
	
	int GetPos ()
	{
		return GetScrollPos (_hWnd, SB_CTL);
	}
	
	void SetPos (int i)
	{
		_iPos = i;
		SetScrollPos (_hWnd, SB_CTL, i, TRUE);
	}
	
protected:
	
	void SetRange (int range)
	{
		SetScrollRange (_hWnd, SB_CTL, 0, range - 1, FALSE);
	}
	
	int     _iPos;
	int     _cPos;
	int     _cPage;
};

class ScrollBarMap: public ScrollBar
{
public:
	ScrollBarMap (HWND hwndParent, int id)
		: ScrollBar (hwndParent, id)
	{}
	void AddValue (int val)
	{
		Assert (_cPos < MAX_POS);
		_aVal [_cPos] = val;
		_cPos++;
	}
	
	void Init (int range, int cPage)
	{
		Assert (range == _cPos);
		ScrollBar::Init (range, cPage);
	}
	void Clear () 
	{ 
		_cPos = 0; 
		_iPos = 0; 
	}
	int  GetValue () const { return _aVal [_iPos]; }

	void SetPosByVal(int val)
	{
		for(int i = 0; i < _cPos; ++i){
			if(_aVal[i] == val){
				SetPos(i);
				break;
			}
		}
	}
protected:
	int _aVal [MAX_POS];
};

#endif
