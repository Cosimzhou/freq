#if !defined XPTR_H
#define XPTR_H

template <class T>
class XPtr
{
public:
	~XPtr () { delete _p; }
	T* Acquire ()
	{
		T * pTmp = _p;
		_p = 0;
		return pTmp;
	}
	T * operator->() { return _p; }
	T const & GetAccess () const { return *_p; }
protected:
	// protected constructors
	XPtr (): _p (0) {}
	XPtr (T* p): _p (p) {}
	
	T * _p;
};

template <class T>
class XArray
{
public:
	XArray (int size)
		: _arr (new T [size]) {}
	~XArray () { delete []_arr; }
	T * Acquire ()
	{
		T * aTmp = _arr;
		_arr = 0;
		return aTmp;
	}
	T const & operator [] (int i) const { return _arr [i]; }
	T& operator [] (int i) { return _arr [i]; }
protected:
	T * _arr;
};

#endif