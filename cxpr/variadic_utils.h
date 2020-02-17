#pragma once

//////////////////////////////////////////////////////////////////////////

namespace cxpr
{
	template <typename ... Ts>
	struct _pop_first_type;

	template <typename T, typename ... Ts>
	struct _pop_first_type<T, Ts...>
	{
		using type = std::decay_t<T>;
	};

	template <typename ... Ts>
	struct max_size;

	template <typename T, typename ... Ts>
	struct max_size<T, Ts...>
	{
		static constexpr size_t value = std::max(sizeof(T), max_size<Ts...>::value);
	};

	template <typename T>
	struct max_size<T>
	{
		static constexpr size_t value = sizeof(T);
	};

	template <typename ... Ts>
	struct min_size;

	template <typename T, typename ... Ts>
	struct min_size<T, Ts...>
	{
		static constexpr size_t value = std::min(sizeof(T), min_size<Ts...>::value);
	};

	template <typename T>
	struct min_size<T>
	{
		static constexpr size_t value = sizeof(T);
	};
}