#pragma once

//////////////////////////////////////////////////////////////////////////

namespace cxpr
{
	template  <typename T1, typename T2>
	class static_pair
	{
	public:
		using my_t = static_pair<T1, T2>;

		constexpr static_pair() : first{}, second{} {};
		constexpr static_pair(const T1& t1, const T2& t2) : first(t1), second(t2) {}

		template <typename _t1, typename _t2>
		constexpr static_pair(const _t1& _first, const _t2& _second) : first(_first), second(_second) {}
		constexpr static_pair(const my_t& other) : first(other.first), second(other.second) {}
		constexpr static_pair(my_t&& other) : first(std::move(other.first)), second(std::move(other.second)) {}

		constexpr my_t& operator=(const my_t&& other)
		{
			first  = std::move(other.first);
			second = std::move(other.second);
			return *this;
		}

		constexpr my_t& operator=(const my_t& other)
		{
			first  = other.first;
			second = other.second;
			return *this;
		}


		T1 first;
		T2 second;

	protected:
		
	};

	template  <typename T1, typename T2>
	[[nodiscard]] constexpr bool operator<(const static_pair<T1, T2>& l, const static_pair<T1, T2>& r)
	{
		return l.first < r.first;
	}

	//////////////////////////////////////////////////////////////////////////

	template <typename T1, typename T2>
	constexpr decltype(auto) make_pair(const T1& t1, const T2& t2)
	{
		return static_pair<T1, T2>(t1, t2);
	}
}