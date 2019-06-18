
#ifndef JED_MEM_ALLOCATORS
#define JED_MEM_ALLOCATORS

#include <stdlib.h> //size_t, malloc, free
#include <new> // bad_alloc, bad_array_new_length
#include <memory>
#include <iostream>

namespace mem
{

// this wrapper is required to use the general allocators with SharedAllocator
template <typename T, typename Alloc>
class Allocator: public Alloc
{
public:

	// static assert alloc can allocate a T
	template <typename U>
	using can_alloc_type = typename Alloc::template can_alloc_type<U>;

	// ctor
	Allocator(void * memory, size_t size) noexcept
	:Alloc(memory, size)
	{}

	// copy ctor deleted
	Allocator(Allocator const&) = delete;

	// move ctor
	Allocator(Allocator && other)
	:Alloc(std::move(other))
	{
	}

	// move assignment optor
	Allocator& operator=(Allocator other)
	{
		swap(*this, other);
		return *this;
	}

	~Allocator() = default;

	T* allocate(size_t n)
	{
		return static_cast<T*>(this->alloc(sizeof(T)*n, alignof(T)));
	}

	void deallocate(T * ptr, size_t n)
	{
		this->dealloc(ptr, sizeof(T)*n);
	}
};

template <size_t N, size_t Alignment>
class PoolAllocator
{
public:
	template <typename T>
	using can_alloc_type = typename std::conditional<(sizeof(T) <= N && alignof(T) <= Alignment), std::true_type, std::false_type>::type;

	PoolAllocator(void * memory, size_t size)
	{}

	PoolAllocator(PoolAllocator const&) = delete;

	PoolAllocator(PoolAllocator && other)
	{
		swap(*this, other);
	}

	PoolAllocator& operator=(PoolAllocator other)
	{
		swap(*this, other);
		return *this;
	}

	~PoolAllocator() = default;

	void* alloc(size_t size, size_t alignment)
	{
		return nullptr;
	}

	void dealloc(void *const ptr, size_t size)
	{}

	friend void swap(PoolAllocator & first, PoolAllocator & second)
	{
		using std::swap;
	}
private:
	
};

class LinearAllocator
{
public:

	template <typename T>
	using can_alloc_type = std::true_type;

	LinearAllocator() = delete;

	// non owning of the memory, 
	LinearAllocator(void * memory, size_t size) noexcept
	:m_memory{memory}, m_head{m_memory}, m_size{size}
	{
		std::cout << "LinearAllocator ctor\n";
	}

	// can't be copied, so allocations don't stomp over the memory (m_head falls out of sync)
	LinearAllocator(LinearAllocator const&) = delete;

	// can move it
	LinearAllocator(LinearAllocator && other)
	{
		std::cout << "LinearAllocator move ctor\n";
		swap(*this, other);
	}

	// move constructs other
	// swaps other into this
	// ensures the allocator swapped in is zeroed
	LinearAllocator& operator=(LinearAllocator other)
	{
		std::cout << "LinearAllocator move operator=\n";
		swap(*this, other);
		return *this;
	}

	~LinearAllocator() = default;

	void* alloc(size_t size, size_t alignment)
	{
		uint8_t * const memory = static_cast<uint8_t*>(m_memory);

		size_t alignment_offset = alignment - (reinterpret_cast<size_t>(m_head))%alignment - 1;

		uint8_t * head = static_cast<uint8_t*>(m_head) + alignment_offset;

		if(size == 0)
		{
			return nullptr;
		}

		if(size > static_cast<size_t>(-1))
		{
			throw std::bad_alloc();
		}

		if(size > m_size) { throw std::bad_alloc(); }

		// if the allocation would overflow the linear allocators memory, move the head back to the start
		if(memory + m_size < head + size)
		{
			head = memory;
			alignment_offset = alignment - ((size_t)head)%alignment - 1;
			head = head + alignment_offset;
		}

		void * pv = head;
		head += size;
		m_head = static_cast<void*>(head);

		std::cout << "Giving memory here: " << pv << std::endl;

		return pv;
	}

	void dealloc(void *const ptr, size_t size)
	{} // no-op

	friend void swap(LinearAllocator & first, LinearAllocator & second)
	{
		using std::swap;
		
		// if T has it's own swap method it will be used here
		// instead of std::swap, looked up by ADL
		swap(first.m_memory, second.m_memory);
		swap(first.m_head, second.m_head);
		swap(first.m_size, second.m_size);
	}

	friend
	std::ostream& operator<<(std::ostream& os, LinearAllocator const& alloc)
	{
		os << "Linear Allocator:\n\tm_memory: " << alloc.m_memory << "\n\tm_head: " << alloc.m_head << "\n\tm_size: " << alloc.m_size;
		return os;
	}

private:

	void * m_memory{nullptr};
	void * m_head{nullptr};
	size_t m_size{0};

};



template <class T, class Alloc = std::allocator<T>>
class SharedAllocator
{
public:
	using value_type = T;
	using parent_allocator_type = Alloc;

	// use this line to only allow a SharedAllocator<T> to use an Allocator for type T
	//static_assert((std::is_same<typename parent_allocator_type::value_type, value_type>::value), "Parent Allocator type must be the same as the Value Type");

	SharedAllocator(Alloc && allocator)
	:m_alloc{std::make_shared<Alloc>(std::move(allocator))}
	{
		std::cout << "SharedAllocator Constructor\n";
	}

	SharedAllocator(SharedAllocator<T, Alloc> const& other)
	:m_alloc{other.m_alloc}
	{
		std::cout << "SharedAllocator Copy Constructor\n";
	}

	//move ctor
	SharedAllocator(SharedAllocator<T, Alloc> && other)
	:m_alloc{std::move(other.m_alloc)}
	{
		std::cout << "SharedAllocator Move Constructor\n";
	}

	~SharedAllocator()
	{
		std::cout << "SharedAllocator Destructor\n";
	}

/*
	template<typename U, typename AllocU,
		typename = typename std::enable_if<AllocU::template can_alloc_type<value_type>::value>::type>
	SharedAllocator(SharedAllocator<U, AllocU> const& other) noexcept
	:m_alloc{other.m_alloc, reinterpret_cast<Alloc*>(other.m_alloc.get())}
	{
		std::cout << "SharedAllocator Converting Copy Constructor\n";
	}
*/

	template<typename U, typename AllocU>
	SharedAllocator(SharedAllocator<U, AllocU> const& other) noexcept
	:m_alloc{other.m_alloc, reinterpret_cast<Alloc*>(other.m_alloc.get())}
	{
		static_assert((AllocU::template can_alloc_type<value_type>::value) == true, "");

		std::cout << "SharedAllocator Converting Copy Constructor\n";
	}

	// allocators are equal if each can destroy pointers in the other
	template<class U>
	bool operator==(SharedAllocator<U> const& other) const noexcept
	{
		return m_alloc == other.m_alloc;
	}

	template<class U>
	bool operator!=(SharedAllocator<U> const& other) const noexcept
	{
		return !(*this == other);
	}

	T* allocate(const size_t n)
	{
		return m_alloc->allocate(n);
	}
	
	void deallocate(T * p, size_t n) noexcept
	{
		m_alloc->deallocate(p, n);
	}

	/*
	friend void swap(SharedAllocator<T> & first, SharedAllocator<T> & second)
	{
		using std::swap;
		
		// if T has it's own swap method it will be used here
		// instead of std::swap, looked up by ADL
		
	}
	*/

	friend std::ostream& operator<<(std::ostream& os, SharedAllocator<T> const& alloc)
	{
		os << "SharedAllocator:\n\tm_memory: " << alloc.m_alloc.get() << "\n";
		return os;
	}

private:
	std::shared_ptr<Alloc> m_alloc{nullptr};

	template <class U, class B>
	friend class SharedAllocator;
};

/*

If I have my custom allocator, it also has to be copy proof.. what does that mean..

It can't directly own resources. No Alloc * or void * members in the allocator.

if it only does a single allocation it could take a & to a parent 

Types:
SharedAllocator<T, Alloc>
	- takes a regularly non copyable allocators(below) and maintains a shared_ptr to it so it's copyable
	- can be used with std::allocator_traits/for use with stl containers mostly

OwningAllocator<Alloc, Dealloc>
	- not compatible with std::allocator_traits/stl containers
	- for when the Allocator needs to be able to alloc and dealloc from another allocator, but doesn't own it
	- Allocates from the scheme in Alloc, gets initial pool of memory from Dealloc allocator
	- holds a reference to Dealloc allocator for the allocation and deallocation
	- cannot be copy constructed, only moved

Other Allocators - allocate from a void* pool of memory
	- not compatible with std::allocator_traits/stl containers
	- no new or delete, constructed with a pool of memory that it neither constructs, destructs, news, nor deletes
	- needs a constructor Allocator(void * memory, size_t size_in_bytes)
	- just wraps memory handed to it
	- for if you know the memory will outlive MyAllocator, and the memory will be deallocated from higher up
	- i.e for if you have a vector member of a state, and the states memory was allocated in a big block that will be deallocated all at once
			This won't deallocate it's block, so only use when you won't ever have to reclaim this memory
			from where it was allocated. i.e. it has the same lifetime as where it came from
			(maybe get it from a unique_ptr in it's parent??? hm..)
	- cannot be copy constructed, only moved

SharedAllocator<OwningAllocator<MyAllocator<T>, ParentAllocator<T>>>
	- a way to wrap an allocator with a parent to deallocate with that can be used with stl
	- i.e. for having a hierarchy of allocators splitting up initial starting memory
	
-vs-

SharedAllocator<MyAllocator<T>>
	- compatible with std::allocator_traits/stl containers
	- doesn't need to be deallocated, similar in use to just MyAllocator, need to know lifetime
*/

}; // namespace mem

#endif