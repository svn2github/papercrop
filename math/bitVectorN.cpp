#include "stdafx.h"
#include "mathclass.h"
#include ".\bitvectorn.h"

bitvectorn::bitvectorn()
{
	m_nSize=0;
}

bitvectorn::bitvectorn(const bitvectorn& other)
{
	m_nSize=0;
	this->operator =(other);
}

bitvectorn::bitvectorn(const vectorn& other)
{
	m_nSize=0;
	this->operator =(other);
}

bitvectorn::~bitvectorn()
{
}

void bitvectorn::setSize(int size)
{
	int BAsize=calcBitArrayPos(size)+1;
	if(BAsize>m_aBitArrays.size())
	{
		m_aBitArrays.resize(BAsize);		
	}
	m_nSize=size;
}

void bitvectorn::resize(int size)
{
	int prevSize=m_nSize;
	setSize(size);
	setValue(prevSize,size, false);
}

bitvectorn& bitvectorn::operator=( bitvectorn const& a)
{
    bitvectorn &c = (*this);
    c.setSize( a.size() );

    for( int i=0; i<c.size(); i++ )
		c.setValue(i, a[i]);
    
	return c;
}

bitvectorn& bitvectorn::operator=( vectorn const& a)
{
	bitvectorn &c = (*this);
    c.setSize( a.size() );

    for( int i=0; i<c.size(); i++ )
		c.setValue(i, (!isSimilar(a[i],0.0) ));
    
	return c;
}
bool bitvectorn::operator==(bitvectorn const& other) const
{
	if(size()!=other.size()) return false;
	for(int i=0; i<size(); i++)
		if((*this)[i]!=other[i])
			return false;
	return true;
}

void bitvectorn::findZeroCrossing(const vectorn& vec, zeroCrossingMode mode)
{
	int i;
	setSize(vec.size());
	clearAll();
	switch(mode)
	{
	case ZC_MIN:
		for(i=0; i < size()-1; i++)
			if(vec[i]<0 && vec[i+1]> 0)		
				setAt(i);
		break;
	case ZC_MAX:
		for(i=0; i < size()-1; i++)
			if(vec[i]>0 && vec[i+1]<0)		
				setAt(i);

		for(i=1; i < size()-1; i++)
			if(vec[i-1]>0 && vec[i+1]< 0)
			{
				if(!value(i-1) && !value(i))
				{
					printf("%d\n", i);
					setAt(i);
				}
			}

		break;
	case ZC_ALL:
		for(i=0; i < size()-1; i++)
			if(vec[i]*vec[i+1]<0)		
				setAt(i);
		break;
	}
}

void bitvectorn::findLocalOptimum(const vectorn& vec, zeroCrossingMode mode)
{
	if(vec.size()<=2)
	{
		setSize(vec.size());
		clearAll();
		return;
	}
	vectorn deriv;
	deriv.derivative(vec);
	findZeroCrossing(deriv, mode);
}

int bitvectorn::find(int start, int end, bool bValue) const
{
	int j;
	for(j=start; j<end; j++)
	{
		if((*this)[j]==bValue)
			return j;
	}
	return j;	// return end;
}

int bitvectorn::find(int start, bool bValue) const
{
	int j;
	for(j=start; j<size(); j++)
	{
		if((*this)[j]==bValue)
			return j;
	}
	return j;	// return size();
}

int bitvectorn::findPrev(int start, bool bValue) const
{
	int j;
	for(j=start-1; j>=0; j--)
	{
		if((*this)[j]==bValue)
			return j;
	}
	return j;	// return -1;
}


void bitvectorn::or(const bitvectorn& a, const bitvectorn& b)
{
	bitvectorn &c = (*this);
    assert( a.size()==b.size() );
    c.setSize( a.size() );

    for( int i=0; i<a.size(); i++ )
		c.setValue(i, a[i] || b[i] );
}

void bitvectorn::and(const bitvectorn& a, const bitvectorn& b)
{
	bitvectorn &c = (*this);
    assert( a.size()==b.size() );
    c.setSize( a.size() );

    for( int i=0; i<a.size(); i++ )
        c.setValue(i, a[i] && b[i] );
}

void bitvectorn::setAt(const intvectorn& aIndex)
{
	for(int i=0; i<aIndex.size(); i++)
	{
		setAt(aIndex[i]);
	}
}

void bitvectorn::clearAt(const intvectorn& aIndex)
{
	for(int i=0; i<aIndex.size(); i++)
	{
		clearAt(aIndex[i]);
	}
}

void bitvectorn::makeJumpIndex(intvectorn& jumpIndex) const
{
	jumpIndex.setSize(size());
	int curr=0;
	while(1)
	{
		bool bValue=getValue(curr);
		int next=find(curr+1, !bValue);
		for(int i=curr; i<next; i++)
			jumpIndex[i]=next;
		if(next==size()) break;
		curr=next;
	}
}

bitvectorn& bitvectorn::centers(int start, int end, const intvectorn& jumpIndex, const bitvectorn& bits, bool bLocal)
{
	//! output ũ��� (end-start)*2-1�̴�. true�� ������ center��ġ�� true�� assign�Ѵ�.
	/// input    1 0 1 1 1 0 1 1 0 0
	/// centers  1000001000000100000 
	setSize((end-start)*2-1);
	clearAll();

	int left, right;
	left=start;
	if(!bits[left]) 
		left=jumpIndex[left];
	else
	{
		if(!bLocal)
			left=bits.findPrev(left, false)+1;
	}
	ASSERT(left>=end || bits[left]);

	while(left<end)
	{
		right=jumpIndex[left]-1;
		if(bLocal && right>end-1) 
			right=end-1;
		
		int centerIndex=left+right-2*start;
		if(centerIndex>=0 && centerIndex<size())
			setAt(centerIndex);
		if(right+1>=jumpIndex.size()) break;
		left=jumpIndex[right+1];
	}
	
/*	CString msg("bits");
	bits.output(msg, start, end);
	msg+="centers";
	output(msg);
	msg+="\n";
	TRACE(msg);*/
	return *this;
}

void bitvectorn::output(TString& id, int start, int end) const
{
	if(end>size()) end=size();
	id+="[";
	for(int i=start; i<end; i++)
	{
		id.add("%1d", (*this)[i]);
	}
	id+="]";

}

TString bitvectorn::output(int start, int end) const
{
	if(end>size()) end=size();
	TString id;	
	id+="[";
	for(int i=start; i<end; i++)
	{
		id.add("%1d",(int)(*this)[i]);
	}
	id+="]";
	return id;
}

void bitvectorn::setValue(int start, int end, bool bit)
{
	for(int i=start; i<end; i++)
		setValue(i,bit);
}

bitvectorn& bitvectorn::op(int (*s2_func)(int,int), const intvectorn& source, int value, int start, int end)
{
	if(start<0) start=0;
	if(end>source.size()) end=source.size();

	setSize(end-start);
	clearAll();

	for(int i=start; i<end; i++)
		if(s2_func(source[i], value)) setAt(i-start);

	return *this;
}

bitvectorn& bitvectorn::op(double (*s2_func)(double ,double), const vectorn& source, double value, int start, int end)
{
	if(start<0) start=0;
	if(end>source.size()) end=source.size();

	setSize(end-start);
	clearAll();

	for(int i=start; i<end; i++)
		if(s2_func	(source[i], value)) setAt(i-start);

	return *this;
}
/*
#include "../utility/textfile.h"

void bitvectorn::save(const char* filename)
{
	// text file�� ����.ex) a.txt = 1 3 6 12
	FILE* file;
	file=fopen(filename, "wt");

	if(file)
	{
		for(int i=0; i<size(); i++)
		{
			if(value(i))
				fprintf(file, "%d ", i);
		}
		fclose(file);
	}
	else
	{
		Msg::print("file open error %s", filename);
	}
}


void bitvectorn::load(int numFrame, const char* filename)
{
	setSize(numFrame);
	clearAll();
	CTextFile stopMarking;
	if(stopMarking.OpenReadFile(filename))
	{
		char* token;
		while(token=stopMarking.GetToken())
		{
			int index=atoi(token);
			if(index>size())
			{
				Msg::print("Warning! bitvectorn loading = invalid index at %d\n", index);
				resize(index+1);
			}
			setAt(index);
		}
	}
	else
	{
		Msg::print("file open error %s", filename);
	}
}
*/

int bitvectorn::findNearest(float i, bool bValue) const
{
	int start=int(floor(i));
	if(i-floor(i) > 0.5)
	{
		// +�� ���� ������ �Ѵ�.
		for(int i=0; i<size(); i++)
		{
			int right=start+i+1;
			int left=start-i;
			if(right<size() && value(right)==bValue) return right;
			if(left>=0 && value(left)==bValue) return left;
			if(left<0 || right>=size())
				return -1;
		}
	}
	else
	{
		// -�� ���� ������ �Ѵ�.
		for(int i=0; i<size(); i++)
		{
			int right=start+i+1;
			int left=start-i;
			if(left>=0 && value(left)==bValue) return left;
			if(right<size() && value(right)==bValue) return right;
			if(left<0 || right>=size())
				return -1;
		}
	}
	return -1;
}

float bitvectorn::distanceToNearest(float i, bool bValue) const
{
	int n=findNearest(i,bValue);
	if(n==-1) return FLT_MAX;
	return ABS(i-(float)n);
}