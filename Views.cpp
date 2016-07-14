#include "views.h"
#include "canvas.h"
#include "fft.h"
#include "recorder.h"

COLORREF MapColor (int s)
{
	if ( s < 16 )
		return RGB(0, 0, 128);
	else if ( s < 32)
		return RGB(0, 0, 255);
	else if ( s < 64 )
		return RGB(0, 255, 0);
	else if ( s < 128)
		return RGB(255, 255, 0);
	else if ( s < 256 )
		return RGB(255, 128, 0);
	else
		return RGB(255, 0, 0);
}

inline COLORREF ElecColor (double s)
{
	if(s<0.5) return RGB(0, 127, 0);
	if(s<0.8) return RGB(255 * (s-0.5) * 2, 127 + 128 * 3 * (s-0.5), 0);
	return RGB(255 * (0.6 + (s-0.8)*2), 255 - 5*255*(s-0.8), 0);
}
void ViewFreq::Clear ()
{
	if (Hwnd ())
	{
		UpdateCanvas canvas (Hwnd ());
		ClientRect rect (Hwnd ());
		canvas.ClearBlack(rect);
	}
}

void ViewFreq::Update (Fft const &fftTransformer)
{
	UpdateCanvas canvas (Hwnd ());
	ClientRect rect (Hwnd ());
	{
		// Erase background for current spectrum
		BlackPen pen(canvas);
		canvas.Line (_xRecord, 0, _xRecord, rect.bottom);
		canvas.Line (_xRecord + 1, 0, _xRecord + 1, rect.bottom);
	}
	
	
	for (int i = 0; 
		i < fftTransformer.Points() / 2 && i < rect.bottom; 
		i++ )
	{
		int s = int (fftTransformer.GetIntensity(i) / 256);
		
		COLORREF color;
		if (s > 8)
		{
			color = MapColor (s);
			
			canvas.Point (_xRecord, rect.bottom - i - 1, color);
			canvas.Point (_xRecord + 1, rect.bottom - i - 1, color);
		}
	}
	
	_xRecord += 2;
	if (_xRecord >= rect.right)
		_xRecord = 0;
	
	{
		// Draw white vertical mark
		WhitePen pen(canvas);
		canvas.Line (_xRecord, 0, _xRecord, rect.bottom);
	}
}

void ViewFreq::Fake ()
{
	UpdateCanvas canvas (Hwnd ());
	ClientRect rect (Hwnd ());
	
	{
		// Erase background for current spectrum
		BlackPen pen(canvas);
		canvas.Line (_xRecord, 0, _xRecord, rect.bottom);
		canvas.Line (_xRecord + 1, 0, _xRecord + 1, rect.bottom);
	}
	
	
	_xRecord += 2;
	if (_xRecord >= rect.right)
		_xRecord = 0;
	
	{
		// Draw white vertical mark
		WhitePen pen(canvas);
		canvas.Line (_xRecord, 0, _xRecord, rect.bottom);
	}
}

void ViewWave::Update (Fft const &fftTransformer)
{
	UpdateCanvas canvas (Hwnd ());
	ClientRect rect (Hwnd ());
	canvas.ClearBlack(rect);
	int cMaxPoints = min (fftTransformer.Points(), _poly.Points());
	for ( int i = 0; i < cMaxPoints; i++ )
	{
		int s  = fftTransformer.Tape(i) / 512 + (rect.bottom - 1) / 2;
		
		//·´ÏàµÄ
//		int ns = -fftTransformer.Tape(i) / 512 + (rect.bottom - 1) / 2;
		
		if (i >= rect.right)
		{
			_poly.Add( i, rect.right - 1, (rect.bottom - 1) / 2);
		}
		else
		{
			if ( s < 0 )
			{
				_poly.Add (i, i, 0);							
			}
			else if (s >= rect.bottom)
			{
				_poly.Add (i, i, rect.bottom - 1);				
			}
			else
			{
				_poly.Add (i, i, s);				
			}
		}
	}
	
	PenHolder pen (canvas, _penGreen);	
	_poly.Paint (canvas, cMaxPoints);	
}

void ViewElecLevel::Update (Fft const &fftTransformer)
{
	UpdateCanvas canvas (Hwnd ());
	ClientRect rect (Hwnd ());
//	canvas.ClearBlack(rect);
	
	double sum = 0, val;
//	fftTransformer.

	for(int i = 0; i < fftTransformer.Points(); i++)
	{
//		val = fabs(fftTransformer.Tape(i)) ;
//		if(sum < val) max = val;
		sum += fabs(fftTransformer.Tape(i));
	}

	_elecLevel = sum/(1<<13)/fftTransformer.Points();
	
	_elecLevel = (exp(_elecLevel)-1.0)/(exp(1.0)-1.0);

	if(_elecLevel > 1) _elecLevel = 1.0;
	_oldElec -= 0.001*(_fallTime++);

	if(_oldElec <= _elecLevel)
	{
		_oldElec = _elecLevel + 0.001;
		_fallTime = 0;
	}

	CsmRect secRect;
	secRect.top = 0;
	secRect.bottom = rect.bottom;
	if(_elecLevel > 0.5)
	{
		secRect.left = 0;
		secRect.right = rect.right/2;
		canvas.FillRectangle(secRect, ElecColor(0.0));
		for(double j = 50; j < 100; ++j)
		{
			secRect.left = j/100.0 * rect.right;
			if(_elecLevel >= (j+1.0)/100.0)
			{
				secRect.right = (j+1.0)/100.0 * rect.right;
			}
			else if(_elecLevel > j/100.0)
			{
				secRect.right = _elecLevel * rect.right;
			}
			else
				break;
			
			canvas.FillRectangle(secRect, ElecColor(j/100.0));
		}
	}else
	{
		secRect.left = 0;
		secRect.right = _elecLevel * rect.right;
		canvas.FillRectangle(secRect, ElecColor(0.0));
	}

	if(secRect.right < rect.right)
	{
		secRect.left = secRect.right;
		secRect.right = rect.right;
		canvas.ClearBlack(secRect);
	}
//	canvas.

	PenHolder pen (canvas, _penYellow);	
	canvas.Line(rect.Cx()*_oldElec, 0, rect.Cx()*_oldElec, rect.Cy());
	//_poly.Paint (canvas, cMaxPoints);
}



void ViewPiano::SetRank(int rnk)
{
	_iYRank = rnk;
//	if(rnk < 0)
//		_;
}
void ViewPiano::Update (Fft const &fftTransformer)
{
	UpdateCanvas canvas(Hwnd());
	ClientRect rect (Hwnd ());
	
	CsmRect pianoRect(rect.right - 40, rect.right, rect.top, rect.bottom), blackKey ;

	canvas.ClearBlack(rect);

	canvas.ClearWhite(pianoRect);
	float x = rect.Cy() / 52;
	
	blackKey.left = pianoRect.left;
	blackKey.right = pianoRect.left + pianoRect.Cx()/2;
	blackKey.top = pianoRect.bottom -x/3;
	blackKey.bottom = pianoRect.bottom+x/3;

	for(int i = 1; i <= 52; ++i)
	{
		canvas.Line(pianoRect.left, rect.bottom-i*x, pianoRect.right, rect.bottom-i*x);
		if(i== 52) break;
		blackKey.Move(0, -x);
		if(i%7!=2 && i%7!=5)
			canvas.FillRectangle(blackKey, 0);
	}


}