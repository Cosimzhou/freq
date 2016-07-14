#if !defined control_h
#define control_h

#include <windows.h>
#include "wassert.h"
#include "control.h"
#include "views.h"
#include "display.h"
#include "metactrl.h"
#include "Cassette.h"
// private message processed _after_ WM_CREATE is done

extern HINSTANCE TheInstance;

class Controller
{
public:
//	WaveFormat _wavFormat;
    Controller(HWND hwnd);
    ~Controller ();
    void Command (HWND hwnd, int id, int code);
	void Size(HWND hwnd, int, int);
    void Paint (HWND hwnd);
    void ReInit (HWND hwnd);
    void Stop (HWND hwnd);

	void OnMouseMove(int , int);

	friend class SettingDlg_Controller;
private:
    void PaintScale ();

    bool                _isStopped;

    int                 _bitsPerSample;
    int                 _fftPoints;
    int                 _samplesPerBuf;
	int					_samplesPerSecond;

    StaticEnhMetafileControl    _scaleFreq;

	CassetteTape		_pcmTape;
    ViewWave            _viewWave;
    ViewFreq            _viewFreq;
	ViewElecLevel		_viewElec;
	ViewPiano			_viewPiano;
    Painter             _display;

	HWND				_hwnd;
};



class SettingDlg_Controller  
{
public:
	void OnCommand(HWND hwnd, int ctrlid, int code);
	SettingDlg_Controller(HWND, LPARAM);
	virtual ~SettingDlg_Controller(){}


	void ChangePath(HWND);
	void OKSave();
    void Scroll (HWND hwnd, int cmd, int pos); 
	
private:
	void UpdatePCM();
    void InitScrollPositions ();


//	WaveFormat		_wavFormat;
	ScrollBarMap	_scroll;
	EditReadOnly	_editSectionNum, _editPCM;
	Edit			_editTime;
	RadioButton		_radio8, _radio16, _radio32, _radioMono, _radioStereo;
	Combo			_comboFreq, _comboPoints;
	Controller*		_pOwner;

	int				_samplesPerSecond;
	int				_fftPoints;
    int				_samplesPerBuf;
	int				_bitsPerSample;
	int				_nChannel;
	DWORD			_waveTime;
};

#endif