#if !defined CANVAS_H
#define CANVAS_H
//////////////////////////////////////
//  canvas.h
//  Graphics
//  (c) Cosim Sestal, 2006
//////////////////////////////////////

#include <windows.h>

class Canvas
{
public:
    operator HDC () 
    { 
        return _hdc; 
    }

    void SetViewportOrg (int x, int y)
    {
        ::SetViewportOrgEx (_hdc, x, y, 0);
    }

    void SetMapModeText ()
    {
        SetMapMode (_hdc, MM_TEXT);
    }

    void Point (int x, int y, COLORREF color)
    {
        ::SetPixel (_hdc, x, y, color);
    }

    void MoveTo (int x, int y)
    {
        ::MoveToEx (_hdc, x, y, 0);
    }

    void Line (int x1, int y1, int x2, int y2)
    {
        ::MoveToEx (_hdc, x1, y1, 0);
        ::LineTo (_hdc, x2, y2);
    }

    void LineTo (int x, int y)
    {
        ::LineTo (_hdc, x, y);
    }

    void ClearWhite ( RECT& rect )
    {
        FillRect ( _hdc, &rect, (HBRUSH) GetStockObject (WHITE_BRUSH));
    }

    void ClearBlack ( RECT& rect )
    {
        FillRect ( _hdc, &rect, (HBRUSH) GetStockObject (BLACK_BRUSH));
    }

	void FillRectangle(RECT& rect, COLORREF color)
	{
		HBRUSH hbr = ::CreateSolidBrush(color);
		FillRect ( _hdc, &rect, hbr);
		::DeleteObject(hbr);
	}

    void Text ( int x, int y, char* buf, int cBuf )
    {
        ::TextOut ( _hdc, x, y, buf, cBuf );
    }

    void Char ( int x, int y, char c )
    {
        ::TextOut ( _hdc, x, y, &c, 1 );
    }

    void SelectObject (void* pObj)
    {
        ::SelectObject (_hdc, pObj);
    }

protected:
    Canvas(HDC hdc) :_hdc(hdc) {}

    HDC  _hdc;
};

// Use for painting after WM_PAINT

class PaintCanvas: public Canvas
{
public:
    PaintCanvas (HWND hwnd)
    :   Canvas (BeginPaint (hwnd, &_paint)),
        _hwnd (hwnd)
    {}

    ~PaintCanvas ()
    {
        EndPaint(_hwnd, &_paint);
    }
 
protected:
                 
    PAINTSTRUCT _paint;
    HWND        _hwnd;
};

// Device Context
// Use for painting other than WM_PAINT

class UpdateCanvas: public Canvas
{
public:
    UpdateCanvas (HWND hwnd)
    :   Canvas (GetDC(hwnd)),
        _hwnd(hwnd)
    {}
    
    ~UpdateCanvas ()
    {
        ReleaseDC (_hwnd, _hdc);
    }

protected:

    HWND _hwnd;
};

class MemCanvas: public Canvas
{
public:
    MemCanvas (HDC hdc) 
        : Canvas (CreateCompatibleDC (hdc))
    {}
    ~MemCanvas ()
    {
        DeleteDC(_hdc); 
    }
};

class DrawItemCanvas: public Canvas
{
public:
    DrawItemCanvas (DRAWITEMSTRUCT *pDraw)
        : Canvas (pDraw->hDC), _pDraw (pDraw)
    {}
    int Left () const { return _pDraw->rcItem.left; }
    int Top () const { return _pDraw->rcItem.top; }
    int Right () const { return _pDraw->rcItem.right; }
    int Bottom () const { return _pDraw->rcItem.bottom; }
    int Width () const 
    { 
        return _pDraw->rcItem.right - _pDraw->rcItem.left; 
    }
    int Height () const 
    { 
        return _pDraw->rcItem.bottom - _pDraw->rcItem.top; 
    }
private:
    DRAWITEMSTRUCT * _pDraw;
};

class StockObject
{
public:
    StockObject (HDC hdc, int type)
      : _hdc(hdc)
    {
        _hObjOld = SelectObject ( _hdc, GetStockObject(type));
    }

    ~StockObject()
    {
        SelectObject ( _hdc, _hObjOld );
    }
private:
    HGDIOBJ  _hObjOld;
    HDC      _hdc;
};

class WhitePen : public StockObject
{
public:
    WhitePen (HDC hdc): StockObject (hdc, WHITE_PEN) {}
};

class BlackPen : public StockObject
{
public:
    BlackPen (HDC hdc): StockObject (hdc, BLACK_PEN) {}
};

class Pen
{
public:
    Pen (COLORREF color)
    {
        _hPen = CreatePen (PS_SOLID, 0, color);
    }
    ~Pen ()
    {
        DeleteObject (_hPen);
    }
    operator HPEN () { return _hPen; }
private:
    HPEN    _hPen;
};

class PenHolder
{
public:
    PenHolder (HDC hdc, HPEN hPen)
        : _hdc (hdc)
    {
        _hPenOld = (HPEN)SelectObject (_hdc, hPen); 
    }
    ~PenHolder ()
    {
        SelectObject (_hdc, _hPenOld);
    }
private:
    HDC     _hdc;
    HPEN    _hPenOld;
};

class ColorPen
{
public:
    ColorPen (HDC hdc, COLORREF color)
        : _hdc (hdc)
    {
        _hPen = CreatePen (PS_SOLID, 0, color);
        _hPenOld = (HPEN)SelectObject (_hdc, _hPen); 
    }
    ~ColorPen ()
    {
        SelectObject (_hdc, _hPenOld);
        DeleteObject (_hPen);
    }
private:
    HDC     _hdc;
    HPEN    _hPen;
    HPEN    _hPenOld;
};

class ColorBrush
{
public:
    ColorBrush (HDC hdc, COLORREF color)
        : _hdc (hdc)
    {
        _hBrush = CreateSolidBrush (color);
        _hBrushOld = (HBRUSH)SelectObject (_hdc, _hBrush); 
    }
    ~ColorBrush ()
    {
        SelectObject (_hdc, _hBrushOld);
        DeleteObject (_hBrush);
    }
private:
    HDC     _hdc;
    HBRUSH  _hBrush;
    HBRUSH  _hBrushOld;
};

class Bitmap
{
public:
    Bitmap () : _hbm (0) {}
    ~Bitmap () 
    { 
        if (_hbm)
            DeleteObject (_hbm); 
    }
    operator HBITMAP () { return _hbm; }
    void Load (HINSTANCE hInst, char const * name)
    {
        _hbm = LoadBitmap (hInst, name);
    }
protected:
    HBITMAP _hbm;
};

class PolyLine
{
public:
    PolyLine (int cPoints)
    : _cPoints (cPoints)
    {
        _points = new POINT [cPoints];
    }

    ~PolyLine ()
    {
        delete []_points;
    }

    void Add (int i, int x, int y)
    {
        _points [i].x = x;
        _points [i].y = y;
    }

    void Paint (Canvas& canvas)
    {
        Paint (canvas, _cPoints);
    }

    void Paint (Canvas& canvas, int cPoints)
    {
        Polyline (canvas, _points, cPoints);
    }

    int Points () const { return _cPoints; }
private:
    int     _cPoints;
    POINT   *_points;
};

class CsmRect:public RECT
{
public:
	CsmRect(){};
	CsmRect(int l, int r, int t, int b)
		{left=l, right=r, top=(t), bottom=(b);}
	~CsmRect(){};

	long Cx() const {return right-left;}
	long Cy() const {return bottom-top;}

	void Move(int x, int y){ left+=x, right+=x; top+=y, bottom+=y;}
	bool IsContain(int x, int y){return (left<=x && x<= right)&&(top<=y && y<=bottom);}
};

class ClientRect: public CsmRect
{
public:
    ClientRect (HWND hwnd)
    {
        ::GetClientRect (hwnd, this);
    }
	ClientRect& GetClientRect(HWND hwnd)
	{
        ::GetClientRect (hwnd, this);
		return *this;
	}
	friend class WindowRect;
protected:
	ClientRect (const RECT &rect)
    {
        left = rect.left;
		right = rect.right;
		top = rect.top;
		bottom = rect.bottom;			
    }
};

class WindowRect: public CsmRect
{
public:
    WindowRect (HWND hwnd)
    {
        ::GetWindowRect (hwnd, this);
    }

	WindowRect& GetWindowRect(HWND hwnd)
	{
        ::GetWindowRect (hwnd, this);
		return *this;
	}

	ClientRect GetClientRect(HWND hwnd)
	{
		POINT point;
		point.x = left, point.y = top;
		ScreenToClient(hwnd, &point);
		left = point.x, top = point.y;
		
		point.x = right, point.y = bottom;
		ScreenToClient(hwnd, &point);
		right = point.x, bottom = point.y;
		return ClientRect(*this);
	}
};
#endif
