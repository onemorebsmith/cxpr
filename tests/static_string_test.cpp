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
		EXPECT_EQ(ss, "This is too long");
	}

	{	// different sizes, smaller to larger
		auto ss1 = cxpr::fixed_string<32, cxpr::no_transform, cxpr::overrun_behavior_trunc>();
		auto ss2 = cxpr::fixed_string<64, cxpr::no_transform, cxpr::overrun_behavior_trunc>();
		ss1 = "Random data 12345";
		ss2 = ss1;
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