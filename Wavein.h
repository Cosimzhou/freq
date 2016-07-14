#if !defined WAVEIN_H
#define WAVEIN_H

#include "thread.h"
#include <windows.h>
#include <mmsystem.h>

class WaveFormat: public WAVEFORMATEX
{
public:
    WaveFormat ( 
        WORD    nCh, // number of channels (mono, stereo)
        DWORD   nSampleRate, // sample rate
        WORD    BitsPerSample)
	{
		wFormatTag = WAVE_FORMAT_PCM;
		nChannels = nCh;
		nSamplesPerSec = nSampleRate;
		nAvgBytesPerSec = nSampleRate * nCh * BitsPerSample/8;
		nBlockAlign = nChannels * BitsPerSample/8;
		wBitsPerSample = BitsPerSample;
		cbSize = 0;
	}   

	bool isInSupported (UINT idDev)
	{
		MMRESULT result = waveInOpen
			(0, idDev, this, 0, 0, WAVE_FORMAT_QUERY);
		return result == MMSYSERR_NOERROR;
	}
};

class WaveHeader: public WAVEHDR
{
public:
    bool IsDone () const { return dwFlags & WHDR_DONE; }
};

class WaveInDevice
{
public:
	WaveInDevice ();
	WaveInDevice (UINT idDev, WaveFormat& format, Event& event);
	~WaveInDevice ();
	bool    Open (UINT idDev, WaveFormat& format, Event& event);
	void    Reset ();
	bool    Close ();
	void    Prepare (WaveHeader * pHeader);
	void    UnPrepare (WaveHeader * pHeader);
	void    SendBuffer (WaveHeader * pHeader);
	bool    Ok () { return _status == MMSYSERR_NOERROR; }
	void    Start () { waveInStart(_handle); }
	void    Stop () { waveInStop(_handle); }
	bool    isInUse () { return _status == MMSYSERR_ALLOCATED; }
	DWORD   GetPosSample ();
	UINT    GetError () { return _status; }
	void    GetErrorText (char* buf, int len);
private:
	HWAVEIN		_handle;
	MMRESULT	_status;
};

inline WaveInDevice::WaveInDevice ()
{
	_status = MMSYSERR_BADDEVICEID;
}

inline WaveInDevice::WaveInDevice (UINT idDev, WaveFormat& format, Event& event)
{
	Open (idDev, format, event);
}

inline WaveInDevice::~WaveInDevice ()
{
	if (Ok())
	{
		waveInReset(_handle);
		waveInClose (_handle);
	}
}

inline bool WaveInDevice::Open (UINT idDev, WaveFormat& format, Event& event)
{
	_status = waveInOpen (
        &_handle, 
        idDev, 
        &format, 
        (DWORD) HANDLE (event),
        0, // callback instance data
        CALLBACK_EVENT);

	return Ok();
}

inline void WaveInDevice::Reset ()
{
	if (Ok())
		waveInReset (_handle);
}

inline bool WaveInDevice::Close ()
{
	if ( Ok() && waveInClose (_handle) == 0)
	{
		_status = MMSYSERR_BADDEVICEID;
		return true;
	}
	else
		return false;
}

inline void WaveInDevice::Prepare (WaveHeader * pHeader)
{
	waveInPrepareHeader(_handle, pHeader, sizeof(WAVEHDR));
}

inline void WaveInDevice::SendBuffer (WaveHeader * pHeader)
{
	waveInAddBuffer (_handle, pHeader, sizeof(WAVEHDR));
}

inline void WaveInDevice::UnPrepare (WaveHeader * pHeader)
{
	waveInUnprepareHeader (_handle, pHeader, sizeof(WAVEHDR));
}

inline DWORD WaveInDevice::GetPosSample ()
{
	MMTIME mtime;
	mtime.wType = TIME_SAMPLES;
	waveInGetPosition (_handle, &mtime, sizeof (MMTIME));
	return mtime.u.sample;
}

inline void WaveInDevice::GetErrorText (char* buf, int len)
{
	waveInGetErrorText (_status, buf, len);
}

#endif
