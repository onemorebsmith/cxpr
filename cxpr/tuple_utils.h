#pragma once

#include <type_traits>
#include <tuple>
#include <utility>

//////////////////////////////////////////////////////////////////////////

namespace cxpr
{
	// NOTE: Anything below that works with typeset will work with tuple also

	// simple type that collects param types without all the overhead/expansion of tuple
	template <typename ... ts>
	struct typeset {};

	//////////////////////////////////////////////////////////////////////////
	// Calls a functor with the passed params as template args and returns the result.
	// An example functor is below
	//
	// template <typename ... params_t>
	// struct example_fold_function { constexpr decltype (auto) operator()() {...} };
 	//
	// call site:
	//	 constexpr auto res = cxpr::fold<example_fold_function>(cxpr::typeset<...>{})
	//
	template<template<typename ...> class folder_t, template<typename ...> typename tuple_t,
		typename ... params_t>
	constexpr decltype(auto) fold(tuple_t<params_t...>) noexcept
	{
		return folder_t<params_t...>{}();
	}

	/////////////////////////////////////////////////////////////////////////
	// visit_tuple
	// Calls functor_t with each tuple member (for each over the tuple)
	template<typename functor_t, typename tuple_t, std::size_t N>
	constexpr decltype(auto) visit_tuple(functor_t fun, tuple_t&& tt)
	{
		fun(std::forward<decltype(std::get<N>(tt))>(std::get<N>(tt)));
		if constexpr (N < (std::tuple_size_v<std::decay_t<tuple_t>> -1))
		{
			visit_tuple<functor_t, tuple_t, N + 1>(fun, tt);
		}
	}

	template<typename functor_t, typename tuple_t>
	constexpr decltype(auto) visit_tuple(functor_t fun, tuple_t&& tt)
	{
		return visit_tuple<functor_t, tuple_t, 0>(fun, tt);
	}

	namespace __detail
	{
		template <typename type_t, typename tuple_t, std::size_t N>
		constexpr decltype(auto) _find_tuple_type(tuple_t&& tt) noexcept
		{
			if constexpr (std::is_same_v<type_t, std::decay_t<decltype(std::get<N>(tt))>>)
			{
				return std::get<N>(tt);
			}
			else
			{
				if constexpr (N < (std::tuple_size_v<std::decay_t<tuple_t>> -1))
				{
					return _find_tuple_type<type_t, tuple_t, N + 1>(tt);
				}
				else
				{
					//static_assert(false, "Tuple does not contain type");
					throw std::logic_error{ "Tuple does not contain type" }; // not real, this will stop the compile
				}
			}
		}
	}

	/////////////////////////////////////////////////////////////////////////
	// first_match
	// Returns the first type in the tuple that exactly matches type_t (internal tuple type will be decayed)
	// If no types match, this will force a compiler error
	template<typename type_t, typename tuple_t>
	constexpr decltype(auto) first_match(tuple_t&& tt) noexcept
	{
		return __detail::_find_tuple_type<type_t, tuple_t, 0>(tt);
	}

	namespace __detail
	{
		template<typename functor_t, typename tuple_t, typename ret_t, std::size_t N>
		constexpr decltype(auto) _visit_tuple_capture(functor_t fun, tuple_t&& tt, ret_t&& ret)
		{
			auto&& result = fun(std::forward<decltype(std::get<N>(tt))>(std::get<N>(tt)));
			auto wrapped = std::tuple_cat(std::move(ret), std::tie(result));
			if constexpr (N < (std::tuple_size_v<std::decay_t<tuple_t>> -1))
			{
				return _visit_tuple_capture<functor_t, tuple_t, decltype(wrapped), N + 1>(fun, tt, std::move(wrapped));
			}
			else
			{
				return wrapped;
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// visit_tuple_capture
	// Visits every member of the tuple and returns a tuple representing the result of each invocation
	template<typename functor_t, typename tuple_t>
	constexpr decltype(auto) visit_tuple_capture(functor_t fun, tuple_t&& tt)
	{
		std::tuple<> ret = {};
		return __detail::_visit_tuple_capture<functor_t, tuple_t, decltype(ret), 0>(fun, tt, std::move(ret));
	}

	struct starter_marker {};
	using type_collector = typeset<starter_marker>;

	template<template<typename ...> class typeset_t,
		template<typename ...> class right_t,
		typename ... collapsed_ts,
		typename ... right_ts>
		decltype(auto) operator<<(const typeset_t<collapsed_ts...>, const right_t<right_ts...>*)
	{
		if constexpr (std::is_same_v<typeset_t<collapsed_ts...>, type_collector>)
		{	// collapsed_ts is just the marker type, ignore it and just take the right side
			return typeset<right_ts...>{};
		}
		else
		{
			// we're rolling now, take both sides 
			return typeset<collapsed_ts..., right_ts...>{};
		}
	}

	//////////////////////////////////////////////////////////////////////////

	namespace __detail
	{
		template <typename ... Ts>
		struct unique_types;

		template <typename curr_t, typename ... Ts>
		struct unique_types<curr_t, Ts...>
		{
			template <typename ... out_ts>
			static constexpr decltype(auto) iterate(typeset<out_ts...>* output = nullptr)
			{
				// check if any types are the same as the current type, if so skip it 
				constexpr bool type_exists = (... || std::is_same_v<std::decay<curr_t>, std::decay<out_ts>>);
				if constexpr (type_exists == false)
				{
					return unique_types<Ts...>::iterate((typeset<out_ts..., curr_t>*)0);
				}
				else
				{
					return unique_types<Ts...>::iterate((typeset<out_ts...>*)0);
				}
			}

			static constexpr decltype(auto) iterate()
			{
				return unique_types<Ts...>::iterate((typeset<curr_t>*)0);
			}
		};

		template <>
		struct unique_types<>
		{
			// end of expansion, return the wrapped types
			template <typename ... out_ts>
			static constexpr decltype(auto) iterate(typeset<out_ts...>* output = nullptr)
			{
				return typeset<out_ts...>();
			}
		};

		template<template<typename ...> class tuple_t, typename ... types_t>
		constexpr decltype(auto) tuple_unique(const tuple_t<types_t...>* tt = nullptr)
		{
			return unique_types<types_t...>::iterate();
		}

		template<template<typename ...> class wrapper_t, typename ... types_t>
		constexpr decltype(auto) pack_size(const wrapper_t<types_t...>* tt = nullptr)
		{
			return sizeof...(types_t);
		}

		template<template<typename ...> class mutator_t, 
			     template<typename ...> class wrapper_t, 
			     typename ... types_t>
		constexpr decltype(auto) mutate_types(const wrapper_t<types_t...>* tt = nullptr)
		{
			return typeset<mutator_t<types_t>...>{};
		}


		

		template <typename ... wrapped_types_t>
		constexpr decltype(auto) collapse_types(wrapped_types_t*... types)
		{
			return (type_collector{} << ... << types);
		}

		template<template<typename ...> class wrapper_t, typename ... types_t>
		constexpr decltype(auto) collapse_nested_tuple(const wrapper_t<types_t...>* tt = nullptr)
		{
			return decltype(collapse_types((types_t*)(0)...)){};
		}
	}

	// The following decomposes a pack of types into the unique components (aka removes duplicate types).
	// The implementation decays types before comparison, so int& and int will be discarded to just int
	template <typename ... types_t>
	using unique_types_t = decltype(__detail::unique_types<types_t...>::iterate());

	// The following decompose a tuple into it's unique components (aka removes duplicate types).
	// The implementation decays types before comparison, so int& and int will be discarded to just int
	template<typename tuple_t>
	using tuple_unique_t = decltype(__detail::tuple_unique((tuple_t*)0));

	// The following decompose a tuple into it's unique components (aka removes duplicate types).
	// The implementation decays types before comparison, so int& and int will be discarded to just int
	template<typename tuple_t, template<typename ...> class mutator_t>
	using mutate_types_t = decltype(__detail::mutate_types<mutator_t>((tuple_t*)0));

	// collapses a param pack full of tuples to one typeset
	// ie: tuple<int, double>, tuple<string, char> -> tuple<int, double, string, char>
	template<typename ... tuple_t>
	using collapse_tuples_t = decltype(__detail::collapse_types<tuple_t...>((tuple_t*)(0)...));

	// collapses a tuple full of tuples to one typeset
	// ie: tuple<tuple<int, double>, tuple<string, char>> -> tuple<int, double, string, char>
	template<typename tuple_t>
	using collapse_nested_tuple_t = decltype(__detail::collapse_nested_tuple((tuple_t*)(0)));

	// returns the number of types in the tuple
	// ie: tuple<tuple<int, double> -> 2
	template<typename tuple_t>
	static constexpr size_t param_count_v = __detail::pack_size((tuple_t*)0);
}