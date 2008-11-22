#pragma once
 
//! BitArray�� Fast Version
/*! �ٸ� ������ BitArray�� BitProxy class�� ����Ͽ� a[10]=0 ó�� lvalue�� ���Ǵ� ���� ����������,
�� BitArray�� lvalue�� ���Ǵ� ��� Assign �Ǵ� SetAt, ClearAt�� ����Ͽ��� �Ѵ�. 
���� 32���� ū ũ���� array�� ����� �� ����.  
�� ��� ���� bitwise operation�� �����ϴ� �Ϳ� ���� ���� performance overhead�� ����.
*/
class BitArray  
{
public:
	BitArray(const BitArray& other) { m_Bits=other.m_Bits; };
	BitArray() { m_Bits=0; };
	~BitArray()	{};
	//! b=a[10] ó�� ����� �� �ִ�.
	inline bool operator[](int nIndex) const				{ return (m_Bits>>nIndex &1);}
	//! SetAt(1)�� a[1]=true�� �ش� 
	inline void SetAt(int nIndex)							{ m_Bits|=1<<nIndex; }
	inline void SetAt(int nIndex,int value)			{ if(value)m_Bits|=1<<nIndex;else m_Bits&=~(1<<nIndex); }
	//! ClearAt(1)�� a[1]=false�� �ش� 
	inline void ClearAt(int nIndex)							{ m_Bits&=~(1<<nIndex); }

	inline void Assign(int nIndex, bool bit)				{ if(bit) SetAt(nIndex); else ClearAt(nIndex);}

	inline void ClearAll()									{ m_Bits=0;}
	int GetCount() const				{	int count=0; for(int i=0; i<32; i++) if(m_Bits>>i&1)	count++; return count; }

	// bitwise or
	friend BitArray operator|(const BitArray& a, const BitArray& b)	{ BitArray c; c.m_Bits=a.m_Bits|b.m_Bits; return c;}
	// bitwise and
	friend BitArray operator&(const BitArray& a, const BitArray& b)	{ BitArray c; c.m_Bits=a.m_Bits&b.m_Bits; return c;}

	BitArray& operator|=( const BitArray & other)					{ m_Bits|=other.m_Bits; return *this;}
	BitArray& operator&=( const BitArray & other)					{ m_Bits&=other.m_Bits; return *this;}
	void operator=(const BitArray& other)							{ m_Bits=other.m_Bits;}
	unsigned int m_Bits;
};

class vectorn;
class intvectorn;
/**
 * \ingroup group_math
 *
 * bit�� vector
 *
 */
class bitvectorn
{
public:
    bitvectorn(void);
	bitvectorn(const bitvectorn& other);
	bitvectorn(const vectorn& other);
	~bitvectorn(void);

	void setSize(int size);
	void resize(int size);

	//! b=a[10] ó�� ����� �� �ִ�.
	inline bool operator[](int nIndex) const				{ ASSERT(nIndex<m_nSize); return m_aBitArrays[calcBitArrayPos(nIndex)][calcBitArrayIndex(nIndex)];};
	inline bool operator()(int nIndex) const				{ return (*this)[nIndex];}

	bool getValue(int nIndex) const							{ return this->operator [](nIndex);};
	bool value(int nIndex) const							{ return this->operator [](nIndex);};

	//! SetAt(1)�� a[1]=true�� �ش� 
	void setAt(int nIndex)									{ ASSERT(nIndex<m_nSize); m_aBitArrays[calcBitArrayPos(nIndex)].SetAt(calcBitArrayIndex(nIndex));};
	void setAt(const intvectorn& aIndex);
	
	//! ClearAt(1)�� a[1]=false�� �ش� 
	void clearAt(int nIndex)								{ ASSERT(nIndex<m_nSize); m_aBitArrays[calcBitArrayPos(nIndex)].ClearAt(calcBitArrayIndex(nIndex));};
	void clearAt(const intvectorn& aIndex);
    
	inline void setValue(int nIndex, bool bit)				{ if(bit) setAt(nIndex); else clearAt(nIndex);};
	inline void setAllValue(bool bit)						{ if(bit) setAll(); else clearAll();}
	void setValue(int start, int end, bool bit);
	inline void setAll()									{ setValue(0, size(), true); }
	inline void clearAll()									{ int index=calcBitArrayPos(m_nSize)+1; for(int i=0; i<index; i++) m_aBitArrays[i].ClearAll();};
	int count()	const										{ if(m_nSize==0) return 0; int index=calcBitArrayPos(m_nSize)+1; int count=0; for(int i=0; i<index; i++) count+=m_aBitArrays[i].GetCount(); return count;};
	int size() const										{ return m_nSize;};
	bitvectorn& operator=( bitvectorn const& );
	bitvectorn& operator=( vectorn const& );

	bitvectorn& op(int (*s2_func)(int,int), const intvectorn& source, int value, int start=0, int end=INT_MAX);
	bitvectorn& op(double (*s2_func)(double,double), const vectorn& source, double value, int start=0, int end=INT_MAX);

	// primary functions
	// binary OP
	void or(const bitvectorn& a, const bitvectorn& b);
	void and(const bitvectorn& a, const bitvectorn& b);
	// unary OP
	bitvectorn& operator|=(bitvectorn const& a) { (*this).or((*this),a); return *this;};
	bitvectorn& operator&=(bitvectorn const& a) { (*this).and((*this),a); return *this;};
	bool operator==(bitvectorn const& other) const;
	bool operator!=(bitvectorn const& other) const	{ return !operator==(other);};
	// utility functions
	enum zeroCrossingMode { ZC_MIN, ZC_MAX, ZC_ALL};	
	//! zero-crossing�� ã�´�.	ZC_MIN: negative to positive crossing, ZC_MAX: positive to negative crossing
	void findZeroCrossing(const vectorn& signal, zeroCrossingMode mode=ZC_ALL);
	//! local optimum�� ã�´�. ZC_MIN: local minimum, ZC_MAX: local maximum
	void findLocalOptimum(const vectorn& signal, zeroCrossingMode mode=ZC_MIN);
	//! ������ windowed global optimum�̴�. �� candidate�� localOptimum�̴�. �ҽ��ڵ� ����.
	void refineLocalOptimum(const vectorn& signal, const bitvectorn& localOptimum, int windowSize, zeroCrossingMode mode=ZC_MIN);
	//! true ������� clustering�Ѵ�. �� ������ �ִ� �͵鳢�� �ϳ��� ���� center�� ���Ѵ�. (kmean���)
	void cluster(int windowSize);
	void makeJumpIndex(intvectorn& jumpIndex) const;
	//! output ũ��� (end-start)*2-1�̴�. true�� ������ center��ġ�� true�� assign�Ѵ�.
	/// input    1 0 1 1 1 0 1 1 0 0
	/// centers  1000001000000100000 
	bitvectorn& centers(int start, int end, const intvectorn& jumpIndex, const bitvectorn& bits, bool bLocal=true);

	/// if there is no bValue return end;
	int find(int start, int end, bool bValue=true) const;
	int find(int start, bool bValue=true) const;
	int findPrev(int start, bool bValue=true) const;
	
	// find bValue which is nearest to i. if fail, return -1;
    int findNearest(float i, bool bValue=true) const;
	float distanceToNearest(float i, bool bValue=true) const;

	// deprecated.
	void output(TString& id, int start=0, int end=INT_MAX) const;
	TString output(int start=0, int end=INT_MAX) const;

	void save(const char* filename);	// text file�� ����.ex) a.txt = 1 3 6 12
	void load(int size, const char* filename);	// text file���� �б�.ex) a.txt = 1 3 6 12
private:
	inline int calcBitArrayPos(int nIndex) const			{ return nIndex/32;};
	inline int calcBitArrayIndex(int nIndex) const			{ return nIndex%32;};
	int m_nSize;
	std::vector<BitArray> m_aBitArrays;
	friend class BinaryFile;
};


