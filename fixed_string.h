#pragma once

//////////////////////////////////////////////////////////////////////////

namespace cxpr
{
	template <typename char_t>
	[[nodiscard]] constexpr char_t cx_tolower(char_t in) noexcept
	{
		static constexpr char_t offset = char_t('Z') - char_t('z');
		if ((in <= 'Z') && (in >= 'A'))
		{
			in -= offset;
		}

		return in;
	}

	template <typename char_t>
	[[nodiscard]] constexpr char_t cx_toupper(char_t in) noexcept
	{
		static constexpr char_t offset = char_t('Z') - char_t('z');
		if ((in <= 'z') && (in >= 'a'))
		{
			constexpr int offset = ('z' - 'Z');
			in -= offset;
		}

		return in;
	}

	[[nodiscard]] constexpr cxpr::hash_t HashStringInvariant(std::string_view in) noexcept
	{
		if (in.size() == 0)
		{
			return 0;
		}

		// djb2 hash by Dan Bernstein
		auto it = std::begin(in);
		const auto endIt = std::end(in);

		cxpr::hash_t hash = 5381;

		while (it != endIt)
		{
			// this is expected to overflow,  VS will still throw 4307 if it does though, known
			// VS issue that suppressing doesn't fix, fix in the future if they patch
			hash = ((hash << 5) + hash) + cx_tolower(*it++);
		}

		return hash;
	}

	//////////////////////////////////////////////////////////////////////////

	constexpr size_t cx_strlen(const char* in) noexcept
	{
		const char* ptr = in;
		size_t sz = 0;
		while (*ptr != '\0') { ptr++; }

		return ptr - in;
	}

	//////////////////////////////////////////////////////////////////////////

	struct no_transform
	{
		template <typename char_t>
		constexpr char_t operator()(char_t in) const noexcept { return in; }
	};

	struct lower_case
	{
		template <typename char_t>
		constexpr decltype(auto) operator()(char_t in) const noexcept
		{
			return cx_tolower(in);
		}
	};

	struct upper_case
	{
		template <typename char_t>
		constexpr decltype(auto) operator()(char_t in) const noexcept
		{
			return cx_toupper(in);
		}
	};

	//////////////////////////////////////////////////////////////////////////

	struct overrun_behavior_trunc {};
	struct overrun_behavior_throw {};

	//////////////////////////////////////////////////////////////////////////

	template <typename data_t,
		size_t max_sz,
		typename transform = no_transform,
		typename overrun_behavior = overrun_behavior_trunc>
		class basic_fixed_string
	{
	public:
		using traits_t = std::char_traits<data_t>;
		using container_t = std::array<data_t, max_sz>;
		using my_t = basic_fixed_string<data_t, max_sz, transform, overrun_behavior>;
		using value_type = typename container_t::value_type;
		using size_type = typename container_t::size_type;
		using difference_type = typename container_t::difference_type;
		using pointer = typename container_t::pointer;
		using const_pointer = typename container_t::const_pointer;
		using reference = typename container_t::reference;
		using const_reference = typename container_t::const_reference;
		using iterator = typename container_t::iterator;
		using const_iterator = typename container_t::const_iterator;
		using reverse_iterator = typename container_t::reverse_iterator;
		using const_reverse_iterator = typename container_t::const_reverse_iterator;

		static constexpr bool throw_on_overrun = std::is_same_v<overrun_behavior, overrun_behavior_throw>;
		static constexpr bool trunc_on_overrun = std::is_same_v<overrun_behavior, overrun_behavior_trunc>;

		constexpr basic_fixed_string()									noexcept : container{} {}
		constexpr basic_fixed_string(const basic_fixed_string& other) noexcept : container(other.container) {}
		constexpr basic_fixed_string(basic_fixed_string&& other)		noexcept : container(other.container) {}
		constexpr basic_fixed_string(const std::basic_string_view<data_t> str) noexcept : container{}
		{
			assign(str.begin(), str.cend());
		}

		constexpr const_pointer c_str() const noexcept { return &container[0]; }
		constexpr const_pointer data() const noexcept { return &container[0]; }
		[[nodiscard]] constexpr cxpr::hash_t hash_code() const noexcept { return HashStringInvariant(&container[0]); }
		[[nodiscard]] constexpr cxpr::hash_t hash()		 const noexcept { return HashStringInvariant(&container[0]); }
		[[nodiscard]] constexpr bool operator<(const my_t& other) const noexcept
		{
			const size_t sz = std::min(size(), other.size());
			return traits_t::compare(c_str(), other.c_str(), sz) < 0;
		}

		constexpr my_t& operator=(const my_t& other) noexcept
		{
			assign(other.begin(), other.end());
			return *this;
		}

		constexpr my_t& operator=(const std::string& newData) noexcept
		{
			assign(newData.begin(), newData.end());
			return *this;
		}

		constexpr my_t& operator=(const data_t* newData) noexcept
		{
			auto newLength = traits_t::length(newData);
			assign(newData, newData + newLength);
			return *this;
		}

		template <size_t max_sz,
			typename other_transform_t,
			typename other_overrun_t>
		constexpr my_t& operator=(
			const basic_fixed_string<data_t, max_sz, other_transform_t, other_overrun_t>& other) noexcept
		{
			assign(other.begin(), other.end());
			return *this;
		}

		constexpr void clear()			 noexcept { container.clear(); }
		constexpr decltype(auto) begin() noexcept { return container.begin(); }
		constexpr decltype(auto) begin() const noexcept { return container.begin(); }
		constexpr decltype(auto) end()	 noexcept { return container.end(); }
		constexpr decltype(auto) end()	 const noexcept { return container.end(); }
		constexpr size_t size()			 const noexcept { return traits_t::length(data()); }
		constexpr size_t capacity() const noexcept { return max_sz; }

		constexpr operator std::basic_string_view<data_t>() const
		{
			return std::basic_string_view<data_t>(data(), size());
		}

		constexpr bool operator==(const std::basic_string_view<data_t> r) const
		{
			bool equal = false;
			const auto rsize = r.size();
			if (size() == rsize)
			{
				equal = traits_t::compare(data(), r.data(), rsize) == 0;
			}

			return equal;
		}

	private:
		union
		{
			const data_t debug[max_sz];
			container_t container;
		};

		template <typename iterator_t>
		constexpr void assign(iterator_t it, iterator_t end)
		{
			auto distance = std::distance(it, end);

			if constexpr (trunc_on_overrun)
			{
				if (distance > max_sz)
				{
					end = it + max_sz;
				}

				std::transform(it, end, std::begin(container), transform());
				container[max_sz - 1] = '\0';
			}
			else // throw
			{
				if (distance > max_sz)
				{
					throw std::runtime_error("string too small for contents, aborting assignment");
				}
				else
				{
					std::transform(it, end, std::begin(container), transform());
				}
			}
		}
	};

	//////////////////////////////////////////////////////////////////////////

	template <size_t max_sz,
		typename transform = no_transform,
		typename overrun_behavior = overrun_behavior_trunc>
		using fixed_string = basic_fixed_string<char, max_sz, transform, overrun_behavior>;

	//////////////////////////////////////////////////////////////////////////

	template <size_t max_sz,
		typename transform = no_transform,
		typename overrun_behavior = overrun_behavior_trunc>
		using wfixed_string = basic_fixed_string<wchar_t, max_sz, transform, overrun_behavior>;

	//////////////////////////////////////////////////////////////////////////
	// Below make an fixed string large enough to contain the passed value, rounds to closest power of 2
	// ie: a 24 byte string will return a 32 bit fixed_string
	template <typename transform_t = no_transform, size_t n>
	constexpr decltype(auto) make_fixed_string(const char(&in)[n])
	{
		static constexpr size_t rounded = cxpr::round_pow_2_v<n>;
		return basic_fixed_string<char, rounded, transform_t>(in);
	}

	template <typename transform_t = no_transform, size_t n>
	constexpr decltype(auto) make_wfixed_string(const wchar_t(&in)[n])
	{
		static constexpr size_t rounded = cxpr::round_pow_2_v<n>;
		return basic_fixed_string<wchar_t, rounded, transform_t>(in);
	}

	//////////////////////////////////////////////////////////////////////////

	constexpr decltype(auto) operator"" _fstr32(const char* n, size_t sz)
	{
		return fixed_string<32>(n);
	}

	constexpr decltype(auto) operator"" _fstr256(const char* n, size_t sz)
	{
		return fixed_string<256>(n);
	}

	constexpr cxpr::hash_t operator"" _hash(const char* n, size_t sz)
	{
		return HashStringInvariant(n);
	}
}
