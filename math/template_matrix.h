//
// template_matrix.H 
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

#ifdef _DEBUG
//#define _DEBUG_INFO	-> ���� ����.
#endif

#include "template_math.h"
template <class T>
class _tmat
{
private:

#ifdef _DEBUG_INFO
	// For convenient use of debugger.
	std::vector<T> _arrayDEBUG;	// valid only if owner==true in debug mode. 
#endif

	int      n,	m,
			stride,	// stride between each row.
			on;	// size of memory allocated( which is size of buffer). Not the number of row. (by taesoo)
	T* buffer;

	friend class _tvectorn<T, T, 1>;
protected:

	// disable default copy constructor
	_tmat(const _tmat<T>& other) { ASSERT(0);}
	_tmat();
	_tmat(T* _ptr, int _n, int _m, int _stride) { buffer=_ptr; n=_n; m=_m; stride=_stride; on=-1;	} // reference
	
	// reference�� �޾ƿ´�. 
	void _assignRef( _tmat<T> const&);

	template <class matViewType>
	matViewType _range(int startRow, int endRow, int startColumn, int endColumn)
	{
		if(endColumn>cols()) endColumn=cols();
		return matViewType(&value(startRow, startColumn), endRow-startRow, endColumn-startColumn, stride);
	}

	template <class matViewType>
	const matViewType _range(int startRow, int endRow, int startColumn, int endColumn) const
	{
		if(endColumn>cols()) endColumn=cols();
		return matViewType(&value(startRow, startColumn), endRow-startRow, endColumn-startColumn, stride);
	}

	// L-value�� ���ɼ� �ִ� reference vector �� return�Ѵ�.

	template <class vecViewType>
	vecViewType _row(int i)const			{ assert(i>=0 && n>i); return vecViewType (&value(i,0), cols(), 1); }
	template <class vecViewType>
	vecViewType _column(int i)const			{ assert(i>=0 && m>i); return vecViewType (&buffer[i], rows(), stride); }


public:
	virtual ~_tmat();

	void load(const char* filename);
	void save(const char* filename) const;

	bool	isReference() const				{ return on==-1;}
	// inquiry functions
	T    getValue( int, int ) const;

	int	rows()  const			{ return n; }
	int	nrows()  const			{ return n; }
	int	cols() const			{ return m; }
	int	ncols() const			{ return m; }

	void	setDiagonal(T value);
	void	setDiagonal(const _tvectorn<T>& );

	// set value
	void    setValue( int, int, T );
	void	setAllValue(T);
	void	setAllValue(int nrow, int ncol, T value)			{ setSize(nrow, ncol); setAllValue(value);}


	// end���ϴ� end-start��ŭ ���� �ö󰣴�. �� matrixũ�Ⱑ end-start��ŭ ���η� �۾�����.
	void deleteRows(int start, int end);	
	// end���ϴ� end-start��ŭ �������� �������. �� matrixũ�Ⱑ end-start��ŭ ���η� �۾�����.
	void deleteCols(int start, int end);
	// nrow���ϴ� nbubble��ŭ �Ʒ��� ������. �� matrixũ�Ⱑ nbubble��ŭ ���η� Ŀ����, ��ĭ�� �����.	
	void bubbles(int nrow, int nbubbles);
	// ncolumn������ nbubble��ŭ �������� �δ�. �� matrixũ�Ⱑ nbubble��ŭ ���η� Ŀ����, ��ĭ�� �����.
	void bubbleColumns(int ncol, int nbubbles);

	void	  pushFront(const _tvectorn<T>& rowVec);	//!< slow operation, hypermatrix�� ���Ҹ� pushFront�ϸ� ������ ����� ����
	void	  popFront();
	void	  popFront(_tvectorn<T>& rowVec);		//!< slow operation, hypermatrix�� ���Ҹ� popFront�ϸ� ������ ����� ����
	
	void	  pushBack(const _tvectorn<T>& rowVec);
	void	  popBack();
	void	  popBack(_tvectorn<T>& rowVec);

	void extractRows(_tmat<T> const& mat, _tvectorn<int> const& rows);
	void extractColumns(_tmat<T> const& mat, _tvectorn<int> const& columns);

#ifdef _DEBUG
	template <class T> class RangeCheck
	{
	public:
		RangeCheck(T* p, int col):mPtr(p), mCol(col){}
		T& operator[](int i) const	{ ASSERT(i>=0 && i<mCol); return *(mPtr+i);}		
		inline operator T*() const	{ return mPtr;}
		T* mPtr;
		int mCol;
	};
	RangeCheck<T> operator[](int i) const		{ ASSERT(i<rows() ); return RangeCheck<T>((T*)(buffer+i*stride), cols());}
#else
	T*  operator[](int i) const					{ ASSERT(i<rows() ); return (T*)(buffer+i*stride);}	
#endif
	inline T&  value(int i, int j) const		{ ASSERT(i<rows() && j<cols()); return *((buffer+i*stride)+j);}
	inline T&	 operator()(int i, int j) const			{ return value(i,j);}


	void	setSize( int, int );  // ���� ����Ÿ ���� ���� ���� ����.
	void    setSameSize( const _tmat<T>& other)	{ setSize(other.rows(), other.cols());};  // ���� ����Ÿ ���� ���� ���� ����.
	void    resize( int, int );	// ���ڸ��� 0���� ä�� , ���� ����Ÿ ����.
	void _getPrivate(T*& buffer2, int & stride2, int& n2, int& m2, int& on2) const;

	// reference�� value�� ī�ǵȴ�. reference�� �޾ƿ��� ������, assignRef�� ����� ��.
	_tmat<T>&  assign( _tmat<T> const&);


	// unary operations
	void transpose( _tmat<T> const& );
    void operator+=( _tmat<T> const& );
    void operator-=( _tmat<T> const& );
	void operator*=( T );
	void operator/=( T );
	void concatRow( _tmat<T> const&);

	// binary operations
	void mult( _tmat<T> const&, _tmat<T> const& );
	void multABt(_tmat<T> const& a, _tmat<T> const& b); //!< a*b^T
	void multAtB(_tmat<T> const& a, _tmat<T> const& b); //!< a^T*b
	void multAtBt(_tmat<T> const& a, _tmat<T> const& b); //!< a^T*b^T
	void mult( _tmat<T> const& a, T b );
	void add( _tmat<T> const&, _tmat<T> const& );
	void subtract( _tmat<T> const&, _tmat<T> const& );
	void concatRow( _tmat<T> const&, _tmat<T> const&);
	void concatColumn( _tmat<T> const&, _tmat<T> const&);
	

	// �Ʒ��� ��� deprecated
	// row(i), col(i), range(rowstart, rowend, columnstart, columnend) ���� ����� ��.
	void      getRow( int, _tvectorn<T>& ) const;
	void	  getColumn( int, _tvectorn<T>& ) const;

	void	  setRow(int i, T value);
	void      setRow( int i, const _tvectorn<T>& );
	void	  setColumn(int j, T value);
	void	  setColumn( int j, const _tvectorn<T>& );

	void	  setValue(int rowstart, int rowend, int columnstart, int columnend, T);
	void	  setValue(int row, int column, _tmat<T> const& mat);	// this�� row,column���� row+mat.rows(), column+mat.cols()���� ä��

};


template <class T> 
_tmat<T>::_tmat()
{
	on = n = m= stride=0;
}

template <class T> 
_tmat<T>::~_tmat()
{
	//if( on>0) delete[] buffer;
#ifndef _DEBUG_INFO
	if( on>0) free(buffer);	
#endif
}

template <class T> 
T _tmat<T>::getValue( int row, int column ) const
{
	const _tmat<T>& v=*this;
	return v[row][column];
}

template <class T>
void _tmat<T>::setValue(int rowstart, int rowend, int columnstart, int columnend, T d)
{
	_tmat<T>& v=*this;
	for(int i=rowstart; i<rowend; i++)
	{
		T* v_i=v[i];
		for(int j=columnstart; j<columnend; j++)
			v_i[j]=d;
	}
}

template <class T>
void _tmat<T>::setAllValue(T value)
{
	_tmat<T>& v=*this;
	for(int i=0; i<rows(); i++)
	{
		T* v_i=v[i];
		for(int j=0; j<cols(); j++)
			v_i[j]=value;
	}
}



template <class T>
void _tmat<T>::setValue( int row, int column, T value )
{
	_tmat<T>& v=*this;
	v[row][column]=value;
}

template <class T>
void _tmat<T>::setSize( int nRow, int nColumn )
{
	if(n==nRow && m==nColumn) return;

	if(isReference())
		Msg::error("setSize called, but not an owner");

	if(on<nRow*nColumn)	// buffer�� ���ڶ����� �ٽ� ����
	{
		int capacity;
		if(on)	
		{
#ifndef _DEBUG_INFO
			free(buffer);
#endif
			capacity=on;
			// capacity�� nsize�� �����Ҷ����� doubling
			while(capacity<nRow*nColumn)	capacity*=2;
		}
		else
			capacity=nRow*nColumn;

		// �ѹ��� ũ�� �Ҵ��Ѵ�. ������ ��� ���͸��� loop�� ���鼭 �Ҵ��ϴ� ����� �뷫 �����ʴ�.
		// ������ new operator�� ����� �ð��� �����ɸ��� �����̱� �����̴�. (delete�� ��������)
		//buffer=new T [nRow*nColumn];	
#ifdef _DEBUG_INFO
		_arrayDEBUG.reserve(capacity);
		_arrayDEBUG.resize(nRow*nColumn);
		buffer=&_arrayDEBUG[0];
#else
		buffer=(T*)malloc(sizeof(T)*capacity);
		Msg::verify(buffer!=NULL, "malloc (size %d) failed", sizeof(T)*capacity );
#endif

		on=capacity;
	}
#ifdef _DEBUG_INFO
	_arrayDEBUG.resize(nRow*nColumn);
#endif

	n = nRow;
	m = nColumn;
	stride=nColumn;
}


template <class T>
void _tmat<T>::resize(int nRow, int nColumn)
{
	if(n==nRow && m==nColumn) return;

	if(isReference())
		Msg::error("setSize called, but not an owner");

	if(nColumn==cols())
	{
		if(nRow*stride<on)
		{
			int prev_row=rows();
			n = nRow;
			setValue(prev_row, nRow, 0, cols(), 0);	
			return;
		}
		else if(stride==cols())
		{
			// need to reallocate memory
			// buffer�� �˳��ϰ� ��´�. (���� ���Ҵ��� ���´�.)
			int capacity=MAX(on,50);
			// capacity�� nsize�� �����Ҷ����� doubling
			while(capacity<nRow*nColumn)	capacity*=2;

#ifdef _DEBUG_INFO
			_arrayDEBUG.reserve(capacity);
			_arrayDEBUG.resize(nRow*nColumn);
			buffer=&_arrayDEBUG[0];
#else
			// realloc is possible
			if(on)	
				buffer=(T*)realloc(buffer, sizeof(T)*capacity);
			else
				buffer=(T*)malloc(sizeof(T)*capacity);

			Msg::verify(buffer!=NULL, "malloc (size %d) failed", sizeof(T)*capacity );
#endif

			int prevRow=n;
			n=nRow;
			on=capacity;

			for(int i=prevRow; i<rows(); i++)
			{
				T* v_i=(*this)[i];
				for(int j=0; j<cols(); j++)
					v_i[j]=0;
			}

			return;
		}
	}
	else if(nRow*stride<on)
	{
		if(nColumn<=stride)
		{
			int prev_row=rows();
			int prev_col=cols();
			n = nRow;
			m = nColumn;
			setValue(prev_row, nRow, 0, cols(), 0);
			setValue(0, rows(), prev_col, nColumn, 0);
			return;
		}
		else if(nRow*nColumn<on)
		{
			ASSERT(nColumn>stride);
			// enough capacity
			_tmat<T> backup;
			backup._assignRef(*this);	// get reference (which is valid only when the data is not overwritten)

			// backward copy is safe (����Ÿ�� �׻� ���������� �и��ϱ� �ڿ������� ī���Ѵ�.).
			int minRow=MIN(nRow, backup.rows());
			int minColumn=MIN(nColumn, backup.cols());
			n = nRow;
			m = nColumn;
			stride=nColumn;
			for(int i=minRow-1; i>=0; i--)
			{
				T* v_i=(*this)[i];
				T* backup_i=backup[i];
				for(int j=minColumn-1; j>=0; j--)
					v_i[j]=backup_i[j];
			}

			setValue(minRow, nRow, 0, cols(), 0);
			setValue(0, rows(), minColumn, cols(), 0);
			return;
		}
	}
	
	// default: backup, malloc and copy!

	// buffer�� �˳��ϰ� ��´�. (���� ���Ҵ��� ���´�.)
	int capacity=MAX(on,50);
	// capacity�� nsize�� �����Ҷ����� doubling
	while(capacity<nRow*nColumn)	capacity*=2;

	// data copy is needed since shape is modified.
#ifdef _DEBUG_INFO
	_tmat<T> backup;
	backup.assign(*this);
	_arrayDEBUG.reserve(capacity);
	_arrayDEBUG.resize(nRow*nColumn);
	buffer=&_arrayDEBUG[0];
	on=capacity;
	setSize(nRow, nColumn);
#else
	_tmat<T> backup;

	T* bufferBackup=(on)? buffer : NULL;

	backup._assignRef(*this);	// get reference (which is still valid until bufferBackup is freed.)
	
	// �ѹ��� ũ�� �Ҵ��Ѵ�. ������ ��� ���͸��� loop�� ���鼭 �Ҵ��ϴ� ����� �뷫 �����ʴ�.
	// ������ new operator�� ����� �ð��� �����ɸ��� �����̱� �����̴�. (delete�� ��������)
	//buffer=new T[capacity];	

	buffer=(T*)malloc(sizeof(T)*capacity);
	Msg::verify(buffer!=NULL, "malloc (size %d) failed", sizeof(T)*capacity );
	on=capacity;

	setSize(nRow, nColumn);
#endif

	int backupRow=MIN(backup.rows(), nRow);
	int backupColumn=MIN(backup.cols(), nColumn);

	this->setAllValue(0);
	for(int i=0; i<backupRow; i++)
	{
		T* v_i=(*this)[i];
		for(int j=0; j<backupColumn; j++)
			v_i[j]=backup[i][j];
	}

#ifndef _DEBUG_INFO
	if(bufferBackup) free(bufferBackup);
#endif

}


template <class T>
void _tmat<T>::_getPrivate(T*& buffer2, int & stride2, int& n2, int& m2, int& on2) const
{
	buffer2=buffer;
	stride2=stride;
	n2=n;
	m2=m;
	on2=on;
}

template <class T>
void _tmat<T>::_assignRef( _tmat<T> const& a )
{
	a._getPrivate(buffer, stride, n, m, on);
	on=-1;
}

template <class T>
_tmat<T>& _tmat<T>::assign( _tmat<T> const& a )
{
	_tmat<T> &c = (*this);
	c.setSize( a.rows(), a.cols() );

	int new_size=a.rows()*a.cols();


	if(new_size>0)
	{
		for(int i=0; i<rows(); i++)
		{
			T* c_i=c[i];
			T* a_i=a[i];
			for(int j=0; j<cols(); j++)
				c_i[j]=a_i[j];
		}
	}

	return c;
}


template <class T>
void _tmat<T>::setValue(int r, int c, _tmat<T> const& mat)
{
	ASSERT(rows()>=r+mat.rows());
	ASSERT(cols()>=c+mat.cols());

	for(int i=0; i<mat.rows(); i++)
		for(int j=0; j<mat.cols(); j++)
			(*this)[i+r][j+c]=mat[i][j];
}


template <class T>
void _tmat<T>::deleteRows(int start, int end)
{
	// end���ϴ� end-start��ŭ ���� �ö󰣴�. �� matrixũ�Ⱑ end-start��ŭ ���η� �۾�����.
	int numRows=end-start;

	for(int i=end; i<rows(); i++)
	{
		T* ti_numRows=(*this)[i-numRows];
		T* ti=(*this)[i];
		for(int j=0; j<cols(); j++)
			ti_numRows[j]=ti[j];
	}

	resize(rows()-numRows, cols());
}

template <class T>
void _tmat<T>::deleteCols(int start, int end)
{
	// end���ϴ� end-start��ŭ ���� �ö󰣴�. �� matrixũ�Ⱑ end-start��ŭ ���η� �۾�����.
	int numCols=end-start;

	for(int j=0; j<rows(); j++)
	{
		T* this_j=(*this)[j];
		for(int i=end; i<cols(); i++)
			this_j[i-numCols]=this_j[i];
	}

	resize(rows(), cols()-numCols);
}

template <class T>
void _tmat<T>::bubbles(int nrow, int nbubbles)
{
	// nrow���ϴ� nbubble��ŭ �Ʒ��� ������. �� matrixũ�Ⱑ nbubble��ŭ ���η� Ŀ����, ��ĭ�� �����.
	int prev_row=rows();
	resize(rows()+nbubbles, cols());

	for(int i=prev_row-1; i>=nrow; i--)
		for(int j=0; j<cols(); j++)
			(*this)[i+nbubbles][j]=(*this)[i][j];

	for(int i=nrow; i<nrow+nbubbles; i++)
		for(int j=0; j<cols(); j++)
			(*this)[i][j]=0;
}

template <class T>
void _tmat<T>::bubbleColumns(int ncolumn, int nbubbles)
{
	// ncolumn������ nbubble��ŭ �������� �δ�. �� matrixũ�Ⱑ nbubble��ŭ ���η� Ŀ����, ��ĭ�� �����.
	int prev_col=cols();
	resize(rows(), cols()+nbubbles);

	for(int i=0; i<rows(); i++)
	{
		for(int j=prev_col-1; j>=ncolumn; j--)
			(*this)[i][j+nbubbles]=(*this)[i][j];

		for(int j=ncolumn; j<ncolumn+nbubbles; j++)
			(*this)[i][j]=0;
	}
}

template <class T>
void _tmat<T>::setRow( int x, const _tvectorn<T>& vec )
{
	_tmat<T>& v=*this;
	ASSERT(vec.size()==m);
	for( int i=0; i<m; i++ )
		v[x][i] = vec[i];
}

template <class T>
void _tmat<T>::getRow( int row, _tvectorn<T>& out ) const
{
	const _tmat<T>& v=*this;
	out.setSize(cols());
	out.setValues(v[row]);
}

template <class T>
void _tmat<T>::setColumn( int x, const _tvectorn<T>& vec )
{
	_tmat<T>& v=*this;
	ASSERT(vec.size()==n);
	for (int i=0; i<n; i++)
		v[i][x] = vec[i];
}

template <class T>
void _tmat<T>::getColumn( int col, _tvectorn<T>& out) const
{
	out.setSize(rows());
	for(int i=0; i<n; i++)
		out[i]=(*this)[i][col];
}

template <class T>
void _tmat<T>::pushFront(const _tvectorn<T>& rowVec)
{
	bubbles(0,1);
	setRow(0, rowVec);
}

template <class T>
void _tmat<T>::popFront()
{
	deleteRows(0,1);
}


template <class T>
void _tmat<T>::popFront(_tvectorn<T>& out)
{
	out.assign(_row<_tvectorn<T>>(0));

	deleteRows(0,1);
}

template <class T>
void _tmat<T>::pushBack(const _tvectorn<T>& rowVec)
{
	ASSERT(rows()==0 || rowVec.size()==cols());
	resize(rows()+1, rowVec.size());
	setRow(rows()-1, rowVec);
}

template <class T>
void _tmat<T>::popBack()
{
	resize(rows()-1, cols());
}

template <class T>
void _tmat<T>::setColumn(int j, T value)
{
	for(int i=0; i<rows(); i++)
		(*this)[i][j]=value;
}

template <class T>
void _tmat<T>::setRow(int i, T value)
{
	for(int j=0; j<cols(); j++)
		(*this)[i][j]=value;
}

template <class T>
void _tmat<T>::setDiagonal(T value)
{
	ASSERT(rows()==cols());
	for(int i=0; i<n ;i++)
		(*this)[i][i]=value;
}

template <class T>
void _tmat<T>::setDiagonal(const _tvectorn<T>& v)
{
	ASSERT(rows()==cols());
	ASSERT(rows()==v.size());
	for(int i=0; i<n ;i++)
		(*this)[i][i]=v[i];
}

template <class T>
void _tmat<T>::extractRows(_tmat<T> const& mat, _tvectorn<int> const& rows)
{
	setSize(rows.size(), mat.cols());

	for(int i=0; i<rows.size(); i++)
	{
		for(int j=0; j<cols();j++)
			value(i,j)=mat.value(rows[i], j);
	}
}

template <class T>
void _tmat<T>::extractColumns(_tmat<T> const& mat, _tvectorn<int> const& columns)
{
	setSize(mat.rows(), columns.size());

	for(int i=0; i<mat.rows(); i++)
	{
		for(int j=0; j<columns.size();j++)
			value(i,j)=mat.value(i, columns[j]);
	}
}

template <class T>
void _tmat<T>::load(const char* filename)
{
	TFile file;
	file.OpenReadFile(filename);
	int nrow=file.UnpackInt();
	int ncolumn=file.UnpackInt();
	setSize(nrow, ncolumn);
	for(int i=0; i<rows(); i++)
		file.UnpackArray(&value(i,0), rows()*cols(), sizeof(T));
	file.CloseFile();
}

template <class T>
void _tmat<T>::save(const char* filename) const
{
	TFile file;
	file.OpenWriteFile(filename);
	file.PackInt(rows());
	file.PackInt(cols());
	for(int i=0; i<rows(); i++)
		file.PackArray(&value(i,0), cols(), sizeof(T));
	file.CloseFile();
}

template <class T>
void _tmat<T>::transpose( _tmat<T> const& a )
{
    _tmat<T> &c = (*this);
    c.setSize( a.cols(), a.rows() );

    for( int i=0; i<a.rows(); i++ )
	{
		T* a_i=a[i];
		for( int j=0; j<a.cols(); j++ )
			c(j,i)=a_i[j];
	}
}


template <class T>
void _tmat<T>::mult( _tmat<T> const& a, _tmat<T> const& b )
{
	if(&a==this)
	{
		_tmat<T> aT;
		aT.transpose(a);
		return multAtB(aT,b);
	}

	_tmat<T> bT;
	bT.transpose(b);
	return multABt(a,bT);
}

template <class T>
void _tmat<T>::multABt(_tmat<T> const& a, _tmat<T> const& b)
{
	_tmat<T> &c = (*this);
    assert( a.cols()==b.cols() );
    c.setSize( a.rows(), b.rows() );

    for( int i=0; i<a.rows(); i++ )
	{
		T* a_i=a[i];
		for( int j=0; j<b.rows(); j++ )
		{
			T subSum=0;

			T* b_j=b[j];
			
			for( int k=0; k<a.cols(); k++ )
				subSum += a_i[k] * b_j[k];

			c[i][j] = subSum;
		}
	}
}
template <class T>
void _tmat<T>::multAtB(_tmat<T> const& a, _tmat<T> const& b)
{
	// A^T B= (B A^T)^T

	_tmat<T> &c = (*this);
    assert( a.cols()==b.cols() );
    c.setSize( a.rows(), b.rows() );

    for( int i=0; i<b.rows(); i++ )
	{
		T* b_i=a[i];
		for( int j=0; j<a.rows(); j++ )
		{
			T subSum=0;

			T* a_j=b[j];
			
			for( int k=0; k<b.cols(); k++ )
				subSum += b_i[k] * a_j[k];

			c[j][i] = subSum;
		}
	}
}

template <class T>
void _tmat<T>::multAtBt(_tmat<T> const& a, _tmat<T> const& b)
{
	if(&a==this)
	{
		_tmat<T> aT;
		aT.transpose(a);
		return multABt(aT,b);
	}

	_tmat<T> bT;
	bT.transpose(b);
	return multAtB(a,b);
}

#define __private_tmat_for_each2(op)\
	_tmat<T> &c = (*this);\
    ASSERT( a.rows()==b.rows() );\
	ASSERT( a.cols()==b.cols() );\
	c.setSize(a.rows(), a.cols());\
	int a_rows=a.rows(); int a_cols=a.cols();\
	for(int i=0; i<a_rows; i++)\
	{\
		T* c_i=c[i];\
		T* a_i=a[i];\
		T* b_i=b[i];\
		for(int j=0; j<a_cols; j++)\
			c_i[j]=a_i[j] op b_i[j];\
	}


template <class T>
void  _tmat<T>::add( _tmat<T> const& a, _tmat<T> const& b)
{
	__private_tmat_for_each2(+);
}

template <class T>
void  _tmat<T>::subtract( _tmat<T> const& a, _tmat<T> const& b)
{
	__private_tmat_for_each2(-);
}

#define __private_tmat_for_each1(op)\
	_tmat<T> &c = (*this);\
	ASSERT(c.rows()==a.rows());\
	ASSERT(c.cols()==a.cols());\
	int c_rows=c.rows(); int c_cols=c.cols();\
	for(int i=0; i<c_rows; i++)\
	{\
		T* c_i=c[i];\
		T* a_i=a[i];\
		for(int j=0; j<c_cols; j++)\
			c_i[j] op;\
	}

template <class T>
void _tmat<T>::operator+=( _tmat<T> const& a )
{
	__private_tmat_for_each1(+= a_i[j])
}

template <class T>
void _tmat<T>::operator-=( _tmat<T> const& a )
{
	__private_tmat_for_each1(-= a_i[j])
}

template <class T>
void _tmat<T>::mult( _tmat<T> const& a, T b )
{
	__private_tmat_for_each1(= a_i[j]*b)
}



#define __private_tmat_for_each1_scalar(op)\
	_tmat<T> &c = (*this);\
	int c_rows=c.rows(); int c_cols=c.cols();\
	for(int i=0; i<c_rows; i++)\
	{\
		T* c_i=c[i];\
		for(int j=0; j<c_cols; j++)\
			c_i[j] op a;\
	}

template <class T>
void _tmat<T>::operator*=( T a )
{
	__private_tmat_for_each1_scalar(*=)
}

template <class T>
void _tmat<T>::operator/=( T aa )
{
	T a=1.0/aa;
	__private_tmat_for_each1_scalar(*=)
}

template <class T>
void _tmat<T>::concatRow( _tmat<T> const& a, _tmat<T> const& b)
{
	ASSERT(a.cols()==b.cols());
	setSize(a.rows()+b.rows(), a.cols());

	for(int i=0; i<a.rows(); i++)
	{
		_row<_tvectorn<T>>(i)=a._row<_tvectorn<T>>(i);
	}
	for(int i=0; i<b.rows(); i++)
	{
		_row<_tvectorn<T>>(i+a.rows())=b._row<_tvectorn<T>>(i);
	}
}

template <class T>
void _tmat<T>::concatColumn( _tmat<T> const& a, _tmat<T> const& b)
{
	ASSERT(a.rows()==b.rows());
	setSize(a.rows(), a.cols()+b.cols());

	for(int i=0; i<a.rows(); i++)
	{
		_row<_tvectorn<T>>(i).concat(a._row<_tvectorn<T>>(i),b._row<_tvectorn<T>>(i));
	}
}

template <class T>
void _tmat<T>::concatRow( _tmat<T> const& b)
{
	_tmat<T>& a=*this;
	ASSERT(a.rows()==0 || a.cols()==b.cols());
	int prevRow=a.rows();
	resize(a.rows()+b.rows(), b.cols());

	for(int i=0; i<b.rows(); i++)
	{
		_row<_tvectorn<T>>(i+prevRow)=b._row<_tvectorn<T>>(i);
	}
}
	
