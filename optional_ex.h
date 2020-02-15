#pragma once

#include <optional>

//////////////////////////////////////////////////////////////////////////

namespace cxpr
{
	template <typename T>
	struct optional_ex;

	//////////////////////////////////////////////////////////////////////////

	namespace __impl
	{
		template <template <typename...> class C, typename...Ts>
		constexpr bool __is_optional(const C<Ts...>*) 
		{
			return std::is_same_v<optional_ex<Ts...>, C<Ts...>>;
		}

		constexpr bool __is_optional(...) { return false; }
	}

	//////////////////////////////////////////////////////////////////////////

	template <typename T>
	constexpr bool is_optional_v() { return __impl::__is_optional((T*)0); }

	//////////////////////////////////////////////////////////////////////////

	template <typename T>
	using add_optional_t = std::conditional_t<is_optional_v<T>(), T, optional_ex<T>>;

	//////////////////////////////////////////////////////////////////////////

	template <typename T>
	struct optional_ex : public std::optional<T>
	{
		using std::optional<T>::optional;

		template <typename fun_t>
		constexpr decltype(auto) apply(fun_t&& fun) noexcept
		{
			using ret_t = add_optional_t<std::invoke_result_t<fun_t, T>>;
			if (this->has_value())
			{
				return ret_t{ std::invoke(fun, this->value()) };
			}

			return ret_t{};
		}

		template <typename fun_t>
		constexpr decltype(auto) and_then(fun_t&& fun) noexcept 	// call functor has value
		{
			return apply(std::forward<fun_t>(fun));
		}

		template <typename fun_t>
		constexpr decltype(auto) or_else(fun_t&& fun) noexcept	// call functor if no value
		{
			if (this->has_value() == false)
			{
				fun();
			}

			return *this;
		}
	};
}