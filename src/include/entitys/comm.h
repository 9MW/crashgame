#pragma once

#include"Gameprops0.h"
#include"Common/ecs.hpp"
#include"Render/ShaderStructs.h"
#include<AI/Config.h>
#include"Render/RenderCommon.h"
#include"Common/UtilMT.h"
#include"io/AudioManager.h"
#include"Graphic/modelcom.h"
namespace pj2 {
	template <typename... Ts, typename std::enable_if_t<sizeof...(Ts) >= 2, bool> = true>
	constexpr auto get(auto& pkg) {
		return std::forward_as_tuple(std::get<Ts>(pkg)...);
	}
	template <typename  T>
	constexpr auto& get(auto& pkg) {
		return (std::get<T>(pkg));
	}
	namespace n_objProp {
		template<class ...Ts>
		struct tinteract :Utility::MyTuple<Ts...> {
			constexpr auto size() { return sizeof...(Ts); }
		};

		template<>
		struct tinteract <> {
			constexpr auto size() { return 0; }
		};
		//struct unitcp0
		//{
		//	Manager::ObjState _state;
		//	GameProps::Cmd0 currentCmd;
		//	//state _aistate;
		//	//set 0 when switch state
		//	//animation switching minus tick determine if state is fully switched
		//	//switch time could be find based on previous and target state
		//	//state switch need wait after minimal allow current state tick
		//	int _statetick, CommandId;
		//};
		struct archstate
		{
			uint a_frstart, size;
		};
		struct staticuinfo
		{
			std::array<archstate, (int)(GameProps::ENU::state::count)> ds;
			staticuinfo() {
				memset(ds.data(), 0, sizeof(ds));
			}
		};
		struct sren
		{
			int modid, renderid;
		};
		using dRender = sStruct::instData1;
	}
	struct PrefDesc
	{
		std::string Name;
	};
	template <typename... Ts, typename std::enable_if_t<sizeof...(Ts) >= 2, bool> = true>
	constexpr auto buildat(const std::uint64_t& i, auto& inc, auto& tup) {
		((inc.getcmp<Ts>(i) = std::get<Ts>(tup)), ...);
	}
	namespace n_BasObj {
		struct baseobj {
			using cmps = void;
			void onRegist(void* p) {}
		};
		struct gcobj
		{

		};
		template<class ...Ts>
		struct obj;
		template<class ...Ts>
		struct obj<ECS::ECSclass<Ts...>> :baseobj {
			using cmps = ECS::ECSclass<Ts...>;
			using unpackTup = cmps::typkg;
			using Tpref = typename ECS::cat<unpackTup, PrefDesc>::tup2;
			using t_interact = n_objProp::tinteract<>;
			using t_cpkgs = ECS::CompPackage<unpackTup>;
			using EntityId = GameProps::EntityId;
			ECS::CompPackage<unpackTup> comps;
			int tid = -1;//used for GPU Cullin
			int behid, gid;//crresponding to Controller var
			EntityId ient;
			//std::vector<Utility::MyTuple<Tpref>> prefabs;
			/*static int update(Manager::UpdatePara const*) {
				return 0;
			}*/
			void onRegist(void* p) {
				ient.tid0 = behid;
				ient.tid = gid;
			}
			inline int Build() {
				const auto d = comps.add();
				return d;
			}
			EntityId& GetEnt() { return ient; }
			template<class T>
			static constexpr int hasClass(){
				int i = -1;
				ECS::visit_all_convertible<T, cmps>([&i](auto I) {i = I; });
				return i;
			}
			int update(const size_t& W) {
				return 0;
			}
			inline auto size() {
				return comps.size();
			}
			static  std::string* bename() {
				return NULL;
			}
			inline auto& gettid() { return tid; }
			template<class T>
			inline T& getcmp(const size_t& W) {
				return comps.get<T>(W);
			}

			inline auto& getcmp(const size_t& W) {
				return comps.get(W);
			}
			inline void resize(math::ui64 num) { comps.resize(num); }
			inline void reserve(math::ui64 num){
				comps.reserve(num);
			}
			template <typename... Ts, typename std::enable_if_t<sizeof...(Ts) >= 2, bool> = true>
			constexpr auto getcmps(const std::uint64_t& i) {
				return comps.get<Ts...>(i);
			}
			template <typename... Ts, typename std::enable_if_t<sizeof...(Ts) >= 2, bool> = true>
			constexpr auto getcmp(const std::uint64_t& i) {
				return comps.get<Ts...>(i);
			}
			template<class T>
			constexpr auto& getcomp() {
				return comps.getcomp<T>();
			}
			template<class T>
			constexpr static bool Hascomp() {
				return (ECS::convertible<T, Ts>() || ...);
			}
			template<class... _Types>
			inline void addPrefab(_Types&&... _Args) {
			}
			template<class T, class T1>
			inline auto& getComp(T1& tup) { return std::get<T&>(tup); }
		};
		template<class ...Ts>
		struct ecsobj:obj<ECS::ECSclass<Ts...>> {
		};
		using GameProps::ENU::state;
		constexpr int vilstate = (int)state::count;
		struct Renderble
		{
			math::v4f BBoxhalf;
			mtransfrom BoxTrans;
			std::array<Audio::audioinfo, vilstate> audioladrss;
			std::array<std::string, vilstate> audnames;
			std::string Name, model;
			GameProps::RenderIf irender = -1, modelid = -1;
			Renderble() {
				BBoxhalf.setConstant(0.5);
			}
		};
		struct pool {
			using T = uint;
			aint poolc,pd;
			aint activemax;
			std::vector<math::uint> _pooldel;
			std::vector<math::uint> _extractpool;
			pool(int maxcap = 10000,int maxdel = 2000) {
				poolc.t = 0;
				_pooldel.resize(maxdel);
				_extractpool.resize(maxcap);
			}
			int Put(uint id) {
				auto c = poolc.add1();
				if (c>= _pooldel.size())
				{
					std::abort();
				}
				else {
					_pooldel[c]=(id);
				}
				return 1;
			}
			template<class ...Ts>
			int poolresize(Ts... args);
			int Get() { 
				auto c =pd.mns1()-1;
			 return c < 0 ? -1: _extractpool[c];
			}
			int Get(Utility::span<T> tout) {
				auto c = 3;// pd.mns1();
				return c < 0 ? -1 : _extractpool[c];
			}
			inline int poolget() {
				auto pex = this->Get();
				return pex <= 0 ? activemax.add1() : pex;
			}
			//thread unsafe function
			template<class ...Ts>
			int uusf(obj<Ts...>& tt) {
				if (tt.size() < activemax()) {
					tt.resize(activemax());
				}
				if (poolc() > _pooldel.size()) {
					_pooldel.resize(poolc());
				}
				spin();
				return 1;
			}
			//dump _pools=>_poolsdele
			//not a threadsafe call
			void spin() {
				using T = uint;
				auto& pv = pd.t._Storage._Value;
				auto& pv1 = poolc.t._Storage._Value;
				pv = pv < 0 ? pv1 : pv+pv1;
				while (pv >(_extractpool.size() * 0.8)) {
					_extractpool.resize(_extractpool.size() * 2);
				}
				std::memcpy(&_extractpool[pv - pv1], &_pooldel[0], sizeof(T) * pv1);
				pv1 = 0;
			}
		};

		template<class ...Ts>
		struct poolobj :pool, obj<Ts...> {
			inline int uusf() {
				return pool::uusf(*this);
			}

			int Delete(uint id) {
				pool::Put(id);
				return 1;
			}
		};
		template<class ...Ts>
		struct rpoolobj :poolobj<Ts...> {
			inline int uusf() {
				return pool::uusf(*this);
			}
			int Delete(uint id) {
				auto& dr = this->getcmp<sStruct::instData1>(id);
				dr.tid = -1;
				pool::Put(id);
				return 1;
			}
		};
		/*template<class ...Ts>
		struct obj<std::tuple<Ts...>> :baseobj {
			using cmps = ECS::ECSclass<Ts...>;
			static int update(Manager::UpdatePara const*) {}
		};*/
		template<class ...Ts>
		struct tparaobj;
		template<class TSub, class ...Ts>
		struct tparaobj<TSub, ECS::ECSclass<Ts...>> {
			using eclass = ECS::ECSclass<Ts...>;
			using t_name = TSub;
		};
		template<class T>
		struct prefbs;
		template<class ...Ts>
		struct prefbs <ECS::ECSclass<Ts...>> {
			using unpackTup = ECS::ECSclass<Ts...>::typkg;
			using Tpref = typename ECS::cat<unpackTup, PrefDesc>::tup2;
		};
		struct Serilizable {
			int Ser(void* js) { return -1; }
			int DeSer(void* js) { return -1; }
		};
		using spocup = Utility::FixAy;
		using ecsphysobj = ECS::ECSclass<mtransfrom, Collid, Physics::LODShape, spocup>;
		using stphyobj = ECS::ECSclass<mtransfrom, Physics::SCollid>;
		using ecspoobserve = ECS::ECSclass<ecsphysobj, Physics::Lavnes>;
		using ecsVirobj = ECS::ECSclass<mtransfrom, Physics::VirtCollid, Physics::LODShape, spocup>;
		using ecsvirtualobserve = ECS::ECSclass<ecsVirobj, Physics::Lavnes>;
		using targetSensor =std::array<Physics::LAVNe,1>;
		//using RenderAble= ECS::ECSclass<>
	}
	namespace n_Component {
		using prefabID = GameProps::EntityId;
		struct BuildPara
		{
			math::v4f pos,tpos;
			prefabID hostid;
			//mtransform trans;
			//std::array<math::i4,4>adda;
			int pfid,id;
			uint begframe = -1;
		};
		using n_objProp::dRender;
		struct physiscsprop
		{
			mtransfrom::v4 vel;
		};
		struct ImpactInfo
		{
			ui64 frameImpact;
		};
		struct romdesc
		{
			std::string Name, Desc;
		};
		struct updateunsafe {
			inline int uusf() { return -1; }
		};
	}
}