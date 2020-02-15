#pragma once

namespace cxpr
{
	// Dumping ground for algorithms that the stl doesn't currently define as constexpr, typically just a copy/paste
	// of the implementations from cppreference.com
	
	template<class InputIterator, class UnaryPredicate>
	[[nodiscard]] constexpr InputIterator find_if(InputIterator first, InputIterator last, UnaryPredicate pred)
	{
		while (first != last) 
		{
			if (pred(*first)) return first;
			++first;
		}
		return last;
	}

	struct less
	{
		constexpr less() noexcept = default;

		template <typename T>
		constexpr bool operator()(const T& t1, const T& t2) const
		{
			return t1 < t2;
		}

		template <typename T1, typename T2>
		constexpr bool operator()(const T1& t1, const T2& t2) const
		{
			return t1 < t2;
		}

		template <typename K, typename V>
		constexpr bool operator()(const cxpr::static_pair<K, V>& t1, const cxpr::static_pair<K, V>& t2) const
		{
			return t1.first < t2.first;
		}
	};

	template <typename T>
	constexpr void swap(T& t1, T& t2)
	{
		if constexpr (std::is_move_assignable_v<T>)
		{
			T temp = std::move(t1);
			t1 = std::move(t2);
			t2 = std::move(temp);
		}
		else
		{
			static_assert(false, "non-cxpr-swappable");
		}
	}

	//////////////////////////////////////////////////////////////////////////
	/* Function to sort an array using insertion sort*/
	template<class it1, typename it2>
	constexpr void copy(it1 first, it1 last, it2 outIt)
	{
		while (first != last)
		{
			*outIt = *first;
			++first;
			++outIt;
		}

	}

	//////////////////////////////////////////////////////////////////////////
	/* Function to sort an array using insertion sort*/
	template<class randIt_t, typename pred_t>
	constexpr void sort(randIt_t first, randIt_t last, pred_t pred)
	{
		// lazy insertion sort, O(n^2)
		if (first > last) { return; }

		while (first != last)
		{
			auto it = first;
			while (++it != last)
			{
				if (pred(*it, *first))
				{
					cxpr::swap(*first, *it);
				}
			}
			++first;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	template<class ForwardIt, class T>
	[[nodiscard]] constexpr ForwardIt lower_bound(ForwardIt first, ForwardIt last, const T& value)
	{
		ForwardIt it = {};
		auto count = std::distance(first, last);
		while (count > 0) 
		{
			auto it = first;
			const auto step = count / 2;
			std::advance(it, step);
			if (*it < value) 
			{
				first = ++it;
				count -= step + 1;
			}
			else
			{
				count = step;
			}
		}
		return first;
	}

	constexpr uint32_t fast_log2(uint32_t v) noexcept // find the log base 2 of 32-bit v
	{
		constexpr const int MultiplyDeBruijnBitPosition[32] =
		{
		  0, 9, 1, 10, 13, 21, 2, 29, 11, 14, 16, 18, 22, 25, 3, 30,
		  8, 12, 20, 28, 15, 17, 24, 7, 19, 27, 23, 6, 26, 5, 4, 31
		};

		v |= v >> 1; // first round down to one less than a power of 2 
		v |= v >> 2;
		v |= v >> 4;
		v |= v >> 8;
		v |= v >> 16;

		return MultiplyDeBruijnBitPosition[(uint32_t)(v * 0x07C4ACDDU) >> 27];
	}

	//////////////////////////////////////////////////////////////////////////
	// Rounds to the next highest power of 2
	constexpr uint32_t round_base2(uint32_t v) noexcept // find the log base 2 of 32-bit v
	{
		v--;
		v |= v >> 1;
		v |= v >> 2;
		v |= v >> 4;
		v |= v >> 8;
		v |= v >> 16;
		v++;
		return v;
	}

	//////////////////////////////////////////////////////////////////////////
	// Rounds to the next closest power of 2 and returns that number
	// ie: 24 returns 32
	template <size_t val>
	static constexpr size_t round_pow_2_v = round_base2(val);

	//////////////////////////////////////////////////////////////////////////
	// Rounds to the next closest power of 2 then returns the log2 of that value
	// ie: 24 -> (rounds) 32 -> returns 5
	template <size_t val>
	static constexpr size_t log2_v = fast_log2(round_pow_2_v<val>);
}