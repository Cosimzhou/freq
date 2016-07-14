#include "recorder.h"
#include "wassert.h"
#include "cassette.h"

// Call BufferDone after creating the iterator


Recorder::Recorder (CassetteTape &tape,
					int cSamples, 
					int cSamplePerSec, 
					int nChannels, 
					int bitsPerSample)
					:TapeUser(tape, cSamples, cSamplePerSec, nChannels, bitsPerSample),
					_isStarted (false)
{

}

Recorder::~Recorder()
{
	Stop();
	_Tape.Reset();
}

void Recorder::SetHeaderBuffer(int n)
{
	Assert(n < NUM_BUF);
	if(!_Tape.GetSafeAddress(n*_cbBuf, _header[n].lpData))
		Assert(0);
}

bool Recorder::Start(Event& event)
{
	WaveFormat format ( 
		_nChannels,
		_cSamplePerSec,
		_bitsPerSample );
	
	if (!format.isInSupported(0))
	{
		MessageBox (0, "格式不支持", "Recorder", MB_OK);
		return false;
	}
	
	_waveInDevice.Open (0, format, event);
	if (!_waveInDevice.Ok())
	{	// something unexpected occurs here
		char buf[164];
		if (_waveInDevice.isInUse())	// device is occupied by other applications.
			strcpy (buf, "其它程序正在录音. 请关闭该程序后重试.");
		else	// something errors is unknown
			_waveInDevice.GetErrorText (buf, sizeof (buf));
		MessageBox (0, buf, "Recorder", MB_OK);
		return false;
	}
	
	// Don't initialize the last buffer
	// It will be initialized in the
	// first call to BufferDone
	for ( int i = 0; i < NUM_BUF - 1; i++ )
	{
		SetHeaderBuffer(i);
		_header[i].dwBufferLength = _cbBuf;
		_header[i].dwFlags = 0;
		_header[i].dwLoops = 0;
		
		// prepare the headers for sending
		_waveInDevice.Prepare (&_header[i]);
		
		// send to the device for writing data into
		_waveInDevice.SendBuffer (&_header [i]);
	}
	_isStarted = true;
	_iBuf = 0;
	
	// start device up
	_waveInDevice.Start();
	return true;
}

bool Recorder::BufferDone ()
{
	Assert (IsBufferDone ());
	_waveInDevice.UnPrepare (&_header [_iBuf]);
	int prevBuf = _iBuf - 1;
	if (prevBuf < 0)
		prevBuf = NUM_BUF - 1;
	
	// Next buffer to be filled
	_iBuf++;
	if ( _iBuf == NUM_BUF )
		_iBuf = 0;

	_Tape.GetNextBuffer(_cbBuf,  _header[prevBuf].lpData);

	_header[prevBuf].dwBufferLength = _cbBuf;
	_header[prevBuf].dwFlags = 0;
	_header[prevBuf].dwLoops = 0;
	_waveInDevice.Prepare (&_header [prevBuf]);
	
	_waveInDevice.SendBuffer (&_header [prevBuf]);
	return true;
}

void Recorder::Stop()
{
	_isStarted = false;
	_waveInDevice.Reset ();
	_waveInDevice.Close ();
}

