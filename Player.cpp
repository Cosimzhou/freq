
#include "Player.h"
#include "wassert.h"
#include "cassette.h"

Player::Player (CassetteTape &tape,
					int cSamples, 
					int cSamplePerSec, 
					int nChannels, 
					int bitsPerSample)
					: TapeUser(tape, cSamples, cSamplePerSec, nChannels, bitsPerSample),
					_isStarted (false)
{
	
}

Player::~Player()
{
	Stop();
	_Tape.Reset();
}



void Player::SetHeaderBuffer(int n)
{
	Assert(n < NUM_BUF);
	if(!_Tape.GetSafeAddress(n*_cbBuf, _header[n].lpData))
		Assert(0);
}

bool Player::Start(Event& event)
{
	WaveFormat format ( 
		_nChannels,
		_cSamplePerSec,
		_bitsPerSample );
	
	if (!format.isInSupported(0))
	{
		MessageBox (0, "格式不支持", "Player", MB_OK);
		return false;
	}
	
	_waveOutDevice.Open (0, format, event);
	if (!_waveOutDevice.Ok())
	{	// something unexpected occurs here
		char buf[164];
		if (_waveOutDevice.isInUse())	// device is occupied by other applications.
			strcpy (buf, "其它程序正在录音. 请关闭该程序后重试.");
		else	// something errors is unknown
			_waveOutDevice.GetErrorText (buf, sizeof (buf));
		MessageBox (0, buf, "Player", MB_OK);
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
		_waveOutDevice.Prepare (&_header[i]);
		
		// send to the device for writing data into
//		_waveOutDevice.SendBuffer (&_header [i]);
	}
	_isStarted = true;
	_iBuf = 0;
	
	// start device up
//	_waveOutDevice.Start();
	return true;
}

bool Player::BufferDone ()
{
	Assert (IsBufferDone ());
	_waveOutDevice.UnPrepare (&_header [_iBuf]);
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
	_waveOutDevice.Prepare (&_header [prevBuf]);
	
//	_waveOutDevice.SendBuffer (&_header [prevBuf]);
	return true;
}

void Player::Stop()
{
	_isStarted = false;
	_waveOutDevice.Reset ();
	_waveOutDevice.Close ();
}
