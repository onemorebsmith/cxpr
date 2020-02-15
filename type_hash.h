#pragma once

//////////////////////////////////////////////////////////////////////////

namespace cxpr
{
	namespace __detail
	{
		//////////////////////////////////////////////////////////////////////////
		// Generates a unique string based on the type template param. This string will be unique per type, but constant for the same type
		template <typename obj_t>
		struct compiletime_unique_str
		{
			static constexpr const char* data()
			{
				// this returns the fully-decorated function name, which will be unique for each type passed in.
				// ie for double this will be approx: "const char *__cdecl name_hash<double>::hash_data(void)"
				return __FUNCSIG__;
			}

			static constexpr size_t length = cxpr::cx_strlen(compiletime_unique_str<obj_t>::data());
		};
	}

	template <typename obj_t>
	constexpr const char* name_hash_v = __detail::compiletime_unique_str<obj_t>::data();

	//////////////////////////////////////////////////////////////////////////
	// Struct that computes a run-time hash using the PJW hash algorithm 
	// (https://en.wikipedia.org/wiki/PJW_hash_function)
	constexpr size_t hash_typename(const char* s)
	{
		size_t h = 0;
		size_t high = 0;
		while (*s)
		{
			h = (h << 4) + *s++;
			if (high = h & 0xF0000000)
				h ^= high >> 24;
			h &= ~high;
		}
		return h;
	}

	// Accessor type for the actual hashed value
	template <typename obj_t>
	struct type_hash
	{
		static constexpr hash_t value = hash_typename(name_hash_v<obj_t>);
	};

	template <typename obj_t>
	static constexpr auto typehash_v = type_hash<obj_t>::value;
}

