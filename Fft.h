#if !defined FFT_H
#define FFT_H

//------------------------------------
//  fft.h
//  Fast Fourier Transform
//  (c) Cosim Sestall, 2007
//------------------------------------

#include "complex.h"
#include "wassert.h"

class SampleIter;

class Fft
{
public:
	Fft (int Points, long sampleRate);
	~Fft ();
	int     Points () const { return _Points; }
	void    Transform ();
	void    CopyIn (SampleIter& iter);
	
	double  GetIntensity (int i) const		// Intensity depends on the A in ploynomial "A*sin(w*x+s)"
	{ 
		Assert (i < _Points);
		return _X[i].Mod()/_sqrtPoints; 
	}
	
	int     GetFrequency (int point) const
	{
		// return frequency in Hz of a given point
		Assert (point < _Points);
		long x =_sampleRate * point;
		return x / _Points;
	}
	
	int     HzToPoint (int freq) const 
	{ 
		return (long)_Points * freq / _sampleRate; 
	}
	
	int     MaxFreq() const { return _sampleRate; }
	
	int     Tape (int i) const
	{
		Assert (i < _Points);
		return (int) _aTape[i];
	}
	
	
	
private:
	
	void PutAt ( int i, double val )
	{
		_X [_aBitRev[i]] = Complex (val);
	}
	
	int			_Points;
	long		_sampleRate;
	int			_logPoints;
	double		_sqrtPoints;
	int		   *_aBitRev;       // bit reverse vector
	Complex	   *_X;             // in-place fft array, of cause the result will be placed here, too.
	Complex	  **_W;             // exponentials
	double     *_aTape;         // recording tape, where audio signal input.
};

#endif
