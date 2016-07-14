#if !defined(AFX_TAPEUSER_H__C528687A_52FA_4BF0_9C6E_2D81E2FA230C__INCLUDED_)
#define AFX_TAPEUSER_H__C528687A_52FA_4BF0_9C6E_2D81E2FA230C__INCLUDED_

#include "wavedev.h"
#include "wassert.h"

class CassetteTape;

class TapeUser  
{
	friend class SampleIter;
public:
	
	enum { NUM_BUF = 8 };
	TapeUser(CassetteTape &tape,
		int cSamples, 
		int cSamplePerSec, 
		int nChannels, 
		int bitsPerSample);
	
	virtual ~TapeUser();
	
	virtual DWORD   GetSamplePos () =0;
	
	bool    IsBufferDone () const 
	{ 
		return _header [_iBuf].IsDone (); 
	}
	int     SampleCount () const { return _cSamples; }
	int     BitsPerSample () const { return _bitsPerSample; }
	int     SamplesPerSecond () const { return _cSamplePerSec; }
protected:
	virtual int GetSample (char *pBuf, int i) const = 0;
	char * GetData () const { return _header [_iBuf].lpData; }
	
	
	int             _cSamplePerSec;     // sampling frequency
	int             _cSamples;          // samples per buffer
	int             _nChannels;
	int             _bitsPerSample;
	int             _cbSampleSize;      // bytes per sample
	
	CassetteTape&	_Tape;
	int             _cbBuf;             // bytes per buffer
	int             _iBuf;              // current buffer #
	WaveHeader      _header [NUM_BUF];  // pool of headers/**/
	
};

class SampleIter
{
public:
	SampleIter (TapeUser const &recorder)
		: _recorder (recorder), _iCur(0)
	{
		_pBuffer = recorder.GetData ();
		_iEnd = recorder.SampleCount();
	};
	

	SampleIter& operator++(){_iCur++; return *this;}
	int operator*()const {return _recorder.GetSample(_pBuffer, _iCur);}

	bool AtEnd () const { return _iCur == _iEnd;}
	void Rewind () { _iCur = _iEnd - _recorder.SampleCount(); }
	int  Count () const { return _recorder.SampleCount(); }
private:
	char	   *_pBuffer;
	TapeUser const &_recorder;
	int         _iCur;
	int         _iEnd;
};


#endif // !defined(AFX_TAPEUSER_H__C528687A_52FA_4BF0_9C6E_2D81E2FA230C__INCLUDED_)
