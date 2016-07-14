#include "display.h"
#include "views.h"
#include "canvas.h"
#include "recorder.h"
#include "fft.h"
#include "winex.h"
#include "wassert.h"

Painter::Painter (
				  HWND hwnd,
				  ViewWave& viewWave,
				  ViewFreq& viewFreq,
				  ViewElecLevel &viewElec,
				  ViewPiano &viewPiano,
				  int samplesPerBuf, 
				  int samplesPerSec, 
				  int fftPoints,
				  CassetteTape& tape 
				  )
				  :_hwnd (hwnd), 
				  _pcmTape(tape),
				  _viewWave (viewWave),
				  _viewFreq (viewFreq),
				  _viewElec (viewElec),
				  _viewPiano(viewPiano),
				  _samplesPerBuf (samplesPerBuf),
				  _samplesPerSecond (samplesPerSec),
				  _fftPoints (fftPoints),
				  _bitsPerSample (16),
				  _pFftTransformer (fftPoints, samplesPerSec),
				  _pPlayer(samplesPerBuf, samplesPerSec, tape),
				  _pRecorder (samplesPerBuf, samplesPerSec, tape)
{
	// Let the caller Resume explicitly
	//_thread.Resume ();
}

void Painter::Run ()
{
	for (;;)
	{
		_event.Wait ();
		if (_isDying)
			return;

		Lock lock (_mutex);
		switch(_workType){
		case RECORD:
			if (_pRecorder->IsBufferDone ())
				LokWaveInData ();
			break;
		}
	}
}

void Painter::FlushThread ()
{
	_event.Release ();
}

bool Painter::ReInit (
					  int samplesPerBuf, 
					  int samplesPerSec, 
					  int fftPoints,
					  int bitsPerSample)
{
	Lock lock (_mutex);
	
	if (_pRecorder->BitsPerSample() == bitsPerSample &&
		_pRecorder->SamplesPerSecond() == samplesPerSec &&
		_pFftTransformer->Points() == fftPoints &&
		_pRecorder->SampleCount() == samplesPerBuf)
	{
		return true;
	}
	
	_samplesPerBuf = samplesPerBuf;
	_samplesPerSecond = samplesPerSec;
	_fftPoints = fftPoints;
	_bitsPerSample = bitsPerSample;
	
	bool isStarted = _pRecorder->IsStarted();
	if (isStarted)
		_pRecorder->Stop();
	
	bool isError = false;
	
	try
	{
		_pFftTransformer.ReInit ( _fftPoints, _samplesPerSecond);
		_pRecorder.ReInit (_bitsPerSample, _samplesPerBuf, _samplesPerSecond, _pcmTape);
	}
	catch ( WinException e )
	{
		char buf[50];
		wsprintf ( buf, "%s, Error %d", e.GetMessage(), e.GetError() );
		MessageBox (0, buf, "Exception", MB_ICONEXCLAMATION | MB_OK);
		isError = true;
	}
	catch (...)
	{
		MessageBox (0, "Unknown", "Exception", MB_ICONEXCLAMATION | MB_OK);
		isError = true;
	}
	
	if (isStarted)
	{
		isError = !_pRecorder->Start(_event);
	}
	
	return !isError;
}

bool Painter::Start (WorkingMode type)
{
	Lock lock (_mutex);
	
	_workType = type;
	return _pRecorder->Start (_event);
}

void Painter::Stop ()
{
	Lock lock (_mutex);
	_pRecorder->Stop ();
}

void Painter::LokWaveInData ()
{
	SampleIter iter(_pRecorder.GetAccess());
	// Quickly release the buffer
	if (!_pRecorder->BufferDone ())
		return;
	
	_pFftTransformer->CopyIn (iter);
	_pFftTransformer->Transform();
	_viewFreq.Update (_pFftTransformer.GetAccess());
	_viewWave.Update (_pFftTransformer.GetAccess());
	_viewElec.Update (_pFftTransformer.GetAccess());
	_viewPiano.Update(_pFftTransformer.GetAccess());
}


