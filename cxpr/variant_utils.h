namespace cxpr
{
	template <typename fun_t, template<typename ...> typename variant_t, typename ... Ts>
	constexpr decltype(auto) jump_visit(fun_t&& fun, const variant_t<Ts...>& v)
	{
 		using functor_t = void(*)(fun_t && fun, decltype(v) var);
 		constexpr std::array<functor_t, sizeof...(Ts)> jump_table =
 		{
 			[](fun_t&& fun, auto& var) constexpr
 			{
 				return fun(*std::get_if<Ts>(&var));
 			}...
 		};
 
 		return jump_table[v.index()](std::forward<fun_t>(fun), v);
	}
}