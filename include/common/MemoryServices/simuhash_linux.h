#ifndef _SIMU_HASH_LINUX_H
#define _SIMU_HASH_LINUX_H

#include <functional>
#define _STD_BEGIN namespace std {
#define _STD_END }
#define _STD ::std::
#define _HASH_SEED (size_t)0xdeadbeef

namespace __gnu_cxx {
template<class _T> struct hash<_T*>
{
	size_t operator()(_T* __x) const 
	{
		return ((size_t)(void*)__x & _HASH_SEED);
	}
};

template<class _T> struct hash<_T&>
{
	size_t operator()(_T& __x) const 
	{
		return ((size_t)(void*)__x & _HASH_SEED);
	}
};

template<> struct hash<std::pair<long int, int> >
{
	size_t operator()(std::pair<long int, int> __x) const
	  {
	    return ((size_t)__x.first & _HASH_SEED);
	  }

};

template<> struct hash<long long unsigned int>
{
	size_t operator ()(const long long unsigned int __x) const
	{ return __x; }	
};

template<> struct hash<const long long unsigned int>
{
	size_t operator ()(const long long unsigned int __x) const
	{ return __x; }	
};

template<> struct hash<long long int>
{
	size_t operator ()(const long long int __x) const
	{ return __x; }	
};

template<> struct hash<float>
{
	size_t operator ()(const float __x) const
	{ return __x; }	
};

template<> struct hash<std::basic_string<wchar_t, std::char_traits<wchar_t>, std::allocator<wchar_t> > >
{
	typedef std::basic_string<wchar_t, std::char_traits<wchar_t>, std::allocator<wchar_t> > value_type;
	size_t operator ()(value_type const & __x) const
	{ 
	 unsigned int b = 378551;
	 unsigned int a = 63689;
	 unsigned int hash = 0;

	 for(value_type::const_iterator ii = __x.begin(), ee = __x.end(); ii != ee; ++ii)
	 {
	  hash = hash * a + *ii;
	  a = a * b;
	 }
	 return (hash & 0x7FFFFFFF);
	}	
};
template<> struct hash<const std::basic_string<wchar_t, std::char_traits<wchar_t>, std::allocator<wchar_t> > >
{
	typedef std::basic_string<wchar_t, std::char_traits<wchar_t>, std::allocator<wchar_t> > value_type;
	size_t operator ()(value_type const & __x) const
	{ 
	 unsigned int b = 378551;
	 unsigned int a = 63689;
	 unsigned int hash = 0;

	 for(value_type::const_iterator ii = __x.begin(), ee = __x.end(); ii != ee; ++ii)
	 {
	  hash = hash * a + *ii;
	  a = a * b;
	 }
	 return (hash & 0x7FFFFFFF);
	}	
};
template<> struct hash<std::basic_string<char, std::char_traits<char>, std::allocator<char> > >
{
	typedef std::basic_string<char, std::char_traits<char>, std::allocator<char> > value_type;
	size_t operator ()(value_type const & __x) const
	{ 
	 unsigned int b = 378551;
	 unsigned int a = 63689;
	 unsigned int hash = 0;

	 for(value_type::const_iterator ii = __x.begin(), ee = __x.end(); ii != ee; ++ii)
	 {
	  hash = hash * a + *ii;
	  a = a * b;
	 }
	 return (hash & 0x7FFFFFFF);
	}	
};
} //end namespace __gnu_cxx


namespace Simutronics
{

using std::equal_to;
using __gnu_cxx::hash;

// TEMPLATE STRUCT less
template<class _Ty>
struct less
	: public std::binary_function<_Ty, _Ty, bool>
{	// functor for operator<
	bool operator()(const _Ty& _Left, const _Ty& _Right) const
	{	// apply operator< to operands
		return (_Left < _Right);
	}
};
template<class _Init>
	inline size_t _Hash_value(_Init _Begin, _Init _End)
	{
	size_t _Val = 2166136261U;
	while(_Begin != _End)
		_Val = 1677619U * _Val ^ (size_t)*_Begin++;
	return (_Val);
	}

template<class _Kty> inline
	size_t hash_value(const _Kty& _Keyval)
	{
	return ((size_t)_Keyval ^ _HASH_SEED);
	}

	class big_hash 
	{
	public:
		size_t operator()(const char* _Str) const
		{
			return (_Hash_value(_Str, _Str+ ::strlen(_Str)));
		}
		size_t operator()(const wchar_t* _Str) const
		{
			return (_Hash_value(_Str, _Str + ::wcslen(_Str)));
		}
		
		template<class _Kty>
		size_t operator()(const _Kty& _Keyval) const
		{
			//return (size_t)1;
			return ((size_t)_Keyval ^ _HASH_SEED);
		}
		
	};


template<class _Kty,
class _Pr = equal_to<_Kty> /*less<_Kty> >*/,
class _Ha = hash<_Kty> >
class hash_compare
{	// traits class for hash containers
public:
	enum
	{	// parameters for hash table
		bucket_size = 4,	// 0 < bucket_size
		min_buckets = 8};	// min_buckets = 2 ^^ N, 0 < N

		hash_compare()
			: comp(), hashp()
		{	// construct with default comparator
			
		}

		hash_compare(_Pr _Pred)
			: comp(_Pred), hashp(_Pred)
		{	// construct with _Pred comparator
		}

		size_t operator()(const _Kty& _Keyval) const
		{	// hash _Keyval to size_t value by pseudorandomizing transform
			return ((size_t)hashp(_Keyval));
		}

		bool operator()(const _Kty& _Keyval1, const _Kty& _Keyval2) const
		{	// test if _Keyval1 ordered before _Keyval2
			return (comp(_Keyval1, _Keyval2));
		}

protected:
	_Pr comp;	// the comparator object
	_Ha hashp;
};

  template<class _Kty, int _Bucket_size = 4, int _Min_buckets = 8,
  class _Pr = equal_to<_Kty>, class _Ha = big_hash >
  class configured_hash_compare
  {     // traits class for hash containers
  public:
          enum
          {     // parameters for hash table
                  bucket_size = 4,      // 0 < bucket_size
                  min_buckets = 8};     // min_buckets = 2 ^^ N, 0 < N

                  configured_hash_compare()
                          : comp(), hashp()
                  {     // construct with default comparator
                  }

                  configured_hash_compare(_Pr _Pred)
                          : comp(_Pred), hashp(_Pred)
                  {     // construct with _Pred comparator
                  }

                  size_t operator()(const _Kty& _Keyval) const
                  {     // hash _Keyval to size_t value
			return ((size_t)hashp(_Keyval));
                  }

                  bool operator()(const _Kty& _Keyval1, const _Kty& _Keyval2) const
                  {     // test if _Keyval1 ordered before _Keyval2
                          return (comp(_Keyval1, _Keyval2));
                  }

  protected:
          _Pr comp;     // the comparator object
	  _Ha hashp;
  };

  template <typename TK, typename TV, class _Class = GlobalMemoryPool/*, class HC = stdext::hash_compare<T>*/ >
  class hash_map;

  template<typename TK, typename TV, class _Class>
  bool operator==(const USER_STL::hash_map<TK, TV, _Class>& __hm1,
           const USER_STL::hash_map<TK, TV,_Class>& __hm2);

  template<typename TK, typename TV, class _Class>
  bool operator!=(const USER_STL::hash_map<TK, TV, _Class>& __hm1,
           const USER_STL::hash_map<TK, TV,_Class>& __hm2);

template <typename TK, typename TV, class _Class/*, hash<TK>*/ >
  class hash_map
  {
  private:
	typedef hash_map<TK, TV, _Class> _Myt;
	typedef __gnu_cxx::hash_map<TK, TV, hash_compare<TK>, hash_compare<TK> /*, equal_to<TK> >*/
	#if defined(HE_USE_SIMUTRONICS_MEMORY_POOLING)
	  , USER_STL::simu_allocator< std::pair< TK, TV>, _Class > 
	#endif
	> _Mybase;
	_Mybase _BaseHMObj;
  public:
	typedef typename _Mybase::hasher hasher;
	typedef typename _Mybase::key_equal key_equal;
	typedef TK key_type;
	typedef TV mapped_type;
	typedef _STD pair<const TK, TV> value_type;
	typedef
	#if defined(HE_USE_SIMUTRONICS_MEMORY_POOLING)
	  USER_STL::simu_allocator< std::pair< TK, TV>, _Class > 
	#else
	  GlobalMemoryPool
	#endif
	  allocator_type;
	typedef typename _Mybase::iterator iterator;
	typedef typename _Mybase::const_iterator const_iterator;
	typedef size_t size_type;

	hasher hash_funct() const { return _BaseHMObj.hash_funct(); }
	key_equal key_eq() const { return _BaseHMObj.key_eq(); }
	allocator_type get_allocator() const { return _BaseHMObj.get_allocator(); }

	hash_map() : _BaseHMObj() { }

	template <class _InputIterator>
	hash_map(_InputIterator __f, _InputIterator __l)
		: _BaseHMObj(__f, __l) { }

	template <class _InputIterator>
	hash_map(_InputIterator __f, _InputIterator __l, size_t __n)
		: _BaseHMObj(__f, __l, __n) { }

	template <class _InputIterator>
	hash_map(_InputIterator __f, _InputIterator __l, size_t __n, const hasher& hf) : _BaseHMObj(__f, __l, __n, hf) { }
	
	size_t size() const { return _BaseHMObj.size(); }
	size_t max_size() const { return _BaseHMObj.max_size(); }
	bool empty() const { return _BaseHMObj.empty(); }
	void swap(hash_map& __hs) { _BaseHMObj.swap(__hs._BaeHMOBj); }

	friend bool operator==<TK, TV, _Class> (const USER_STL::hash_map<TK, TV, _Class>&,
				const USER_STL::hash_map<TK, TV, _Class>&);
	friend bool operator!=<TK, TV, _Class> (const USER_STL::hash_map<TK, TV, _Class>&,
				const USER_STL::hash_map<TK, TV, _Class>&);
	iterator begin() { return _BaseHMObj.begin(); }
	iterator end() { return _BaseHMObj.end(); }
	const_iterator begin() const { return _BaseHMObj.begin(); }
	const_iterator end() const { return _BaseHMObj.end(); }
	iterator find(const key_type& __key) { return _BaseHMObj.find(__key); }
	const_iterator find(const key_type& __key) const { return _BaseHMObj.find(__key); }

	__gnu_cxx::pair<iterator, bool> insert(const value_type& __obj)
		{ return _BaseHMObj.insert(__obj); }
	template <class _InputIterator>
	void insert(_InputIterator __f, _InputIterator __l)
		{ _BaseHMObj.insert(__f, __l); }

	__gnu_cxx::pair<iterator, bool> insert_noresize(const value_type& __obj)
		{ return _BaseHMObj.insert_noresize(__obj); }


	TV& operator[](const key_type& __key) { return _BaseHMObj.operator[](__key); }
	size_t count(const key_type& __key) const { return _BaseHMObj.count(__key); }
	__gnu_cxx::pair<iterator, iterator> equal_range(const key_type& __key) { return _BaseHMObj.equal_range(__key); }
	__gnu_cxx::pair<const_iterator, const_iterator> equal_range(const key_type& __key) const { return _BaseHMObj.equal_range(__key); }
	size_t erase(const key_type& __key) { return _BaseHMObj.erase(__key); }
	//void erase(iterator __it) { _BaseHMObj.erase(__it); }
	void erase(iterator __f, iterator __l) { _BaseHMObj.erase(__f, __l); }
	iterator erase(iterator where)
	{
	  iterator ii = where;
	  ++ii;
	  _BaseHMObj.erase(where);
	  return ii;
	}
	void clear() { _BaseHMObj.clear(); }

	void resize(size_t __hint) { _BaseHMObj.resize(__hint); }
	size_t bucket_count() const { return _BaseHMObj.bucket_count(); }
	size_t max_bucket_count() const { return _BaseHMObj.max_bucket_count(); }
	size_t elems_in_bucket(size_t __n) const {return _BaseHMObj.elems_in_bucket(__n); }



  };

template <class TK, class TV, class _Class>
inline bool
operator==(const USER_STL::hash_map<TK, TV, _Class>& __hm1,
	   const USER_STL::hash_map<TK, TV, _Class>& __hm2)
	{
		return (__hm1._BaseHMObj == __hm2._BaseHMObj);
	}

template <class TK, class TV, class _Class>
inline bool
operator!=(const USER_STL::hash_map<TK, TV, _Class>& __hm1,
	   const USER_STL::hash_map<TK, TV, _Class>& __hm2)
	{
		return (__hm1._BaseHMObj != __hm2._BaseHMObj);
	}


template <class TK, class TV, class _Class>
inline void
swap(const hash_map<TK, TV, _Class>& __hm1,
	   const hash_map<TK, TV, _Class>& __hm2)
	{
		__hm1.swap(__hm2);
	}
  template <typename TK, typename TV, class _Class = GlobalMemoryPool/*, class HC = stdext::hash_compare<TK>*/ >
  class hash_multimap: public __gnu_cxx::hash_multimap<TK, TV, hash_compare<TK>, hash_compare<TK>
#if defined(HE_USE_SIMUTRONICS_MEMORY_POOLING)
	  , USER_STL::simu_allocator< std::pair< TK, TV>, _Class > 
#endif
  >
  {
  public:
  typedef hash_multimap<TK, TV, _Class> _Myt;
  typedef __gnu_cxx::hash_multimap<TK, TV, hash_compare<TK>, USER_STL::hash_compare<TK, equal_to<TK> >
#if defined(HE_USE_SIMUTRONICS_MEMORY_POOLING)
	  , USER_STL::simu_allocator< std::pair< TK, TV>, _Class > 
#endif
  > _Mybase;
  typedef TK key_type;
  typedef TV mapped_type;
  typedef _STD pair<const TK, TV> value_type;
  typedef
#if defined(HE_USE_SIMUTRONICS_MEMORY_POOLING)
	  USER_STL::simu_allocator< std::pair< TK, TV>, _Class > 
#else
	  GlobalMemoryPool
#endif
	  allocator_type;
  allocator_type MyAlloc;
  typedef typename _Mybase::iterator iterator;
  typedef size_t size_type;

  allocator_type get_allocator() const {
	  // return allocator object for values
	  return (&MyAlloc);
  }


  };

  template <typename TK, typename TV, int _Bucket_size = 4, int _Min_buckets = 4096, 
  class _Class = GlobalMemoryPool/*, class HC = USER_STL::configured_hash_compare<TK, _Bucket_size, _Min_buckets>*/ >
  class large_hash_map
  {

  public:
	  typedef large_hash_map<TK, TV, _Bucket_size, _Min_buckets,_Class> _Myt;
	  typedef __gnu_cxx::hash_map<TK, TV, configured_hash_compare<TK, _Bucket_size, _Min_buckets>, configured_hash_compare<TK, _Bucket_size, _Min_buckets>
#if defined(HE_USE_SIMUTRONICS_MEMORY_POOLING)
		  , USER_STL::simu_allocator< std::pair< TK, TV>, _Class > 
#endif
	  > _Mybase;
	  typedef TK key_type;
	  typedef TV mapped_type;
	  typedef _STD pair<const TK, TV> value_type;
	  typedef
#if defined(HE_USE_SIMUTRONICS_MEMORY_POOLING)
		  USER_STL::simu_allocator< std::pair< TK, TV>, _Class > 
#else
		  GlobalMemoryPool
#endif
		  allocator_type;
	  typedef typename _Mybase::iterator iterator;
	  typedef size_t size_type;

	  iterator insert(iterator, const value_type& _Val)
	  {	// try to insert node with value _Val, ignore hint
		  return (insert(_Val).first);
	  }

	  allocator_type get_allocator() const {
		  // return allocator object for values
		  return (this->get_allocator());
	  }
	  
};

  template <typename TK, typename TV, int _Bucket_size = 4, int _Min_buckets = 4096, 
  class _Class = GlobalMemoryPool/*, class HC = USER_STL::configured_hash_compare<TK, _Bucket_size, _Min_buckets>*/ >
  class large_hash_multimap: public __gnu_cxx::hash_multimap<TK, TV, USER_STL::configured_hash_compare<TK, _Bucket_size, _Min_buckets>, USER_STL::configured_hash_compare<TK, _Bucket_size, _Min_buckets>
#if defined(HE_USE_SIMUTRONICS_MEMORY_POOLING)
	  , USER_STL::simu_allocator< std::pair< TK, TV>, _Class >
#endif
  >
  {
  public:
	  typedef hash_multimap<TK, TV, _Class> _Myt;
	  typedef __gnu_cxx::hash_multimap<TK, TV, configured_hash_compare<TK, _Bucket_size, _Min_buckets>, USER_STL::configured_hash_compare<TK, _Bucket_size, _Min_buckets>
#if defined(HE_USE_SIMUTRONICS_MEMORY_POOLING)
		  , USER_STL::simu_allocator< std::pair< TK, TV>, _Class > 
#endif
	  > _Mybase;
	  typedef TK key_type;
	  typedef TV mapped_type;
	  typedef _STD pair<const TK, TV> value_type;
	  typedef
#if defined(HE_USE_SIMUTRONICS_MEMORY_POOLING)
		  USER_STL::simu_allocator< std::pair< TK, TV>, _Class > 
#else
		  GlobalMemoryPool
#endif
		  allocator_type;
  	  allocator_type MyAlloc;
	  typedef typename _Mybase::iterator iterator;
	  typedef size_t size_type;

	  allocator_type get_allocator() const {
		  // return allocator object for values
		  return (this->get_allocator());
	  }

  };

  template <typename T, class _Class = GlobalMemoryPool/*, class HC = stdext::hash_compare<T>*/ >
  class hash_set;

  template<typename T, class _Class>
  bool operator==(const USER_STL::hash_set<T, _Class>& __hs1,
           const USER_STL::hash_set<T, _Class>& __hs2);

  template<typename T, class _Class>
  bool operator!=(const USER_STL::hash_set<T, _Class>& __hs1,
           const USER_STL::hash_set<T, _Class>& __hs2);

  template <typename T, class _Class/*, class HC = stdext::hash_compare<T>*/ >
  class hash_set
  {
  private:
	  typedef hash_set<T, _Class> _Myt;
	  typedef __gnu_cxx::hash_set<T, hash_compare<T>, hash_compare<T> /*, equal_to<TK> >*/
#if defined(HE_USE_SIMUTRONICS_MEMORY_POOLING)
		  , USER_STL::simu_allocator<T, _Class > 
#endif
	  > _Mybase;
	  _Mybase _BaseHMObj;
  public:
	  typedef typename _Mybase::hasher hasher;
	  typedef typename _Mybase::key_equal key_equal;
	  //typedef T key_type;
	  //typedef TV mapped_type;
	  typedef typename _Mybase::key_type key_type;
	  typedef typename _Mybase::value_type value_type;
	  //typedef _STD pair<const T, T> value_type;
	  //typedef _STD pair<const TK, TV> value_type;
	  typedef
#if defined(HE_USE_SIMUTRONICS_MEMORY_POOLING)
		  USER_STL::simu_allocator<T, _Class > 
#else
		  GlobalMemoryPool
#endif
		  allocator_type;
	  typedef typename _Mybase::iterator iterator;
	  typedef typename _Mybase::const_iterator const_iterator;
	  typedef size_t size_type;

	  hasher hash_funct() const { return _BaseHMObj.hash_funct(); }
	  key_equal key_eq() const { return _BaseHMObj.key_eq(); }
	  allocator_type get_allocator() const { return _BaseHMObj.get_allocator(); }

	  hash_set() : _BaseHMObj() { }

	  template <class _InputIterator>
	  hash_set(_InputIterator __f, _InputIterator __l)
		  : _BaseHMObj(__f, __l) { }

	  template <class _InputIterator>
	  hash_set(_InputIterator __f, _InputIterator __l, size_t __n)
		  : _BaseHMObj(__f, __l, __n) { }

	  template <class _InputIterator>
	  hash_set(_InputIterator __f, _InputIterator __l, size_t __n, const hasher& hf) : _BaseHMObj(__f, __l, __n, hf) { }

	  size_t size() const { return _BaseHMObj.size(); }
	  size_t max_size() const { return _BaseHMObj.max_size(); }
	  bool empty() const { return _BaseHMObj.empty(); }
	  void swap(hash_set& __hs) { _BaseHMObj.swap(__hs._BaseHMOBj); }

	  friend bool operator==<T, _Class>(const USER_STL::hash_set<T, _Class>&,
		  const USER_STL::hash_set<T, _Class>&);
	  friend bool operator!=<T, _Class>(const USER_STL::hash_set<T, _Class>&,
		  const USER_STL::hash_set<T, _Class>&);
	  iterator begin() { return _BaseHMObj.begin(); }
	  iterator end() { return _BaseHMObj.end(); }
	  const_iterator begin() const { return _BaseHMObj.begin(); }
	  const_iterator end() const { return _BaseHMObj.end(); }
	  iterator find(const key_type& __key) { return _BaseHMObj.find(__key); }
	  const_iterator find(const key_type& __key) const { return _BaseHMObj.find(__key); }

	  __gnu_cxx::pair<iterator, bool> insert(const value_type& __obj)
	  { return _BaseHMObj.insert(__obj); }
	  template <class _InputIterator>
	  void insert(_InputIterator __f, _InputIterator __l)
	  { _BaseHMObj.insert(__f, __l); }

	  __gnu_cxx::pair<iterator, bool> insert(const_iterator& __i, const value_type& __t)
	  {
	    return insert(__t);
	  }

	  __gnu_cxx::pair<iterator, bool> insert_noresize(const value_type& __obj)
	  { return _BaseHMObj.insert_noresize(__obj); }


	  T& operator[](const key_type& __key) { return _BaseHMObj.operator[](__key); }
	  size_t count(const key_type& __key) const { return _BaseHMObj.count(__key); }
	  __gnu_cxx::pair<iterator, iterator> equal_range(const key_type& __key) { return _BaseHMObj.equal_range(__key); }
	  __gnu_cxx::pair<const_iterator, const_iterator> equal_range(const key_type& __key) const { return _BaseHMObj.equal_range(__key); }
	  size_t erase(const key_type& __key) { return _BaseHMObj.erase(__key); }
	  void erase(iterator __f, iterator __l) { _BaseHMObj.erase(__f, __l); }
	  iterator erase(iterator where)
	  {
		  iterator ii = where;
		  ++ii;
		  _BaseHMObj.erase(where);
		  return ii;
	  }
	  void clear() { _BaseHMObj.clear(); }

	  void resize(size_t __hint) { _BaseHMObj.resize(__hint); }
	  size_t bucket_count() const { return _BaseHMObj.bucket_count(); }
	  size_t max_bucket_count() const { return _BaseHMObj.max_bucket_count(); }
	  size_t elems_in_bucket(size_t __n) const {return _BaseHMObj.elems_in_bucket(__n); }
};

template <class T, class _Class>
bool
operator==(const USER_STL::hash_set<T, _Class>& __hs1,
	   const USER_STL::hash_set<T, _Class>& __hs2)
	{
		return (__hs1._BaseHMObj == __hs2._BaseHMObj);
	}

template <>
inline bool
operator==<unsigned long long, USER_STL::GlobalMemoryPool>(const USER_STL::hash_set<unsigned long long, USER_STL::GlobalMemoryPool>& __hs1,
           const USER_STL::hash_set<unsigned long long, USER_STL::GlobalMemoryPool>& __hs2)
        {
                return (__hs1._BaseHMObj == __hs2._BaseHMObj);
        }


template <class T, class _Class>
inline bool
operator!=(const USER_STL::hash_set<T, _Class>& __hs1,
	   const USER_STL::hash_set<T, _Class>& __hs2)
	{
		return (__hs1._BaseHMObj != __hs2._BaseHMObj);
	}

  template <typename T, int _Bucket_size = 4, int _Min_buckets = 4096, 
  class _Class = GlobalMemoryPool/*,class HC = USER_STL::configured_hash_compare<T, _Bucket_size, _Min_buckets>*/ >
  class large_hash_set
  {
  public:
	  typedef large_hash_set<T, _Bucket_size, _Min_buckets, _Class> _Myt;
	  typedef __gnu_cxx::hash_set<T, configured_hash_compare<T, _Bucket_size, _Min_buckets>, configured_hash_compare<T, _Bucket_size, _Min_buckets >
#if defined(HE_USE_SIMUTRONICS_MEMORY_POOLING)
		  , USER_STL::simu_allocator<T, _Class > 
#endif
	  > _Mybase;
	  typedef T key_type;
	  typedef T value_type;
	  typedef
#if defined(HE_USE_SIMUTRONICS_MEMORY_POOLING)
		  USER_STL::simu_allocator<T, _Class > 
#else
		  GlobalMemoryPool
#endif
		  allocator_type;
	  typedef typename _Mybase::iterator iterator;
	  typedef size_t size_type;

	  allocator_type get_allocator() const {
		  // return allocator object for values
		  return (this->get_allocator());
	  }
  };
}

namespace std {
  template <typename _Type, class _Class >
    class insert_iterator<USER_STL::hash_set<_Type, _Class/*USER_STL::hash_compare<_Type>, USER_STL::hash_compare<_Type> 
    #if defined(HE_USE_SIMUTRONICS_MEMORY_POOLING)
          , USER_STL::simu_allocator<_Type, _Class>
    #endif
    */> > {
      protected:      
        typedef USER_STL::hash_set<_Type, _Class/*USER_STL::hash_compare<_Type>, USER_STL::hash_compare<_Type>
        #if defined(HE_USE_SIMUTRONICS_MEMORY_POOLING)
          , USER_STL::simu_allocator<_Type, _Class>
        #endif
        */> _Container;
        _Container* container;
      public:
        typedef _Container container_type;
        typedef output_iterator_tag iterator_category;
        typedef void value_type;
        typedef void difference_type;
        typedef void pointer;
        typedef void reference;

        insert_iterator(_Container& __x) : container(&__x) {}
        insert_iterator(_Container& __x, typename _Container::iterator) : container(&__x) {}
        insert_iterator<_Container>& operator=(const typename _Container::value_type& __value) {
          container->insert(__value);
          return *this;
        }
        insert_iterator<_Container>& operator*() { return *this;}
        insert_iterator<_Container>& operator++() { return *this;}
        insert_iterator<_Container>& operator++(int) { return *this;}
    };
  template <typename _Type, class _Class >
    class insert_iterator<USER_STL::hash_map<_Type, _Class/*USER_STL::hash_compare<_Type>, USER_STL::hash_compare<_Type> 
    #if defined(HE_USE_SIMUTRONICS_MEMORY_POOLING)
          , USER_STL::simu_allocator<_Type, _Class>
    #endif
    */> > {
      protected:      
        typedef USER_STL::hash_map<_Type, _Class/*USER_STL::hash_compare<_Type>, USER_STL::hash_compare<_Type>
        #if defined(HE_USE_SIMUTRONICS_MEMORY_POOLING)
          , USER_STL::simu_allocator<_Type, _Class>
        #endif
        */> _Container;
        _Container* container;
      public:
        typedef _Container container_type;
        typedef output_iterator_tag iterator_category;
        typedef void value_type;
        typedef void difference_type;
        typedef void pointer;
        typedef void reference;

        insert_iterator(_Container& __x) : container(&__x) {}
        insert_iterator(_Container& __x, typename _Container::iterator) : container(&__x) {}
        insert_iterator<_Container>& operator=(const typename _Container::value_type& __value) {
          container->insert(__value);
          return *this;
        }
        insert_iterator<_Container>& operator*() { return *this;}
        insert_iterator<_Container>& operator++() { return *this;}
        insert_iterator<_Container>& operator++(int) { return *this;}
    };
}
#endif

