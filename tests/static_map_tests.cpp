#include <iostream>

#include "gtest/gtest.h"
#include <cxpr.h>
#include "static_map.h"

//////////////////////////////////////////////////////////////////////////

TEST(static_map_tests, constructor_tests)
{
	{
		constexpr auto created = cxpr::make_static_map<char, double>(
				{ 
					{'e', 10.0},
					{'d', 11.1},
					{'c', 12.2},
					{'b', 13.3},
					{'a', 14.4}
				}
			);

		// verify that we can if constexpr with the value
		if constexpr (created.get_entry<'a'>().first)
		{
			// success
		}
		else
		{
			FAIL() << "compile-time constant check failed";
		}

		EXPECT_TRUE(created.get_entry<'a'>().first);
		EXPECT_TRUE(created.get_entry<'b'>().first);
		EXPECT_TRUE(created.get_entry<'c'>().first);
		EXPECT_TRUE(created.get_entry<'d'>().first);
		EXPECT_TRUE(created.get_entry<'e'>().first);

		EXPECT_TRUE(created.has_key('a'));
		EXPECT_TRUE(created.has_key('b'));
		EXPECT_TRUE(created.has_key('c'));
		EXPECT_TRUE(created.has_key('d'));
		EXPECT_TRUE(created.has_key('e'));
	}
}

TEST(static_map_tests, iterate_test)
{
	constexpr auto created = cxpr::make_static_map<char, double>(
		{
			{'e', 10.0},
			{'d', 11.1},
			{'c', 12.2},
			{'b', 13.3},
			{'a', 14.4}
		}
	);

	// should be in alphabetical order
	char start = 'a';
	double val = 14.4;
	for (auto& it : created)
	{
		EXPECT_EQ(it.first, start);
		EXPECT_DOUBLE_EQ(it.second, val);
		start++;
		val -= 1.1;
	}

}

template <typename data_t>
struct dispatcher_test
{
	using functor_t = void(*)(const void*, std::ostream& stream);

	static constexpr functor_t generateFunctor()
	{
		return [](const void* data, std::ostream& stream)
		{
			stream << *((data_t*)(data)) << " ";
		};
	}
};

TEST(static_map_tests, functor_test)
{
	using functor_t = typename dispatcher_test<int>::functor_t;
	// Building a constexpr jump tree here that can print values based on the type
	constexpr static auto created = cxpr::make_static_map<cxpr::hash_t, functor_t>(
		{
			{ cxpr::typehash_v<double>,		 dispatcher_test<double>::generateFunctor()},
			{ cxpr::typehash_v<int>,		 dispatcher_test<int>::generateFunctor()},
			{ cxpr::typehash_v<std::string>, dispatcher_test<std::string>::generateFunctor()},
			{ cxpr::typehash_v<float>,		 dispatcher_test<float>::generateFunctor()},
		}
	);

	// test data, the char types should be skipped as they don't have a handler in the map above
	const auto testData = std::make_tuple<int, double, float, char, std::string, std::string, float, std::string, char>(
		10, 12.34, -5.0f, 'b', "String1", "String2", -12.3f, "String3", 'a');

	std::string check = "10 12.34 -5 String1 String2 -12.3 String3 ";
	std::stringstream outStream;
	// walk the tuple and print out the types with handlers
	cxpr::visit_tuple([&](const auto& data) constexpr
	{
		using clean_type = std::decay_t<decltype(data)>;
		// this would be lovely, but no constexpr structured bindings yet :(
		//auto [success, entry] = created.get_entry<cxpr::typehash_v<clean_type>>();
		//if (success)
		//{
		//	entry->second(&data, outStream);
		//}
		constexpr auto entry = created.get_entry<cxpr::typehash_v<clean_type>>();
		if constexpr (entry.first)
		{
			std::invoke(*(entry.second), &data, outStream);
		}

	}, testData);

	EXPECT_EQ(check, outStream.str());
}

//////////////////////////////////////////////////////////////////////////


template <typename ... params_t>
struct table_fold_generator
{
	constexpr decltype (auto) operator()()
	{
		using functor_t = typename dispatcher_test<int>::functor_t;
		return cxpr::make_static_map<cxpr::hash_t, functor_t>(
			{
				{ cxpr::typehash_v<params_t>, dispatcher_test<params_t>::generateFunctor()  }...
			}
		);
	}
};

TEST(static_map_tests, functor_test_fold)
{
	// same as functor_test but using folds to generate the table
	using supported_types_t = cxpr::typeset<double, int, std::string, float>;
	constexpr static auto jump_table = cxpr::fold<table_fold_generator>(supported_types_t{});

	const auto testData = std::make_tuple<int, double, float, char, std::string, 
		std::string, float, std::string, char>(
		10, 12.34, -5.0f, 'b', "String1", "String2", -12.3f, "String3", 'a');

	std::string check = "10 12.34 -5 String1 String2 -12.3 String3 ";
	std::stringstream outStream;

	cxpr::visit_tuple([&](const auto& data) constexpr
	{
		using clean_type = std::decay_t<decltype(data)>;
		constexpr auto entry = jump_table.get_entry<cxpr::typehash_v<clean_type>>();
		if constexpr (entry.first)
		{
			std::invoke(*(entry.second), &data, outStream);
		}
	}, testData);

	EXPECT_EQ(check, outStream.str());
}