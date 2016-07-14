// TapeUser.cpp: implementation of the TapeUser class.
//
//////////////////////////////////////////////////////////////////////

#include "TapeUser.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TapeUser::TapeUser(CassetteTape &tape,
					 int cSamples, 
					 int cSamplePerSec, 
					 int nChannels, 
					 int bitsPerSample)
					 : _Tape(tape),
					 _iBuf(0),
					 _cSamplePerSec (cSamplePerSec), 
					 _cSamples (cSamples), 
					 _cbSampleSize (nChannels * bitsPerSample/8), 
					 _cbBuf (cSamples * nChannels * bitsPerSample/8),	// PCM size for one second
					 _nChannels (nChannels),
					 _bitsPerSample (bitsPerSample)//					_isStarted (false)
{

}

TapeUser::~TapeUser()
{

}
