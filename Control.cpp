#include "control.h"
#include "resource.h"
#include "display.h"
#include "winex.h"
#include "params.h"
#include "views.h"
#include <string.h>
#include <stdio.h>

#define WM_MAIN_DLG_REINIT	(WM_USER+25)


// Helper functions

bool Is16BitSampling (DWORD format);
bool Is11_025kHz (DWORD format);
bool Is22_05kHz (DWORD format);
bool Is44_1kHz (DWORD format);

// Main dialog handler

BOOL CALLBACK DialogProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static Controller* pControl = 0;
	switch (message)
	{
	case WM_INITDIALOG:
		try
		{
			pControl = new Controller (hwnd);
		}
		catch (WinException e)
		{
			MessageBox (0, e.GetMessage (), "Exception", MB_ICONEXCLAMATION | MB_OK);
			return -1;
		}
		catch (...)
		{
			MessageBox (0, "Unknown", "Exception", MB_ICONEXCLAMATION | MB_OK);
			return -1;
		}
		return TRUE; 
	case WM_GETMINMAXINFO:
		((MINMAXINFO*)lParam)->ptMinTrackSize.x = 400;
		((MINMAXINFO*)lParam)->ptMinTrackSize.y = 300;
		return 0;
	case WM_COMMAND:
		pControl->Command(hwnd, LOWORD (wParam), HIWORD (wParam));
		return TRUE;

	case WM_SIZE:
		if(SIZE_MINIMIZED != wParam) 
			pControl->Size(hwnd, LOWORD (lParam), HIWORD (lParam));
		
		return 0;
	case WM_CLOSE:
		delete pControl;
		DestroyWindow (hwnd);
		return TRUE;
	case WM_MAIN_DLG_REINIT:
		pControl->ReInit(hwnd);
		break;
	case WM_MOUSEMOVE:
		pControl->OnMouseMove( LOWORD (lParam), HIWORD (lParam));
		break;
	}
	return FALSE;
}

BOOL CALLBACK AboutDlgProc
(HWND hwnd, UINT message, UINT wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD (wParam))
		{
		case IDOK:
		case IDCANCEL:
			EndDialog (hwnd, 0);
			return TRUE;
		case IDC_WWW:
			ShellExecute (hwnd, "open", "http://hi.baidu.com/sestall", 0, 0, SW_SHOWNORMAL);			
			return TRUE;
		}
		break;
	}
	return FALSE;
}

BOOL CALLBACK SettingDlgProc(HWND hwnd, UINT message, UINT wParam, LPARAM lParam)
{
	static SettingDlg_Controller* pControl = 0;
	switch(message)
	{
	case WM_INITDIALOG:
		pControl = new SettingDlg_Controller(hwnd, lParam);
		return TRUE;
	case WM_CLOSE:
		delete pControl;
		EndDialog (hwnd, 0);	
		DestroyWindow (hwnd);
		break;
	case WM_COMMAND:
		if(pControl)
			pControl->OnCommand(hwnd, LOWORD (wParam), HIWORD (wParam));
		break;

	case WM_HSCROLL:
		if(pControl)
			pControl->Scroll (hwnd, LOWORD (wParam), HIWORD (wParam));
		return 0;
	}
	return FALSE;
}

Controller::Controller (HWND hwnd) 
			:_isStopped (true),
			_hwnd(hwnd),
			_bitsPerSample (16),
			_samplesPerSecond (SAMPLES_SEC),	//1 sec
			_fftPoints (FFT_POINTS * 4),
			_samplesPerBuf (FFT_POINTS * 2),
			_viewWave (hwnd, IDS_WAVE_PANE, FFT_POINTS * 8),
			_viewFreq (hwnd, IDS_FREQ_PANE),
			_viewElec(hwnd, IDS_ELEC_LEVEL),
			_viewPiano(hwnd, IDS_INSTRUCT_PANE),
			_scaleFreq (hwnd, IDC_FREQ_SCALE),
//			_wavFormat(1, SAMPLES_SEC, 16),
			_display (hwnd, 
					  _viewWave, 
					  _viewFreq,
					  _viewElec,
					  _viewPiano,
					  _samplesPerBuf,
					  _samplesPerSecond,
					  _fftPoints,
					  _pcmTape)
{

	_pcmTape.Alloc(1024*1024);

	int iLen;
	char szBuf[1024];
	
	FILE *pfile;
	if(pfile = fopen("config.cfg", "rb+"))
	{/*
		fread(&_nChannel, sizeof(int), 1, pfile);
		fread(&_bitsPerSample, sizeof(int), 1, pfile);
		fread(&_samplesPerSecond, sizeof(int), 1, pfile);
		fread(&_waveTime, sizeof(DWORD), 1, pfile);
		fread(&_samplesPerBuf, sizeof(int), 1, pfile);
		fread(&_fftPoints, sizeof(int), 1, pfile);
		fread(&iLen, sizeof(int), 1, pfile);
		fread(szBuf, sizeof(char), iLen, pfile);
		
	*/	fclose(pfile);
	}else{
//		_waveTime = 1000;
//		_nChannel = 1;
		GetTempPath(1024, szBuf);	
	}

	if (waveInGetNumDevs() == 0)
		throw WinException ("No sound card installed !");

	WAVEINCAPS waveInCaps;
	if (waveInGetDevCaps (0, &waveInCaps, sizeof(WAVEINCAPS)) != MMSYSERR_NOERROR)
		throw WinException ("Cannot determine sound card capabilities !");

	if (!Is16BitSampling (waveInCaps.dwFormats))
	{
//		_wavFormat.wBitsPerSample = 
		_samplesPerSecond = 8;
	}

	// Attach icon to main dialog
	HICON hIcon = LoadIcon (TheInstance, MAKEINTRESOURCE (ICO_LOGO));
	SendMessage (hwnd, WM_SETICON, WPARAM (ICON_SMALL), LPARAM (hIcon));
	HWND hwndButton = GetDlgItem (hwnd, IDB_ABOUT);
	SendMessage (hwndButton, BM_SETIMAGE, (WPARAM)IMAGE_ICON,(LPARAM)(HANDLE)hIcon);
	
	// Display RS icon over the button
	hIcon = LoadIcon (TheInstance, MAKEINTRESOURCE (ICO_RECORD));
	hwndButton = GetDlgItem (hwnd, IDB_RECORD);
	SendMessage (hwndButton, BM_SETIMAGE, (WPARAM)IMAGE_ICON,(LPARAM)(HANDLE)hIcon);
	
	hIcon = LoadIcon (TheInstance, MAKEINTRESOURCE (ICO_STOP));
	hwndButton = GetDlgItem (hwnd, IDB_STOP);
	SendMessage (hwndButton, BM_SETIMAGE, (WPARAM)IMAGE_ICON,(LPARAM)(HANDLE)hIcon);
	
	hIcon = LoadIcon (TheInstance, MAKEINTRESOURCE (ICO_PLAY));
	hwndButton = GetDlgItem (hwnd, IDB_PLAY);
	SendMessage (hwndButton, BM_SETIMAGE, (WPARAM)IMAGE_ICON,(LPARAM)(HANDLE)hIcon);
	
	PaintScale ();
	_display.Resume ();
	
	//	SendMessage(hwnd, WM_SYSCOMMAND, )
	
	PostMessage (hwnd, WM_COMMAND, IDB_RECORD, 0);

}

Controller::~Controller ()
{
	_display.Kill ();
	PostQuitMessage (0);
}

// called when _samplesPerSec, _fftPoints or _bitsPerSample changed
void Controller::ReInit (HWND hwnd)
{
	if (!_display.ReInit (
        _samplesPerBuf,
		_samplesPerSecond,
        _fftPoints,
        _bitsPerSample))
	{
		PostQuitMessage (1);
	}
	
	PaintScale ();
}

void Controller::Size (HWND hwnd, int cx, int cy) 
{
	static bool inited = false;
	static CsmRect wndRect;
	if(!inited)
	{
		::GetClientRect(hwnd, &wndRect);
		inited = true;
	}
	
	CsmRect rect;	
	::InvalidateRect(hwnd, NULL, TRUE);
	
	// Resize viewWave
	WindowRect wrect(_viewWave.Hwnd());
	rect = wrect.GetClientRect(hwnd);
	MoveWindow(_viewWave.Hwnd(), rect.left, rect.top, 
		cx-wndRect.Cx()+rect.Cx(), rect.Cy(), TRUE);
	
	// Resize viewFreq
	wrect.GetWindowRect(_viewFreq.Hwnd());
	rect = wrect.GetClientRect(hwnd);
	MoveWindow(_viewFreq.Hwnd(), rect.left, rect.top, 
		cx-wndRect.Cx()+rect.Cx(), cy-wndRect.Cy()+rect.Cy(), TRUE);
	
	// Resize scaleFreq
	wrect.GetWindowRect(_scaleFreq.Hwnd());
	rect = wrect.GetClientRect(hwnd);
	MoveWindow(_scaleFreq.Hwnd(), rect.left, rect.top, rect.Cx(), 
		cy-wndRect.Cy()+rect.Cy(), TRUE);
	
	// Resize state
	wrect.GetWindowRect(GetDlgItem(hwnd, IDC_STATIC_STATE));
	rect = wrect.GetClientRect(hwnd);
	MoveWindow(GetDlgItem(hwnd, IDC_STATIC_STATE), rect.left, cy-wndRect.Cy()+rect.top, 
		cx-wndRect.Cx()+rect.Cx(), rect.Cy(), TRUE);
	
	// Resize viewElecLevel
	wrect.GetWindowRect(_viewElec.Hwnd());
	rect = wrect.GetClientRect(hwnd);
	MoveWindow(_viewElec.Hwnd(), rect.left, cy-wndRect.Cy()+rect.top, 
		cx-wndRect.Cx()+rect.Cx(), rect.Cy(), TRUE);
	
	
	wndRect.right = wndRect.left + cx;
	wndRect.bottom = wndRect.top + cy;
}

void Controller::Stop (HWND hwnd)
{
	_display.Stop ();
	EnableWindow (GetDlgItem(hwnd, IDB_STOP), FALSE);
	EnableWindow (GetDlgItem(hwnd, IDB_START), TRUE);
}

void Controller::OnMouseMove(int x, int y)
{
	WindowRect rect(	_viewPiano.Hwnd());

	if(rect.IsContain(x, y))
	{
//		rect.left=0;
		_viewPiano.SetRank(rect.bottom - y);
	}else{
		_viewPiano.SetRank(- 1);
	}
}

// Child control command processing

void Controller::Command ( HWND hwnd, int ctrlid, int code)
{
	int i = 0;
	switch (ctrlid)
	{
	case IDC_RELIBUTTON:
		DialogBox (TheInstance, MAKEINTRESOURCE (DLG_ABOUT), hwnd, DLGPROC(AboutDlgProc));
		break;
	case ID_MENU_SETTING_PARAMETER:
		DialogBoxParam(TheInstance, MAKEINTRESOURCE (DLG_SET), hwnd, DLGPROC(SettingDlgProc), (LPARAM)this);
//		DialogBox (TheInstance, MAKEINTRESOURCE (DLG_SET), hwnd, DLGPROC(SettingDlgProc));
		break;
	case ID_MENU_SETTING_VOLUMN_OUTPUT:
		ShellExecute (hwnd, "open", "sndvol32", 0, 0, SW_SHOWNORMAL);			
		break;
	case ID_MENU_SETTING_VOLUMN_INPUT:
		ShellExecute (hwnd, "open", "sndvol32", "/r", 0, SW_SHOWNORMAL);			
		break;

	case IDB_STOP:
		if( ! _isStopped)
		{
			Stop (hwnd);
			EnableWindow(GetDlgItem (hwnd, IDB_RECORD), TRUE);
			EnableWindow(GetDlgItem (hwnd, IDB_STOP), FALSE);
			_isStopped = true;
		}
		break;
	case IDB_RECORD:
		if (_isStopped)
		{
			_isStopped = !_display.Start(RECORD);
			EnableWindow(GetDlgItem (hwnd, IDB_RECORD), FALSE);
			EnableWindow(GetDlgItem (hwnd, IDB_STOP), TRUE);

		}
		break;
	}
}

void Controller::PaintScale ()
{
	// Get the rectangle (in pixels)
	RECT &rect = _scaleFreq.Rect ();
	// Get the reference canvas
	UpdateCanvas canvRef (_scaleFreq.Hwnd ());
	// Translate rectangle to hundredths of millimiters
	RectHmm rectHmm (canvRef, rect);
	// Create Enhanced Metafile control canvas
	CanvasEMFCtrl canvas (rectHmm, canvRef);
	
	canvas.ClearWhite (rect);
	int x0 = rect.right;
	int y0 = rect.bottom - 1;
	
	// draw a notch every 100 Hz
	int s1000 = _display.HzToPoint(1000);
	int s100 = s1000 / 10;
	int maxS = _display.Points() / 2;
	int count = 0;
	canvas.Line (x0, rect.top, x0, rect.bottom);
	int xx = x0;
	if ( s100 < 3)
	{
		for (int s = 0; s < maxS; s += s1000 )
		{
			int y = y0 - s;
			if (y <= 0)
				break;
			int c = 6;
			canvas.Line (x0 - c, y, x0, y);
			count++;
		}
	}
	else
	{
		for (int s = 0; s < maxS; s += s100)
		{
			int c = (count % 10) == 0? 6: 3;
			int y = y0 - s;
			if ( y <= 0 )
				break;
			canvas.Line (x0 - c, y, x0, y);
			count++;
		}
	}
	// Attach the Enhanced Metafile to the control
	_scaleFreq.Attach (canvas);
}

bool Is16BitSampling (DWORD format)
{
	return (format & WAVE_FORMAT_1M16) ||
		(format & WAVE_FORMAT_1S16) ||
		(format & WAVE_FORMAT_2M16) ||
		(format & WAVE_FORMAT_2S16) ||
		(format & WAVE_FORMAT_4M16) ||
		(format & WAVE_FORMAT_4S16);
}

bool Is11_025kHz (DWORD format)
{
	return (format & WAVE_FORMAT_1M08) ||
		(format & WAVE_FORMAT_1M16) ||
		(format & WAVE_FORMAT_1S08) ||
		(format & WAVE_FORMAT_1S16);
}

bool Is22_05kHz (DWORD format)
{
	return (format & WAVE_FORMAT_2M08) ||
		(format & WAVE_FORMAT_2M16) ||
		(format & WAVE_FORMAT_2S08) ||
		(format & WAVE_FORMAT_2S16);
}

bool Is44_1kHz (DWORD format)
{
	return (format & WAVE_FORMAT_4M08) ||
		(format & WAVE_FORMAT_4M16) ||
		(format & WAVE_FORMAT_4S08) ||
		(format & WAVE_FORMAT_4S16);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////		SettingDlg_Controller		//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SettingDlg_Controller::SettingDlg_Controller(HWND hwnd, LPARAM ptr):  
				_radio8 (hwnd, IDC_8_BITS),
				_radio16 (hwnd, IDC_16_BITS),
				_radio32(hwnd, IDC_32_BITS),
				_radioMono(hwnd, IDC_MONO), 
				_radioStereo(hwnd, IDC_STEREO),
				_scroll (hwnd, IDC_SCROLLBAR),
				_editSectionNum (hwnd, IDC_EDIT_SECNUM),
				_editTime(hwnd, IDC_EDIT_TIME),
				_editPCM(hwnd, IDC_EDIT_PCM),
				_comboFreq (hwnd, IDC_SAMPLING),
				_comboPoints (hwnd, IDC_POINTS),
				_pOwner((Controller*)ptr)
{
	int iLen;
	char szBuf[1024];

	FILE *pfile;
	if(pfile = fopen("config.cfg", "rb+"))
	{
		fread(&_nChannel, sizeof(int), 1, pfile);
		fread(&_bitsPerSample, sizeof(int), 1, pfile);
		fread(&_samplesPerSecond, sizeof(int), 1, pfile);
		fread(&_waveTime, sizeof(DWORD), 1, pfile);
		fread(&_samplesPerBuf, sizeof(int), 1, pfile);
		fread(&_fftPoints, sizeof(int), 1, pfile);
		fread(&iLen, sizeof(int), 1, pfile);
		fread(szBuf, sizeof(char), iLen, pfile);

		fclose(pfile);
	}else{
		_waveTime = 1000;
		_nChannel = 1;
		GetTempPath(1024, szBuf);

	}

	_fftPoints = _pOwner->_fftPoints;

	WAVEINCAPS waveInCaps;
	if (waveInGetDevCaps (0, &waveInCaps, sizeof(WAVEINCAPS)) != MMSYSERR_NOERROR)
		throw WinException ("Cannot determine sound card capabilities !");

	_radio32.Disable();
	_radioMono.Check();
	_radioStereo.Disable();

	if (Is16BitSampling (waveInCaps.dwFormats))
	{
		if(8 == _pOwner->_samplesPerSecond) _radio8.Check();
//		_wavFormat.wBitsPerSample = 8;
	}else{

	}

	// Initialize radio buttons (bits per sample)
	if(8 == _pOwner->_bitsPerSample) 
	{
//		_radio16.Disable();
		_radio8.Check();
		_bitsPerSample = 8;
	}else{
		_bitsPerSample = 16;
		_radio16.Check();
	}

	_editTime.SetString("1000");

	// Initialize scroll bar (fft's per second)
	InitScrollPositions ();
	// Initialize the combo box with sampling frequencies
	if (Is11_025kHz (waveInCaps.dwFormats))
	_comboFreq.AddValue (SAMPLES_SEC);       
	if (Is22_05kHz (waveInCaps.dwFormats))
	_comboFreq.AddValue (2 * SAMPLES_SEC);  
	if (Is44_1kHz (waveInCaps.dwFormats))
	_comboFreq.AddValue (4 * SAMPLES_SEC); 


	// Select current sampling frequency
	_samplesPerSecond = _pOwner->_samplesPerSecond;
	_comboFreq.SetPosByVal(_samplesPerSecond);

	_samplesPerBuf = _pOwner->_samplesPerBuf;
	_scroll.SetPosByVal(_samplesPerBuf);
	_editSectionNum.SetNumber (_samplesPerSecond / _samplesPerBuf);

	// Initialize combo with FFT points
	_comboPoints.AddValue (FFT_POINTS);			// 0
	_comboPoints.AddValue (2 * FFT_POINTS);		// 1
	_comboPoints.AddValue (4 * FFT_POINTS);		// 2
	_comboPoints.AddValue (8 * FFT_POINTS);		// 3
	_fftPoints = _pOwner->_fftPoints;
	_comboPoints.SetPosByVal(_fftPoints);		// 4 * FFT_POINTS

	UpdatePCM();
}


void SettingDlg_Controller::InitScrollPositions ()
{
	// This scrollbar controls fft's per second
	// by storing and returning samples per buffer
	_scroll.Clear ();
	// We perform one fft per buffer
	// so start with fftPoints large buffer
	// and shrink it down to MIN_SAMP_BUF

	int sampPerBuf = _fftPoints;
	for (int i = 0; i < MAX_POS; i++)
	{
		_scroll.AddValue (sampPerBuf);
		sampPerBuf -= MIN_SAMP_BUF;
		if (sampPerBuf <= 0)
			break;
	}
	
	_scroll.Init (i + 1, 1);
	
	// let's start with one buffer per fft
	_samplesPerBuf = _fftPoints;
	_scroll.SetPos (0);
	// fft's per second
	_editSectionNum.SetNumber (_samplesPerSecond / _samplesPerBuf);

}


void SettingDlg_Controller::Scroll (HWND hwnd, int code, int pos) 
{ 

	_scroll.Command (code, pos);
	_samplesPerBuf = _scroll.GetValue(); 
	Assert (_samplesPerBuf <= _fftPoints);
	Assert (_samplesPerBuf >= MIN_SAMP_BUF);
/*	if (!_display.ReInit (
		_samplesPerBuf,
		_samplesPerSecond,
		_fftPoints,
		_bitsPerSample))
	{
		PostQuitMessage (1);
	}*/
	_editSectionNum.SetNumber (_samplesPerSecond / _samplesPerBuf);
	
}

void SettingDlg_Controller::OKSave()
{
	_pOwner->_samplesPerBuf = _samplesPerBuf;
	_pOwner->_samplesPerSecond = _samplesPerSecond;
	_pOwner->_bitsPerSample = _bitsPerSample;////_bits
	_pOwner->_fftPoints = _fftPoints;


	::PostMessage(_pOwner->_hwnd, WM_MAIN_DLG_REINIT, 0, 0);
}

void SettingDlg_Controller::ChangePath(HWND hwnd)
{
	OPENFILENAME ofn;
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hwnd;
//	ofn.hInstance = AfxGetInstanceHandle();
//	ofn.lpstrFilter = "PCM文件|*.pcm||";
	ofn.nFilterIndex = 1;
	ofn.lpstrFile = new char[256];
	ofn.nMaxFile = 255;
	//		ofn.lpstrFileTitle = .lpstrFile
	//			.nMaxFileTitle = .nMaxFile
	ofn.lpstrInitialDir = "";//App.Path '"C:\"
	ofn.lpstrTitle = "打开文件";
	ofn.Flags = 0;
	if(GetOpenFileName(&ofn))
	{
		//--------------------
	}
	
	delete[] ofn.lpstrFile;

}

void SettingDlg_Controller::OnCommand(HWND hwnd, int ctrlid, int code)
{
	switch (ctrlid)
	{
	case IDOK:
		OKSave();	// note: no 'break;' need here.
	case IDCANCEL:
		EndDialog (hwnd, 0);
		break;
	case IDC_8_BITS:
		if (_radio8.IsClicked (code))
		{
			_bitsPerSample = 8;
			UpdatePCM();
		}
		break;
	case IDC_16_BITS:
		if (_radio16.IsClicked (code))
		{
			_bitsPerSample = 16;
			UpdatePCM();
		}
		break;
	case IDC_32_BITS:
		if (_radio32.IsClicked (code))
		{
			_bitsPerSample = 32;
			UpdatePCM();
		}
		break;

	case IDC_SAMPLING:
		if (_comboFreq.IsChanged (code))
		{
			_samplesPerSecond = _comboFreq.GetValue ();
			InitScrollPositions ();
			UpdatePCM();
		}    
		break;
	case IDC_POINTS:
		if (_comboPoints.IsChanged(code))
		{
			_fftPoints = _comboPoints.GetValue ();
			InitScrollPositions ();
		}    
		break;
	case IDC_EDIT_TIME:
		if(code == EN_CHANGE)
		{
			_waveTime = _editTime.GetVal();
			UpdatePCM();
		}
		break;
	case IDB_PATH:
		ChangePath(hwnd);
		break;
	}
}

void SettingDlg_Controller::UpdatePCM()
{
	const char* unitMark[6] ={"Bytes", "KB", "MB", "GB", "TB", "PB"};
	__int64 size = __int64(_waveTime) * (_bitsPerSample>>3) * _samplesPerSecond * _nChannel;
	int kmg = 0;
	double dsize;

	while(size > 1024)
	{
		dsize = size/1024.0;
		size >>= 10;
		kmg ++;
		if(kmg >= 5)break;
	}

	char txtbuf[32];
	if(kmg != 0)
	{
		sprintf(txtbuf, "%.2f %s", dsize, unitMark[kmg]);
	}else{
		sprintf(txtbuf, "%d %s", size, unitMark[kmg]);
	}
	_editPCM.SetString(txtbuf);
}
