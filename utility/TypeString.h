//
// TypeString.h
//
// Copyright 2004 by Taesoo Kwon.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Library General Public License for more details.
//
// You should have received a copy of the GNU Library General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA.
//

#pragma once

class TString;

namespace sz0
{
	struct Operator
	{
		Operator(){}
		virtual ~Operator(){}
		virtual void calc(TString& c) const;
	};
}

/// Null terminated string. Do not make virtual function.
class TString  
{
public:
	TString();
	TString(const char* str);
	TString(const char* str, int i);
	// if(str=="aaa" && i=0) -> this="aaa0"
	TString(const TString& str);
	TString(const sz0::Operator& op);
	~TString();

	const char* ptr(int n=0) const			{ if(n>=length()) return NULL; return m_psData+n; }
	inline operator const char*()	const	{ return ptr();}
	int length() const						{ return m_nDataLen; }
	void replace(char a, char b);
	void replace(const char* a, const char* b);

	/// Almost private functions. ���� access. Do not forget to updateLength!!!
	char* ptr_ref(int n=0)					{ return m_psData+n; }
	void allocate(int AllocLen);
	void alloc(const char* strData, int nLen = -1);
	void alloc(const TString& Src, int start = 0, int end = INT_MAX);	
	void _format(const char* pszFormat, va_list& argList);

	void updateLength()						{ m_nDataLen=(int)strlen(m_psData); }

	//find
	//start���� nLen�� ���Ͽ� ã�´�.(0 - indexing)
	int findChar(int start, char one) const;    
	int findCharRight(char one, int start=INT_MAX) const;
	// ��ã���� -1 return 
	int findStr(int start, const char* src, int Len=-1) const;
	int find(const char* src) const;
	
	void makeUpper();
	TString toUpper();
	// i��° character�� src�� ������ �ϳ�����?
	bool isOneOf(int i, const TString& src) const;

	// loop�� ���鼭 start�� end�� �Ǹ� �����.
	void token(int &start, const TString& delimiter, TString& token) const;

	//delimiter�� ���еǴ� index��° ��ū (1 - indexing) ex) "abc def".token(" ,",1)=="abc"
	TString token(char delimiter, int index = 1) const;
	TString token(const char* delimiter, int index = 1) const;

	//substring : [start, end] ������ string
	TString subString( int start = 0, int end = INT_MAX) const;

	/// n<0 �ΰ�� �ݴ��ʺ��� ī��Ʈ.
	TString left(int n) const				{ if(n<0) return subString(0, length()+n); return subString(0, n);}
	/// n<0 �ΰ�� �ݴ��ʺ��� ī��Ʈ.
	TString right(int n) const				{ if(n<0) return TString(ptr(-1*n)); int start=length()-n; return subString(start);}

	void trimLeft(const char* delimiter);
	void trimRight(const char* delimiter);

	const char* right2(int n) const			{ int start=length()-n; return ptr(start);}

	//compare
	int strncmp(const TString &str, int n=-1) const;              
	int strcmp(const TString &str) const;
	int strcmp(const char* str) const;

	int isEmpty() const;
	void empty();

	void op0(const sz0::Operator& op)	{op.calc(*this);}

	//sprintf
	TString& add(const char* pszFormat, ...);
	TString& format(const char* pszFormat, ...);
	
	//OPerater "="
	const TString& operator =( const TString& Str );
	const TString& operator =( const char* strStr);
	//OPerator "+="
	const TString& operator +=( const TString& Str );
	const TString& operator +=( const char* mbstrStr );
	//OPerator "[]"
	char& operator []( int index ) const	{	return m_psData[index];	}
	char& value(int index) const			{	return m_psData[index];	}
	
	//Concat 
	void concat(const TString &str, int n = 0);   
	void concat(const char*  str, int n =0);

	//operator "+"
	TString operator +(const TString &Str2) const;
	TString operator +(const char* Str2) const;
	friend TString operator +(const char* Str1, const TString &Str2);

	//operator "=="
	bool operator ==(const TString &Str2) const;
	bool operator ==(const char* Str2) const;
	friend bool  operator ==(const char* Str1, const TString &Str2);
	
	//operator "!="
	bool operator !=(const TString &Str2) const;
	bool operator !=(const char* Str2) const;
	friend bool operator !=(const char* Str1, const TString &Str2);

private:

	void initData();
	void freeData();

	char* m_psData;

	//data,alloc Len
	int m_nDataLen;
	int m_nAllocLen;



};


#include <vector>
class TStrings 
{
	std::vector<TString> mStrings;
public:
	TStrings(){}
	virtual~TStrings(){}

	TString& operator[](int i)				{ return mStrings[i];}
	TString& data(int i)					{ return mStrings[i];}
	TString& back()							{ return data(size()-1);}
	
	TString const& operator[](int i)const	{ return mStrings[i];}
	TString const& data(int i)const			{ return mStrings[i];}
	TString const& back() const				{ return data(size()-1);}
	void init(int n);
	// eg) setStrings(3, "hihi", "hehe", "hohoho");
	void setStrings( int n, const char* s1, ... );

	int size() const						{ return (int)mStrings.size();}
	void resize(int n)						{ mStrings.resize(n);}
	void pushBack(const TString& other)		{ mStrings.push_back(other);}

	TString prefix() const;	// ��� ���ڿ��� ���� prefix�� �����ϴ� ��� �ش� prefix return.
    void trimSamePrefix(const TStrings& other);	// ��� ���ڿ��� ���� prefix�� �����ϴ� ��� �߶��ش�.
	// ��ã���� size() return;
	int find(const char* other) const;

};

#pragma warning (disable: 4786)
#include <map>
#include <string>
#include <functional>

#pragma warning (disable: 4800)

struct cmpTString: std::binary_function<TString const &, TString const &, bool>
{
	bool operator()(TString const & _X, TString const & _Y) const
	{
		return _X.strcmp(_Y)<0;
	}
};
