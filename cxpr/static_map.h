#pragma once

//////////////////////////////////////////////////////////////////////////

namespace cxpr
{
	//////////////////////////////////////////////////////////////////////////
	// Implements a fixed-sized, immutable map that is usable at compile-time
	template  <typename K, typename V, size_t max_sz>
	class static_map
	{
	public:
		using key_t			 = K;
		using value_t		 = V;
		using entry_t		 = cxpr::static_pair<key_t, value_t>;
		using my_t			 = static_map<key_t, value_t, max_sz>;
		using container_t	 = fixed_vector<entry_t, max_sz>;
		using iterator		 = typename container_t::iterator;
		using const_iterator = typename container_t::const_iterator;

		template <typename in_t, typename sorter>
		constexpr static_map(const in_t& in, sorter compare = cxpr::less()) noexcept 
			: entries{}
		{
			// TODO: could insert & sort in one statement here
			cxpr::copy(std::begin(in), std::end(in), std::begin(entries));
			cxpr::sort(std::begin(entries), entries.end(), compare);
		}

		constexpr void clear()			 noexcept		{ entries.clear();			}
		constexpr decltype(auto) begin() noexcept		{ return entries.begin();	}
		constexpr decltype(auto) end()	 noexcept		{ return entries.end();		}
		constexpr decltype(auto) begin() const noexcept	{ return entries.begin();	}
		constexpr decltype(auto) end()	 const noexcept	{ return entries.end();		}
		constexpr size_t size()			 const noexcept { return entries.size();	}

		[[nodiscard]] constexpr const_iterator find(const key_t& k) const noexcept
		{
			return getEntry(k);
		}

		[[nodiscard]] constexpr bool has_key(const key_t& k) const noexcept
		{
			return getEntry(k) != std::end(entries);
		}

		template <key_t key>
		[[nodiscard]] constexpr decltype(auto) get_entry() const noexcept
		{
			entry_t search = { key, {} };
			auto found = cxpr::lower_bound(std::begin(entries), std::end(entries), search);
			if (found->first == key)
			{
				return std::make_pair(true, &found->second);
			}
			else
			{
				//throw std::logic_error("Entry does not exist");
				return std::make_pair(false, static_cast<const value_t*>(nullptr));
			}
		}

		[[nodiscard]] constexpr decltype(auto) get_entry(key_t key) const noexcept
		{
			entry_t search = { key, {} };
			auto found = cxpr::lower_bound(std::begin(entries), std::end(entries), search);
			if (found->first == key)
			{
				return std::make_pair(true, &found->second);
			}
			else
			{
				return std::make_pair(false, static_cast<const value_t*>(nullptr));
			}
		}

		[[nodiscard]] constexpr const value_t& operator[](const key_t& k) const
		{
			const auto found = find(k);
			if (found == std::end(entries))
			{
				throw std::runtime_error("entry does not exist in cxpr::static_map");
			}

			return found->second;
		}

	protected:
		std::array<entry_t, max_sz> entries; // fixed_vector is overkill here, use vanilla array

		decltype(auto) getEntry(const key_t& k) const
		{
			entry_t search = { k, {} };
			auto found = cxpr::lower_bound(std::begin(entries), std::end(entries), search);
			if (found->first == k)
			{
				return found;
			}
			else
			{
				return std::end(entries);
			}
		}
	};

	//////////////////////////////////////////////////////////////////////////

	template <typename K, typename V, size_t n, typename pred = cxpr::less>
	constexpr decltype(auto) make_static_map(const cxpr::static_pair<K, V>(&in)[n], pred compare = pred{})
	{
		return static_map<K, V, n>(in, compare);
	}


}