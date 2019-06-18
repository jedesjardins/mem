
#include "mem/allocators.hpp"

#include <vector>
#include <cstdint>

template<class T>
struct A
{
	A() = default;

	template<class U>
	A(A<U> const& other)
	{}
};

int main()
{
	/*
	std::vector<uint8_t, mem::LinearAllocator<uint8_t, 64>> vec(64, 1);

	std::cout << (void*)vec.data() << std::endl;

	for(size_t i = 0; i < vec.size()+10000000; ++i)
	{
		std::cout << i << " " << (int)vec[i]++ << " " << (int)vec[i]<< "\n";
	}

	auto alloc = mem::LinearAllocator<uint8_t, 64>(std::allocator<uint8_t>());
	

	mem::LinearAllocator<int> alloc(10);

	//mem::LinearAllocator<int> alloc2(std::move(alloc));

	std::cout << alloc << "\n";

	std::vector<int, mem::LinearAllocator<int>> vec(1, std::move(alloc));

	std::cout << alloc << "\n";
	*/


	/*
	A<int> a1;
	A<uint> a2(a1);
	A<int> a3(a2);

	using std::shared_ptr;

	auto sa1 = std::make_shared<A<int>>();

	std::cout << "Count: " << sa1.use_count() << "\n";

	shared_ptr<A<uint>> sa2(sa1, reinterpret_cast<A<uint>*>(sa1.get()));

	std::cout << "Count: " << sa1.use_count() << "\n";
	std::cout << "Count: " << sa2.use_count() << "\n";
	*/


	/*
	std::shared_ptr<A<int>> a1;

	std::cout << "Count: " << a1.use_count() << "\n";

	std::shared_ptr<A<uint32_t>> a2(a1, static_cast<A<uint32_t>*>(a1.get()));
	//shared_ptr<B> thisB(thisA, static_cast<B*>(thisA.get()));

	std::cout << "Count: " << a1.use_count() << "\n";

	std::cout << sizeof(mem::SharedAllocator<int, std::allocator<int>>) << " " << sizeof(std::shared_ptr<char>) << "\n";
	*/

	/*
	mem::SharedAllocator<int, std::allocator<int>> alloc((std::allocator<int>()));

	std::cout << "\nOriginal: " << alloc << "\n";

	mem::SharedAllocator<int, std::allocator<int>> alloc_copy(alloc);

	std::cout << "\nOriginal: " << alloc << "Copy: " << alloc_copy << "\n";

	mem::SharedAllocator<int, std::allocator<int>> alloc_move(std::move(alloc));

	std::cout << "\nOriginal: " << alloc << "Move: " << alloc_move << "\n";

	std::vector<int, decltype(alloc)> vec(4, 1, alloc);
	*/

	//mem::SharedAllocator<uint, std::allocator<uint>> alloc((std::allocator<int>()));

	//mem::SharedAllocator<int, std::allocator<uint>> alloc2(alloc);

	//std::vector<int, decltype(alloc)> vec(1, 0, alloc);

	/*
	char mem[128];

	mem::LinearAllocator a(mem, 128);

	std::cout << "First " << a << "\n\n";

	//mem::LinearAllocator b(std::move(a)); // error!
	mem::LinearAllocator b(std::move(a));

	std::cout << "\nFirst " << a << "\nSecond " << b << "\n\n";

	//mem::LinearAllocator c = b; // error!
	mem::LinearAllocator c(&b, 0);

	std::cout << "\nFirst " << a << "\nSecond " << b << "\nThird " << c << "\n\n";

	c = std::move(b);

	std::cout << "\nFirst " << a << "\nSecond " << b << "\nThird " << c << "\n";

	std::cout << alignof(char) << " " << alignof(uint16_t) << " " << alignof(uint32_t) << "\n";
	*/

	/*
	char a[1];

	size_t alignment = alignof(int);

	std::cout << &a << " " << ((size_t)a)%alignment << "\n";
	*/

	/*
	char static_block[128];

	mem::LinearAllocator alloc(static_block, 128);

	std::cout << "Memory block at: " << &static_block << "\n";

	void* first_allocation = alloc.allocate(1, 1);

	void* second_allocation = alloc.allocate(4, 4);

	std::cout << first_allocation << " " << second_allocation << "\n";

	void* third = alloc.allocate(8, 8);
	*/

	char a[128];

	mem::Allocator<int, mem::PoolAllocator<sizeof(int), alignof(int)>> alloc{&a, 128};

	mem::Allocator<int, mem::PoolAllocator<sizeof(int), alignof(int)>> alloc2{std::move(alloc)};

	alloc = std::move(alloc2);

	int * an_int = alloc.allocate(1);

	static_assert(mem::LinearAllocator::can_alloc_type<long>::value == true, "");

	static_assert(mem::PoolAllocator<sizeof(int), alignof(int)>::can_alloc_type<char>::value == true, "");

	static_assert(mem::PoolAllocator<sizeof(int), alignof(int)>::can_alloc_type<int>::value == true, "");

	static_assert(mem::PoolAllocator<sizeof(uint64_t), alignof(int)>::can_alloc_type<uint64_t>::value == false, "");


	mem::SharedAllocator<int, mem::Allocator<int, mem::PoolAllocator<sizeof(int), alignof(int)>>> alloc3{std::move(alloc)};

	mem::SharedAllocator<int, mem::Allocator<int, mem::PoolAllocator<sizeof(int), alignof(int)>>> alloc4{alloc3};

	mem::SharedAllocator<char[4], mem::Allocator<char[4], mem::PoolAllocator<sizeof(int), alignof(int)>>> alloc5{alloc3};

	//mem::SharedAllocator<uint, mem::Allocator<uint, mem::PoolAllocator<sizeof(int), alignof(int)>>> alloc4{alloc3};

	return 0;
}


