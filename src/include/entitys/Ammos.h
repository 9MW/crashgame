#pragma once

#include"comm.h"
#include"io/BSeri.h"
#include"entitys/unit.h"
#include"GObjs0.h"
#include"Common/UtilMT.h"
namespace pj2 {


	namespace n_weapon {
		using mtrans = mtransform;
		struct projectile :n_BasObj::Renderble {
			int build(const n_Component::BuildPara para) { return -1; }
		};
		template<class T>
		struct ptilptef;
		template<class ...Ts>
		struct ptilptef <ECS::ECSclass<Ts...>> :
			ECS::ECSclass<Ts...>,
			//ecsvirtualobserve,
			n_BasObj::rpoolobj<ECS::ECSclass<Ts...>>,
			n_seri::DependencyLevel<0>,
			projectile{
			using t_prefab = typename prefbs<ECS::ECSclass<Ts...>>::Tpref;
			using t_prefabs = std::vector<t_prefab>;
			t_prefabs prefabs;
			n_Component::romdesc desc;
			n_Component::romdesc& Desc() { return desc; };
			int Delete(uint id) {
				auto& dr = this->getcmp<sStruct::instData1>(id);
				dr.tid = -1;
				pool::Put(id);
				return 1;
			}
			//thread unsafe function
			int uusf() {
				pool::uusf();
				return 1;
			}
			/*int Build(int i, Utility::span<int> inew) {
				uint d, c = 0;
				auto szaft = _pool.size() - i;
				while (i > 0 && _pool.size() > 0)
				{
					d = _pool.back();
					_pool.pop_back();
					auto& dr = this->getcmp<sStruct::instData1>(d);
					dr.tid = this->tid;
					dr.id = d;
					inew[c++] = d;
					i--;
				}
				while (i > 0)
				{
					d = this->comps.add();
					auto& dr = this->getcmp<sStruct::instData1>(d);
					dr.tid = this->tid;
					dr.id = d;
					inew[c++] = d;
					i--;
				}
				return 1;
			}*/
		};
		using namespace math::Fix;
		template<int numprop = 8>
		struct gAmmo0//, n_BasObj::obj<Cmp_unit>
		{
			//using GameProps::ENU::t_Dmg;
			static constexpr int eft = 3;
			//static constexpr std::array<int, sizeof...(lo)> EffLoc = { (int)lo... };
			using t_data = Fix::fix<eft>;
			using Tbset = std::bitset<numprop>;
			//std::conditional_t < EffLoc.size() < 4,Fix::fix4,Fix::fix<EffLoc.size()>> datas;
			t_data datas;
			//Tbset peft;
			std::array<UCHAR, 4> peft;//0 for count ,follow by effet id
			fix_s Td, CrSum;
			int energy, activeCounter;
			uint frimpact, begframe = -1;
			GameProps::EntityId LastImpact;

			//static ui64 offaudio;
			static void* ptr_mesh;
			//static constexpr t_Dmg _tDmg = dmg;
			//df() {}
			inline gAmmo0() {

			}
			gAmmo0(const gAmmo0<numprop>& sf) {
				*this = sf;
			}
		private:
			using t_self = gAmmo0<numprop>;
			/*template<class t_self, class Registry, class Eclass>
			static int update(Manager::UpdatePara const* par) {

				auto& para = *par;
				auto&& [trans, aminfo] = Cmps<Eclass>(para.dfac, para.id).template getref<mtransfrom, t_self>();
				if (aminfo.frimpact < para.ct.freamid)
					return 1;

				mtransfrom& tgtrans = para.ct.template GetEnti<physobj<>>(aminfo.targetid).template get<math::mtransfrom>();
				auto& tgpos = tgtrans.pos;
				auto&& tgdead = tgtrans.Scale4.isZero();
				if (tgdead) {
					aminfo._state.SetState(state::died);
					return -1;
				}
				const auto dissq = math::distancesq(trans.pos, tgpos);
				if (dissq < aminfo.Tdsq && (math::sqrt(dissq) < (aminfo.CrSum))) {

					ECS::visit_convertible<Registry, n_BasObj::Cmp_unit>(aminfo.targetid.tid, [&, &aprop = aminfo](auto I) {
						using cmp = typename n_Registry::Registry::t<I>;
						using tprop = ECS::derif<cmp, n_BasObj::Ubase>;
						GameProps::deltaProp& dp = Cmps<cmp>(para.ct.getArch(aprop.targetid.tid), aprop.targetid.id).template get<GameProps::deltaProp>();

						t_self::IterEff([&](auto i, auto propid) {
							dp.atomic_rep[propid].fetch_add(-aprop.datas[i], std::memory_order_relaxed);
							});
						});
				}
				return 0;
			}*/
		};
		using namespace n_BasObj;
		using gammo = gAmmo0<8>;
		using b_bullet = ECS::ECSclass<ecsvirtualobserve, gammo, GameProps::Cmd0, sStruct::instData1>;
		struct fuse {
			b_bullet::typkg data;
		};
		struct bullet0 :ptilptef<b_bullet>,Serilizable,n_Component::updateunsafe
		{
			n_Component::BuildPara bp;
			int tid0;
			float vel;
			bullet0() {}
			//not thread safe
			//template<int tid0>
			int build(const n_Component::BuildPara para);
			int build(const n_Component::BuildPara para, cmps::typkg& archs);
			//thread unsafe function
			int uusf() {
				pool::uusf(*this);
				return 1;
			}

			/*int build(unpackTup& tup) {
				return 0;
			}*/
			inline auto& Prefabs() { return this->prefabs; }
			int equipwith() {

			}
			using unpackTup = b_bullet::typkg;
			using Tpref = typename ECS::cat<unpackTup, PrefDesc>::tup2;
			std::vector<Utility::MyTuple<Tpref>> prefabs;
		};
	}
}