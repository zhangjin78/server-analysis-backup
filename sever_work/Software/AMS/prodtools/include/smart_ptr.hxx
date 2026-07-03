#ifndef _SMART_PTR_H
#define _SMART_PTR_H

template <typename T> class smart_ptr
{
private:
	T*    _ptr;
	long* _count;

public:
	explicit smart_ptr<T>(T* p=0) throw(): _ptr(p)
	{
		if (p) 	{
			_count = new long;
			*_count = 1;
		}
		else
			_count = 0;
	};
	smart_ptr<T>(const smart_ptr<T>& p) throw()
	{
		if (this != &p) arrange(p);
	};
	~smart_ptr() throw()
	{
		dispose();
	};

public:
	bool last()
	{
		return (*_count == 1);
	};
	void release()
	{
		delete _count;
		_count = 0;
	};
public:
	T *ptr() { return _ptr; };
	smart_ptr<T>& operator =(const smart_ptr<T>& p) throw()
	{
		dispose();

		if (this != &p) 
			arrange(p);
		return *this;
	};
	T& operator*() const throw()  { return *_ptr; };
	T& operator[](long id) const throw()  { return _ptr[id]; };
	T* operator->() const throw() { return _ptr; };

private:
	void arrange(const smart_ptr<T>& p)
	{
		_ptr = p._ptr;
		if ((_count=p._count))
			++*_count;
	};
	void dispose()
	{
		if (_count && --*_count <= 0) {
			release();
			// Next line gives valgrind warning on delete/delete[] mismatch
			if (_ptr) {
				delete[] _ptr;
				_ptr = 0;
			}
		}
	};
};

#endif
