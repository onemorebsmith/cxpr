#include <iostream>

#include "gtest/gtest.h"
#include <cxpr.h>

//////////////////////////////////////////////////////////////////////////

TEST(tuple_tests, visit_tuple_test)
{
	{	// basic visit
		std::tuple<int, double, float> tt(10, 123.456, -69.0f);
		auto counter = 0;
		cxpr::visit_tuple([&](auto)
		{
			counter++;
		}, tt);
		EXPECT_EQ(counter, 3);
	}

	{	// rvalue visit
		std::tuple<const int, double&&, float&&> tt(10, 123.456, -69.0f);
		auto counter = 0;
		cxpr::visit_tuple([&](auto& val)
		{
			counter++;
		}, tt);
		EXPECT_EQ(counter, 3);
	}

	{	// basic visit
		std::tuple<int, double, float> tt(10, 123.456, -42.f);
		std::stringstream outStream;
		cxpr::visit_tuple([&](auto&& v)
		{
			outStream << v << " ";
		}, tt);

		EXPECT_EQ(outStream.str(), "10 123.456 -42 ");
	}

	{	// select each type one at a time
		std::tuple<int, double, std::string, const char*> tt(10, 123.456, "Test", nullptr);
		EXPECT_EQ(cxpr::first_match<int>(tt), 10);
		EXPECT_DOUBLE_EQ(cxpr::first_match<double>(tt), 123.456);
		EXPECT_EQ(cxpr::first_match<std::string>(tt), "Test");
		EXPECT_EQ(cxpr::first_match<const char*>(tt), nullptr);
	}

	{	// select, modify, & compare
		std::tuple<int, double, std::string, const char*> tt(10, 123.456, "Test", nullptr);
		EXPECT_EQ(cxpr::first_match<std::string>(tt), "Test");
		cxpr::first_match<std::string>(tt) = "modified";
		EXPECT_EQ(cxpr::first_match<std::string>(tt), "modified");
	}
}

//////////////////////////////////////////////////////////////////////////

TEST(tuple_tests, visit_tuple_pack_test)
{
	{	// basic visit
		std::tuple<int, double, float> tt(10, 123.456, -69.0f);
		auto res = cxpr::visit_tuple_capture([&](auto vv)
		{
			return vv;
		}, tt);
		EXPECT_EQ(std::tuple_size_v<decltype(res)>, 3);
	}

	{	// rvalue visit
		std::tuple<const int, double&&, float&&> tt(10, 123.456, -69.0f);
		auto counter = 0;
		cxpr::visit_tuple([&](auto& val)
		{
			counter++;
		}, tt);
		EXPECT_EQ(counter, 3);
	}

	{	// modify visit
		std::tuple<int, double&&, float&&> tt(10, 123.456, -69.0f);
		cxpr::visit_tuple([&](auto& val)
			{
				val++;
			}, tt);

		EXPECT_EQ(std::get<0>(tt), 11);
		EXPECT_DOUBLE_EQ(std::get<1>(tt), 124.456);
		EXPECT_FLOAT_EQ(std::get<2>(tt), -68.0f);
	}
}

template <typename inner_t>
struct mutator_test
{
	using inner_type = inner_t;
};

template <typename T>
using un_mutator = typename T::inner_type;

TEST(tuple_tests, unique_type_test)
{
	// everything below should just fail to compile if the test 'fails'
	std::tuple<int, double, double, char, std::string, std::string, double, char> tt;
	using uniq_params_t = cxpr::unique_types_t<int, double, double, char, std::string, std::string, double, char>;
	using uniq_tuple_t = cxpr::tuple_unique_t<decltype(tt)>;

	static_assert(std::is_same_v<cxpr::typeset<int, double, char, std::string>, uniq_params_t>, 
		"Unique failed on param pack");

	static_assert(std::is_same_v<cxpr::typeset<int, double, char, std::string>, uniq_tuple_t>,
		"Unique failed on tuple");

	using stripped_t = std::tuple<std::string&&, const int, const double*, int>;
	using stripped_res_t = cxpr::mutate_types_t<stripped_t, std::decay_t>;

	static_assert(std::is_same_v<cxpr::typeset<std::string, int, double const*, int>, stripped_res_t>,
		"Decay mutate failed");

	{	// nested tuple, collapse to one tuple will all types
		std::tuple<std::tuple<std::string>, cxpr::typeset<float, float>, std::tuple<unsigned int, double>> nested;
		using nested_collapsed_t = cxpr::collapse_nested_tuple_t<decltype(nested)>;

		static_assert(std::is_same_v<cxpr::typeset<std::string, float, float, unsigned int, double>, nested_collapsed_t>,
			"nested collapse failed");

		// now for fun, let's do a unique collapse in one horrible statement
		using nested_collapsed_uniq_t = cxpr::tuple_unique_t<cxpr::collapse_nested_tuple_t<decltype(nested)>>;

		static_assert(std::is_same_v<cxpr::typeset<std::string, float, unsigned int, double>, 
			nested_collapsed_uniq_t>,"nested collapse unique failed");
	}

	{ // mutate tests
		std::tuple<mutator_test<std::string>, mutator_test<double>, mutator_test<int>, mutator_test<std::string>> inner;
		using unmutated_t = cxpr::mutate_types_t<decltype(inner), un_mutator>;

		// we should have fetch the inner type from the outer mutate wrapper
		static_assert(std::is_same_v<cxpr::typeset<std::string, double, int, std::string>,
			unmutated_t>, "failed to mutate inner type");
	}
}

//////////////////////////////////////////////////////////////////////////

