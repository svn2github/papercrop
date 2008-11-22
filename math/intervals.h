#pragma once

class intIntervals
{
public:
	intIntervals(){}
	~intIntervals(){}
	int numInterval() const			{ return m_vStart.size();}
	int size() const				{ return numInterval();}

	void setSize(int n)				{ m_vStart.setSize(n); m_vEnd.setSize(n);}
	void resize(int n)				{ m_vStart.resize(n); m_vEnd.resize(n);}
	void removeInterval(int i);
	int& start(int iInterval) const	{ return m_vStart[iInterval];}
	int& end(int iInterval) const	{ return m_vEnd[iInterval];}
	void load(const char* filename);
	void pushBack(int start, int end) 	{m_vStart.pushBack(start);m_vEnd.pushBack(end);}

	// [start, end) �� ��ġ�� interval�� index�� return. ������ -1
	int findOverlap(int start, int end, int startInterval=0);

	/**
	* RunLength encoding�� ���Ѵ�.
	* [1 1 1 0 0 0 0 1 0] -> [0 3], [7 8]
	* 0 ���� 3���� 1�� ������, 7���� 8���� ������ 1�� ���´ٴ� ��.	 
	*/
	void runLengthEncode(const bitvectorn& source, int start=0, int end=INT_MAX);

	/**
	* RunLength encoding�� ���Ѵ�.
	* [1 1 1 3 3 3 3 4 4] -> [0 3], [3 7], [7 9]  
	* 0 ���� 3���� 1�� ������, 3���� 7���� 3�� ������, 4�� 9���� ���´ٴ� ��.
	*/
	void runLengthEncode(const intvectorn& source);
	/**
	* CutState�� RunLength encoding�� ���Ѵ�.
	* depricated. -> moved into intIntervals class.
	* [1 0 0 1 0 0 0 1 0] -> [0 3, 3 7, 7 9]
	* 0 ���� 3���� �� ù��° ����, 3~7�� �ι�° ����, 7~9�� ����° ����.*/
	void runLengthEncodeCut(const bitvectorn& cutState, int start=0, int end=INT_MAX);

	void encodeIntoVector(intvectorn& out);
	void decodeFromVector(const intvectorn& in);
	void offset(int offset);

	void toBitvectorn(bitvectorn& bitVector);

	intvectorn m_vStart;
	intvectorn m_vEnd;
};

