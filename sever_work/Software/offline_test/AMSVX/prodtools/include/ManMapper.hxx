#ifndef _MANMAPPER_H
#define _MANMAPPER_H

#include <vector>
using std::vector;
#include <map>
using std::map;
#include <string>
using std::string;
#include <utility>
using std::pair;
#include <algorithm>

#include "ManCommon.hxx"

//_____________________________________________________________________
// My interface to stl vector

template<typename T> class ivector: public vector<T>
{
#define VECT vector<T>

public:
	ivector<T>(): vector<T>()
	{
	};
	virtual ~ivector()
	{
		rm();
	};

public:
	int len() const { return (int)VECT::size(); };

	int add(const T& value, int pos=-1)
	{
		if (pos < 0 || pos >= len()-1)
		{
			// Add to the end
			VECT::insert(VECT::end(),value);
			return len()-1;
		}
		else
		{
			// Insert after pos
			VECT::insert(VECT::begin()+pos+1,value);
			return pos;
		}
	};

	int ins(const T& value, int pos=-1)
	{
		if (pos < 0 || pos >= len())
		{
			// Add to the end
			VECT::insert(VECT::end(),value);
			return len()-1;
		}
		else
		{
			// Insert before pos
			VECT::insert(VECT::begin()+pos,value);
			return pos;
		}
	};

	T get(int pos) const throw(int)
	{
		if (pos >= 0 && pos < len())
			return (*this)[pos];
		else
			throw(0);
	};

	const T first() throw(int)
	{
		if (len() > 0)
		{
			T value = get(0);
			VECT::erase(VECT::begin());
			return value;
		}
		else
			throw(0);
	};

	void set(const T& value, int pos)
	{
		T old = get(pos);
		VECT::erase(VECT::begin()+pos);
		destruct(old);

		VECT::insert(VECT::begin()+pos,value);
	};

	void rm(int pos)
	{
		if (pos < len())
		{
			T value = get(pos);
			destruct(value);
			VECT::erase(VECT::begin()+pos);
		}
	};

	void rm(int pos, int sz)
	{
		int maxlen = len();

		if (pos < maxlen)
		{
			if (pos+sz > maxlen)
				sz = maxlen - pos;
			if (sz)
			{
				for (int i=0; i<sz; i++)
				{
					T value = get(pos+i);
					destruct(value);
				}
				VECT::erase(VECT::begin()+pos,VECT::begin()+pos+sz);
			}
		}
	};

	void rm()
	{
		rm(0,len());
	};

	void clear(int pos)
	{
		if (pos < len())
			VECT::erase(VECT::begin()+pos);
	};

	void clear(int pos, int sz)
	{
		int maxlen =  len();

		if (pos < maxlen)
		{
			if (pos+sz > maxlen)
				sz = maxlen - pos;
			if (sz)
				VECT::erase(VECT::begin()+pos,VECT::begin()+pos+sz);
		}
	};

	void clear()
	{
		clear(0,len());
	};

	int n_value(const T& value) const
	{
		for (int i=0,k=len(); i<k; i++)
			try
			{
				T current = get(i);
				if (eq(current,value))
					return i;
			}
			catch (...)
			{
				break;
			}
		return -1;
	};

protected:
	virtual void destruct(T &value)
	{
	};

public:
	virtual bool eq(const T& value1, const T& value2) const
	{
		return value1 == value2;
	};

#undef VECT
};

//_____________________________________________________________________
// ivector extension to work with pointers

template<typename T> class pvector: public ivector<T>
{
#define VECT ivector<T>

public:
	pvector<T>(): ivector<T>()
	{
		m_accFull = true;
	};
	pvector<T>(bool access): ivector<T>()
	{
		m_accFull = access;
	};
	virtual ~pvector()
	{
		VECT::rm();
	};

	// ATTRIBUTES
private:
	bool m_accFull;

	// SERVICE METHODS
public:
	void lock() { m_accFull = false; };

	// OVERRIDES

protected:
	virtual void destruct(T &value)
	{
		if (m_accFull && value)
		{
			delete value;
			value = 0;
		}
	};

public:
	virtual bool eq(const T& value1, const T& value2) const
	{
		// In most cases we compare template class pointers,
		// not values.  In the second case we add inline eq
		// implementation, see two methods below
		return VECT::eq(value1,value2);
	};

#undef VECT
}; 

template<> inline bool pvector<char *>::eq(char *const &value1,
										   char *const &value2) const
{
	return (!strcmp(value1,value2));
};

template<> inline bool pvector<CharString *>::eq(CharString *const &value1,
												 CharString *const &value2) const
{
	return *value1 == *value2;
};

//_____________________________________________________________________
// Very similar to ivector, but also gives some features like map
// containers. These features are slow but it is enough for most cases

template <typename T> struct str_eq
{
private:
	const char *m_key;
public:
	str_eq(const char *key) { m_key = key; };

	bool operator () (const typename vector<pair<char*, T> >::value_type &it) const
	{
		return strcmp(it.first,m_key) == 0;
	};
};

template<typename T> class imapper: public vector<pair<char*, T> >
{
#define VECT vector<pair<char*, T> >

public:
	imapper<T>(): VECT()
	{
	};
	virtual ~imapper()
	{
		rm();
	};

public:
	int len() const { return (int)VECT::size(); };

	int add(const char *key, const T& value, int pos=-1)
	{
		if (key == 0) key  = (char *)&"\0";

		pair<char*, T> p(common::strdup(key),value);
		if (pos < 0 || pos >= len()-1)
		{
			// Add to the end
			insert(VECT::end(),p);
			return len()-1;
		}
		else
		{
			// Insert after pos
			insert(VECT::begin()+pos+1,p);
			return pos;
		}
	};

	int ins(const char *key, const T& value, int pos=-1)
	{
		if (key == 0) key  = (char *)&"\0";

		pair<char*, T> p(common::strdup(key),value);
		if (pos < 0 || pos >= len())
		{
			// Add to the end
			insert(VECT::end(),p);
			return len()-1;
		}
		else
		{
			// Insert before pos
			insert(VECT::begin()+pos,p);
			return pos;
		}
	};

	const char *key(int pos) const throw(int)
	{
		if (pos >= 0 && pos < len())
			return (*this)[pos].first;
		else
			throw(0);
	};

	const T get(int pos) const throw(int)
	{
		if (pos >= 0 && pos < len())
			return (*this)[pos].second;
		else
			throw(0);
	};

	const T get(const char *key) const throw(int)
	{
		int k = n_key(key);

		if (k >= 0) return get(k);
		  else		throw(0);
	};

	void set(const T& value, int pos)
	{
		T old = (*this)[pos].second;
		destruct(old);
		(*this)[pos].second = value;
	};

	void rm(int pos)
	{
		if (pos >= len()) return;

		char *key  = (*this)[pos].first;
		T    value = (*this)[pos].second;

		if (key)
			delete[] key;
		destruct(value);
		VECT::erase(VECT::begin()+pos);
	};

	void rm()
	{
		int sz = len();

		for (int i=0; i<sz; i++)
		{
			char *key  = (*this)[i].first;
			T    value = (*this)[i].second;

			if (key)
				delete[] key;
			destruct(value);
		}
		VECT::clear();
	};

	void rm(const char *key)
	{
		int k = n_key(key);

		if (k >= 0) rm(k);
	};

	int n_key(const char *key) const
	{
		if (key == 0) key  = (char *)&"\0";

		typename VECT::const_iterator it = find_if(VECT::begin(),VECT::end(),str_eq<T>(key));

		if (it != VECT::end())
			return static_cast<int> (distance(VECT::begin(),it));
		return -1;
	};

protected:
	virtual void destruct(T &value)
	{
	};
#undef VECT
};

//_____________________________________________________________________
// imapper extension to work with pointers

template<typename T> class pmapper: public imapper<T>
{
#define VECT imapper<T>

public:
	pmapper<T>(): VECT()
	{
		m_accFull = true;
	};
	pmapper<T>(bool access): VECT()
	{
		m_accFull = access;
	};
	virtual ~pmapper()
	{
		VECT::rm();
	};

private:
	bool m_accFull;

public:
	void lock() { m_accFull = false; };

protected:
	virtual void destruct(T &value)
	{
		if (m_accFull && value)
		{
			delete value;
			value = 0;
		}
	};
#undef VECT
};


//_____________________________________________________________________
// One more CharString extension for advanced string parsing

class CharStringToken: public CharString
{
public:
	CharStringToken(): CharString()
	{
	};
	CharStringToken(const char *str): CharString(str)
	{
	};
	CharStringToken(const string &str): CharString(str)
	{
	};
	CharStringToken(const CharString &str): CharString(str)
	{
	};
	CharStringToken(const char *str, const char *pattern): CharString(str)
	{
		strtok((char *)*this,pattern);
	};
	CharStringToken(const string &str, const char *pattern): CharString(str)
	{
		strtok(this->c_str(),pattern);
	};
	CharStringToken(const CharString &str, const char *pattern): CharString(str)
	{
		strtok(this->c_str(),pattern);
	};
	virtual ~CharStringToken()
	{
	};

private:
	pvector<char *> m_Token;

private:
	void token4(char *p)
	{
		char *p2 = common::strdup(p);
		common::trim(p2);
		m_Token.add(p2);
	};

public:
	void strtok(const char *str, const char *pattern)
	{
		if (strlen(str) < 1)
			return;
		m_Token.rm();

		bool reflect = false;
		unsigned last = 0;
		char *p = new char[strlen(str)+1];
		p[last] ='\0';

		for (unsigned i=0; i<strlen(str); i++)
			if (str[i] == '\\' && strrchr(pattern,str[i+1]))
				reflect = true;
			else
			{
				if (strrchr(pattern,str[i]) && !reflect)
				{
					p[last] = '\0';
					token4(p);
					last = 0;
					p[last] = '\0';
				}
				else
					p[last++] = str[i];
				reflect = false;
			}

		p[last] = '\0';
		if (strlen(p) > 0) token4(p);

		delete[] p;
	};
	void strtok2(const char *str, const char *pattern)
	{
		if (strlen(str) < 1)
			return;
		m_Token.rm();

		unsigned last = 0;
		char *p = new char[strlen(str)+1];
		p[last] ='\0';

		for (unsigned i=0; i<strlen(str); i++) {
			string s = &str[i];
			if (s.find(pattern) == 0) {
				p[last] = '\0';
				token4(p);
				last = 0;
				p[last] = '\0';
				i += strlen(pattern)-1;
			}
			else
				p[last++] = str[i];
		}
		p[last] = '\0';
		if (strlen(p) > 0) token4(p);

		delete[] p;
	};
	void strtok(const char *pattern)
	{
		strtok((char *)*this,pattern);
	};
	void strtok2(const char *pattern)
	{
		strtok2((char *)*this,pattern);
	};
	int tokens() const
	{
		return m_Token.len();
	};
	const char *token(int pos) const
	{
		try
		{
			return m_Token.get(pos);
		}
		catch (int)
		{
			return 0;
		}
	};
	int ftoken(const char *pattern, int pos=0)
	{
		for (int i=pos; i<tokens(); i++)
			if (strstr(token(i),pattern))
				return i;
		return -1;
	};
	int rftoken(const char *pattern, int pos=-1)
	{
		if (pos<0 || pos>=tokens())
			pos = tokens()-1;
		for (int i=pos; i>=0; i--)
			if (strstr(token(i),pattern))
				return i;
		return -1;
	};
	const pvector<char *>& reftok() const
	{
		return m_Token;
	};
	string tostr(const char *pattern, int pos=-1) const
	{
		string result;
		if (pos<0) pos = 0;
		  else if (pos>=tokens()) pos = tokens()-1;
		for (int i=pos; i<tokens(); i++) {
			result += token(i);
			if (i<tokens()-1) result += pattern;
		}
		return result;
	}
};

#endif
