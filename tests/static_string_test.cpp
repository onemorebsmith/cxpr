#include <iostream>

#include "gtest/gtest.h"
#include <cxpr.h>

//////////////////////////////////////////////////////////////////////////

TEST(fixed_string_tests, char_constructor_tests)
{
	{	// basic
		auto ss = cxpr::make_fixed_string("stRings ARE COOL 12345 %@$%");
		std::string cppStr = "stRings ARE COOL 12345 %@$%";
		EXPECT_EQ(ss, cppStr);
		EXPECT_EQ(ss.size(), cppStr.size());
	}

	{	// lower transform
		auto ss = cxpr::make_fixed_string<cxpr::lower_case>("stRings ARE COOL 12345 %@$%");
		std::string cppStr = "strings are cool 12345 %@$%";
		EXPECT_EQ(ss, cppStr);
		EXPECT_EQ(ss.size(), cppStr.size());
	}

	{	// upper transform
		auto ss = cxpr::make_fixed_string<cxpr::upper_case>("stRings are COOL 12345 %@$%");
		std::string cppStr = "STRINGS ARE COOL 12345 %@$%";
		EXPECT_EQ(ss, cppStr);
		EXPECT_EQ(ss.size(), cppStr.size());
	}

	{	// custom transform
		struct minus_one { constexpr char operator()(char in) const noexcept { return in - 1; } };

		auto ss = cxpr::make_fixed_string<minus_one>("stRings ARE COOL 12345 %@$%");
		std::string cppStr = "stRings ARE COOL 12345 %@$%";
		for (auto& it : cppStr)
		{
			it--;
		}
		EXPECT_EQ(ss, cppStr);
		EXPECT_EQ(ss.size(), cppStr.size());
	}

	{ // udl constructors
		using namespace cxpr;
		const auto ss = "stRings ARE COOL 12345 %@$%"_fstr32;
		static_assert(sizeof(ss) == 32, "unexpected size, should be 32 bytes");
		std::string cppStr = "stRings ARE COOL 12345 %@$%";
		EXPECT_EQ(ss, cppStr);
		EXPECT_EQ(ss.size(), cppStr.size());
	}

	{ // udl constructors
		using namespace cxpr;
		const auto ss = "stRings ARE COOL 12345 %@$%"_fstr256;
		static_assert(sizeof(ss) == 256, "unexpected size, should be 256 bytes");
		std::string cppStr = "stRings ARE COOL 12345 %@$%";
		EXPECT_EQ(ss, cppStr);
		EXPECT_EQ(ss.size(), cppStr.size());
	}
}

TEST(fixed_string_tests, wchar_constructor_tests)
{
	{	// basic
		auto ss = cxpr::make_wfixed_string(L"StrinGs");
		std::wstring cppStr = L"StrinGs";
		EXPECT_EQ(ss, cppStr);
		EXPECT_EQ(ss.size(), cppStr.size());
	}

	{	// lower transform
		auto ss = cxpr::make_wfixed_string<cxpr::lower_case>(L"StRIngs are COOL 12345 %@$%");
		std::wstring cppStr = L"strings are cool 12345 %@$%";
		EXPECT_EQ(ss, cppStr);
		EXPECT_EQ(ss.size(), cppStr.size());
	}

	{	// upper transform
		auto ss = cxpr::make_wfixed_string<cxpr::upper_case>(L"striNGs aRe cooL 12345 %@$%");
		std::wstring cppStr = L"STRINGS ARE COOL 12345 %@$%";
		EXPECT_EQ(ss, cppStr);
		EXPECT_EQ(ss.size(), cppStr.size());
	}
}

//////////////////////////////////////////////////////////////////////////

TEST(fixed_string_tests, assignment_test)
{
	{	// no transform, allow truncation
		auto ss = cxpr::fixed_string<17, cxpr::no_transform, cxpr::overrun_behavior_trunc>();
		ss = "This is too long for the buffer should be truncated";
		EXPECT_EQ(ss.size(), std::string("This is too long").size());
		EXPECT_EQ(ss, "This is too long");
	}

	{	// different sizes, smaller to larger
		auto ss1 = cxpr::fixed_string<32, cxpr::no_transform, cxpr::overrun_behavior_trunc>();
		auto ss2 = cxpr::fixed_string<64, cxpr::no_transform, cxpr::overrun_behavior_trunc>();
		ss1 = "Random data 12345";
		ss2 = ss1;
		EXPECT_EQ(ss1.size(), ss2.size());
		EXPECT_EQ(ss2, "Random data 12345");
	}

	{	// different sizes, larger to smaller, data fits smaller
		auto ss1 = cxpr::fixed_string<32, cxpr::no_transform, cxpr::overrun_behavior_trunc>();
		auto ss2 = cxpr::fixed_string<64, cxpr::no_transform, cxpr::overrun_behavior_trunc>();
		ss2 = "Random data 12345";
		ss1 = ss2;
		EXPECT_EQ(ss1, "Random data 12345");
	}

	{	// different sizes, larger to smaller, data too large for smaller so it should trunc
		auto ss1 = cxpr::fixed_string<16, cxpr::no_transform, cxpr::overrun_behavior_trunc>();
		auto ss2 = cxpr::fixed_string<32, cxpr::no_transform, cxpr::overrun_behavior_trunc>();
		ss2 = "Random data 123457890";
		ss1 = ss2;
		EXPECT_EQ(ss1, "Random data 123");
	}
}

constexpr std::string_view large_data_first_32 = "Lorem ipsum dolor sit amet, con";
constexpr std::string_view large_data = "Lorem ipsum dolor sit amet, consectetuer adipiscing elit. Aenean commodo ligula eget dolor. Aenean massa. Cum sociis natoque penatibus et magnis dis parturient montes, nascetur ridiculus mus. Donec quam felis, ultricies nec, pellentesque eu, pretium quis, sem. Nulla consequat massa quis enim. Donec pede justo, fringilla vel, aliquet nec, vulputate eget, arcu. In enim justo, rhoncus ut, imperdiet a, venenatis vitae, justo. Nullam dictum felis eu pede mollis pretium. Integer tincidunt. Cras dapibus. Vivamus elementum semper nisi. Aenean vulputate eleifend tellus. Aenean leo ligula, porttitor eu, consequat vitae, eleifend ac, enim. Aliquam lorem ante, dapibus in, viverra quis, feugiat a, tellus. Phasellus viverra nulla ut metus varius laoreet. Quisque rutrum. Aenean imperdiet. Etiam ultricies nisi vel augue. Curabitur ullamcorper ultricies nisi. Nam eget dui. Etiam rhoncus. Maecenas tempus, tellus eget condimentum rhoncus, sem quam semper libero, sit amet adipiscing sem neque sed ipsum. Nam quam nunc, blandit v";

TEST(fixed_string_tests, large_string_tests)
{
	{	// ss1 is too large to be indexed (see use_emplace_indexed). Setting from a smaller indexed should still work
		auto ss1 = cxpr::fixed_string<1024, cxpr::no_transform, cxpr::overrun_behavior_trunc>(large_data);
		auto ss2 = cxpr::fixed_string<32, cxpr::no_transform, cxpr::overrun_behavior_trunc>();
		ss2 = "large_data";
		ss1 = ss2;
		EXPECT_EQ(ss1.size(), ss2.size());
		EXPECT_EQ(ss1, ss2);
	}

	{	// large, non-indexed -> smaller, fits
		auto ss1 = cxpr::fixed_string<1024>();
		auto ss2 = cxpr::fixed_string<32>();
		ss2 = "Random data 123457890";
		ss2 = ss1;
		EXPECT_EQ(ss1.size(), ss2.size());
		EXPECT_EQ(ss1, ss2);
	}

	{	// large, non-indexed -> smaller, indexed with trunc
		auto ss1 = cxpr::fixed_string<1024>(large_data);
		auto ss2 = cxpr::fixed_string<32, cxpr::no_transform, cxpr::overrun_behavior_trunc>();
		ss2 = ss1;
		EXPECT_NE(ss1.size(), ss2.size());
		EXPECT_EQ(ss2, large_data_first_32);
	}

	{	// large to large
		auto ss1 = cxpr::fixed_string<1024>();
		auto ss2 = cxpr::fixed_string<1024>();
		ss2 = "Random data 123457890";
		ss2 = ss1;
		EXPECT_EQ(ss1.size(), ss2.size());
		EXPECT_EQ(ss1, ss2);
	}

	{	// large to larger
		auto ss1 = cxpr::fixed_string<1024>(large_data);
		auto ss2 = cxpr::fixed_string<2024>();
		ss2 = "Random data 123457890";
		ss2 = ss1;
		EXPECT_EQ(ss1.size(), ss2.size());
		EXPECT_EQ(ss1, ss2);
	}

	{	// large assigned
		auto ss1 = cxpr::fixed_string<1024>();
		ss1 = large_data;
		EXPECT_EQ(ss1, large_data);
		ss1.clear();
		EXPECT_EQ(ss1.size(), 0);
		EXPECT_EQ(ss1, "");
	}
}

TEST(fixed_string_tests, char_manipulation)
{
	{	// fixed to cpp str
		auto ss1 = cxpr::make_fixed_string("RANdom Data 1234");
		auto ss2 = cxpr::to_lower<std::string>(ss1);
		EXPECT_EQ(ss2, "random data 1234");
	}

	{	// fixed to fixed
		auto ss1 = cxpr::make_fixed_string("RANdom Data 1234");
		auto ss2 = cxpr::to_lower(ss1);
		EXPECT_EQ(ss2, "random data 1234");
	}
	//{	// fixed to cpp str
	//	auto ss1 = cxpr::make_fixed_string<std::string>("RANdom Data 1234");
	//	auto ss2 = cxpr::to_lower(ss1);
	//	EXPECT_EQ(ss2, "random data 1234");
	//}
}