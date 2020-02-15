#pragma once
#include <array>

//////////////////////////////////////////////////////////////////////////

namespace cxpr
{
	template  <typename T, size_t max_sz>
	class fixed_vector
	{
	public:
		using data_t = T;
		using my_t = fixed_vector<T, max_sz>;
		using container_t = std::array<T, max_sz>;

		// forwards from the internal type
		using value_type		= typename container_t::value_type;
		using size_type			= typename container_t::size_type;
		using difference_type	= typename container_t::difference_type;
		using pointer			= typename container_t::pointer;
		using const_pointer		= typename container_t::const_pointer;
		using reference			= typename container_t::reference;
		using const_reference	= typename container_t::const_reference;

		using iterator			= typename container_t::iterator;
		using const_iterator	= typename container_t::const_iterator;

		using reverse_iterator	= typename container_t::reverse_iterator;
		using const_reverse_iterator = typename container_t::const_reverse_iterator;

		constexpr fixed_vector()							: currentSz{ 0 }, mem{} {};
		constexpr fixed_vector(const fixed_vector& other) : currentSz(other.currentSz) { mem = other.mem; };
 		constexpr fixed_vector(fixed_vector&& other)	
			: currentSz(other.currentSz), mem{ std::move(other.mem) } {};

		constexpr fixed_vector(const T(&in)[max_sz]) noexcept : fixed_vector()
		{
			auto ptr = std::begin(in);
			const auto endPtr = std::end(in);
			while (ptr != endPtr)
			{
				push_back(*ptr);
				ptr++;
			}
		}

		constexpr void push_back(T val) 
		{
			if (currentSz < max_sz)
			{
				mem[currentSz++] = std::move(val);
			}
			else
			{
				throw std::out_of_range("static_vector::push_back out of range");
			}
		}

		template <typename ... params_t>
		constexpr T& emplace_back(param_pack_t params)
		{
			if (currentSz < max_sz)
			{
				new (& mem[currentSz++]) T(perfect_forward(params));
				return mem[currentSz - 1]; // todo: atomic
			}

			throw std::out_of_range("static_vector::emplace_back out of range");
		}

		constexpr const_reference operator[](size_t idx) const  { return mem[idx]; }
		constexpr reference		  operator[](size_t idx)		{ return mem[idx]; }

		constexpr void clear() noexcept 
		{ 
			if constexpr(std::is_trivially_destructible_v<T>)
			{
				currentSz = 0;
			}
			else
			{
				for (size_t i = 0; i < currentSz; i++)
				{
					mem[i].~T();
					new(&mem[i]) T();
				}				

				currentSz = 0;
			}
		}
		constexpr iterator		 begin() noexcept		{ return mem.begin(); }
		constexpr const_iterator begin() const noexcept	{ return mem.begin(); }
		constexpr iterator		 end()   noexcept		{ return mem.begin() + currentSz; }
		constexpr const_iterator end()   const noexcept	{ return mem.begin() + currentSz; }
		constexpr size_t		size()	 const noexcept { return currentSz; }
		constexpr bool	   saturated()   const noexcept { return currentSz >= max_sz; }

	protected:
		size_t currentSz;

		std::array<T, max_sz> mem;
	};

	template <typename T, size_t n>
	constexpr decltype(auto) make_static_vector(const T(&in)[n])
	{
		return fixed_vector<T, n>(in);
	}
}