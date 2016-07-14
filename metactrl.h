#if !defined METACTRL_H
#define METACTRL_H

#include "canvas.h"
#include "controls.h"

// Rectangle in hundredths of millimiters
class RectHmm: public RECT
{
public:
    RectHmm (Canvas & canvRef, RECT const & rect)
    {
        int hor     = GetDeviceCaps (canvRef, HORZSIZE);
        int vert    = GetDeviceCaps (canvRef, VERTSIZE);

        int horPix  = GetDeviceCaps (canvRef, HORZRES);
        int vertPix = GetDeviceCaps (canvRef, VERTRES);

        int horScale = hor * 100;
        horScale /= horPix;   // hundredths of mm per pixel
        int vertScale = vert * 100;
        vertScale /= vertPix; // hundredths of mm per pixel

        top    = rect.top * vertScale;
        bottom = rect.bottom * vertScale;
        left   = rect.left * horScale;
        right  = rect.right * horScale;
    }
};

class CanvasEMFCtrl;

// Enhanced metafile
// Initialized using metafile canvas

class EnhMetafile
{
public:
    EnhMetafile () : _hemf (0) {}

    ~EnhMetafile ()
    {
        if (_hemf)
            ::DeleteEnhMetaFile (_hemf);
    }

    void AcquireCanvas (CanvasEMFCtrl &canvas);

    operator HENHMETAFILE () { return _hemf; }

private:
    HENHMETAFILE  _hemf;
};

// Static control using enhanced metafile
// The metafile is created using CanvasEMGCtrl 
// and then attached to the control

class StaticEnhMetafileControl: public SimpleControl
{
    friend class CanvasEMFCtrl;
public:
    StaticEnhMetafileControl (HWND hwndParent, int id)
        : SimpleControl (hwndParent, id)
    {}

    RECT& Rect ()
    {
        GetClientRect (_hWnd, &_rect);
        return _rect;
    }

    void Attach (CanvasEMFCtrl& canvas)
    {
        _metaFile.AcquireCanvas (canvas);
        PostMessage (_hWnd, STM_SETIMAGE, IMAGE_ENHMETAFILE, 
            (LPARAM) (HENHMETAFILE) _metaFile);
    }
private:
    EnhMetafile _metaFile;
    RECT        _rect;
};

// Canvas used to draw a metafile for a static metafile control

class CanvasEMFCtrl: public Canvas
{
public:
    CanvasEMFCtrl (RectHmm &rect, Canvas &canvRef)
        : Canvas (CreateEnhMetaFile (canvRef, 0, &rect, 0))
    {}

    ~CanvasEMFCtrl () 
    {
        if (_hdc)
        {
            HENHMETAFILE hemf = ::CloseEnhMetaFile(_hdc);
            ::DeleteEnhMetaFile (hemf);
        }
    }
    void Clear () { _hdc = 0; }
};

inline void EnhMetafile::AcquireCanvas (CanvasEMFCtrl &canvas)
{
    if (_hemf)
        ::DeleteEnhMetaFile (_hemf);
    _hemf = ::CloseEnhMetaFile(canvas);
    canvas.Clear ();
}

#endif
