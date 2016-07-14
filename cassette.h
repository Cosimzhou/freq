#if !defined(AFX_CASSETTE_H__8FF2F281_F764_4A8C_B136_A67ED76047A1__INCLUDED_)
#define AFX_CASSETTE_H__8FF2F281_F764_4A8C_B136_A67ED76047A1__INCLUDED_

//////////////////////////////////////
//  cassette.h
//  record wave signal & fft result
//  (c) Cosim Sestal, 2011
//////////////////////////////////////


class Cassette  
{
public:
	Cassette():_iLen(0),_iBand(0){}
	~Cassette(){if(_iLen)Clear();}

	void AllocFFT(int band, int len)
	{
		Clear();
		_iBand = band; _iLen = len;
		_data = new char*[len];
		for(int i = 0; i < len; i++)
			_data[i] = new char[band];
	}

	void Clear()
	{
		if(_iLen)
		{
			for(int i = 0; i < _iLen; i++)
				delete[] _data[i];
			delete[] _data;
		}
		_iLen = _iBand = 0;
	}

private:
	char** _data;
	int _iLen, _iBand;
};

class CassetteTape  
{
public:
	CassetteTape():_iLen(0),_iCur(0){};
	~CassetteTape(){Clear();}
	
	void Reset(){_iCur = 0;}
	void Alloc(int len)
	{
		Clear();
		_iLen = len;
		_data = new char[_iLen];
	}
	
	void Clear()
	{
		if(_iLen)
		{
			delete[] _data;
			_iLen = _iCur = 0;
		}
	}
	
	bool GetSafeAddress(long offset, char* &lp)
	{
		if(offset < _iLen){
			lp = _data + offset;
			return true;
		}else{
			offset %= _iLen;
			lp = _data + offset;
			return false;
		}
	}
	bool GetNextBuffer(int cbuf, char* &lp)
	{
//		Assert(cbuf <= _iLen);
		if(cbuf*(_iCur+1) > _iLen)
		{
			lp = _data;
			_iCur = 1;
			return false;
		}else{
			lp = _data + _iCur*cbuf;
			_iCur++;
			return true;
		}
	}
private:
	char* _data;
	int _iLen, _iCur;
};
#endif // !defined(AFX_CASSETTE_H__8FF2F281_F764_4A8C_B136_A67ED76047A1__INCLUDED_)
