#pragma once

//////////////////////////////////////////////////////////////////////////

namespace cxpr
{
	namespace __detail
	{
		template<typename T, size_t N>
		constexpr decltype(auto) __std_array_size(const std::array<T, N>*) noexcept { return N; }
	}

	// helper for std::array
	template <typename T, typename ... params_t>
	constexpr decltype(auto) make_std_array(param_pack_t params)
	{
		return std::array<T, sizeof...(params_t)> { perfect_forward(params) };
	}

	template<typename T, size_t N>
	constexpr decltype(auto) std_array_size(const std::array<T, N>&) noexcept { return N; }


	template<typename T>
	constexpr decltype(auto) std_array_size(const T* a = nullptr) noexcept
	{
		return __impl::__std_array_size(a);
	}


	template<typename ... Ts>
	constexpr decltype(auto) std_array_size_sum() noexcept
	{
		return (std_array_size<std::decay_t<Ts>>() + ...);
	}


	template <typename out_t, typename T>
	static constexpr void __fill(size_t& outIdx, out_t& outArray, T&& src)
	{
		for (size_t i = 0; i < src.size(); i++)
		{
			outArray[outIdx] = std::move(src[i]);
			outIdx++;
		}
	}

	template <typename out_t, typename T, typename ... Ts>
	static constexpr void __fill(size_t& outIdx, out_t& outArray, T&& src, Ts&&... params)
	{
		for (size_t i = 0; i < src.size(); i++)
		{
			outArray[outIdx] = std::move(src[i]);
			outIdx++;
		}
		__fill(outIdx, outArray, perfect_forward(params));
	}

	template<typename ... Ts>
	constexpr decltype(auto) std_array_cat(Ts&&... data) noexcept
	{
		constexpr auto sz = (cxpr::std_array_size_sum<decltype(data)...>());
		using value_t = typename _pop_first_type<Ts...>::type;

		std::array<typename value_t::value_type, sz> ret = {};

		size_t outIdx = 0;
		__fill(outIdx, ret, std::move(data)...);

		return ret;
	}
}