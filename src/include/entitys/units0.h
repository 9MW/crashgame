#pragma once
#include "comm.h"
#include"Comps1.h"
#include"unit.h"
//#include"GObjs0.h"
#include"io/AudioManager.h"
#include"io/loader.h"
namespace pj2 {
	namespace n_BasObj {
		template<class sclass0, class Eclass0>
		struct Ubase0 :obj<Eclass0>, n_BasObj::ecsphysobj
		{
			using Eclass = Eclass0;
			using sclass = sclass0;
			//int cmdbufid=-1;

			//static int update(Manager::UpdatePara const* par);
			//static int update2(Manager::UpdatePara const* par);
			///*int OnEquip(primaryweapon& wp, GameProps::BasicProp& sfpeop){
			//	prop0.get<PropID::feq>() = ECS::derif<Eclass, primaryweapon>::feq();
			//}*/
			//static inline int SwiState() {}
			//static int Build(Manager::UpdatePara const* par, const mtransfrom& trans_) {
			//	Manager::UpdatePara const& para = *par;
			//	auto& achs = para.dfac;
			//	auto&& tuptop = Manager::Cmps<Eclass>(achs, para.id);
			//	auto&& prop0 = tuptop.get<GameProps::BasicProp>();
			//	auto&& trans = tuptop.get<mtransfrom>();
			//	auto&& renderstate = tuptop.get<n_objProp::dRender>();
			//	renderstate.audiosrc = -1;
			//	renderstate.color = { 0,1,0,0.5 };
			//	trans = trans_;
			//	prop0.get<PropID::feq>() = ECS::derif<Eclass, primaryweapon>::feq();
			//	return 0;
			//}
			//static int Build(Manager::t_Arch& achs, const int& id, const mtransfrom& trans_) {
			//	auto&& tuptop = Manager::Cmps<Eclass>(achs, id);
			//	auto&& prop0 = tuptop.get<GameProps::BasicProp>();
			//	auto&& trans = tuptop.get<mtransfrom>();
			//	auto&& renderstate = tuptop.get<n_objProp::dRender>();
			//	renderstate.audiosrc = -1;
			//	renderstate.color = { 0,1,0,0.5 };
			//	trans = trans_;
			//	//prop0.get<PropID::feq>() = ECS::derif<Eclass, primaryweapon>::feq();
			//	return 0;
			//}
			//namespace ENU=GameProps::ENU;
			static inline int animated(const state s) {
				constexpr int start = (int)(AnimState::dieing);
				return (int)s - start < AStateCount ? (int)s - (int)start : -1;
			}
		};

		struct prefunit {
			using tp = n_BasObj::Cmp_unit::typkg;
			n_BasObj::Cmp_unit::typkg datup;
			operator tp& () { return datup; }
		};
		template<class Eclass0>
		struct Ubase1 :public rpoolobj<Eclass0>, Ubaseanim, Eclass0// n_BasObj::ecsphysobj
		{
			inline auto& Prefabs() { return this->prefabs; }
			int equipwith() {

			}
			using unpackTup = Eclass0::typkg;
			using Tpref = typename ECS::cat<unpackTup, PrefDesc>::tup2;
			std::vector<Utility::MyTuple<Tpref>> prefabs;
		};
	}
	namespace n_units {
		using namespace GameProps::ENU;
		using namespace n_BasObj;
		using  GameProps::ubuilder0;
		using  GameProps::unitdesc;
		using n_BasObj::prefunit;
		struct visibleTargets
		{
			int _size;
			GameProps::EntityId eids[15];
			inline const int& size()const { return _size; }
			inline auto operator[](const  int& i) { return eids[i]; }
		};
		struct audiosrc
		{

		};
		struct repoele
		{
			std::string Name;
			GameProps::equipment eq;
			int num=0;
			int stack(GameProps::IItem& item) {
				if (item.Eqprefid.hashnum == eq.Eqprefid.hashnum&& num<eq.maxstack&&num<eq.items.size()) {
					eq.items[num] = item.cdata[0];
					num++;
					return 1;
				}
				return -1;
			}
			repoele() {
				std::memset(this, 0, sizeof(*this));
			}
			repoele(const repoele& re) {
				std::memcpy(this, &re, sizeof(re));
			}
			//void Seri(void*js,void* ct)
			int set(GameProps::IItem& item) {
				assert(num == 0);
				item.Eqprefid = eq.Eqprefid;
					eq.items[num] = item.cdata[0];
					num++;
					return 1;
			}
			inline bool operator==(const  Equip& eqi) { return false; }//eqi==eq.eqtp; }
		};
		struct repopara1
		{
			int usself;

		};
		template<int cap>
		struct repo;
		template<>
		struct repo<0> {

		};
		using bepo = repo<0>;
		//using repoele = GameProps::equipment;
		template<int cap>
		struct repo:bepo
		{
			using repodef = GameProps::soltDef;
			array<repoele, cap> elements;
			math::Fix::fix_s weight, vol,maxweight;
			i64 selecteditem = -1;
			repo() {

			}
			template<class T>
			void onclick(const ui64 i, const repopara1& ac, T& para) {

				repoele& e = elements[i];
				selecteditem = i;
				ECS::visit_at<(int)Equip::count>(e.eq.eqtp, [&](auto I) {
					//e.eq.use<(Equip)I>();
					});
			}
			/*void Seri(void* js){}
			void DeSeri(void* js){}*/
			inline repoele& get(const size_t num) {
				return elements[num];
			}
			inline auto size() { return cap; }
			template<int I>
			inline repoele& get() { return elements[I]; }
			/*void remove(size_t i){
				while ((i+1)<cap)
				{
					repoele& next = elements[i + 1];
					repoele& po = elements[i];
					if (next.eq.weight <= 0) { break; }
					po = next;
					i++;
				}
			}
			template<Equip teq,class F >
			void use(F&& f) {
				if (weight <= 0)
					return ;
				for (size_t i = 0; i < cap; i++)
				{
					repoele& p = elements[i];
					if (p == teq) {
						f(p);
						p.num--;
						if (p.num <= 0) {
							remove(i);
						}
						break;
					}
				}
			}
			inline bool find(int i) {

				for (size_t i = 0; i < cap; i++)
				{
					repoele& p = elements[i];
					if (p == i)
						return true;
				}
			}
			template<class F>
			void use(F&& f) {
				if (weight <= 0)
					return;
				for (size_t i = 0; i < cap; i++)
				{
					repoele& p = elements[i];
					if (f(p)) {

						p.num--;
						if (p.num <= 0) {
							remove(i);
						}
						break;
					}
				}
			}
			void PutIn(Objs::equpbuf& fes) {

			}*/
		};
		using df_repo = repo<16>;
		/*using Cmp_unit = ECS::ECSclass<pj2::AI::postp,
			Ub0<GameProps::ArDm<Armor::nan, t_Dmg::nan>>, sStruct::instData, GameProps::deltaProp, GameProps::BasicProp>;*/

		struct plclose :n_BasObj::Ubase1<ECS::ECSclass<n_BasObj::Cmp_unit, n_weapon::gun0, df_repo>> {
			void dfd();
			int update(const size_t& W) { return 0; }
		};
		struct pl0 :muzzle4, public n_BasObj::Ubase1<ECS::ECSclass<n_BasObj::Cmp_unit, n_weapon::gun0, df_repo, targetSensor>> {
			template<typename... Ts>
			inline void Copy(const int& i, std::tuple<Ts...>& src) {
				((comps.get <Ts>(i) = std::get<Ts>(src)), ...);
			}
			pl0(){}
			pl0(const pj2::n_units::pl0& p) { *this = p; }
			void operator =(const pl0& p) {
				comps = p.comps;
				this->muzzels = p.muzzels;
			}
			inline void Copy(const int& dest, const int& src) {
				comps.copy(dest, src);
			}
			int update(const size_t& W);
			/*template<class Tp>
			static int Build(Manager::t_Arch& achs, const int& id, Tp& arg);*/
			/*static int Build(Manager::t_Arch& achs, const int& id, ubuilder0& arg) {
				const mtransfrom& trans_ = Utility::get<mtransfrom>(arg);
				Bs::Build(achs, id, trans_);
				auto&& tuptop = Manager::Cmps<cmps>(achs, id);
				unitdesc& ud = Utility::get<unitdesc>(arg);
				auto& wd = Utility::get<GameProps::weaponpara>(arg);
				auto&& gun = tuptop.get<n_weapon::gun0>();
				gun.audiolength = wd.audiolength;
				gun.audiosrc = wd.audiosrc;
				gun.bullet = 10000;
				auto&& [dp, bp] = achs.template
					getCompFrom<cmps, GameProps::deltaProp, GameProps::BasicProp>(id);
				bp = ud.peop;
				//equip
				{
					bp.get<PropID::feq>() = wd.freq;
				}
				//gun.audiosrc=
				return 0;
			}
			void build(Manager::t_Arch& ach, const int& id, const mtransfrom& pos);

			static int update(Manager::UpdatePara const* par);
			int update(const int& id);
			static int Render(Manager::RenderPara& rp,n_Render::rdbatch& rb) {
				{
					Diligent::MapHelper<mtransfrom> tranbuf(rp.rct.m_pImmediateContext, rb.Bufs[0], MAP_TYPE::MAP_READ, MAP_FLAGS::MAP_FLAG_DO_NOT_WAIT);
				}

				rp.Barriers.emplace_back(rb.Bufs[0], RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_SHADER_RESOURCE, STATE_TRANSITION_FLAG_UPDATE_STATE);
			}*/
			/*static int update(Manager::UpdatePara const* ptrup) {

			}*/
		};

	}

}