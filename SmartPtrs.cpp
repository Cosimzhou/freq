//////////////////////////////////////
//  SmartPtrs.cpp
//  Smart pointers to objects
//  (c) Cosim Sestal, 2006
//////////////////////////////////////

#include "smartptrs.h"

void PtrPlayer::ReInit (int bitsPerSample,
						int cSamples, 
						int cSamplePerSec,
						CassetteTape &tape)
{
    delete _p;
    _p = 0;
    if (bitsPerSample == 8)
    {
        _p = new PlayerM8 (tape, cSamples, cSamplePerSec);
    }
    else
    {
		Assert (bitsPerSample == 16);
        _p = new PlayerM16 (tape, cSamples, cSamplePerSec);
    }	
}



void PtrRecorder::ReInit (int bitsPerSample,
                          int cSamples, 
                          int cSamplePerSec,
						  CassetteTape &tape)
{
    delete _p;
    _p = 0;
    if (bitsPerSample == 8)
    {
        _p = new RecorderM8 (tape, cSamples, cSamplePerSec);
    }
    else
    {
	    Assert (bitsPerSample == 16);
        _p = new RecorderM16 (tape, cSamples, cSamplePerSec);
    }
}

void PtrFft::ReInit(int points, long sampleRate)
{
    delete _p;
    _p = 0;
    _p = new Fft(points, sampleRate);
}