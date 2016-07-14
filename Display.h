#if !defined DISPLAY_H
#define DISPLAY_H

#include "active.h"
#include "fft.h"
#include "smartptrs.h"

class ViewWave;
class ViewFreq;
class ViewElecLevel;
class ViewPiano;

typedef enum {IDLE=0, RECORD, PLAY, PRODUCE, WAVE}WorkingMode;


class Painter: public ActiveObject
{
public:
    Painter (
        HWND hwnd,
        ViewWave& viewWave,
        ViewFreq& viewFreq,
		ViewElecLevel   &viewElec,
		ViewPiano &viewPiano,
        int samplesPerBuf, 
        int samplesPerSec,
        int fftPoints,
		CassetteTape&);

    bool ReInit (
        int samplesPerBuf, 
        int samplesPerSec, 
        int fftPoints,
        int bitsPerSample);

    bool Start (WorkingMode);
    void Stop ();

    int HzToPoint (int hz)
    { 
        Lock lock (_mutex);
        return _pFftTransformer->HzToPoint (hz); 
    }

    int Points () 
    { 
        Lock lock (_mutex);
        return _pFftTransformer->Points (); 
    }

private:
    void InitThread () {}
    void Run ();
    void FlushThread ();

    void LokWaveInData ();

    ViewWave		&_viewWave;
    ViewFreq		&_viewFreq;
    ViewElecLevel   &_viewElec;
	ViewPiano		&_viewPiano;
	CassetteTape	&_pcmTape;

	WorkingMode	_workType;
    int         _samplesPerBuf;
    int         _samplesPerSecond;
    int         _fftPoints;
    int         _bitsPerSample;

    HWND        _hwnd;

    Mutex       _mutex;
    Event       _event;

	PtrPlayer	_pPlayer;
    PtrRecorder _pRecorder;
    PtrFft      _pFftTransformer;
};

#endif
