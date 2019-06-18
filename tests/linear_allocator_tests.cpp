
#include "catch2/catch.hpp"
#include "mem/allocators.hpp"

#include <vector>

SCENARIO("Memory can be allocated from a LinearAllocator", "[LinearAllocator]")
{
	GIVEN("A LinearAllocator<int>")
	{
		auto alloc = mem::LinearAllocator<int>(1);

		WHEN("Allocating an int")
		{
			int* value = alloc.allocate(1);

			THEN("The integer can be used")
			{
				*value = 1;

				REQUIRE(*value == 1);
			}

			THEN("The integer can be deallocated")
			{
				alloc.deallocate(value, 1);
			}
		}

		WHEN("Using it in a vector<int, mem::LinearAllocator<int>>")
		{
			std::vector<int, mem::LinearAllocator<int>> vec = std::vector<int, mem::LinearAllocator<int>>(1, alloc);

			THEN("You can use the vector")
			{
				//std::cout << vec.data() << std::endl;

				//REQUIRE(vec[0] == 1);
			}
		}
	}
}

/*
SCENARIO("Description", "[tag]")
{
	GIVEN("Preconditions")
	{
		WHEN("Doing an action")
		{
			THEN("The result")
			{
				REQUIRE()
			}
		}
	}
}
*/