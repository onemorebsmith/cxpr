# cxpr
A collection of c++17 compile-time classes, utilities, and their corresponding unit tests

# files
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

