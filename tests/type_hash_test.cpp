#include "gtest/gtest.h"
#include <cxpr.h>

//////////////////////////////////////////////////////////////////////////

template <typename ... params_t>
struct hasher
{
	constexpr decltype(auto) operator()()
	{
		return cxpr::make_std_array<cxpr::hash_t>(cxpr::typehash_v<params_t>...);
	}
};

TEST(typehash_tests, uniqueness_test)
{
	{	// types should be unique with regards to other types
		using test_types_t = cxpr::typeset<int, float, double, char, unsigned char, long long, std::string, char*>;
		static constexpr auto hashes = cxpr::fold<hasher>(test_types_t{});

		for (size_t i = 0; i < hashes.size(); i++)
		{
			const auto val = hashes[i];
			auto match = std::find(std::begin(hashes) + i + 1, std::end(hashes), val);
			EXPECT_EQ(match, std::end(hashes));
		}
	}

	{	// cv-qualifications shouldn't change hash code 
		using test_types_t = cxpr::typeset<int, int&, const int&>;
		static constexpr auto hashes = cxpr::fold<hasher>(test_types_t{});
		EXPECT_EQ(hashes[0], hashes[1]);
		EXPECT_EQ(hashes[0], hashes[2]);
	}
}