#ifndef _STL_EXT_internal
#define _STL_EXT_internal

using std::swap;

#ifdef	__BORLANDC__
#define M_NOSTDCONTAINERS_EXT
#endif
#ifdef	_M_AMD64
#define M_DONTDEFERCLEAR_EXT
#endif

template <class T>
class	xalloc	{
public:
	typedef	size_t		size_type;
	typedef ptrdiff_t	difference_type;
	typedef T*			pointer;
	typedef const T*	const_pointer;
	typedef T&			reference;
	typedef const T&	const_reference;
	typedef T			value_type;

public:
	template<class _Other>	
	struct rebind			{	typedef xalloc<_Other> other;	};
public:
							pointer					address			(reference _Val) const					{	return (&_Val);	}
							const_pointer			address			(const_reference _Val) const			{	return (&_Val);	}
													xalloc			()										{	}
													xalloc			(const xalloc<T>&)						{	}
	template<class _Other>							xalloc			(const xalloc<_Other>&)					{	}
	template<class _Other>	xalloc<T>&				operator=		(const xalloc<_Other>&)					{	return (*this);	}
							pointer					allocate		(size_type n, const void* p=0) const	{	return xr_alloc<T>((u32)n);	}
							char*					_charalloc		(size_type n)							{	return (char*)allocate(n); }
							void					deallocate		(pointer p, size_type n) const			{	xr_free	(p);				}
							void					deallocate		(void* p, size_type n) const			{	xr_free	(p);				}
							void					construct		(pointer p, const T& _Val)				{	::new(p) T(_Val);	}
							void					destroy			(pointer p)								{	p->~T();			}
							size_type				max_size		() const								{	size_type _Count = (size_type)(-1) / sizeof (T);	return (0 < _Count ? _Count : 1);	}
};

struct xr_allocator {
	template <typename T>
	struct helper {
		typedef xalloc<T>	result;
	};

	static	void	*alloc		(const u32 &n)	{	return xr_malloc((u32)n);	}
	template <typename T>
	static	void	dealloc		(T *&p)			{	xr_free(p);					}
};

template<class _Ty,	class _Other>	inline	bool operator==(const xalloc<_Ty>&, const xalloc<_Other>&)		{	return (true);							}
template<class _Ty, class _Other>	inline	bool operator!=(const xalloc<_Ty>&, const xalloc<_Other>&)		{	return (false);							}

namespace std
{
	template<class _Tp1, class _Tp2>	inline	xalloc<_Tp2>&	__stl_alloc_rebind(xalloc<_Tp1>& __a, const _Tp2*)	{	return (xalloc<_Tp2>&)(__a);	}
	template<class _Tp1, class _Tp2>	inline	xalloc<_Tp2>	__stl_alloc_create(xalloc<_Tp1>&, const _Tp2*)		{	return xalloc<_Tp2>();			}
};

// string(char)
typedef		std::basic_string<char, std::char_traits<char>, xalloc<char> >	xr_string;

// vector
template	<typename T, typename allocator = xalloc<T> >
using xr_vector = std::vector<T, allocator>;

template <typename T>
void clear_and_reserve(xr_vector<T> &vector_object) {
    if (vector_object.capacity() <= (vector_object.size() + vector_object.size() / 4)) {
        vector_object.clear();
    } else {
        u32 old = vector_object.size();
        vector_object.clear();
        vector_object.reserve(old);
    }
}

// deque
template <typename T, typename allocator = xalloc<T> >
using xr_deque = std::deque<T, allocator>;

// stack
template <typename _Ty, class _C = xr_vector<_Ty> >
class xr_stack {
public:
	typedef typename _C::allocator_type						allocator_type;
	typedef typename allocator_type::value_type				value_type;
	typedef typename allocator_type::size_type				size_type;

	//explicit			stack(const allocator_type& _Al = allocator_type()) : c(_Al) {}
	allocator_type		get_allocator	() const							{return (c.get_allocator()); }
	bool				empty			() const							{return (c.empty()); }
	u32					size			() const							{return c.size(); } 
	value_type&			top				()									{return (c.back()); }
	const value_type&	top				() const							{return (c.back()); }
	void				push			(const value_type& _X)				{c.push_back(_X); }
	void				pop				()									{c.pop_back(); }
	bool				operator==		(const xr_stack<_Ty, _C>& _X) const	{return (c == _X.c); }
	bool				operator!=		(const xr_stack<_Ty, _C>& _X) const	{return (!(*this == _X)); }
	bool				operator<		(const xr_stack<_Ty, _C>& _X) const	{return (c < _X.c); }
	bool				operator>		(const xr_stack<_Ty, _C>& _X) const	{return (_X < *this); }
	bool				operator<=		(const xr_stack<_Ty, _C>& _X) const	{return (!(_X < *this)); }
	bool				operator>=		(const xr_stack<_Ty, _C>& _X) const	{return (!(*this < _X)); }

protected:
	_C c;
};

template	<typename T, typename allocator = xalloc<T> >									class	xr_list 		: public std::list<T,allocator>			{ public: u32 size() const {return (u32)__super::size(); } };
template	<typename K, class P=std::less<K>, typename allocator = xalloc<K> >				class	xr_set			: public std::set<K,P,allocator>		{ public: u32 size() const {return (u32)__super::size(); } };
template	<typename K, class P=std::less<K>, typename allocator = xalloc<K> >				class	xr_multiset		: public std::multiset<K,P,allocator>	{ public: u32 size() const {return (u32)__super::size(); } };
template	<typename K, class V, class P=std::less<K>, typename allocator = xalloc<std::pair<K,V> > >	class	xr_map 			: public std::map<K,V,P,allocator>		{ public: u32 size() const {return (u32)__super::size(); } };
template	<typename K, class V, class P=std::less<K>, typename allocator = xalloc<std::pair<K,V> > >	class	xr_multimap		: public std::multimap<K,V,P,allocator>	{ public: u32 size() const {return (u32)__super::size(); } };

struct pred_str		: public std::binary_function<char*, char*, bool>	{	
	IC bool operator()(const char* x, const char* y) const				{	return xr_strcmp(x,y)<0;	}
};
struct pred_stri	: public std::binary_function<char*, char*, bool>	{	
	IC bool operator()(const char* x, const char* y) const				{	return _stricmp(x,y)<0;	}
};

// STL extensions
#define DEF_LIST(N,T)				typedef xr_list< T > N;			typedef N::iterator N##_it;
#define DEF_MAP(N,K,T)				typedef xr_map< K, T > N;		typedef N::iterator N##_it;

#define DEFINE_DEQUE(T,N,I)			typedef xr_deque< T > N;		typedef N::iterator I;
#define DEFINE_VECTOR(T,N,I)		typedef xr_vector< T > N;		typedef N::iterator I;
#define DEFINE_MAP(K,T,N,I)			typedef xr_map< K , T > N;		typedef N::iterator I;
#define DEFINE_MAP_PRED(K,T,N,I,P)	typedef xr_map< K, T, P > N;	typedef N::iterator I;
#define DEFINE_MMAP(K,T,N,I)		typedef xr_multimap< K, T > N;	typedef N::iterator I;
#define DEFINE_SVECTOR(T,C,N,I)		typedef svector< T, C > N;		typedef N::iterator I;
#define DEFINE_SET(T,N,I)			typedef xr_set< T > N;			typedef N::iterator I;
#define DEFINE_SET_PRED(T,N,I,P)	typedef xr_set< T, P > N;		typedef N::iterator I;
#define DEFINE_STACK(T,N)			typedef xr_stack< T > N;

#include "FixedVector.h"

// auxilary definition
DEFINE_VECTOR(bool,boolVec,boolIt);
DEFINE_VECTOR(BOOL,BOOLVec,BOOLIt);
DEFINE_VECTOR(BOOL*,LPBOOLVec,LPBOOLIt);
DEFINE_VECTOR(Frect,FrectVec,FrectIt);
DEFINE_VECTOR(Irect,IrectVec,IrectIt);
DEFINE_VECTOR(Fplane,PlaneVec,PlaneIt);
DEFINE_VECTOR(Fvector2,Fvector2Vec,Fvector2It);
DEFINE_VECTOR(Fvector,FvectorVec,FvectorIt);
DEFINE_VECTOR(Fvector*,LPFvectorVec,LPFvectorIt);
DEFINE_VECTOR(Fcolor,FcolorVec,FcolorIt);
DEFINE_VECTOR(Fcolor*,LPFcolorVec,LPFcolorIt);
DEFINE_VECTOR(LPSTR,LPSTRVec,LPSTRIt);
DEFINE_VECTOR(LPCSTR,LPCSTRVec,LPCSTRIt);
//DEFINE_VECTOR(string64,string64Vec,string64It);
DEFINE_VECTOR(xr_string,SStringVec,SStringVecIt);

DEFINE_VECTOR(s8,S8Vec,S8It);
DEFINE_VECTOR(s8*,LPS8Vec,LPS8It);
DEFINE_VECTOR(s16,S16Vec,S16It);
DEFINE_VECTOR(s16*,LPS16Vec,LPS16It);
DEFINE_VECTOR(s32,S32Vec,S32It);
DEFINE_VECTOR(s32*,LPS32Vec,LPS32It);
DEFINE_VECTOR(u8,U8Vec,U8It);
DEFINE_VECTOR(u8*,LPU8Vec,LPU8It);
DEFINE_VECTOR(u16,U16Vec,U16It);
DEFINE_VECTOR(u16*,LPU16Vec,LPU16It);
DEFINE_VECTOR(u32,U32Vec,U32It);
DEFINE_VECTOR(u32*,LPU32Vec,LPU32It);
DEFINE_VECTOR(float,FloatVec,FloatIt);
DEFINE_VECTOR(float*,LPFloatVec,LPFloatIt);
DEFINE_VECTOR(int,IntVec,IntIt);
DEFINE_VECTOR(int*,LPIntVec,LPIntIt);

#ifdef __BORLANDC__
DEFINE_VECTOR(AnsiString,AStringVec,AStringIt);
DEFINE_VECTOR(AnsiString*,LPAStringVec,LPAStringIt);
#endif

#endif