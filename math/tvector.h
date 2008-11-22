#pragma once
//============================================================================]
// The two following templated classes contain a static member function (doOperation)
// performing assignation operation between two values(= or +=).
// so T_lhs and T_rhs can be float, const float, double, a Complex class or whatever.
//============================================================================]
struct AssignOpAssign
{
  template <typename T_lhs, typename T_rhs>
  inline static void doOperation(T_lhs & lhs, T_rhs & rhs) { lhs = rhs; }
};
struct AssignOpAdd
{
  template <typename T_lhs, typename T_rhs>
  inline static void doOperation(T_lhs & lhs, T_rhs & rhs) { lhs += rhs; }
};
struct AssignOpSub
{
  template <typename T_lhs, typename T_rhs>
  inline static void doOperation(T_lhs & lhs, T_rhs & rhs) { lhs -= rhs; }
};
//============================================================================]
// The AssignOpLoopUnroller class contains the code performing the loop unrolling
// using the template recursion principle.
// This principle is simple: a template with an integer parameter contains a static
// function exec. Loop<N::exec calls Loop<N-1::exec
// Explicit specialisation is used to stop the recursion: Loop<0::exec() does nothing.
//============================================================================]
template <typename T_lhs, typename T_rhs, typename T_operator>
struct AssignOpLoopUnroller
{
  template <int N>
  struct Loop
  {
    inline static void exec(T_lhs * aLArray, T_rhs * aRArray)
    {
      T_operator::doOperation(aLArray[N - 1], aRArray[N - 1]);
      Loop<N - 1>::exec(aLArray, aRArray);
    }
  };  
  template <>
  struct Loop<0>
  {
    static inline void exec(T_lhs * aLArray, T_rhs * aRArray) { }
  };
};

struct ConstAssignOpSub
{
  template <typename T_lhs, typename T_rhs>
  inline static void doOperation(T_lhs & lhs, T_rhs rhs) { lhs -= rhs; }
};

struct ConstAssignOpAdd
{
  template <typename T_lhs, typename T_rhs>
  inline static void doOperation(T_lhs & lhs, T_rhs rhs) { lhs += rhs; }
};

struct ConstAssignOpMult
{
  template <typename T_lhs, typename T_rhs>
  inline static void doOperation(T_lhs & lhs, T_rhs rhs) { lhs *= rhs; }
};

template <typename T_lhs, typename T_rhs, typename T_operator>
struct ConstAssignOpLoopUnroller
{
  template <int N>
  struct Loop
  {
    inline static void exec(T_lhs * aLArray, T_rhs c)
    {
      T_operator::doOperation(aLArray[N - 1], c);
      Loop<N - 1>::exec(aLArray, c);
    }
  };  
  template <>
  struct Loop<0>
  {
    static inline void exec(T_lhs * aLArray, T_rhs c) { }
  };
};

struct BinOpSub
{
  template <typename T>
  inline static void doOperation(T& v, T const& l, T const& r) { v=l-r;}
};

struct BinOpAdd
{
  template <typename T>
  inline static void doOperation(T& v, T const& l, T const& r) { v=l+r;}
};


template <typename T, typename T_operator>
struct BinaryOpLoopUnroller
{
  template <int N>
  struct Loop
  {
    inline static void exec(T* aVArray, T const* aLArray, T const* aRArray)
    {
      T_operator::doOperation(aVArray[N-1], aLArray[N - 1], aRArray[N - 1]);
      Loop<N - 1>::exec(aVArray, aLArray, aRArray);
    }
  };  
  template <>
  struct Loop<0>
  {
    static inline void exec(T* aVArray, T const* aLArray, T const* aRArray) { }
  };
};
/**
 *  A toy vector class illustrating the use of this "unrolling metaprogram"
 *  Of course, other kinds of unrollers would have to be developed for other kind
 *  of operations (like a ConstantAssignOpLoopUnroller).
 */
template <typename T, int N>
class _tvector
{
protected:
	T   m_aValues[N];
public:	
	T       & operator [] (int i)			{ return m_aValues[i]; }  
	const T & operator [] (int i) const		{ return m_aValues[i]; }  
	T       & operator () (int i)			{ return m_aValues[i]; }  
	const T & operator () (int i) const		{ return m_aValues[i]; }	
	
	T& x()									{ return m_aValues[0];}
	const T& x() const						{ return m_aValues[0];}
	T& y()									{ return m_aValues[1];}
	const T& y() const						{ return m_aValues[1];}
	T& z()									{ return m_aValues[2];}
	const T& z() const						{ return m_aValues[2];}
	T& w()									{ return m_aValues[3];}
	const T& w() const						{ return m_aValues[3];}
	
	T       & value (int i)					{ return m_aValues[i]; }  
	const T	& value (int i) const			{ return m_aValues[i]; }  


	_tvector  & operator += (const _tvector & rhs)
	{
		AssignOpLoopUnroller<T, const T, AssignOpAdd >::Loop<N>::exec(m_aValues, rhs.m_aValues);
		return *this;
	}  
	_tvector  & operator -= (const _tvector & rhs)
	{
		AssignOpLoopUnroller<T, const T, AssignOpSub >::Loop<N>::exec(m_aValues, rhs.m_aValues);
		return *this;
	}  

	_tvector  & operator = (const _tvector & rhs)
	{
		AssignOpLoopUnroller<T, const T, AssignOpAssign >::Loop<N>::exec(m_aValues, rhs.m_aValues);
		return *this;
	}

	void assign(const _tvector& rhs)
	{
		AssignOpLoopUnroller<T, const T, AssignOpAssign >::Loop<N>::exec(m_aValues, rhs.m_aValues);
	}

	_tvector  & operator *= (const T rhs)
	{
		ConstAssignOpLoopUnroller<T, const T, ConstAssignOpMult >::Loop<N>::exec(m_aValues, rhs);
		return *this;
	}  

	// binary operations
	void add (const _tvector & lhs, const _tvector & rhs) 
	{
		BinaryOpLoopUnroller<T, BinOpAdd>::Loop<N>::exec(m_aValues, lhs.m_aValues, rhs.m_aValues);	
	}
	void sub (const _tvector & lhs, const _tvector & rhs) 
	{
		BinaryOpLoopUnroller<T, BinOpSub>::Loop<N>::exec(m_aValues, lhs.m_aValues, rhs.m_aValues);	
	}

	const _tvector operator + (const _tvector & rhs) const 
	{
		_tvector c;
		c.add(*this, rhs);
		return c;
	}

	const _tvector operator - (const _tvector & rhs) const
	{
		_tvector c;
		c.sub(*this, rhs);
		return c;
	}

	const _tvector operator * (const T rhs) const
	{
		return _tvector(*this) *= rhs;
	}

	const _tvector operator / (const T rhs) const
	{
		return _tvector(*this) *= 1/rhs;
	}

	// negate
	const _tvector operator - () const
	{
		return _tvector(*this)*-1.0;
	}


	void interpolate(T t, const _tvector & a, const _tvector & b)
	{
		add(a*(1.0-t), b*t);
	}

	T operator%( const _tvector& a) const
	{
		T sum=0;
		for(int i=0; i<N; i++)
			sum+=value(i)*a(i);
		return sum;
	}

	T length() const	
	{
		return sqrt((*this)%(*this));
	}

	T squaredDistance(const _tvector & b) const
	{
		_tvector c;
		c.sub(*this, b);
		return c%c;
	}

	inline T distance(const _tvector & b) const
	{
		return sqrt(squaredDistance(b));
	}

	void normalize()
	{
		T len=length();
		this->operator *=(1.0/len);
	}

	void getValues(vectorn& v) const
	{
		v.setSize(N);
		for(int i=0; i<N; i++)
			v[i]=value(i);
	}
	void setValues(vectorn const& v)
	{
		ASSERT(N==v.size());
		for(int i=0; i<N; i++)
			value(i)=v[i];
	}

};

class index2 : public _tvector<int, 2>
{
public:
	index2(){}
	index2(int x, int y)	{m_aValues[0]=x; m_aValues[1]=y;}
	template <class T>
	index2(const T& other) { _tvector<int, 2>::assign(other);}
};

class vector2 : public _tvector<double, 2> 
{
public:
	vector2(){}
	vector2(double x, double y) {m_aValues[0]=x; m_aValues[1]=y;}
	vector2(const _tvector<double, 2>& other) {value(0)=other[0]; value(1)=other[1];}
	void difference(vector3 const& a, vector3 const& b)
	{
		// ignores y coordinate.
		this->value(0)=b.x-a.x;
		this->value(1)=b.z-a.z;
	}

	void rotate90(vector2 v)
	{
		this->value(0)=v[1];
		this->value(1)=v[0]*-1.0;
	}

	vector3 toVector3() const
	{
		vector3 c;
		c.x=value(0);
		c.y=0;
		c.z=value(1);
		return c;
	}
};

class vector2NView;
class vector2N : public _tvectorn<vector2, double, 2>
{	
protected:
	vector2N (double* ptrr, int size, int stride):_tvectorn<vector2, double, 2>(ptrr,size,stride){}
public:
	vector2N ():_tvectorn<vector2, double, 2>(){}

	// ���� ī���ؼ� �޾ƿ´�.	
	template <class VecType>
	vector2N (const VecType& other)	{ assign(other);}

	explicit vector2N ( int x):_tvectorn<vector2, double, 2>() { setSize(x);}
	~vector2N(){}

	// L-value�� ���ɼ� �ִ�, reference array�� ����� return �Ѵ�. 
	// ex) v.range(0,2).setValues(2, 1.0, 2.0);
	vector2NView range(int start, int end, int step=1);
	const vector2NView range(int start, int end, int step=1) const	;

	template <class VecType>
	void operator=(const VecType& other)	{ _tvectorn<vector2, double, 2>::assign(other);}

	// L-value�� ���ɼ� �ִ� reference vector �� return�Ѵ�.
	vectornView		x() const						{ return _column<vectornView>(0);}
	vectornView		y() const						{ return _column<vectornView>(1);}
	vectornView		column(int i) const				{ return _column<vectornView>(i);}
};



class vector2NView :public vector2N
{
public:
	// L-value�� ���ɼ� �ִ�, reference array�� �����. 
	vector2NView (double* ptrr, int size, int stride):vector2N(ptrr, size, stride){}
	// ���� reference�� �޾ƿ´�.
	template <class VecType>
	vector2NView (const VecType& other)	{ assignRef(other);}

	~vector2NView (){}

	// L-value�� ���Ǵ� ���, ���� copy�Ѵ�.
	template <class VecType>
	void operator=(const VecType& other)	{ _tvectorn<vector2, double, 2>::assign(other);}
};

