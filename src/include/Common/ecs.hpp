#pragma once
//#include <entt/core/type_traits.hpp>
//#include <entt/entity/registry.hpp>
#ifdef var
#undef var
//#include"entt/entt.hpp"
#endif // var
#define var auto
//#include <entt\entity\fwd.hpp>
#include <algorithm>
#include <cstddef>
#include <iterator>
#include <memory>
#include<unordered_map>
#include "common/span.hpp"
#include <common\Math1.h>
#include"common\Utility.h"
//#include <boost/align/aligned_allocator.hpp>
#include<parallel_hashmap/phmap.h>
#include<malloc.h>
constexpr int Maxid = -1;
#define ECS_COMP_DE(comp)\
public: static uint type;\
uint id;
#define ECS_COMP_IM(comp)\
uint comp::type=Maxid;

namespace Swi {
	//template <size_t I>
	//struct visit_impl
	//{
	//	static constexpr std::size_t value = I;
	//	template <typename T, typename F>
	//	static void visit(T& tup, size_t idx, F fun)
	//	{
	//		if (idx == I - 1) fun(visit_impl<I>());
	//		else visit_impl<I - 1>::visit(tup, idx, fun);
	//	}
	//	template <typename T, typename F>
	//	constexpr static void visit(size_t idx,F fun)
	//	{
	//		if (idx == I - 1) fun(visit_impl<I>());
	//		else visit_impl<I - 1>::template visit<T>(idx,fun);
	//	}
	//};

	//template <>
	//struct visit_impl<0>
	//{
	//	//static constexpr std::size_t value = 0;
	//	template <typename T, typename F>
	//	static void visit(T& tup, size_t idx, F fun) {
	//		assert(false);
	//	}
	//	template <typename T, typename F>
	//	constexpr static void visit(size_t idx, F fun)
	//	{
	//		assert(false);
	//	}
	//};
	/*template <typename F, typename... Ts>
	constexpr void visit_at(std::tuple<Ts...> const& tup, size_t idx, F&& fun)
	{
		Swi::visit_impl<sizeof...(Ts)>::visit(tup, idx, fun);
	}

	template <typename F, typename... Ts>
	constexpr void visit_at(std::tuple<Ts...>& tup, size_t idx, F&& fun)
	{
		Swi::visit_impl<sizeof...(Ts)>::visit(tup, idx, fun);
	}*/
#define ECS_SWI_switchfw(f,i,imx) Utility::Switch100<imx>(std::forward<F>(f),i)
#define ECS_SWI_switch(f,i,imx) Utility::Switch100<imx>(f,i)

	template <typename F, std::size_t... I>
	constexpr auto visit_atimpl(const size_t& idx, F fun, std::index_sequence<I...>)
	{
		//assert(idx < tuple_size_v<T>);
		constexpr auto imax = sizeof...(I);
		ECS_SWI_switchfw(fun, idx, imax);
		//((I == idx ? fun(std::integral_constant<std::size_t, I>()) : void()), ...);
	}
	template <typename F, typename Fe, std::size_t... I>
	constexpr auto visit_atimpl(const size_t& idx, F&& fun, Fe&& fundef, std::index_sequence<I...>)
	{
		assert(idx < sizeof...(I));
		/*if (idx >= sizeof...(I)) {
			fundef(std::integral_constant<std::size_t, -1>());
		}*/
		constexpr auto imax = sizeof...(I);
		ECS_SWI_switchfw(fun, idx, imax);
		//((I == idx ? fun(std::integral_constant<std::size_t, I>()) : void()), ...);
	}
	template <typename F, std::size_t... I>
	constexpr auto visit_allimpl(F fun, std::index_sequence<I...>)
	{
		//assert(idx < tuple_size_v<T>);
		((fun(std::integral_constant<std::size_t, I>())), ...);
	}
	//template <typename _from, typename _to, typename F, std::size_t... I>
	//inline constexpr void visit_allconvertibleimp(F fun, std::index_sequence<I...>)
	//{
	//	((std::is_convertible_v<std::tuple_element_t<I,_from>,_to> ? visit_allconvertibleimp_1<I>(fun) : void()), ...);
	//}
	template<class _from, class _to>
	constexpr auto convertible() {
		return std::is_convertible_v<_from, _to> || std::is_base_of_v<_to, _from>;
	}
	template <typename _from, typename _to, std::size_t  Ix, std::size_t I, typename F>
	constexpr void visit_convertibleimp(const size_t& idx, F&& fun)
	{
		constexpr auto imax = I;
		Utility::Switch100<I>(
			[&idx, fl = std::forward<F>(fun)](auto Il) {
			if constexpr (convertible<std::tuple_element_t<Il, _from>, _to>())
			{
				fl(Il);
			}}, idx);
		//ECS_SWI_switch(f, idx, imax);
		/*[&idx, f = std::forward<F>(fun)] (auto Il) {
			if constexpr (convertible<std::tuple_element_t<I, _from>, _to>())
			{
				ECS_SWI_switch(f, idx, std::integral_constant<std::size_t, IMAX>());
			}
		}*/

		//if constexpr(I<IMAX){
		//	if constexpr (convertible<std::tuple_element_t<I, _from>, _to>())
		//	{
		//		ECS_SWI_switch((fun), idx, std::integral_constant<std::size_t, IMAX>()));
		//		/*if (I == idx) {
		//			fun(std::integral_constant<std::size_t, I>());
		//		}*/
		//	}
		//	visit_convertibleimp<_from, _to, I+1, IMAX>(idx, std::forward<F>(fun));
		// 
		//}
		//((std::is_convertible_v<std::tuple_element_t<I,_from>,_to>&& I == idx ? fun(std::integral_constant<std::size_t, I>()) : void()), ...);
	}
	template <typename _from, typename _to, std::size_t  I, std::size_t  IMAX, typename F>
	constexpr void visit_allconvertibleimp(F fun)
	{
		if constexpr (I < IMAX) {
			using tI = typename std::tuple_element_t<I, _from>;
			if constexpr (convertible<std::tuple_element_t<I, _from>, _to>())
			{
				fun(std::integral_constant<std::size_t, I>());
			}
			visit_allconvertibleimp<_from, _to, I + 1, IMAX>(std::forward<F>(fun));
		}
		//((std::is_convertible_v<std::tuple_element_t<I,_from>,_to>&& I == idx ? fun(std::integral_constant<std::size_t, I>()) : void()), ...);
	}
#undef ECS_SWI_switch
#undef ECS_SWI_switchfw
}

namespace ECSimp {
	template <typename T, typename Tup, std::size_t... I>
	constexpr auto exist(std::index_sequence<I...>)
	{
		//assert(idx < tuple_size_v<T>);
		return (std::is_same_v<T, std::tuple_element_t<I, Tup>>+...);
	}
	/*template <typename T, class Tup, std::size_t... I>
	constexpr auto baseof(std::index_sequence<I...>)
	{
		return((I == idx ? fun(std::integral_constant<std::size_t, I>()) : void()), ...);
	}*/
}
namespace ECS
{
	using Swi::convertible;
	template <class... _Types>
	struct CompPackage;
#define DERI(ret,_base,regis,f) 	using ret = regis::template derifrom<I, _base>;if constexpr(!std::is_void_v<ret>)f
	/*template <typename T, typename F, typename Indices = std::make_index_sequence<std::tuple_size_v<typename T::typkg>>>
	constexpr void visit_at(const size_t idx, F&& fun)
	{
		Swi::visit_atimpl(idx, fun, Indices{});
	}*/
	template <std::size_t I, typename F, typename I1, typename Indices = std::make_index_sequence<I>>
	constexpr auto visit_at(const I1 idx, F&& fun)
	{
		Swi::visit_atimpl((size_t)idx, fun, Indices{});
	}
	template <typename T, typename F, typename Fdef, typename Indices = std::make_index_sequence<std::tuple_size_v<typename T::typkg>>>
	constexpr void visit_at(const size_t idx, F&& fun, Fdef&& fdf)
	{
		Swi::visit_atimpl(idx, fun, fdf, Indices{});
	}

	template <typename T, typename F, typename Indices = std::make_index_sequence<std::tuple_size_v<typename T::typkg>>>
	constexpr void visit_at(const size_t idx, F&& fun)
	{
		Swi::visit_atimpl(idx, fun, Indices{});
	}
	template <typename T, typename _to, typename F, typename Indices = std::make_index_sequence<std::tuple_size_v<typename T::typkg>>>
	constexpr void visit_convertible(const size_t idx, F&& fun)
	{
		Swi::visit_convertibleimp<typename T::typkg, _to, 0, std::tuple_size_v<typename T::typkg>>(idx, std::forward<F>(fun));
	}
	template <typename T, typename F, typename Indices = std::make_index_sequence<std::tuple_size_v<typename T::typkg>>>
	constexpr void visit_all(F fun)
	{
		Swi::visit_allimpl(fun, Indices{});
	}
	template<class T, class...Ts2>
	constexpr void convert(T& tin, std::tuple<Ts2...>& tout) {
		((std::get<Ts2>(tout) = std::get<Ts2>(tin)), ...);
	}

	template <std::size_t I, typename F, typename Indices = std::make_index_sequence<I>>
	constexpr void visit(F fun)
	{
		Swi::visit_allimpl(fun, Indices{});
	}
	template <typename F, typename... _Types>
	constexpr void visit(F fun, std::tuple<_Types...>& tup)
	{
		Swi::visit_allimpl(fun, std::make_index_sequence<sizeof ...(_Types)>());
	}
	template <typename T, typename _to, typename F, typename tup = typename T::typkg>
	constexpr void visit_all_convertible(F fun)
	{
		Swi::visit_allconvertibleimp<tup, _to, 0, std::tuple_size_v<tup>>(std::forward<F>(fun));
	}
	template <typename T, class Tup, typename Indices = std::make_index_sequence<std::tuple_size_v<typename Tup::typkg>>>
	constexpr auto exist()
	{
		return ECSimp::exist<T, typename Tup::typkg>(Indices{});
	}
	/*template <typename T, class Tup, typename Indices = std::make_index_sequence<std::tuple_size_v<typename Tup::typkg>>>
	constexpr auto baseof()
	{
		return ECSimp::baseof<T, typename Tup::typkg>(Indices{});
	}*/
	//template <typename F, typename T>
	//constexpr size_t visit_at(size_t idx)
	//{
	//	return Swi::visit_impl<sizeof...(Ts)>::visit(tup, idx, fun);
	//}
	using uint = uint32_t;
	template<typename T>
	using toString_t = decltype(std::declval<T&>().toString());
	template <class T, class Tuple>
	struct Index;

	template <class T, class... Types>
	struct Index<T, std::tuple<T, Types...>> {
		static constexpr std::size_t value = 0;
	};

	template <class T, class U, class... Types>
	struct Index<T, std::tuple<U, Types...>> {
		static constexpr  std::size_t value = 1 + Index<T, std::tuple<Types...>>::value;
	};
	template<class T>
	struct ECB
	{
	public:
		//using tut = typename T::typkg;
		static constexpr  auto t_sz() {
			return std::tuple_size_v<typename T::typkg>;
		}
		template<class _base>
		static constexpr  auto is_derive_from() {
			return std::is_base_of_v<_base, typename T::typkg>;
		}
	};

	template <typename T0, typename ... Ts2>
	struct cat
	{};
	/*template <typename T0>
	struct cat<T0>
	{
		using tup2 = T0;
	};*/
	//template <typename T0, typename ... Ts2>
	//struct cat//<T0,Ts2...>
	//{
	//	using tup2 = std::tuple<T0, Ts2...>;
	//};
	template <typename ... Ts1, typename ... Ts2>
	struct cat<std::tuple<Ts1...>, Ts2...>
	{
		using tup2 = std::tuple<Ts1..., Ts2...>;
	};
	/*template <typename ... Ts1, typename ... Ts2>
	struct cat<std::tuple<Ts1...>, Ts2...>
	{
		using tup2 = std::tuple<Ts1..., Ts2...>;
	};*/
	template <typename ... Ts1, typename ... Ts2, typename ... Ts3>
	struct cat<std::tuple<Ts1...>, std::tuple<Ts2...>, Ts3...>
	{
		using tup2 = std::tuple<Ts1..., Ts2..., Ts3...>;
	};

	template <class deriv, class E>
	class BaseOf;
	template <class... _Types>
	class ECSclass;
	template<class ...Ts>
	struct isEclass :std::false_type {};
	template<class ...Ts>
	struct isEclass<ECSclass<Ts...>> :std::true_type {};
	template <class... _Types>
	class ECSclass : public ECB<ECSclass<_Types...>> {
		using self = ECSclass<_Types...>;
	public:
		using typkg = std::tuple<_Types...>;
		template<std::size_t _Index>
		using elei = typename std::tuple_element<_Index, typkg>::type;
		static_assert((... && !isEclass<_Types>::value), "unexpect type");
		template<class T>
		static constexpr  std::size_t Id = Index<T, typkg>::value;
		template <class deriv>
		using t_Basof = typename BaseOf < deriv, self>::baseis;
		template<class _base>
		static constexpr  auto is_derive_from() {
			return std::is_base_of_v<_base, self>;
		}

	};
	template <class... _Types, class... _Types1>
	class ECSclass<ECSclass<_Types...>, _Types1...> :ECSclass<_Types...>//public ECB<ECSclass<_Types..., _Types1...>>
	{
		using self = ECSclass<ECSclass<_Types...>, _Types1...>;
	public:
		using typkg = typename cat<typename ECSclass<_Types...>::typkg, _Types1...>::tup2;// std::tuple<_Types..., _Types1...>;
		template<std::size_t _Index>
		using elei = typename std::tuple_element<_Index, typkg>::type;
		template<class T>
		static constexpr  std::size_t Id = Index<T, typkg>::value;
		template <class deriv>
		using t_Basof = typename BaseOf < deriv, typkg>::baseis;
		template<class _base>
		static constexpr  auto is_derive_from() {
			return std::is_base_of_v<_base, self>;
		}
		static constexpr  auto t_sz() {
			return std::tuple_size_v<typkg>;
		}
	};
	//template <class... _Types>
	//class ECSclass<ECSclass<_Types...>>:public ECSclass<_Types...>
	//{
	//	using self = ECSclass<_Types...>;
	//public:
	//	using typkg = ECSclass<_Types...>;// std::tuple<_Types..., _Types1...>;
	//	template<std::size_t _Index>
	//	using elei = typename std::tuple_element<_Index, typkg>::type;
	//	template<class T>
	//	static constexpr  std::size_t Id = Index<T, typkg>::value;
	//	template <class deriv>
	//	using t_Basof = typename BaseOf < deriv, self>::baseis;
	//	template<class _base>
	//	static constexpr  auto is_derive_from() {
	//		return std::is_base_of_v<_base, self>;
	//	}
	//	static constexpr  auto t_sz() {
	//		return std::tuple_size_v<typkg>;
	//	}
	//};
	template <class... _Types, class... _Types1, class... _Types2>
	class ECSclass<ECSclass<_Types...>, ECSclass<_Types2...>, _Types1...> :ECSclass<_Types...>, ECSclass<_Types2...>//public ECB<ECSclass<_Types..., _Types1...>>
	{
		using self = ECSclass<ECSclass<_Types...>, ECSclass<_Types2...>, _Types1...>;
		//std::tuple<typename ECSclass<_Types...>::typkg, typename ECSclass<_Types2...>::typkg,
		//	_Types2...>; 
	public:
		using typkg = typename cat<typename ECSclass<_Types...>::typkg, typename ECSclass<_Types2...>::typkg, _Types1...>::tup2;// std::tuple<_Types..., _Types1...>;
		template<std::size_t _Index>
		using elei = typename std::tuple_element<_Index, typkg>::type;
		template<class T>
		static constexpr  std::size_t Id = Index<T, typkg>::value;
		template <class deriv>
		using t_Basof = typename BaseOf < deriv, self>::baseis;
		template<class _base>
		static constexpr  auto is_derive_from() {
			return std::is_base_of_v<_base, self>;
		}
		static constexpr  auto t_sz() {
			return std::tuple_size_v<typkg>;
		}
	};
	template<class T, class _base>
	using derif = typename T::template t_Basof<_base>;
	template<class T, class _base>
	using elederi = typename T::template t_Basof<_base>;
	/*template<class T, class _base>
	using derif = typename BaseOf < T, _base>::baseis;*/// typename T::template t_Basof<_base>;
	/*template <class... _Types>
	class ECSclass< std::enable_if_t<(... || !isEclass<_Types>::value), bool>= true> : public ECB<ECSclass<_Types...>> {
	public:
		using typkg = std::tuple<_Types...>;
		template<std::size_t _Index>
		using elei = typename std::tuple_element<_Index, typkg>::type;

		template<class T>
		static constexpr  std::size_t Id = Index<T, typkg>::value;
	};*/
	/*template <class... _Types0, class... _Types2>
	class ECSclass<ECSclass<_Types0...>, _Types2...> {
	public:
		static_assert(false, "not allowed");
		using typkg = std::tuple<_Types...>;
		template<std::size_t _Index>
		using elei = typename std::tuple_element<_Index, typkg>::type;

		template<class T>
		static constexpr  std::size_t Id = Index<T, typkg>::value;
	};*/
	/*template <class... _Types>
	class ECSclass<std::tuple<_Types...>> :public ECB<std::tuple<_Types...>> {
	public:
		using typkg = std::tuple<_Types...>;
		template<std::size_t _Index>
		using elei = typename std::tuple_element<_Index, typkg>::type;

		template<class T>
		static constexpr  std::size_t Id = Index<T, typkg>::value;
	};*/

	//template <class... _Types, class... _Types2, class... _Types1>
	//class ECSclass<ECSclass<ECSclass<_Types...>, _Types2...>, _Types1...> :ECSclass<ECSclass<_Types...>, _Types2...>//public ECB<ECSclass<_Types..., _Types1...>>
	//{
	//	using self = ECSclass<ECSclass<_Types...>, _Types1...>;
	//public:
	//	using typkg = std::tuple<_Types..., _Types1...>;
	//	template<std::size_t _Index>
	//	using elei = typename std::tuple_element<_Index, typkg>::type;
	//	template<class T>
	//	static constexpr  std::size_t Id = Index<T, typkg>::value;
	//	template<class _base>
	//	static constexpr  auto is_derive_from() {
	//		return std::is_base_of_v<_base, self>;
	//	}
	//};
	//template <template<class...> class T, template<class...> class T2, class... t, class... P, class... _Types1>
	//class ECSclass<T<t...>,T2<P...>,
	//	_Types1...
	//	> 
	//{
	//	static_assert(false,"not allowed");
	//	/*using self = ECSclass<ECSclass<_Types...>, _Types1...>;
	//public:
	//	using typkg = std::tuple<_Types..., _Types1...>;
	//	template<std::size_t _Index>
	//	using elei = typename std::tuple_element<_Index, typkg>::type;
	//	template<class T>
	//	static constexpr  std::size_t Id = Index<T, typkg>::value;
	//	template<class _base>
	//	static constexpr  auto is_derive_from() {
	//		return std::is_base_of_v<_base, self>;
	//	}*/
	//};
	template <class _Derived>
	class BaseOf<_Derived, std::tuple<>> {
	public:
		using baseis = void;
		//using baseis = std::conditional_t<std::is_base_of_v<U, _Derived>, U, int>;
			//Baseis<_Derived, ECSclass<_Types...>>::baseis, U>;
	/*protected:
		static constexpr auto baseof() {}*/
	};
	template <class _base, class U, class... _Types>
	class BaseOf<_base, std::tuple<U, _Types...>> {
	public:
		using baseis = std::conditional_t<std::is_base_of_v<_base, U>, U, typename BaseOf<_base, ECSclass<_Types...>>::baseis>;
	};
	template <class _base, class... _Types>
	class BaseOf<_base, ECSclass<_Types...>> {
	public:
		using baseis = typename BaseOf<_base, typename ECSclass<_Types...>::typkg>::baseis;
	};
	template <class ... _Types1, class... _Types>
	class BaseOf<ECSclass<_Types1...>, ECSclass<_Types...>> {
		//static_assert(false, "not support");//add support if necessary
		using bs = ECSclass<_Types...>;
		using dr = ECSclass<_Types1...>;
	public:
		//	using baseis = typename BaseOf<_base, typename ECSclass<_Types...>::typkg>::baseis;
	};

	/*template<class deri, class _base>
	struct elederi
	{

	};
	template<class ...Ts, class _base>
	struct elederi<std::tuple<Ts...>, _base>
	{
		using baseis = typename BaseOf<_base, std::tuple<Ts...>>::baseis;
	};
	template<class ..._Types, class _base>
	struct elederi< ECSclass<_Types...>, _base>
	{
		using baseis = typename BaseOf<_base, typename ECSclass<_Types...>::typkg>::baseis;
	};*/
	template <class deriv, class E>
	using t_Basof = typename BaseOf<deriv, E>::baseis;

	template <class T, class Tuple>
	struct has_type;
	//template<>
	template <class T, class U, class... Types>
	struct has_type<T, ECSclass<U, Types...>>;

	template <class T>
	struct has_type<T, std::tuple<>> {
		//using vt = void;
		static constexpr std::size_t value = 0;
		//static constexpr  bool exist = false;
	};
	template <class T, class... Types>
	struct has_type<T, std::tuple<T, Types...>> {
		//using vt = T;
		static constexpr std::size_t value = 1 + has_type<T, std::tuple<Types...>>::value;
		//static constexpr  bool exist = true;
	};
	template <class T, class U, class... Types>
	struct has_type<T, ECSclass<U, Types...>> {

		//using vt = has_type<T, std::tuple<Types...>>::vt;
		static constexpr  std::size_t value = 0 + has_type<T, std::tuple<Types...>>::value;
		//static constexpr  bool exist = has_type<T, std::tuple<Types...>>::exist;
	};
	template<class..._Types>
	struct Registry
	{
		using typkg = std::tuple<_Types...>;
		using t_Pkgs = CompPackage<_Types...>;
		template<std::size_t _Index>
		using t = std::tuple_element_t<_Index, typkg>;

		template<std::size_t _Index, class T>
		using derifrom = typename t<_Index>::template  t_Basof<T>;
		template<class _Base>
		using firestderi = typename ECS::BaseOf<typkg, _Base>::baseis;
		template<class T>
		static constexpr  std::size_t Id = Index<T, typkg>::value;
		static constexpr math::ui64 size() {
			return std::tuple_size_v<typkg>;
		}
		template<class T>
		static constexpr void visit_at(const math::ui64& i, T&& f) {
			using index_sequence_for = std::make_index_sequence<sizeof...(_Types)>;
			Swi::visit_atimpl(i, f, index_sequence_for{});
		}

	};
	/*template <class... _Types>
	class ECSclass {
		using typkg = decltype(std::tuple_cat(std::declval< t >() ...));;
	};*/
	template <class T, class Tuple>
	constexpr  size_t Index_v = Index<T, Tuple>::value;
	template <class... _Types>
	struct CompAccessor
	{
		using t_tup = std::tuple<_Types& ...>;
		static constexpr auto t_size() {
			return std::tuple_size_v<t_tup>;
		}
		std::tuple<_Types& ...> datas;
		CompAccessor(_Types&&... _Args) :datas(std::forward_as_tuple(_Args...)) {}
		/*CompAccessor() {}*/
		CompAccessor(_Types&... _Args) :datas(_Args...) {}
		CompAccessor(std::tuple<_Types&...>  _Args) :datas(std::move(_Args)) {}
		CompAccessor(Utility::tup<_Types&...>&& _Args) :datas(std::move(_Args.d)) {}
		//template<typename Indices = std::make_index_sequence<t_size()>>
		constexpr auto get(const std::size_t i) {
			return get(i, std::make_index_sequence<t_size()>{});
		}
		template<class T>
		auto&& get(std::size_t i) {
			return std::get<T>(get(i));
		}
		template<std::size_t fi>
		constexpr auto&& get(std::size_t i) {
			return std::get<fi>(get(i));
		}
		//private:

		template<std::size_t... I>
		constexpr auto get(const std::size_t i) {
			return Utility::tup<decltype(std::get<I>(datas))...>(std::forward_as_tuple(std::get<I>(datas)[i]...)); //std::forward_as_tuple(std::get<I>(datas)[i]...);
		}
		template<std::size_t... I>
		constexpr auto get(const std::size_t i, std::index_sequence<I...>) {
			return Utility::tup<decltype(std::get<I>(datas))...>(std::forward_as_tuple(std::get<I>(datas)[i]...));
		}
		/*template<class... _Types1, class T>
		static constexpr auto get(const std::size_t i, T&& datas) {
			using index_sequence_for = std::make_index_sequence<t_size()>;
			if constexpr (sizeof...(_Types1) > 0)
				return Utility::tup(std::forward_as_tuple((datas[eclass::template Id<_Types1>]).template Get<_Types1>(i)...));
			else
				return get(i, std::move(datas), index_sequence_for{});
		}
		template<std::size_t... I, class T>
		static constexpr auto get(const std::size_t i, T&& datas, std::index_sequence<I...>) {
			return Utility::tup(std::forward_as_tuple((datas[I]).template Get<eclass::template elei<I>>(i)...));
		}*/
	};
	//template <class T>
	//struct CompAccessor<T>
	//{
	//	
	//	using t_tup = std::tuple<T& >;
	//	static constexpr auto t_size() {
	//		return std::tuple_size_v<t_tup>;
	//	}
	//	T& datas;
	//	CompAccessor(T&& _Args) :datas(std::forward_as_tuple(_Args)) {}
	//	/*CompAccessor() {}*/
	//	CompAccessor(T& _Args) :datas(_Args) {}
	//	//CompAccessor(std::tuple<T&>  _Args) :datas(std::move(_Args)) {}
	//	//template<typename Indices = std::make_index_sequence<t_size()>>
	//	constexpr auto&& get(const std::size_t i) {
	//			return (datas)[i];
	//	}
	//};
	template <class... _Types>
	struct CompAccessor<ECSclass<_Types...>>
	{
		using eclass = ECSclass<_Types...>;
		static constexpr math::ui64 t_size() {
			return std::tuple_size_v<typename eclass::typkg>;
		}
		using index_sequence_for = std::make_index_sequence<t_size()>;
		/*template<class T>
		static constexpr auto&& get(const std::size_t i,T&& datas) {
			return std::forward_as_tuple((datas[eclass::template Id<_Types>]).Get<_Types>(i)...);
		}*/
		/*template<class... _Types1, class T>
		static constexpr auto get(const std::size_t i, T&& datas) {
			if constexpr (sizeof...(_Types1) > 0)
				return Utility::tup(std::forward_as_tuple((datas[eclass::template Id<_Types1>]).template Get<_Types1>(i)...));
			else
				return get(i, std::move(datas), index_sequence_for{});
		}
		template<std::size_t... I, class T>
		static constexpr auto get(const std::size_t i, T&& datas, std::index_sequence<I...>) {
			return Utility::tup(std::forward_as_tuple((datas[I]).template Get<eclass::template elei<I>>(i)...));
		}
		private:
			template<std::size_t... I, class T>
			static constexpr auto&& getp(const std::size_t i, T&& datas) {
				return Utility::tup(std::forward_as_tuple(std::get<I>(datas)[i]...));
			}*/
			/*template<class T>
			auto&& get(std::size_t i) {
				return std::get<T>(get(i));
			}
			template<std::size_t fi>
			constexpr auto&& get(std::size_t i) {
				return std::get<fi>(get(i));
			}*/
	};
	template <class... _Types>
	struct CompAccessor1
	{
		std::tuple<_Types ...> datas;
		CompAccessor1(_Types... _Args) :datas(std::forward_as_tuple(_Args...)) {}
		/*CompAccessor() {}*/
		CompAccessor1(_Types&... _Args) :datas(_Args...) {}
		CompAccessor1(std::tuple<_Types&...>  _Args) :datas(std::move(_Args)) {}
		constexpr auto get(std::size_t i) {
			return std::forward_as_tuple(std::get<_Types>(datas)[i]...);
		}
		template<class T>
		auto&& get(std::size_t i) {
			return std::get<T>(get(i));
		}
		template<std::size_t fi>
		constexpr auto&& get(std::size_t i) {
			return std::get<fi>(get(i));
		}
	};

	template <size_t sz, class... _Types>
	struct CompPackageFix {
		using v_ele = std::tuple<_Types...>;
		std::tuple<std::array<_Types, sz>...>pkg;
		CompPackageFix() {}
		constexpr auto operator[](std::size_t i) {
			return get_v(i);
		}
		//template<size_t I = 0>
		constexpr auto&& get(const std::uint64_t i) {
			return get_p(i, std::make_index_sequence<sizeof...(_Types)>{});
		}
		template<size_t I = 0>
		constexpr auto&& get(const std::uint64_t i) {
			return std::get<I>(pkg)[i];
		}
		constexpr auto get_v(const std::uint64_t i) {
			return get_v(i, std::make_index_sequence<sizeof...(_Types)>{});
		}
		//template<size_t I = 0, class ...Tp>
		//constexpr void set(const std::size_t i, Tp&&... _Args) {
		//	if constexpr (I < sizeof...(Tp)) {
		//		std::get<I>(pkg)[i] = std::get<I>(std::forward_as_tuple(std::forward<Tp>(_Args)...));;
		//		set<I + 1>(i, _Args...);
		//	}
		//	//set(i, std::make_index_sequence<sizeof...(_Types)>{}, std::forward<_Types>(_Args)...);
		//}
		template<size_t I = 0, class I0>
		constexpr void set(const I0& v2, const v_ele& retu) {
			if constexpr (I < sizeof...(_Types)) {
				(std::get<I>(pkg)[v2] = std::get<I>(retu));
				set<I + 1>(v2, retu);
			}
		}
		template<size_t I = 0, class I0>
		constexpr void setv(const I0& v2, v_ele& retu) {
			if constexpr (I < sizeof...(_Types)) {
				(std::get<I>(retu) = std::get<I>(pkg)[v2]);
				setv<I + 1>(v2, retu);
			}
		}
		constexpr auto size() {
			return sz;
		}
		template<size_t I = 0>
		constexpr void swap(int v1, int v2) {
			if constexpr (I < sizeof...(_Types)) {
				std::swap(std::get<I>(pkg)[v1], std::get<I>(pkg)[v2]);
				//std::get<I>(v2) = tmv;
			/*std::get<I>(v2)=tmv;*/
				swap<I + 1>(v1, v2);
			}
		}
		template<size_t I = 0, class I0>
		constexpr void copy(const int& v1, const I0& v2) {
			if constexpr (I < sizeof...(_Types)) {
				(std::memcpy(&std::get<I>(pkg)[v1], &std::get<I>(pkg)[v2], sizeof(std::tuple_element_t<I, v_ele>)));
				copy<I + 1>(v1, v2);
			}
		}
		template<size_t I = 0, class I0>
		constexpr void move(const I0& dist, I0 Src, const I0& size) {
			if constexpr (I < sizeof...(_Types)) {
				const auto bytesize = sizeof(std::tuple_element_t<I, v_ele>) * size;
				(std::memmove(&std::get<I>(pkg)[dist - size], &std::get<I>(pkg)[Src], bytesize));
				move<I + 1>(dist, Src, size);
			}
		}
	private:
		template<size_t... Is>
		constexpr auto&& get_p(const std::uint64_t i, std::index_sequence<Is...>) {
			return  std::forward_as_tuple(std::get<Is>(pkg)[i]...);
		}
		template<size_t... Is>
		constexpr auto get_v(const std::uint64_t i, std::index_sequence<Is...>) {
			return  v_ele(std::get<Is>(pkg)[i]...);
		}
	};
	template<class T, class SP, class Gk>
	constexpr void quickSort0(T& A, int p, int q, SP&& swap, Gk&& gk)
	{
		int r;
		if (p < q)
		{
			//r = partition(A, p, q);
			auto x = gk(A[p]);
			r = p;
			int j;

			for (j = p + 1; j < q; j++)
			{
				if (gk(A[j]) <= x)
				{
					r = r + 1;
					swap(r, j);
				}

			}
			swap(r, p);
			quickSort0(A, p, r, swap, gk);
			quickSort0(A, r + 1, q, swap, gk);
		}
	}
	/*template<class T,class Cmp>
	constexpr void insertionSort(T&& array,Cmp&& cmp) {
		for (size_t step = 0; ++step != array.size(); ) {
			auto key = std::move(array[step]);
			size_t j = step;
			if (cmp(key, 0)) {
				array.move(++j,(size_t)0,step);
				array.set(0, std::move(key));
			}else{
				size_t i = step;
				for (; cmp(key, --i); j = i)
				{
					array.copy(j, i);
				}
				array.set(j, std::move(key));
			}
		}
	}
	template<uint size>
	struct FixStruct
	{
		std::byte data[size];
	};
	*/
	template<class T, class Cmp>
	constexpr void insertionSort(T&& array, Cmp&& cmp) {
		//void insertionSort(int array[], int size) {
		for (int step = 1; step < array.size(); step++) {
			auto key = std::move(array[step]);
			int j = step - 1;

			// Compare key with each element on the left of it until an element smaller than
			// it is found.
			// For descending order, change key<array[j] to key>array[j].
			while (cmp(key, j) && j >= 0) {
				array.copy(j + 1, j);
				//array[j + 1] = array[j];
				--j;
			}
			array.set(j + 1, std::move(key));
		}
	}

	struct Component
	{
		static uint size;
		//static uint4 ids;
	};
	struct EntityId {
		int tid, id;
	};
	template<typename T>
	int getCompid() {

		return T::Groupid;

	}
	struct Entity {

		uint tid, id;
		Entity() :Entity(-1, -1, -1, -1) {}
		Entity(uint tid, uint id, uint ci, uint si) :tid(tid), id(id) {}
		//Entity(uint tid,uint id,const CollideID cd) :tid(tid), id(id) {}
		template<typename T>
		bool Belong() {
			return T::type == tid;
		}

	};

	namespace {
		using namespace std;
		using namespace nonstd;
#ifndef NDEBUG
		uint alloc;
#endif
		struct Conany
		{

			using memrep = char*;
			/*template <std::size_t N>
			struct MyAllocator
			{
				char data[N];
				void* p;
				std::size_t sz;
				MyAllocator() : p(data), sz(N) {}
				template <typename T>
				T* aligned_alloc(std::size_t a = alignof(T))
				{
					EIGEN_ALIGNED_ALLOCA
					if (std::align(a, sizeof(T), p, sz))
					{
						T* result = reinterpret_cast<T*>(p);
						p = (char*)p + sizeof(T);
						sz -= sizeof(T);
						return result;
					}
					return nullptr;
				}
			};*/
			/*template<typename T>
			friend  Conarray<T>::Conarray(Conany&);*/
			//const size_t tid;
			Conany(const size_t alignment, const size_t type, const size_t reserve = 10) :tid(type), alignment(alignment), ay(NULL), count(0), capacity(reserve)
			{
				//int if = 0;
				var newSize = capacity * (alignment);
				alloc(ay, newSize);
			}
			template<typename T>
			constexpr T& Get(size_t i) {
				return *(reinterpret_cast<T*>(ay + i * alignment));
			}

			constexpr void* Get(size_t i) {
				return ((ay + i * alignment));
			}
			template<typename T>
			constexpr void* GetUncheck(T i) {
				return (ay + i * alignment);
			}
			template<typename T, typename T1>
			constexpr auto& Get(const T1& i) {
				return (*reinterpret_cast<T*>(ay + i * alignment));
			}

			template<typename T>
			constexpr T& last() {
				return *(reinterpret_cast<T*>(ay + (count - 1) * alignment));
			}
			inline void SwapRmove(size_t i) {
				std::memcpy(ay + i * alignment, ay + (--count) * alignment, alignment);
			}
			constexpr void RemoveLast() {
				count--;
			}
			template<typename T>
			inline void expand() {
				capacity = math::ceil(count == 0 ? 10 : count * 1.5);
				var newSize = capacity * sizeof(T);
				alloc(ay, newSize);
			}
			template<typename T>
			inline	T& add() {
				if (count == capacity) {
					expand<T>();
				}
				T& v = *new (ay + count * alignment)T;
				count++;
				//v.id = size++;
				return v;
			}
			constexpr const size_t& size() const {
				return count;
			}
			constexpr void resize(size_t& i) {

			}
			constexpr const auto Gattid()const {
				return tid;
			}
			/*void* operator[](uint i) {
				assert(i > count);
				return (ay + i * alignment);
			}*/
			template<class I>
			void* operator[](const I i) {
				assert(i > count);
				return (ay + i * alignment);
			}
			template<typename T>
			inline T& add(T& t) {
				if (count == capacity) {
					expand<T>();
				}
				const auto tptr = reinterpret_cast<T*>(ay);
				tptr[count++] = t;
				return tptr[count - 1];
				//v.id = size++;
			}
			inline void* addrow(size_t i = 1) {
				auto&& newsz = size() + i;
				if (newsz >= capacity && i > 0) {
					capacity = i > 1 ? newsz : math::ceil(newsz * 1.5);
					alloc(ay, capacity * alignment);
				}
				/*for (size_t i0 = 0; i0 < i; i0++)
				{
					std::construct_at(&(this->operator[](size() + i0)));
				}*/
				auto rp = ay + count * alignment;
				count = newsz;
				return rp;
				//v.id = size++;
			}
			//template<class T>
			//inline void* addrowsafe(T&&fc,size_t i = 1) {
			//	auto newsz = count.fetch_add(i,std::memory_order_relaxed);
			//	if (count >= capacity && i > 0) {
			//		const std::lock_guard<std::mutex> lock(_mutex);
			//		if (count >= capacity)
			//		{
			//			auto newcapacity = i > 1 ? newsz : math::ceil(newsz * 1.5);
			//			alloc(ay, newcapacity * alignment);
			//			capacity = newcapacity;
			//		}
			//	}
			//	
			//	auto rp = ay + count * alignment;
			//	//count = newsz;
			//	return rp;
			//}

			constexpr void reserve(std::size_t newsz) {
				if (newsz >= capacity) {
					capacity = newsz;
					var newSize = capacity * alignment;
					alloc(ay, newSize);
				}
			}
			constexpr void clear() {
				count = 0;
			}
			inline memrep& Row_ptr() {
				return ay;
			}
			inline size_t Capacity() {
				return capacity;
			}
			~Conany()
			{
				free(ay);
			}
			constexpr size_t Alignment()const {
				return alignment;
			}
		protected:
			inline void expandstrict() {
				var newSize = capacity * alignment;
				alloc(ay, newSize);
			}
			inline void alloc(
				memrep& _Block,
				size_t _Size
			) {
				if (void* mem = std::realloc(_Block, _Size))
					_Block = (memrep)(mem);
				else
					throw std::bad_alloc();
			}
			math::ui64 count, capacity;
			/*std::atomic_uint32_t state;
			enum ST:UINT32{resizeing,nan};*/
			const uint alignment, tid;
			memrep ay;
		};
		using ui64 = math::ui64;
		//template<class T>
		struct Conany_thsafe
		{
#define var auto
			using memrep = BYTE;
			using p_memrep = memrep*;
			using uint = uint32_t;
			typedef std::size_t     size_type;
			typedef void* pointer;
			/*template <std::size_t N>
			struct MyAllocator
			{
				char data[N];
				void* p;
				std::size_t sz;
				MyAllocator() : p(data), sz(N) {}
				template <typename T>
				T* aligned_alloc(std::size_t a = alignof(T))
				{
					EIGEN_ALIGNED_ALLOCA
					if (std::align(a, sizeof(T), p, sz))
					{
						T* result = reinterpret_cast<T*>(p);
						p = (char*)p + sizeof(T);
						sz -= sizeof(T);
						return result;
					}
					return nullptr;
				}
			};*/
			//Eigen::aligned_allocator<int[4]> kl;
			/*template<typename T>
			friend  Conarray<T>::Conarray(Conany&);*/
			//const size_t tid;;
			explicit Conany_thsafe(const size_t alignment = 0, const size_t type = 0, const size_t reserve = 0, const size_t bksize = 512) :
				tid(type), alignment(alignment), count(0), _perbkeles(bksize)
			{
				this->reserve(reserve);
				//int if = 0;
				//var newSize = capacity * (alignment);
			}
			Conany_thsafe(Conany_thsafe&& source) noexcept :
				tid(source.tid), alignment(source.alignment), count(source.count._Storage._Value), _perbkeles(source._perbkeles)
			{
				Datas = std::move(source.Datas);
				this->capacity = source.capacity._Storage._Value;
			}
			/*Conany_thsafe(const Conany_thsafe& source) noexcept :
				tid(source.tid), alignment(source.alignment), count(source.count._Storage._Value), _perbkeles(source._perbkeles)
			{
				Datas = std::move(source.Datas);
				this->capacity = source.capacity._Storage._Value;
			}*/
			template<typename T>
			constexpr T& Get(size_t i) {
				return *(reinterpret_cast<T*>(Get(i)));
			}

			inline void* const Get(const size_t i) {
				const auto quo = i / _perbkeles;
				const auto rem = i % _perbkeles;
				return (Datas[quo] + (rem)*alignment);
			}
			template<typename T, typename T1>
			constexpr auto& Get(const T1& i) {
				return (*reinterpret_cast<T*>(Get(i)));
			}

			template<typename T>
			constexpr T& last() {
				return *((T*)last());
			}
			inline void* const last() {
				return Get(size() - 1);
			}
			inline void SwapRmove(size_t i) {
				std::memcpy(Get(i), Get(sizew()--), alignment);
			}
			inline void RemoveLast() {
				sizew()--;
			}
			template<typename T>
			inline	T& add() {
				//v.id = size++;
				return *(T*)addrow();
			}
			constexpr const size_t& size() const {
				return count._Storage._Value;
			}
			constexpr const auto Gattid()const {
				return tid;
			}
			/*void* operator[](uint i) {
				assert(i > count);
				return (ay + i * alignment);
			}*/
			template<class I>
			constexpr void* operator[](const I i) {
				assert(i < count);
				return Get(i);
			}

			inline void addrowunsafa(size_t i) {
				reserve(size() + i);
				sizew() += i;
			}

			template<class T>
			inline void addrowsafe(T&& fc, size_t i) {
				auto newsz = count.fetch_add(i, std::memory_order_relaxed);
				while (newsz + i >= capacity) {
					if (newsz / _perbkeles != (newsz + i) / _perbkeles) {
						const std::lock_guard<std::mutex> lock(_mutex);
						reserve(newsz + i);
						break;
						/*(Datas.emplace_back(bksize*alignment));
						capacity += bksize;*/
					}
				}
				size_t i3 = newsz; newsz += i;
				for (; i3 < newsz; i3++)
				{
					fc(Get(i3));
				}
			}
			//template<class T>
			inline void* const addrow() {
				return Get(addrow(1));
			}

			inline std::size_t addrow(const std::size_t i) {
				auto newsz = count.fetch_add(i, std::memory_order_relaxed);
				while (newsz + i >= capacity) {
					//if (newsz / _perbkeles != (newsz + i) / _perbkeles)
					{
						const std::lock_guard<std::mutex> lock(_mutex);
						reserve(newsz + i);
						break;
						/*(Datas.emplace_back(bksize*alignment));
						capacity += bksize;*/
					}
				}
				return newsz;
			}
			inline void reserve(std::size_t newsz) {
				if (newsz >= capacity) {
					auto dis = (size_t)std::ceil((newsz - capacity) / (double)_perbkeles);
					for (size_t i = 0; i < dis; i++)
					{
						Datas.emplace_back(allocate(_perbkeles * alignment));
					}
					capacity += dis * _perbkeles;
				}
			}
			//inline void expandto(std::size_t newsz) {
			//	if (newsz >= capacity) {
			//		auto dis = (size_t)std::ceil((newsz - capacity) / (double)_perbkeles);
			//		for (size_t i = 0; i < dis; i++)
			//		{
			//			Datas.emplace_back(allocate(_perbkeles * alignment));
			//		}
			//		capacity += dis * _perbkeles;
			//	}
			//	//count.fetch_add(i, std::memory_order_relaxed);
			//}
			inline void clear() {
				count = 0;
			}
			inline size_t Capacity() {
				return capacity;
			}
			constexpr size_t Alignment()const {
				return alignment;
			}
			~Conany_thsafe()
			{
				for (size_t i = 0; i < Datas.size(); i++)
				{
					auto& ptr = Datas[i];
					deallocate(ptr);
					ptr = NULL;
				}
			}

			inline void copyto(void* dest)const {
				ui64 k = 0, perblksize = _perbkeles * Alignment();
				for (const auto blk : Datas)
				{
					std::memcpy(dest, blk, perblksize);
					((char*&)dest) += perblksize;
				}
			}
			inline void CopyTo(void* dest, const size_t size)const {
				//constexpr size_t start = 0;
				const auto div = std::div(size, (int64_t)_perbkeles);
				ui64 k = 0, perblksize = _perbkeles * Alignment();
				for (size_t i = 0; i < div.quot; i++)
				{
					std::memcpy(dest, Datas[i], perblksize);
					((char*&)dest) += perblksize;
				}
				std::memcpy(dest, Datas[div.quot], div.rem * Alignment());
			}
		protected:
			inline p_memrep allocate(size_type num)
			{
				return (p_memrep)malloc(num);
			}

			inline void deallocate(pointer p)
			{
				free(p);
			}
			inline void expandstrict() {
				var newSize = capacity * alignment;
			}
			inline ULONG& sizew() {
				return count._Storage._Value;
			}
			std::vector<p_memrep> Datas;
			std::atomic_ulong count, capacity;
			/*std::atomic_uint32_t state;
			enum ST:UINT32{resizeing,nan};*/
			const uint alignment, tid, _perbkeles;
			std::mutex _mutex = {};
		};
		//using Conany_thsafe = Conany_thsafe<char>;
		template<typename T, typename Cotainer = Conany_thsafe>
		struct Conarray {
			Cotainer& conay;
			//T*& data;
			Conarray(Cotainer& c) :conay(c)//, data((T*&)c.Row_ptr())
			{
			}
			constexpr inline size_t const size() {
				return conay.size();
			}
			/*inline T& add() {
				return conay.add<T>();
			}
			inline void add(T&& t) {
				conay.add(t);
			}
			inline void Remove(const uint i) {
				data[i] = data[conay.count--];
			}*/
			constexpr T& operator[](size_t si) {
				return conay.Get<T>(si);
			}
		};

		struct SubclassDesc
		{
			uint offset, maxnum, size;
			SubclassDesc() :offset(Maxid) {}
		};
		//typedef auto (*Update)(auto& comp);
		struct ECSManager
		{
			using Container = Conany_thsafe;
		private:
			static vector<Conany>con;

		public:
			//static vector<Conany>SubDes;
			ECSManager() {
				con.reserve(100);
			}
			constexpr static vector<Conany>& Objs() {
				return con;
			}
			template<typename T>
			struct GetBase
			{
				using base = typename T::base;
			};
			template<typename T>
			static constexpr void RegistEntity(size_t reserve) {
				assert(T::type == Maxid);
				T::type = con.size();
				con.emplace_back(sizeof(T), T::type, reserve);
				Conany& ele = con[T::type];
			}
			static inline Conany& last() {
				return con[con.size() - 1];
			}
			/*template< typename Tsub,typename T2= SubclassDesc>
			void RegistEntity( span<SubclassDesc> reserve2, size_t reserve = 0) {

				for (size_t i = 0; i < reserve2.size(); i++)
				{
					reserve += reserve2[i].maxnum;
				}
				using T = typename GetBase<Tsub>::base;
					RegistEntity<T>(reserve);
				auto desoldsize = SubDes.size();
				auto newsize = con.size() + 1;
				con.resize(newsize);

				SubDes.resize(desoldsize+1);
				Conany& ele = con[T::type];
				auto& des = SubDes[T::type];

				constexpr var alignment = sizeof(T);
				constexpr var alignmentdisc = sizeof(T2);
				des.size = 0;
				ele.size = 0;
				ele.ay = (std::byte*)malloc(reserve * alignment);
				des.ay= (std::byte*)malloc(reserve2 * alignmentdisc);
				des.capacity = reserve2;
				ele.capacity = reserve;
			}*/
			/*template<typename Tsub>
			Conarray<Tsub> GetComps(Tsub const& param) {
				using T = typename GetBase<Tsub>::base;
				return Conarray<Tsub>(con[T::type]);
			}*/
			static constexpr void CheckId(const uint i) {
				assert(i != Maxid);
			}
			template<typename T>
			static constexpr void CheckId() {
				assert(T::type != Maxid);
			}
			static inline Conany& GetRowComps(const int id) {
				return (con[id]);
			}
			template<typename T>
			static constexpr Conany& GetRowComps() {
				return (con[T::type]);
			}
			template<typename T>
			static constexpr T& GetEntity(const uint tpid, const uint id) {
				return(con[tpid].Get<T>(id));
			}
			template<typename T>
			T& CreateEntity() {
				assert(T::type != Maxid);
				const var id = T::type;
				var& eles = con[id];
				T& v = eles.add<T>();
				return v;
			}
			static inline void erase(const uint tpid, const uint id) {
				var& eles = con[tpid];
				eles.SwapRmove(id);
			}
			template<typename T>
			static constexpr void erase(const uint id1) {
				assert(T::type != Maxid);
				const var id = T::type;
				var& eles = con[id];
				Conarray<T>(eles).Remove(id1);
				//return v;
			}
			/*template<typename T>
				T& CreateEntity(Entity& res) {
				assert(T::type != Maxid);
				const var id = T::type;
				var& eles = con[id];
				T& v = eles.add<T>();
				return v;
			}*/
			template<typename T, typename T2 = SubclassDesc>
			T& CreateEntity(T2& subdes) {
				assert(T::type != Maxid);
				const var id = T::type;
				//auto conaty = GetComps<T>();

				var& eles = con[id];
				T& v = eles.add<T>();
				return v;
			}
		};

		vector<Conany> ECSManager::con;
	}
	template<typename T, typename... Args>
	class Event {
		//Util::WrapFuncObj<T, void, Args...>f;
		T(*f)(void*, Args...);
		void* receiver;
	public:
		Event(T(*t)(Args...)) {
			f = t;
		}
		Event() {
			f = NULL;
		}
		T operator()(Args&&... args) {
			return f(receiver, std::forward<Args>(args)...);
		}
		inline operator const bool() {
			return f;
		}
		//void operator =(T(*t)(void*,Args...), const void* rec) { f = t; receiver = rec; }
		//void set(T(*t)(const void*,Args...),  void* rec) { f = t; receiver = rec; }
		void set(T(*t)(void*, Args...), void*& rec) {
			f = t;
			receiver = rec;
		}
		void reset() {
			f = NULL; receiver = NULL;
		}
	};

	template <class... _Types>
	struct CompPackage//:public CompPackageBase<_Types...>
	{
		using v_ele = Utility::tup<_Types...>;
		template<class T>
		using t_container = std::vector<T>;
		static constexpr math::ui64 CmpSize() {
			return v_ele::size();
		}
		std::tuple<t_container<_Types>...>pkg;
		//v_ele _tukg;
		CompPackage(math::ui64 num = 0) {
			ECS::visit<CmpSize()>([this, &num](auto I) {
				std::get<I>(pkg).resize(num);
				});
		}
		/*constexpr auto operator[](std::size_t i) {
			return pkg.get(i);
		}*/
		void reserve(const math::ui64& num) {
			ECS::visit<CmpSize()>([&](auto I) {
				get<I>().reserve(num);
				});
		}
		constexpr auto operator[](std::size_t i) {
			return get_p(i, std::make_index_sequence<sizeof...(_Types)>{});
		}
		inline auto add() {
			ECS::visit<CmpSize()>([&](auto I) {
				get<I>().emplace_back();
				});
			return (size() - 1);
			//return std::forward_as_tuple(std::get<_Types>(pkg).emplace_back(),...);;
			//auto cp = ECS::CompAccessor(getcp< _Types>().subspan(items, nm)...);
			//return cp;
		}

		inline auto uadd() {
			ECS::visit<CmpSize()>([&](auto I) {
				get<I>().emplace_back();
				});
			return (size() - 1);
		}
		inline auto addget() {
			ECS::visit<CmpSize()>([&](auto I) {
				get<I>().emplace_back();
				});
			return get(size() - 1);
			//return std::forward_as_tuple(std::get<_Types>(pkg).emplace_back(),...);;
			//auto cp = ECS::CompAccessor(getcp< _Types>().subspan(items, nm)...);
			//return cp;
		}
		template <class T>
		constexpr auto& get(const std::uint64_t& i) {
			return std::get<std::vector<T>>(pkg)[i];
		}
		template <typename... Ts,
			typename std::enable_if_t<sizeof...(Ts) >= 2, bool> = true>
			constexpr auto get(const std::uint64_t i, std::tuple<Ts*...>& ot) {
			((std::get<Ts*>(ot) = &std::get<std::vector<Ts>>(pkg)[i]), ...);
		}

		template <typename... Ts,typename std::enable_if_t<sizeof...(Ts) >= 2, bool> = true>
			constexpr auto get(const std::uint64_t i) {
				return std::forward_as_tuple(std::get<t_container<Ts>>(pkg)[i]...);
		}
		template <typename... Ts, typename std::enable_if_t<sizeof...(Ts) >= 2, bool> = true>
		constexpr auto getcomp() {
			return std::forward_as_tuple(std::get<t_container<Ts>>(pkg)...);
		}
		template <typename T>
		constexpr auto& getcomp() {
			return std::get<t_container<T>>(pkg);
		}
		template <typename T>
		constexpr auto& get() {
			return std::get<t_container<T>>(pkg);
		}
		constexpr auto get(const std::uint64_t i) {
			constexpr int tsz = sizeof...(_Types);
			return get_p(i, std::make_index_sequence<tsz>{});
		}
		template<class... _Types0>
		constexpr auto gettup(const std::uint64_t i) {
			return std::forward_as_tuple(std::get<t_container<_Types0>>(pkg)[i]...);
			//return std::get<I>(pkg)[i];
		}
		template<size_t I>
		constexpr auto&& get(const std::uint64_t i) {
			return std::get<I>(pkg)[i];
		}
		template<size_t I = 0>
		constexpr auto&& get() {
			return std::get<I>(pkg);
		}
		constexpr auto get_v(const std::uint64_t i) {
			return get_v(i, std::make_index_sequence<sizeof...(_Types)>{});
		}
		//template<size_t I = 0, class ...Tp>
		//constexpr void set(const std::size_t i, Tp&&... _Args) {
		//	if constexpr (I < sizeof...(Tp)) {
		//		std::get<I>(pkg)[i] = std::get<I>(std::forward_as_tuple(std::forward<Tp>(_Args)...));;
		//		set<I + 1>(i, _Args...);
		//	}
		//	//set(i, std::make_index_sequence<sizeof...(_Types)>{}, std::forward<_Types>(_Args)...);
		//}
		template<size_t I = 0, class I0>
		constexpr void set(const I0& v2, const v_ele& retu) {
			if constexpr (I < sizeof...(_Types)) {
				(std::get<I>(pkg)[v2] = std::get<I>(retu));
				set<I + 1>(v2, retu);
			}
		}
		template<size_t I = 0, class I0>
		constexpr void setv(const I0& v2, v_ele& retu) {
			if constexpr (I < sizeof...(_Types)) {
				(std::get<I>(retu) = std::get<I>(pkg)[v2]);
				setv<I + 1>(v2, retu);
			}
		}
		constexpr auto size() {
			return std::get<0>(pkg).size();
		}
		static constexpr auto se() {
			return sizeof...(_Types);
		}
		template<size_t I = 0>
		constexpr void swap(int v1, int v2) {
			if constexpr (I < sizeof...(_Types)) {
				std::swap(std::get<I>(pkg)[v1], std::get<I>(pkg)[v2]);
				//std::get<I>(v2) = tmv;
			/*std::get<I>(v2)=tmv;*/
				swap<I + 1>(v1, v2);
			}
		}
		template<class I0>
		constexpr void copy(const int& v1, const I0& v2) {
			cpy(v1, v2, std::make_index_sequence<sizeof...(_Types)>());
		}
		template<size_t I = 0, class I0>
		constexpr void move(const I0& dist, I0 Src, const I0& size) {
			if constexpr (I < sizeof...(_Types)) {
				const auto bytesize = sizeof(std::tuple_element_t<I, v_ele>) * size;
				(std::memmove(&std::get<I>(pkg)[dist - size], &std::get<I>(pkg)[Src], bytesize));
				move<I + 1>(dist, Src, size);
			}
		}
		void resize(math::ui64 num) {
			ECS::visit<CmpSize()>([&](auto I) {
				std::get<I>(pkg).resize(num);
				});
		}
	private:
		template <std::size_t... I>
		constexpr auto cpy(const int& v1, const int& v2, std::index_sequence<I...>) {
			((std::get<I>(pkg)[v1]= std::get<I>(pkg)[v2]),...);
		}
		template<size_t... Is>
		constexpr auto&& get_p(const std::uint64_t i, std::index_sequence<Is...>) {
			return  std::forward_as_tuple(std::get<Is>(pkg)[i]...);
		}

		template<size_t... Is>
		constexpr auto&& get_v(const std::uint64_t i, std::index_sequence<Is...>) {
			return  v_ele(std::get<Is>(pkg)[i]...);
		}
	};

	template <class... _Types>
	struct CompPackage<std::tuple<_Types...>> :public CompPackage<_Types...> {
		//CompAccessor<std::vector<_Types>...> pkg;

		using v_ele = Utility::tup<_Types...>;
		static constexpr math::ui64 CmpSize() {
			return v_ele::size();
		}
		CompPackage(math::ui64 num = 0) {
			ECS::visit<CmpSize()>([this, &num](auto I) {
				std::get<I>(t_Par::pkg).resize(num);
				});
		}
		/*constexpr auto operator[](std::size_t i) {
			return t_Par::pkg.get(i);
		}
		//template<size_t I = 0>
		template<size_t I = 0>
		constexpr auto&& get(const std::uint64_t i) {
			return std::get<I>(t_Par::pkg)[i];
		}
		template<size_t I = 0>
		constexpr auto&& get() {
			return std::get<I>(t_Par::pkg);
		}*/
		//template<size_t I = 0, class ...Tp>
		//constexpr void set(const std::size_t i, Tp&&... _Args) {
		//	if constexpr (I < sizeof...(Tp)) {
		//		std::get<I>(t_Par::pkg)[i] = std::get<I>(std::forward_as_tuple(std::forward<Tp>(_Args)...));;
		//		set<I + 1>(i, _Args...);
		//	}
		//	//set(i, std::make_index_sequence<sizeof...(_Types)>{}, std::forward<_Types>(_Args)...);
		//}
		constexpr const auto size() {
			return std::get<0>(t_Par::pkg).size();
		}
		static constexpr auto se() {
			return sizeof...(_Types);
		}
		template<size_t I = 0>
		constexpr void swap(int v1, int v2) {
			if constexpr (I < sizeof...(_Types)) {
				std::swap(std::get<I>(t_Par::pkg)[v1], std::get<I>(t_Par::pkg)[v2]);
				//std::get<I>(v2) = tmv;
			/*std::get<I>(v2)=tmv;*/
				swap<I + 1>(v1, v2);
			}
		}
	private:
		using t_Par = CompPackage<_Types...>;
		/*template<size_t... Is>
		constexpr auto&& get_p(const std::uint64_t i, std::index_sequence<Is...>) {
			return  std::forward_as_tuple(std::get<Is>(t_Par::pkg)[i]...);
		}

		template<size_t... Is>
		constexpr auto&& get_v(const std::uint64_t i, std::index_sequence<Is...>) {
			return  v_ele(std::get<Is>(t_Par::pkg)[i]...);
		}*/
	};

	using uint = math::uint;
	template<class Idtype = uint, class Cont = ECS::Conany_thsafe>
	struct archs
	{
		using t_Con = Cont;
		using mapi = std::unordered_map<Idtype, Idtype>;//phmap::flat_hash_map<Idtype, Idtype>;
		std::atomic_ullong items;
		uint _tid = -1;

		uint _comps;
		std::vector<Cont> ts;
		//std::array<uint,16> Compids;
		std::vector<void (*)(void*)> constructors;
		std::vector<void (*)(void*)> destructors;
		//archs CompAccessor() {}
		archs(uint comps_ = 0) :_comps(comps_) { items = 0; }
		archs(archs<Idtype, Cont>&& ach) {
			using self = archs<Idtype, Cont>;
			//std::memcpy(this,&ach,sizeof(self));
			self& ts = *this;
			ts.constructors = ach.constructors;
			ts.destructors = ach.destructors;
			ts._comps = ach._comps;
			ts._tid = ach._tid;
			ts.ts = std::move(ach.ts);
			ts.tmpi = std::move(ach.tmpi);
			ts.items = (ach.items._Storage._Value);
			//ach.ts..clear();
		}
		//archs(uint comps_ = 0) :_comps(comps_) {}
		//archs(){}
		/*	template <class T>
			void rig() {
				Compids.emplace_back(getid<T>);
			}*/
		template<class T>
		void adcp(const size_t& tid, const size_t reservenum, const size_t bksize) {
			constructors.emplace_back(create<T>);
			destructors.emplace_back(des<T>);
			tmpi.emplace(getid<T>(), _comps);
			ts.emplace_back(sizeof(T), tid, reservenum, bksize);
			//Compids[_comps] = i;
			//auto it= tmp.try_emplace(i, sizeof(T), _comps);
			//ts.emplace_back(it.first->second);
			_comps++;
		}
		/*template <class... _Types>
		auto Reader() {
			return ECS::CompAccessor1(Conarray<_Types, Cont>(getcp<_Types>())...);
		}*/
		//template <E>
		//template <class T>
		//void rig(Idtype reservenum, const size_t tid, const size_t bksize = 512) {
		//	if constexpr (T::t_sz() == 0) {
		//		return;
		//	}
		//	_tid = tid;
		//	using tup = typename T::typkg;
		//	ECS::visit_all<T>([&](auto I) {
		//		adcp<typename T::template elei<I>>(tid, reservenum, bksize);
		//		});
		//	//const tup tp;
		//	//_tid = tid;
		//	//constexpr auto d = sizeof(bksize);
		//	////const int tsz = std::tuple_size_v<tup>;
		//	//std::apply([&](auto ... args) {
		//	//	 (adcp<decltype(args)>(tid, reservenum, bksize),...);
		//	//	},tp);
		//}
		//template <>
		//void rig(Idtype reservenum, const size_t tid, const size_t bksize = 512) {
		//	//constexpr int tpsz= std::tuple_size_v<std::tuple<_Types&...>>;
		//	/*(adcp< _Types>(getid<_Types>()), ...);
		//	add<_Types...>(reserve);*/

		//}
		auto reserve(int nm = 1) {
			for (size_t i = 0; i < ts.size(); i++)
			{
				auto& con = ts[i];
				Cont& ery = con;
				ery.reserve(nm);
			}
		}
		template<class EC, class... _Types>
		auto getComp(const size_t i) {
			//using ecls = ECS::ECSclass<int,bool>;
			return std::forward_as_tuple(ts[EC::template Id<_Types>].template Get<_Types>(i)...);
		}
		template<class EC, class T>
		inline auto&& get1Comp(const size_t i) {
			//using ecls = ECS::ECSclass<int,bool>;
			return ts[EC::template Id<T>].template Get<T>(i);
		}
		template<class EC, class T>
		inline auto& getComps() {
			//using ecls = ECS::ECSclass<int,bool>;
			return ts[EC::template Id<T>];
		}
		//template<class EC, class T>
		//inline auto& getComps() {
		//	//using ecls = ECS::ECSclass<int,bool>;
		//	return ts[EC::template Id<T>];
		//}
		template<class EC, class... _Types>
		inline auto getCompFrom(const size_t i) {
			return std::forward_as_tuple(ts[EC::template Id<_Types>].template Get<_Types>(i)...);
		}
		template<class EC>
		constexpr auto getComps(const size_t i) {
			return ECS::CompAccessor<EC>::get(i, ts);
		}

		//template<class EC>
		//constexpr auto getComps(const size_t i) {
		//	return ECS::CompAccessor<EC>::get(i, ts);
		//}
		/*template<class EC,>
		constexpr auto&& getComps(const size_t i) {
			return ECS::CompAccessor<EC>::get(i, ts);
		}*/
		constexpr auto size() {
			return  items._Storage._Value;
		}
		template<class T>
		constexpr auto getcp() {
			return ECS::Conarray<T, Cont>(ts[tmpi.find(getid<T>())->second]);
			//auto&& con = ts[tmpi.find(getid<T>())->second];
			//T* p = (T*)con.data();
			//return Utility::span<T>((T*)con.data(), con.size());;
		}
		template<class T>
		constexpr auto& getrowcp() {
			return (ts[tmpi.find(getid<T>())->second]);
			//auto&& con = ts[tmpi.find(getid<T>())->second];
			//T* p = (T*)con.data();
			//return Utility::span<T>((T*)con.data(), con.size());;
		}
		template<class T>
		constexpr auto try_getcp() {
			auto fd = tmpi.find(getid<T>());
			if (fd != tmpi.end()) {
				return fd->second;
			}
			return -1;
			//auto&& con = ts[tmpi.find(getid<T>())->second];
			//T* p = (T*)con.data();
			//return Utility::span<T>((T*)con.data(), con.size());;
		}
		constexpr Cont& getcp(const uint id) {
			return ts[tmpi.find(id)->second];
		}
		//template <class... _Types>
		auto add(int nm = 1) {
			auto st = items.fetch_add(nm, std::memory_order_relaxed);
			for (size_t i = 0; i < ts.size(); i++)
			{
				auto& con = ts[i];
				Cont& ery = con;
				//ery.reserve(nm + st);
				ery.addrow(nm + st);
				for (size_t i1 = st; i1 < nm; i1++)
				{
					constructors[i](con.Get(i1));
					//std::memcpy(s + ery.Alignment() * i, ery.Get(0), ery.Alignment());
				}

			}
			return st;
			//auto cp = ECS::CompAccessor(getcp< _Types>().subspan(items, nm)...);
			//return cp;
		}
		//template<class EC, int sz = std::tuple_size_v<typename EC::typkg>>
		//auto add(const size_t nm = 1) {
		//	assert(sz == ts.size());
		//	auto st = items.fetch_add(nm, std::memory_order_relaxed);
		//	for (size_t i = 0; i < sz; i++)
		//	{
		//		auto& con = ts[i];
		//		Cont& ery = con;
		//		//ery.reserve(nm + st);
		//		ery.addrow(nm + st);
		//		size_t i1 = st;
		//		st += nm;
		//		for (; i1 < st; i1++)
		//		{
		//			constructors[i](con.Get(i1));
		//			//std::memcpy(s + ery.Alignment() * i, ery.Get(0), ery.Alignment());
		//		}
		//	}
		//	return st;
		//}
		/*template <class... _Types>
		constexpr auto Cmps() {
			return ECS::CompAccessor(getcp<_Types>()...);
		}*/
		/*template <class T, class T1>
		constexpr auto& CmpFrom() {
			return ts[Index_v<T1, T::typkg>];
		}*/
		//~archs()
		//{
		//	for (size_t i = 0; i < ts.size(); i++)
		//	{
		//		auto& con = ts[i];
		//		Cont& ery = con;
		//		const auto s = ery.addrow(nm);
		//		for (size_t i1 = s; i1 < nm; i1++)
		//		{
		//			constructors[i](con.Get(i1));
		//			//std::memcpy(s + ery.Alignment() * i, ery.Get(0), ery.Alignment());
		//		}
		//	}
		//}
	private:
		/*template <size_t _Idx,class EC>
		constexpr auto&& Cmpele(const size_t i) {
			return ts[EC::template Id<_Types>].Get<_Idx>(i);
		}*/
		template<typename T>
		static uint getid() {
			static const auto id = Next();
			return id;
		}
		static uint Next() {
			static uint val = 0;
			return val++;
		}
		/*template<class T>
		constexpr auto TSpan(ECS::Conany& da, Idtype start = 0, Idtype sz = items) {
			return Utility::span<T>(&(da.Get<T>(start)), sz);
		}*/
		template<class T>
		static void create(void* P)
		{
			new(P)T();
		}
		template<class T>
		static void des(void* p)
		{
			delete reinterpret_cast<T*>(p);
		}
		mapi tmpi;
		//map tmp;
	};

	using dfarchs = archs<>;
	struct RingBufferAllo
	{
		std::atomic_llong ix0;
		std::size_t beg = 0, end = 0;
#ifdef DEBUG
		std::vector<bool> acs;
#endif // DEBUG

		//AppendArray<>
		void* allocate(Conany& any) {
			if (end >= any.Capacity()) {
				end = 0;
			}
			const auto capa = any.Capacity();
			const auto ed = end++ % capa;
#ifdef DEBUG
			assert(!acs[ed]);
			acs[ed] = true;
#endif

			return any.Get(ed);
		}
		void deallocate(Conany& any) {
			const auto bg = beg %= any.Capacity();;
#ifdef DEBUG
			acs[bg] = false;
#endif

			beg++;

		}
	};
	struct typedata
	{
		uint tyid;
		typedata(const uint id) :tyid(id) {}
		inline operator uint& () {
			return tyid;
		}
	};
	template <typename Derived>
	struct Component0
	{
		/*static union
		{
			uint4 ids;
			struct
			{
				uint type,subtype;
			};
		}; */
		using self = Derived;
		//static uint type;
		static typedata type;
		static uint& Gettype() {
			return type;
		}
	};
	template <typename T> typedata Component0<T>::type(Maxid);
	constexpr uint dfid = Maxid;
}