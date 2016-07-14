#if !defined PLAYER_H
#define PLAYER_H

#include "wavedev.h"
#include "cassette.h"
#include "TapeUser.h"

class Player  :public TapeUser
{
public:
//	Player();
	virtual ~Player();


    friend class SampleIter;
    enum { NUM_BUF = 8 };
public:
    Player(CassetteTape&,
        int cSamples, 
        int cSamplePerSec, 
        int nChannels, 
        int bitsPerSecond);
	
    bool    Start (Event& event);
    void    Stop ();
    DWORD   GetSamplePos () 
    { 
        return _waveOutDevice.GetPosSample(); 
    }
	
    bool    BufferDone ();
	


protected:

	void	SetHeaderBuffer(int n);
	
    bool            _isStarted;

    WaveOutDevice    _waveOutDevice;

};



class PlayerM8: public Player  // 8 bit mono
{
public:
    PlayerM8 (CassetteTape &tape, int cSamples, int cSamplesPerSec)
		: Player (tape, cSamples, cSamplesPerSec, 1, 8) {}
protected:
    int GetSample (char *pBuf, int i) const
    {
        return ((unsigned char)pBuf[i] - 128) * 64;
    }
};

class PlayerM16: public Player  // 16 bit mono
{
public:
    PlayerM16 (CassetteTape &tape, int cSamples, int cSamplesPerSec)
		: Player (tape, cSamples, cSamplesPerSec,  1, 16) {}
protected:
    int GetSample (char *pBuf, int i) const
    {
        return ((short *) pBuf)[i];
    }
};




#endif 