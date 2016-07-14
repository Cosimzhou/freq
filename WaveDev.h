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

class WaveOutDevice
{
public:
	WaveOutDevice(){_status = MMSYSERR_BADDEVICEID;};
	WaveOutDevice(UINT idDev, WaveFormat& format, Event& event){Open (idDev, format, event);};
	~WaveOutDevice()
	{
		if (Ok())
		{
			waveOutReset(_handle);
			waveOutClose (_handle);
		}
	}

	bool    Open (UINT idDev, WaveFormat& format, Event& event);
	void    Reset (){if (Ok())waveOutReset (_handle);}
	bool    Close ();
	void    Prepare (WaveHeader * pHeader){waveOutPrepareHeader(_handle, pHeader, sizeof(WAVEHDR));}
	void	UnPrepare (WaveHeader * pHeader){waveOutUnprepareHeader (_handle, pHeader, sizeof(WAVEHDR));}
//	void    SendBuffer (WaveHeader * pHeader){waveOutAddBuffer (_handle, pHeader, sizeof(WAVEHDR));}
	bool    Ok () { return _status == MMSYSERR_NOERROR; }
//	void    Start () { waveOutRestart(_handle); }
//	void    Stop () { waveOutStop(_handle); }
	bool    isInUse () { return _status == MMSYSERR_ALLOCATED; }
	DWORD   GetPosSample ();
	UINT    GetError () { return _status; }
	void    GetErrorText (char* buf, int len){waveOutGetErrorText (_status, buf, len);}
	const char* GetDeviceName(){if(Ok()) return _wavOutCaps.szPname;return NULL;};
	
	static UINT GetNumDevs(){return waveOutGetNumDevs();}
	bool GetDevCaps(){_status = waveOutGetDevCaps(_idDev,&_wavOutCaps,sizeof(WAVEOUTCAPS));return Ok();};
	
private:
	UINT		_idDev;
	WAVEOUTCAPS	_wavOutCaps;
	HWAVEOUT	_handle;
	MMRESULT	_status;
};


class WaveInDevice
{
public:
	WaveInDevice (){_status = MMSYSERR_BADDEVICEID;};
	WaveInDevice (UINT idDev, WaveFormat& format, Event& event){Open (idDev, format, event);};
	~WaveInDevice ()
	{
		if (Ok())
		{
			waveInReset(_handle);
			waveInClose (_handle);
		}
	}

	bool    Open (UINT idDev, WaveFormat& format, Event& event);
	void    Reset (){if (Ok())waveInReset (_handle);}
	bool    Close ();
	void    Prepare (WaveHeader * pHeader){waveInPrepareHeader(_handle, pHeader, sizeof(WAVEHDR));}
	void    UnPrepare (WaveHeader * pHeader){waveInUnprepareHeader (_handle, pHeader, sizeof(WAVEHDR));}
	void    SendBuffer (WaveHeader * pHeader){waveInAddBuffer (_handle, pHeader, sizeof(WAVEHDR));}
	bool    Ok () { return _status == MMSYSERR_NOERROR; }
	void    Start () { waveInStart(_handle); }
	void    Stop () { waveInStop(_handle); }
	bool    isInUse () { return _status == MMSYSERR_ALLOCATED; }
	DWORD   GetPosSample ();
	UINT    GetError () { return _status; }
	void    GetErrorText (char* buf, int len){waveInGetErrorText (_status, buf, len);}
	const char* GetDeviceName(){if(Ok()) return _wavInCaps.szPname;	return NULL;};

	static UINT GetNumDevs(){return waveInGetNumDevs();}
	bool GetDevCaps(){_status = waveInGetDevCaps(_idDev,&_wavInCaps,sizeof(WAVEINCAPS));return Ok();}

private:
	UINT		_idDev;
	WAVEINCAPS	_wavInCaps;
	HWAVEIN		_handle;
	MMRESULT	_status;
};

inline bool WaveOutDevice::Open (UINT idDev, WaveFormat& format, Event& event)
{
	_idDev = idDev;

	if(!GetDevCaps()) 
		return false;

	_status = waveOutOpen (
        &_handle, 
        _idDev, 
        &format, 
        (DWORD) HANDLE (event),
        0, // callback instance data
        CALLBACK_EVENT);

	return Ok();
}

inline bool WaveOutDevice::Close ()
{
	if ( Ok() && waveOutClose (_handle) == 0)
	{
		_status = MMSYSERR_BADDEVICEID;
		return true;
	}
	else
		return false;
}


inline DWORD WaveOutDevice::GetPosSample ()
{
	MMTIME mtime;
	mtime.wType = TIME_SAMPLES;
	waveOutGetPosition (_handle, &mtime, sizeof (MMTIME));
	return mtime.u.sample;
}

inline bool WaveInDevice::Open (UINT idDev, WaveFormat& format, Event& event)
{
	_idDev = idDev;

	if(!GetDevCaps()) 
		return false;

	_status = waveInOpen (
        &_handle, 
        _idDev, 
        &format, 
        (DWORD) HANDLE (event),
        0, // callback instance data
        CALLBACK_EVENT);

	return Ok();
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


inline DWORD WaveInDevice::GetPosSample ()
{
	MMTIME mtime;
	mtime.wType = TIME_SAMPLES;
	waveInGetPosition (_handle, &mtime, sizeof (MMTIME));
	return mtime.u.sample;
}



#endif
