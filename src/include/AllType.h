#pragma once
#include"entitys/Comps1.h"
#include"entitys/Comps2.h"
#include"entitys/units0.h"
#include"entitys/fermions.h"
#include"entitys/Commander.h"
#include"Render/Renders0.h"
#include"Render/fogRender.h"
#include"Render/LineRender.h"
namespace pj2::n_Registry {
	//struct Boid3 {};
	//using namespace pj2;
	using Registry = ECS::Registry<n_units::pl0, n_weapon::bullet0,
		fermion0,n_Objs::ObjBox,n_weapon::guns0, n_eneity::instructor>;
	using ReguRen = ECS::Registry<n_Objs::stateDesc>;
	//using Archtypes = ECS::Registry<n_units::pl0, n_weapon::bullet0,n_weapon::guns0,n_Objs::ObjBox>;
	/*template<typename ...Ts>
	struct Archtype ;*/
	constexpr bool dfd = ECS::convertible<n_Objs::stateDesc, n_BasObj::baseobj>();
	template<typename T>
	struct ArchtypeEC {
		static_assert(ECS::convertible<T, n_BasObj::baseobj>(), "illeigal T");
		T data;
		std::string Desc = R"(name
desc
			)";
	};
	template <class... _Types>
	struct ArchtypeEC<ECS::ECSclass<_Types...>> {
		using ecls = ECS::ECSclass<_Types...>;
		using etup = typename ecls::typkg;
		etup data;
		std::string Desc = R"(name
desc
			)";
	};
	template<typename T>
	struct Archtype {
		static_assert(std::is_same_v<T, n_Objs::stateDesc>,"illegal T");
		T data;
		int tid;
		std::string Desc = R"(name
desc
			)";
	};
	template <class... _Types>
	struct Archtype<ECS::ECSclass<_Types...>> {
		using ecls = ECS::ECSclass<_Types...>;
		using etup = typename ecls::typkg;
		etup data;
		int tid;
		std::string Desc = R"(name
desc
			)";
	};
	//template<typename ...Ts>
	//struct MyStruct;
	//template <class... _Types, class... _Types1>
	//struct MyStruct<ECS::Registry<_Types...>, _Types1...> :public Utility::MyTuple<std::vector<ArchtypeEC<typename _Types::cmps>>...,std::vector<_Types1>...> {
	//	using basetuple = std::tuple<Archtype<typename _Types::cmps>...>;

	//	template <class T>
	//	constexpr auto& get(const std::uint64_t& i) {
	//		return std::get<std::vector<T>>(pkg)[i];
	//	}
	//	template <typename... Ts,
	//		typename std::enable_if_t<sizeof...(Ts) >= 2, bool> = true>
	//	constexpr auto get(const std::uint64_t i, std::tuple<Ts*...>& ot) {
	//		((std::get<Ts*>(ot) = &std::get<std::vector<Ts>>(pkg)[i]), ...);
	//	}
	//	/*template<int I>
	//	using TIV = std::tuple_element_t<I, basetuple>::;*/
	//};
	template <class... _Types>
	struct Archtype<ECS::Registry<_Types...>> :Utility::MyTuple<std::vector<Archtype<std::conditional_t<std::is_void_v<typename _Types::cmps>,
		_Types, typename _Types::cmps>>>...>{
		using basetuple = std::tuple<Archtype<typename _Types::cmps>...>;
		/*template<int I>
		using TIV = std::tuple_element_t<I, basetuple>::;*/
	};
	using Archtypes = Archtype<typename Registry>;// MyStruct<typename Registry, n_Objs::stateDesc>;// ;

	using RenderReg = ECS::Registry<n_Render::SkinRender, n_Render::FixRender, n_Render::OlskRender,
		n_Render::fogRender,n_Render::LineRender>;
	template<class T>
	constexpr  std::size_t Idof = Registry::template Id<T>;
#define Tof(I) n_Registry::Registry::template t<I>;
	
//#define Deriv(T,base) T::template t_Basof<base>;
}