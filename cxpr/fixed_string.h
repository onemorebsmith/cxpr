#pragma once

//////////////////////////////////////////////////////////////////////////

namespace cxpr
{
	namespace __detail
	{
		template <typename char_t>
		static constexpr char_t offset_v = char_t('Z') - char_t('z');
	}

	template <typename data_t,
		size_t max_sz,
		typename transform,
		typename overrun_behavior>
		class basic_fixed_string;

	// transforms

	template <typename char_t>
	[[nodiscard]] constexpr char_t cx_tolower(char_t in) noexcept
	{
		if ((in <= 'Z') && (in >= 'A'))
		{
			in -= __detail::offset_v<char_t>;
		}

		return in;
	}

	template <typename char_t>
	[[nodiscard]] constexpr char_t cx_toupper(char_t in) noexcept
	{
		if ((in <= 'z') && (in >= 'a'))
		{
			in += __detail::offset_v<char_t>;
		}

		return in;
	}


	template <typename out_t>
	[[nodiscard]] constexpr decltype(auto) to_lower(std::string_view in) noexcept
	{
		out_t ret{};
		std::transform(std::begin(in), std::end(in), std::back_inserter(ret), std::tolower);
		return ret;
	}

	template <typename data_t,
		size_t max_sz,
		typename transform,
		typename overrun_behavior>
		[[nodiscard]] constexpr decltype(auto) to_lower(const basic_fixed_string<data_t, max_sz, transform, overrun_behavior>& in) noexcept
	{
		basic_fixed_string<data_t, max_sz, transform, overrun_behavior> ret{};
		std::transform(std::begin(in), std::end(in), std::back_inserter(ret), std::tolower);
		return ret;
	}

	[[nodiscard]] constexpr cxpr::hash_t hash_invariant(std::string_view in) noexcept
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
	// AFAIK these aren't defined anywhere in the stl, that'd sure be nice though
	template <typename data_t> struct str_terminator;
	template <> struct str_terminator<char> { static constexpr char value() { return '\0'; } };
	template <> struct str_terminator<wchar_t> { static constexpr wchar_t value() { return L'\0'; } };
	template <> struct str_terminator<char16_t> { static constexpr wchar_t value() { return char16_t{}; } }; // is this right?

	//////////////////////////////////////////////////////////////////////////
	// Implementation of a fixed-size string buffer. 
	// The contained string will always be null-terminated and can be [0, max_capacity) in length
	// The string is guaranteed to be contiguous in memory and uses std::array as the underlying container
	// For small strings (ie the size can be indexed by a single data_t) the implementation will use
	// indexing for push_back/emplace/size/etc. Larger strings will function similar to a const char* and
	// use a linear search for the the terminator character to determine size 
	template <typename data_t,
		size_t max_capacity,
		typename transform = no_transform,
		typename overrun_behavior = overrun_behavior_trunc>
		class basic_fixed_string
	{
	public:
		// -1 since it doesn't contain the null
		static constexpr size_t max_sz = max_capacity - 1;

		using traits_t = std::char_traits<data_t>;
		using container_t = std::array<data_t, max_sz>;
		using my_t = basic_fixed_string<data_t, max_capacity, transform, overrun_behavior>;
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

		static constexpr bool throw_on_overrun = std::is_same_v<std::decay_t<overrun_behavior>, overrun_behavior_throw>;
		static constexpr bool trunc_on_overrun = std::is_same_v<std::decay_t<overrun_behavior> , overrun_behavior_trunc>;
		static constexpr data_t terminator_value = str_terminator<data_t>::value();

		// If 'use_emplace_indexed' is true, then the final character will be used as a 1-based index into the container
		// This eliminates linear searching for size/emplace/push_back and replaces them with the approprate indexed operations
		// The index is 1-base as opposed to 0-base b/c a size of 0 would also indicate the terminated value for char '\0'
		static constexpr bool use_emplace_indexed = std::numeric_limits<data_t>::max() >= max_capacity;

		constexpr basic_fixed_string()					noexcept : container{}, terminator{terminator_value}{
			if constexpr (use_emplace_indexed) { terminator = 1; }
		}
		constexpr basic_fixed_string(const my_t& other) noexcept : container(other.container), terminator{ other.terminator } {}
		constexpr basic_fixed_string(my_t&& other)		noexcept : container(other.container), terminator{ other.terminator } {}
		constexpr basic_fixed_string(const std::basic_string_view<data_t> str) noexcept : container{}
		{
			assign(str.begin(), str.cend());
		}

		constexpr const_pointer c_str() const noexcept { return &container[0]; }
		constexpr const_pointer data() const noexcept { return &container[0]; }
		[[nodiscard]] constexpr cxpr::hash_t hash_code() const noexcept { return hash_invariant(&container[0]); }
		[[nodiscard]] constexpr cxpr::hash_t hash()		 const noexcept { return hash_invariant(&container[0]); }
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
			assign(other.begin(), other.begin() + other.size());
			return *this;
		}

		constexpr decltype(auto) begin() noexcept { return container.begin(); }
		constexpr decltype(auto) begin() const noexcept { return container.begin(); }
		constexpr decltype(auto) end()	 noexcept { return container.end() + size(); }
		constexpr decltype(auto) end()	 const noexcept { return container.begin() + size(); }
		constexpr size_t capacity() const noexcept { return max_sz; }
		constexpr size_t size()			 const noexcept { 
			if constexpr (use_emplace_indexed) {
				if (terminator == terminator_value){
					return max_sz;
				}
				return static_cast<size_t>(terminator) - 1;
			}
			else
			{
				return traits_t::length(data());
			}
		}
		constexpr void clear() {
			for (auto& it : container) {
				it = terminator_value;
			}
			if constexpr (use_emplace_indexed) {
				terminator = 1;
			}
		}

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

		void push_back(const data_t& _Val) {
			emplace_back(_Val);
		}

		void push_back(data_t&& _Val) {
			emplace_back(_STD move(_Val));
		}

		template <class val_t>
		void emplace_back_indexed(val_t&& _Val) {
			if (terminator == terminator_value){
				if constexpr (throw_on_overrun) {
					throw std::runtime_error("string too small for contents, aborting assignment");
				}

				// pools closed, go home
				return;
			}

			// indexed
			container[static_cast<size_t>(terminator) -1] = std::move(_Val);
			terminator++;
			if (terminator >= max_capacity){
				terminator = terminator_value;
			}
		}

		template <class val_t>
		void emplace_back(val_t&& _Val) {

			if constexpr (use_emplace_indexed) {
				// we're small enough to use the end value as an index
				emplace_back_indexed(std::move(_Val));
			}
			else {	// traditional iterate and check vs null terminator
				auto it = &container[0];
				auto endIt = &terminator;
				while(it != endIt){
					if (*it == terminator){
						*it = std::move(_Val);
						// overrun the end iterator is fine since it's a data_t and already '\0'
						(++it) = terminator_value;
						break;
					}
					++it;
				}

				if constexpr (throw_on_overrun) {
					if (it == endIt) {
						// failed to set
						throw std::runtime_error("string too small for contents, aborting assignment");
					}
				}
			}
		}

	private:
		// Memory layout, we don't expose the terminator value to the user and forcefully set it during creation
		// this is possible b/c the end value can only ever be '\0' due to strings always having to be null terminated
		union
		{
			const data_t debug[max_sz - 1];
			container_t container;
		};
		data_t terminator = terminator_value; // not const since we might index with it

		template <typename iterator_t>
		constexpr void assign(iterator_t it, iterator_t end)
		{
			clear();
			auto distance = std::distance(it, end);

			if constexpr (trunc_on_overrun)
			{
				if (distance > max_sz)
				{
					end = it + max_sz;
				}

				auto containerOut = begin();
				while (it != end)
				{
					*containerOut++ = transform{}(*it++);
				}

				if constexpr (use_emplace_indexed) {
					const auto newLen = std::min(static_cast<size_t>(distance), static_cast<size_t>(max_sz));
					terminator = static_cast<decltype(terminator)>(newLen) + 1; // index can't start at 0 since that's '\0'
					if (distance >= max_sz){
						terminator = terminator_value;
					}
				}
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
		return basic_fixed_string<char, cxpr::round_pow_2_v<n>, transform_t>(in);
	}

	template <typename transform_t = no_transform, size_t n>
	constexpr decltype(auto) make_wfixed_string(const wchar_t(&in)[n])
	{
		return basic_fixed_string<wchar_t, cxpr::round_pow_2_v<n>, transform_t>(in);
	}

	//////////////////////////////////////////////////////////////////////////

	constexpr decltype(auto) operator"" _fixed32(const char* n, size_t sz)
	{
		return fixed_string<32>(n);
	}

	constexpr decltype(auto) operator"" _fixed256(const char* n, size_t sz)
	{
		return fixed_string<256>(n);
	}

	constexpr cxpr::hash_t operator"" _hash(const char* n, size_t sz)
	{
		return hash_invariant(n);
	}

	//////////////////////////////////////////////////////////////////////////

	static constexpr char* marker_str = "marker_string is valid marker_string is valid";
	static constexpr size_t marker_str_len = cxpr::cx_strlen(marker_str);
	template <size_t max_sz>
	class marker_string
	{
	public:
		static constexpr fixed_string<max_sz> marker_data
			= { std::string_view(marker_str, std::min(max_sz, marker_str_len)) };

		constexpr marker_string() noexcept : str(marker_data) {}
		constexpr bool isValid() const { return str == marker_data; }
		constexpr void reset() { str = marker_data; }

	private:
		fixed_string<max_sz> str;
	};

	template <typename ... T>
	std::ostream& operator<<(std::ostream& os, const basic_fixed_string<T...>& str)
	{
		os << "test"
		return os;
	}
}
