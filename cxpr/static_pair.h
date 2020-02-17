#pragma once

//////////////////////////////////////////////////////////////////////////

namespace cxpr
{
	// Constexpr-friend replacement for std::pair, as std::pair is not currently constexpr in the standard.
	// Implements only what is need for other functions in this library. Remove this when pair is constexpr
	template  <typename T1, typename T2>
	class static_pair
	{
	public:
		using my_t = static_pair<T1, T2>;

		constexpr static_pair()
			noexcept(std::is_nothrow_default_constructible_v<T1> && std::is_nothrow_default_constructible_v<T2>) 
			: first{}, second{} {};
		constexpr static_pair(const T1& t1, const T2& t2)
			noexcept(std::is_nothrow_copy_constructible_v<T1>&& std::is_nothrow_copy_constructible_v<T2>)
			: first(t1), second(t2) {}

		template <typename _t1, typename _t2>
		constexpr static_pair(_t1&& _first, _t2&& _second) 
			noexcept(std::is_nothrow_constructible_v<T1> && std::is_nothrow_constructible_v<T2>)
			: first(std::forward<_t1>(_first)), second(std::forward<_t2>(_second)) {}

		constexpr static_pair(const my_t& other) : first(other.first), second(other.second) {}

		constexpr static_pair(my_t&& other)
			noexcept(std::is_nothrow_move_constructible_v<T1>&& std::is_nothrow_move_constructible_v<T2>) 
			: first(std::move(other.first)), second(std::move(other.second)) {}

		constexpr my_t& operator=(my_t&& other)
			noexcept(std::is_nothrow_move_assignable_v<T1> && std::is_nothrow_move_assignable_v<T2>)
		{
			first  = std::move(other.first);
			second = std::move(other.second);
			return *this;
		}

		constexpr my_t& operator=(const my_t& other)
			noexcept(std::is_nothrow_assignable_v<T1, T1>&& std::is_nothrow_assignable_v<T2, T2>)
		{
			first  = other.first;
			second = other.second;
			return *this;
		}

		T1 first;
		T2 second;
	};

	template  <typename T1, typename T2>
	[[nodiscard]] constexpr bool operator<(const static_pair<T1, T2>& l, const static_pair<T1, T2>& r)
	{
		return l.first < r.first;
	}

	//////////////////////////////////////////////////////////////////////////

	template <typename T1, typename T2>
	constexpr decltype(auto) make_pair(T1&& t1, T2&& t2)
	{
		return static_pair<T1, T2>(t1, t2);
	}
}