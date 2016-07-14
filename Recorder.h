#if !defined RECORDER_H
#define RECORDER_H

#include "wavedev.h"
#include "wassert.h"
#include "tapeuser.h"

class Event;
class CassetteTape;

class Recorder  :public TapeUser	
{
public:
	Recorder(CassetteTape&,
		int cSamples, 
		int cSamplePerSec, 
		int nChannels, 
		int bitsPerSecond);
	
	~Recorder();
	bool    Start (Event& event);
	void    Stop ();
	
	DWORD   GetSamplePos () 
	{ 
		return _waveInDevice.GetPosSample(); 
	}
	
	bool    BufferDone ();
	bool    IsStarted () const { return _isStarted; }

protected:
	
	void	SetHeaderBuffer(int n);
	bool            _isStarted;
	WaveInDevice    _waveInDevice;
	
};

class RecorderM8: public Recorder  // 8 bit mono
{
public:
	RecorderM8 (CassetteTape &tape, int cSamples, int cSamplesPerSec)
		: Recorder (tape, cSamples, cSamplesPerSec, 1, 8) {}
protected:
	int GetSample (char *pBuf, int i) const
	{
		return ((unsigned char)pBuf[i] - 128) * 64;
	}
};

class RecorderM16: public Recorder  // 16 bit mono
{
public:
	RecorderM16 (CassetteTape &tape, int cSamples, int cSamplesPerSec)
		: Recorder (tape, cSamples, cSamplesPerSec,  1, 16) {}
protected:
	int GetSample (char *pBuf, int i) const
	{
		return ((short *) pBuf)[i];
	}
};


// for 8 bit stereo return (pBuf[2*i] + pBuf[2*i+1] - 2 * 128) * 128;
// for 16 bit stereo 
// return ( ((short *) pBuf)[2*i] +  ((short *) pBuf)[2*i+1] ) / 2

// This iterator works for any Recorder


#endif
