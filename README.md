# cxpr
[WIP] A collection of c++17 compile-time classes, utilities, and their corresponding unit tests.
Tested, built, and developed with MSVC/Clang.

# Why?
- Compile time errors are easier to debug and cheaper to fix
- Anything done during compile saves work during execution time

This library was designed to focus around manipulating types and generating code at compile-time, with a particular focus around working with tuples. Below are some examples of utilities in the library.

Iterate a tuple:

```cpp
    	std::tuple<int, double, float> tt(10, 123.456, -42);
	cxpr::visit_tuple([&](auto&& v)
	{
		std::cout << v << " ";
	}, tt);
	// output: "10 123.456 -42 "
```

Pick a single element by type:

```cpp
    	std::tuple<int, double, std::string, char*> tt(10, 123.456, "Test", nullptr);
	std::cout << cxpr::find_tuple_type<std::string>(tt).c_str();
	// output: test
	// Bonus: compile error if type isn't present
```

Mutate the types:

```cpp
   using types_t = std::tuple<std::string&&, const int, const double*, int>;
   using decayed_types_t = cxpr::mutate_types_t<types_t, std::decay_t>;
   //decayed_types_t = cxpr::typeset<std::string, int, double const*, int>
```
Collapse a tuple of tuples:

```cpp
    // tuple of tuples
    using nested_t = std::tuple<std::tuple<std::string, double, double>, 
                                std::typeset<float, float>, 
                                std::tuple<unsigned int, double>>;
    // collapse down to a single tuple with all types
    using collapsed_t =  cxpr::collapse_nested_tuple_t<nested_t>
    // collapsed_t = cxpr::typeset<std::string, double, double, float, float, unsigned int, double>
    // take only the unique types
    using unique_collapsed_t = cxpr::tuple_unique_t<collapsed_t>
    // unique_collapsed_t = cxpr::typeset<std::string, double, float, unsigned int> 
```
Compile-time lookup table
```cpp
constexpr static auto lut = cxpr::make_static_map<int, const char*>({
			{ 1, "One"},
			{ 2, "Two"},
			{ 3, "Three"},
			{ 4, "Four"},
		});
		
	constexpr auto res = lut.get_entry(1).second;  // successs
	constexpr auto res2 = lut.get_entry(7).second; // compile-error
```

# Files
- __array_utils.h__: Helpers/utilities focused around std::array<>
- __cxpr.h__: main header for the library, includes all other headers in their proper order
- __cxpr_algo.h__: implementation of necessary std::algorithms that aren't currently constexpr in the standard
- __fixed_string.h__: compile-time constant, fixed-sized string class. Supports both char and wchar
- __fixed_vector.h__: wrapper around std::array that implements push_back/emplace.
- __optional_ex.h__: experimental implementation of functional programming concepts (apply, and_then, or_else) around std::optional
- __static_map.h__: compile-time constant, flat-memory, key-value map. Allows 'if constexpr' access during compile time 
- __static_pair.h__: sparse implementation of std::pair as pair isn't currently constexpr friendly. Implements just what is needed for static_map
- __tuple_utils.h__: large collection of helpers around tuples and parameter packs.
- __type_hash.h__: implementation of a static type system built around hashing the typename during compile
- __variadic_utils.h__:  collecton of utils around variadic templates

