#if !defined SMART_PTRS_H
#define SMART_PTRS_H
////////////////////////////////////////
//  smartptrs.h
//  Smart pointers to objects
//  (c) Cosim Sestal, 2006
////////////////////////////////////////

#include "recorder.h"
#include "player.h"
#include "fft.h"
#include "xptr.h"

class PtrPlayer : public XPtr<Player>
{
public:
	PtrPlayer (int cSamples, 
		int cSamplePerSec, CassetteTape &tape)
		: XPtr<Player>(new PlayerM16 (tape, cSamples, cSamplePerSec)) 
	{}
	
	void ReInit (int bitsPerSample,
		int cSamples, 
		int cSamplePerSec,
		CassetteTape &tape);
};

class PtrRecorder : public XPtr<Recorder>
{
public:
	PtrRecorder (int cSamples, 
		int cSamplePerSec, CassetteTape &tape)
		: XPtr<Recorder>(new RecorderM16 (tape, cSamples, cSamplePerSec)) 
	{}
	
	void ReInit (int bitsPerSample,
		int cSamples, 
		int cSamplePerSec,
		CassetteTape &tape);
};

class PtrFft : public XPtr<Fft>
{
public:
	PtrFft (int points, long sampleRate)
		: XPtr<Fft> (new Fft (points, sampleRate))
	{}
	
	void ReInit (int points, long sampleRate);
};

#endif