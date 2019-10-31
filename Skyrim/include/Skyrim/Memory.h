#pragma once

#include <new>

class Heap
{
public:
	// haven't verified alignment vars
	DEFINE_MEMBER_FN(Allocate, void*, 0x00A48D60, UInt32 size, UInt32 alignment, bool aligned);
	DEFINE_MEMBER_FN(Free, void, 0x00A487B0, void* buf, bool aligned);

	DEFINE_MEMBER_FN(Unk_00A481C0, void, 0x00A481C0);
	DEFINE_MEMBER_FN(Unk_00A48890, UInt32, 0x00A48890);
};

extern Heap* g_formHeap;

void* FormHeap_Allocate(UInt32 size);
void FormHeap_Free(void* ptr);

template<typename T>
inline T* FormHeap_Allocate(void) {
	//return reinterpret_cast<T*>(g_formHeap->Allocate(sizeof(T), alignon(T), true));
	return reinterpret_cast<T*>(FormHeap_Allocate(sizeof(T)));
}

inline void* malloc(std::size_t a_size)
{
	return g_formHeap->Allocate(a_size, 0, false);
}


template <class T>
inline T* malloc(std::size_t a_size)
{
	return static_cast<T*>(malloc(a_size));
}


template <class T>
inline T* malloc()
{
	return malloc<T>(sizeof(T));
}


inline void* aligned_alloc(std::size_t a_alignment, std::size_t a_size)
{
	return g_formHeap->Allocate(a_size, a_alignment, true);
}


template <class T>
inline T* aligned_alloc(std::size_t a_alignment, std::size_t a_size)
{
	return static_cast<T*>(aligned_alloc(a_alignment, a_size));
}


template <class T>
inline T* aligned_alloc()
{
	return aligned_alloc<T>(alignof(T), sizeof(T));
}


inline void* calloc(std::size_t a_num, std::size_t a_size)
{
	return malloc(a_num * a_size);
}


template <class T>
inline T* calloc(std::size_t a_num, std::size_t a_size)
{
	return static_cast<T*>(calloc(a_num, a_size));
}


template <class T>
inline T* calloc(std::size_t a_num)
{
	return calloc<T>(a_num, sizeof(T));
}


inline void free(void* a_ptr)
{
	g_formHeap->Free(a_ptr, false);
}


inline void aligned_free(void* a_ptr)
{
	g_formHeap->Free(a_ptr, true);
}

#define TES_FORMHEAP_REDEFINE_NEW()											\
	inline void*	operator new(std::size_t a_count)													{ return malloc(a_count); }			\
	inline void*	operator new[](std::size_t a_count)													{ return malloc(a_count); }			\
	inline void*	operator new([[maybe_unused]] std::size_t a_count, void* a_plcmnt)					{ return a_plcmnt; }				\
	inline void*	operator new[]([[maybe_unused]] std::size_t a_count, void* a_plcmnt)				{ return a_plcmnt; }				\
	inline void		operator delete(void* a_ptr)														{ if (a_ptr) { free(a_ptr); } }		\
	inline void		operator delete[](void* a_ptr)														{ if (a_ptr) { free(a_ptr); } }		\
	inline void		operator delete([[maybe_unused]] void* a_ptr, [[maybe_unused]] void* a_plcmnt)		{ return; }							\
	inline void		operator delete[]([[maybe_unused]] void* a_ptr, [[maybe_unused]] void* a_plcmnt)	{ return; }
