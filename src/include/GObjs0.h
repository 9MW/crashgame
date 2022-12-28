#pragma once
#include"physics/physics0.h"
#include"Gameprops0.h"
#include"Common/ecs.hpp"
#include"entitys/comm.h"
//#include"cnl/elastic_fixed_point.h"
//#include <Gameprops0.h>
//#inclued"entity/"
namespace pj2{
	using  math::mtransform;
	using  math::ui64;
	using  GameProps::EntityId;

	namespace n_BasObj {
		using namespace GameProps::ENU;
		using namespace math::Fix;
		struct pWeprop {
			int feq;
		};
		struct eqpdesc {
			std::string name, desc;
		};
		struct beqp {
			const eqpdesc& GetDesc(int) { return eqpdesc{}; }
			const eqpdesc& GetDesc() { return eqpdesc{}; }
		};
		struct primaryweapon :beqp {
			mtransform eqpoffset;
			inline auto& posof() { return eqpoffset.pos4; }
		};
		template<state ...st>
		struct statecate
		{
			static constexpr std::array<state, sizeof...(st)> states = { st... };
			template<state t>
			static constexpr bool contain() {
				for (size_t i = 0; i < states.size(); i++)
				{
					if (states[i] == t)
						return true;
				}
				return false;
			}
		};
		struct ustate
		{
			PropID prp = PropID::nan;
			fix_s d, dur;
		};
		struct u0states
		{
			std::array<ustate, 32> _states;
			inline auto&& operator[](const math::ui64& i) {
				return _states[i];
			}
		};

		//template<t_Dmg dmg, PropID...lo>
		//using projectile = ECS::ECSclass<WPM0, bulletp0<am, dmg, lo...>,
		//	n_Component::ImpactInfo, n_Component::dRender>;
	}

	namespace n_Objs {
		struct abcmd
		{
			mtransform::v4 pos;
			EntityId teid;
			std::array<int16[2], 2> cost;
			int desc, canmove = 0;
			Fix::fix_s range = 1.0f;
			int16 time, cd;
		};
		struct abCont
		{
			abcmd abdesc;
			EntityId abifd;
			math::i64 lastative;
		};
		struct astate {
			int16 timer;
		};
		struct stateDesc :n_BasObj::baseobj {
			int dur;
			char propid[2];
			int16 effect[2];
			std::string name, Desc;
		};
		struct stateTab
		{

			bitset<32> pstates;//head 16 bit for passive later 16 for active
			//bitset<16> standardstates;
			std::array<astate, 16>states;
			auto tb16() { return pstates << 16; }
			inline bool atest(const int& i) {
				constexpr int sd = sizeof(states);
				return pstates.test(i + 16);
			}

			inline auto acount() {
				return (pstates >> 16).count();
			}
		};
	}
	namespace n_weapon {
		using namespace math::Fix;
		using namespace n_BasObj;
		struct maxtriggerdis0 {

		};
		enum class wstate :int {
			reload, fire, finsh
		};
		struct fireret
		{
			wstate result;
			fireret(wstate&& st) :result(st) {}
			inline operator bool()const { return result == wstate::finsh; }
		};
		struct u0state :ustate
		{
			/*template<state s, class T>
			static fireret update(T&& data) {
				
				
				unit0prop& prop = data.get<GameProps::BasicProp>().freq;
				return --prop.reload == 0 ? fireret(wstate::finsh) : fireret(wstate::reload);
			}*/
		};
		//template<class T>
		//struct gunbase0 :primaryweapon
		//{
		//	//using pjt = projectile<Armor::count,t_Dmg::collision,PropID::hp>;
		//	static inline pWeprop sprop;
		//	bulletinfo bullet;
		//	int treload, audiosrc, audiolength;
		//	/*STV(int, periodreload)
		//	STV(gunpara0, spara)*/
		//};

		//struct gun0 :gunbase0<gun0>//:n_BasObj::obj<gun0>
		//{
		//	t_Dmg dmgtype;
		//	int cap, dmg;
		//	static auto& prop() {
		//		static gunpara0 sp;
		//		return sp;
		//	};
		//	inline void fire(const mtransform& pos, mtransform::v4& tgpos, const GameProps::EntityId& tgid) {
		//		fireret rst(this->bullet <= 0 ? wstate::finsh : wstate::reload);
		//		
		//	}
		//	inline fireret reload(mtransform::v4 const pos) {
		//		fireret rst(--this->treload <= 0 ? wstate::finsh : wstate::reload);
		//		//rst.result = --this->treload <= 0 ? wstate::finsh:wstate::reload;
		//		return rst;
		//	}
		//	inline fireret reload(const int& sincereload) {
		//		fireret rst(--this->treload >= sincereload ? wstate::finsh : wstate::reload);
		//		//rst.result = --this->treload <= 0 ? wstate::finsh:wstate::reload;
		//		return rst;
		//	}
		//};
		struct AmmPara0
		{
			mtransform::v4 tgpos;
			ui64 physfreamid;GameProps::EntityId eid;
			//trigger distance
			float td;
			//sum of convex radius
			float crsum;
			float s_vel;
			int treload;
		};
		//struct Expolosion0
		//{
		//	inline static int audiosrc, audiolength;
		//	n_BasObj::bulletp0<Armor::nan, t_Dmg::explosion, PropID::hp> prop0;
		//	static int update(Manager::UpdatePara const* par) {

		//	}
		//};
		//struct Shell0 :n_BasObj::obj<n_BasObj::projectile<Armor::nan, t_Dmg::collision, PropID::hp>>
		//{
		//	inline static int audiosrc, audiolength;
		//	static int update(Manager::UpdatePara const* par) {
		//		auto& para = *par;
		//		n_Component::ImpactInfo iinfo = para.dfac.get1Comp<cmps,n_Component::ImpactInfo>(para.id);
		//		if (iinfo.frameImpact > para.ct.freamid)
		//			return 1;
		//		auto&& [trans, aminfo,audiorenderinfo, phyprop] = Cmps<cmps>(para.dfac, para.id).template getref<mtransfrom,
		//			typename cmps::t_Basof<n_BasObj::Ammo>,n_Component::dRender, n_Component::physiscsprop>();
		//		auto& tgpos = aminfo.tgpos;
		//		/*mtransfrom& tgtrans = para.ct.template GetEnti<physobj<>>(aminfo.targetid).template get<math::mtransfrom>();
		//		
		//		auto&& tgdead = tgtrans.Scale4.isZero();
		//		if (tgdead) {
		//			aminfo._state.SetState(state::died);
		//			return -1;
		//		}*/
		//		const auto nextpos = Physics::IntegrateVel(trans.pos4, phyprop.vel);
		//		const auto cross = (nextpos - trans.pos4).head<3>().dot(phyprop.vel.head<3>());
		//		const auto dissq = math::distancesq(trans.pos, tgpos);
		//		if (cross<0) {

		//			ECS::visit_convertible<n_Registry::Registry, Objs::Cmp_unit>(aminfo.targetid.tid, [&, &aprop = aminfo](auto I) {
		//				using cmp = typename n_Registry::Registry::t<I>;
		//				using tprop = ECS::derif<cmp, n_BasObj::Ubase>;
		//				GameProps::deltaProp& dp = Cmps<cmp>(para.ct.getArch(aprop.targetid.tid), aprop.targetid.id).template get<GameProps::deltaProp>();

		//				t_self::IterEff([&](auto i, auto propid) {
		//					dp.atomic_rep[propid].fetch_add(-aprop.datas[i], std::memory_order_relaxed);
		//					});
		//				});
		//		}
		//		return 0;
		//	}
		//	static int update2(Manager::UpdatePara const* par){
		//		auto& para = *par;
		//		using ammoprop = typename cmps::t_Basof<n_BasObj::Ammo>;
		//		auto&& [trans, phyprop] = Cmps<cmps>(para.dfac, para.id).template getref<mtransfrom,
		//			n_Component::physiscsprop>();
		//		trans.pos4 += phyprop.vel;
		//	}
		//};
		//template<class Ts>
	}
}