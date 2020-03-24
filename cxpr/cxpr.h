#pragma once

//////////////////////////////////////////////////////////////////////////

#define param_pack_t params_t&&...
#define perfect_forward(pack) std::forward<decltype(pack)>(pack)...

//////////////////////////////////////////////////////////////////////////
// Required library includes
#include <algorithm>
#include <type_traits>
#include <variant>

namespace cxpr
{
	using hash_t = unsigned long long;
}

#include "type_hash.h"
#include "variadic_utils.h"
#include "static_pair.h"
#include "optional_ex.h"
#include "cxpr_algo.h"
#include "array_utils.h"
#include "fixed_vector.h"
#include "fixed_string.h"
#include "static_map.h"
#include "tuple_utils.h"
#include "variant_utils.h"

//#undef PARAM_PACK_UTILS
//#undef param_pack_t
//#undef perfect_forward