#if !defined view_h
#define view_h

#include "canvas.h"

class Fft;
class SampleIter;
class Painter;

class View
{
public:
    View ();
    void Paint (HWND hwnd, Painter & disp);
private:
    void PaintTimeScale ( HWND hwnd, Painter & disp);
    void PaintFreqScale ( HWND hwnd, Painter & disp);

    StaticEnhMetafileControl    _scaleFreq;
    StaticEnhMetafileControl    _scaleTime;
};

#endif
