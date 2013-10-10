// zsString.h By Zsíroskenyér Team 2013.10.10 11:07
#pragma once



#ifdef STATIC_SIZE 
#define ZSSTRING_STACK_SIZE 256

#include "common.h"
#include <fstream>




class zsString {
public:
	zsString operator + (const zsString& str) const;
	zsString operator + (const WCHAR *str) const;

	zsString& operator += (const WCHAR *str);
	zsString& operator += (const WCHAR val);
	zsString& operator += (int val);
	zsString& operator += (float val);
	zsString& operator += (const zsString& str);

	zsString& operator = (const zsString& str);
	zsString& operator = (const WCHAR *str);

	void Clear();
	void Resize(uint32 newSize);

	void RemoveFromStart(uint16 numWCHARs);
	void AddToStart(WCHAR ch);

	bool FindStr(const WCHAR *str);
	bool StrStr(const WCHAR *str, bool notCaseSensitive = false);

	void ToAnsiChar(char *buffer, uint16 size) const;

	void *GetLine(std::wifstream& inStream);
	const WCHAR *GetDataAfterFirstChar(const WCHAR ch) const;

	bool IsEmpty();

	WCHAR *Str();
	const WCHAR *StrConst() const;

	// @TODO now only check the first WCHARacter
	bool operator < (const zsString& str) const;
	bool operator ==(const zsString& str) const;
	const WCHAR operator [] (uint32 idx) const;
	WCHAR& operator [] (uint32 idx);

	zsString(const zsString& str);
	zsString(const WCHAR *str);
	zsString(const char *str);
	zsString(WCHAR ch);
	zsString(int val);
	zsString(float val);
	zsString();
	~zsString();
protected:
	// Memory from stack instead of the heap to improve performance
	WCHAR staticData[ZSSTRING_STACK_SIZE];

	// If we run out of staticData (stack), we allocate from heap
	WCHAR *dynamicData;

	// Point to the start of staticData all the time
	WCHAR *firstCharPtr;

	// Point to the end of staticData all the time
	WCHAR *lastCharPtr;
};

// zsString + "asdasd"  works, but
// "asdasd" + zsString need to work too
zsString operator + (const WCHAR *cStr, const zsString& str);
zsString operator + (const CHAR *cStr, const zsString& str);


#else // STATIC_SIZE

#include "memory\tlsf.h"
#include <string>
#include <cstdint>
#include <stdexcept>

// allocator class
template <class T>
class TLSFAllocator : public std::allocator_traits<T> {
public:
	// types
	typedef T value_type;
	// construction
	TLSFAllocator() {};
	TLSFAllocator(const TLSFAllocator& other) {}
	TLSFAllocator(TLSFAllocator&& other) {}
	template <class U>
	TLSFAllocator(const TLSFAllocator<U>& other) {}
	template <class U>
	TLSFAllocator(TLSFAllocator<U>&& other) {}
	// allocate
	pointer allocate(size_t n);
	void deallocate(T* ptr, n);
	// equality
	bool operator==(const TLSFAllocator& other);
	bool operator!=(const TLSFAllocator& other);
private:
	// internal pool
	static TLSF memPool;
};


template <class T>
bool TLSFAllocator<T>::operator==((const TLSFAllocator& other) {
		return &memPool == &other.memPool;
}
template <class T>
bool TLSFAllocator<T>::operator!=(const TLSFAllocator& other) {
	return !(*this==other);
}

template <class T>
typename TLSFAllocator<T>::pointer TLSFAllocator<T>::allocate(size_t n) {
	pointer p = (pointer)memPool.malloc(sizeof(value_type)*n);
	if (p==nullptr)
		throw std::bad_alloc;	
}
template <class T>
typename TLSFAllocator<T>::pointer TLSFAllocator<T>::deallocate(pointer ptr, size_t n) {
	memPool.free((void*)ptr);
}

// zsString type
typedef std::basic_string<wchar_t, std::char_traits<wchar_t>, TLSFAllocator<wchar_t>> zsString;



zsString a;



#endif // STATIC_SIZE
