#if !defined VIEWS_H
#define VIEWS_H

#include "controls.h"
#include "canvas.h"

class Fft;
class SampleIter;

class ViewFreq: public SimpleControl
{
public:
    ViewFreq (HWND hwndParent, int id) 
        : SimpleControl (hwndParent, id), _bmpBack(),
          _xRecord(0) {Clear();}
    void Update (Fft const & fftTransformer);
    void Clear ();
    void Fake ();
private:
    int         _xRecord;
	Bitmap      _bmpBack;
};

class ViewPiano: public SimpleControl
{
public:
	ViewPiano(HWND hwndParent, int id) 
        : SimpleControl (hwndParent, id)//,
		 //_penYellow (RGB(255, 255, 0)),
		 //_elecLevel(0.0), _oldElec(0.0), _fallTime(0)
	{}

	void Update (Fft const & fftTransformer);
	void SetRank(int);
private:
	bool		_bScaleLine;
	int			_iYRank;
};

class ViewWave: public SimpleControl
{
public:
    ViewWave (HWND hwndParent, int id, int cSamples) 
        : SimpleControl (hwndParent, id),
          _poly (cSamples),		  
          _penGreen (RGB(0, 255, 128))		  
    {}
    void Update (Fft const & fftTransformer);
private:
    PolyLine    _poly;
    Pen         _penGreen;	
};

class ViewElecLevel: public SimpleControl
{
public:
	ViewElecLevel(HWND hwndParent, int id) 
        : SimpleControl (hwndParent, id),
		 _penYellow (RGB(255, 255, 0)),
		 _elecLevel(0.0), _oldElec(0.0), _fallTime(0)
//		_poly (cSamples),		  
//		_penGreen (RGB(0, 255, 128))		  
    {}
	void Update (Fft const & fftTransformer);
private:
	Pen _penYellow;
	double _elecLevel, _oldElec;
	int _fallTime;
};

#endif