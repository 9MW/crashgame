#pragma once
#include"comm.h"
#include"io/BSeri.h"
#include <entitys/Ammos.h>
//#include"entitys/unit.h"
//#include"unit.h"
//#include <physics/physics.hpp>
//struct CollideID
//{
//	/*enum class state :UINT16 {
//		Dynamic = 0, Static = 1
//	};*/
//	enum class CollidDS :UINT16 {
//		Dynamic = 0, Static = 1, non
//	};
//	//ECS::Entity et;
//	union
//	{
//		struct {
//			CollidDS State;
//			UINT16 tid;
//			int id;
//		};
//		size_t hashnum;
//	};
//
//	constexpr CollideID() :CollideID(-1, -1, CollidDS::non) {}
//	//CollideID(int id) : id(id), tid(-1), State(CollidDS::Dynamic) {}
//	constexpr CollideID(int id, UINT16 tid, CollidDS state) : id(id), tid(tid), State(state) {
//
//	}
//	inline  bool operator ==(const CollideID& rhs)
//	{
//		return reinterpret_cast<const size_t&>(*this) == reinterpret_cast<const size_t&>(rhs);
//	}
//	/*void operator =(const CollideID& rhs)
//	{
//		 std::memcpy(this,&rhs,sizeof(CollideID));
//	}*/
//	inline bool operator >(const CollideID& rhs)const
//	{
//		return hashnum > rhs.hashnum;
//	}
//	inline bool operator <(const CollideID& rhs)const
//	{
//		return hashnum < rhs.hashnum;
//	}
//	constexpr size_t hash()const {
//		return hashnum;
//	}
//	template<typename OStream>
//	friend OStream& operator<<(OStream& os, const CollideID& c)
//	{
//		return os << "[id=" << c.id << " state=" << (int)c.State << "tid=" << c.tid << "]";
//	}
//	/*string ToString() {
//		return fmt::format("state={0} id={1}", (int)State, id);
//	}*/
//	explicit operator size_t()const {
//		return hashnum;
//	}
//};
namespace pj2 {
	namespace Manaer {
		using a_st = GameProps::ENU::state;
		struct ObjState
		{
			inline void SetState(const GameProps::ENU::state st) {
				PState = CState;
				CState = st;
			}
			ObjState(const GameProps::ENU::state st) {
				PState = a_st::nan;
				CState = st;
			}
			ObjState() {
				memset(this, 1, sizeof(*this));
			}
			inline void operator = (const a_st st) {
				SetState(st);
			}
			explicit operator const int()const {
				return (int)CState;
			}
			operator const a_st()const {
				return CState;
			}
		private:
			GameProps::ENU::state PState, CState;
		};
	}
	using v4 = math::mtransform::v4;
	/*namespace Manager {
		using UpdatePara = int;
	}*/
	namespace n_BasObjimp {
	}
	namespace ETS {
		using namespace GameProps::ENU;
		using namespace n_BasObjimp;
		//using ObjState = Manager::ObjState;
		struct b_ammo {
			//GameProps::DataType datas;
			/*CollideID targetid;
			ObjState _state;
			inline operator ObjState& () {
				return _state;
			}*/
			b_ammo() {}
			/*template<GameProps::ENU::PropID P>
			inline auto&& get(){
				return datas.get<(int)P>();
			}*/
		};

	}
	namespace n_Coll {

		using mtrans = math::mtransform;
	}
	namespace n_BasObj {
		using namespace GameProps::ENU;
		using namespace math::Fix;
		using namespace n_BasObj;
		using Ammo = ETS::b_ammo;
		using EntityId = GameProps::EntityId;
		/*using Cmp_unit = ECS::ECSclass<pj2::AI::postp, unitcp0,
			Ub0<GameProps::ArDm<Armor::nan, t_Dmg::nan>>, mtransfrom, n_objProp::dRender,
			GameProps::deltaProp, GameProps::BasicProp, GameProps::Cmd0>;*/
		using Moveobj = ECS::ECSclass<math::mtransform, n_Component::physiscsprop>;
		using WPM0 = ECS::ECSclass<Moveobj, CollideID>;
		using mtrans = math::mtransform;
		using sensor8 = std::array<math::int2, 8>;
		struct Commander :obj<ECS::ECSclass<EntityId, sensor8, sStruct::objrep>> {
			std::vector<int> aliance;
			void SortId();
		};
		struct SystemManager {
			std::vector<int> aliance;
		};
		//template<class Eclass>
		/*template<state ...st>
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
		};*/
		/*template<class T>
		inline auto Cmps(Manager::t_Arch& achs, const std::size_t& i) {
			return achs.template getComps<T>(i);
		}*/
	


		template<class T>
		struct bullet :obj<T>
		{
			using df = ECS::derif<T, Ammo>;
			using t_data = typename df::t_data;// Fix::fix<EffLoc.size()>;
			//std::conditional_t < EffLoc.size() < 4,Fix::fix4,Fix::fix<EffLoc.size()>> datas;
			/*t_data datas;
			fix_s Td, Tdsq, CrSum;
			ui64 frimpact;
			using t_prop = GameProps::ArDm<am, dmg>;
			static ui64 offaudio;
			static void* ptr_mesh;
			//static constexpr t_Dmg _tDmg = dmg;

			template<class F>
			static constexpr void IterEff(F&& f) {
				constexpr auto EffLoc = df::EffLoc;
				ECS::visit<EffLoc.size()>([f = std::forward<F>(f)](auto I) {
					f(I, std::integral_constant<std::size_t, EffLoc[I]>());
				});
			}*/

			//template<class Registry, class Eclass>
			//static int update(Manager::UpdatePara const* par) {
			//	using t_self = df;
			//	auto& para = *par;
			//	auto&& [trans, aminfo] = Cmps<T>(para.dfac, para.id).template getref<mtransfrom, t_self>();
			//	if (aminfo.frimpact < para.ct.freamid)
			//		return 1;

			//	mtransfrom& tgtrans = para.ct.template GetEnti<physobj<>>(aminfo.targetid).template get<math::mtransfrom>();
			//	auto& tgpos = tgtrans.pos;
			//	auto&& tgdead = tgtrans.Scale4.isZero();
			//	if (tgdead) {
			//		aminfo._state.SetState(state::died);
			//		return -1;
			//	}
			//	const auto dissq = math::distancesq(trans.pos, tgpos);
			//	if (dissq < aminfo.Tdsq && (math::sqrt(dissq) < (aminfo.CrSum))) {
			//		/*ECS::visit_convertible<Registry, n_BasObj::Cmp_unit>(aminfo.targetid.tid, [&, &aprop = aminfo](auto I) {
			//			using cmp = typename n_Registry::Registry::t<I>;
			//			using tprop = ECS::derif<cmp, n_BasObj::Ubase>;
			//			GameProps::deltaProp& dp = Cmps<cmp>(para.ct.getArch(aprop.targetid.tid), aprop.targetid.id).template get<GameProps::deltaProp>();

			//			t_self::IterEff([&](auto i, auto propid) {
			//				dp.atomic_rep[propid].fetch_add(-aprop.datas[i], std::memory_order_relaxed);
			//				});
			//			});*/
			//	}
			//	return 0;
			//}
		};
		template<Armor am, t_Dmg dmg, PropID...lo>
		struct Ammo0 :ETS::b_ammo//, n_BasObj::obj<Cmp_unit>
		{
			using df = Ammo0<am, dmg, lo...>;
			static constexpr std::array<int, sizeof...(lo)> EffLoc = { (int)lo... };
			using t_data = Fix::fix<EffLoc.size()>;
			//std::bitset<32> Props;
			//std::conditional_t < EffLoc.size() < 4,Fix::fix4,Fix::fix<EffLoc.size()>> datas;
			t_data datas;
			fix_s Td, Tdsq, CrSum;
			int energy;
			ui64 frimpact;
			GameProps::EntityId LastImpact;
			using t_prop = GameProps::ArDm<am, dmg>;
			//static ui64 offaudio;
			static void* ptr_mesh;
			//static constexpr t_Dmg _tDmg = dmg;
			//df() {}
			
			template<class F>
			static constexpr void IterEff(F f) {
				ECS::visit<EffLoc.size()>([f = std::forward<F>(f)](auto I) {
					f(I, std::integral_constant<std::size_t, EffLoc[I]>());
					});
			}
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
		template<Armor am, t_Dmg dmg, PropID...lo>
		struct Ammo1 :ETS::b_ammo
		{
			using df = Ammo1<am, dmg, lo...>;
			static constexpr std::array<int, sizeof...(lo)> EffLoc = { (int)lo... };
			using t_data = Fix::fix<3>;
			std::bitset<32> Props;
			//std::conditional_t < EffLoc.size() < 4,Fix::fix4,Fix::fix<EffLoc.size()>> datas;
			t_data datas;
			fix_s Td, Tdsq, CrSum;
			ui64 frimpact;
			using t_prop = GameProps::ArDm<am, dmg>;
			//static ui64 offaudio;
			static void* ptr_mesh;
			//static constexpr t_Dmg _tDmg = dmg;
			//df() {}
			template<class F>
			static constexpr void IterEff(F f) {

				for (size_t i = 0, id = 0; i < (size_t)(PropID::count); i++)
				{
					if (Props[i])
						f(i, datas[id++]);
				}
			}
			//template<class t_self, class Registry, class Eclass>
			//static int update(Manager::UpdatePara const* par) {

			//	auto& para = *par;
			//	auto&& [trans, aminfo] = Cmps<Eclass>(para.dfac, para.id).template getref<mtransfrom, t_self>();
			//	if (aminfo.frimpact < para.ct.freamid)
			//		return 1;

			//	mtransfrom& tgtrans = para.ct.template GetEnti<physobj<>>(aminfo.targetid).template get<math::mtransfrom>();
			//	auto& tgpos = tgtrans.pos;
			//	auto&& tgdead = tgtrans.Scale4.isZero();
			//	if (tgdead) {
			//		aminfo._state.SetState(state::died);
			//		return -1;
			//	}
			//	const auto dissq = math::distancesq(trans.pos, tgpos);
			//	if (dissq < aminfo.Tdsq && (math::sqrt(dissq) < (aminfo.CrSum))) {

			//		ECS::visit_convertible<Registry, Cmp_unit>(aminfo.targetid.tid, [&, &aprop = aminfo](auto I) {
			//			/*using cmp = typename n_Registry::Registry::t<I>;
			//			using tprop = ECS::derif<cmp, n_BasObj::Ubase>;
			//			GameProps::deltaProp& dp = Cmps<cmp>(para.ct.getArch(aprop.targetid.tid), aprop.targetid.id).template get<GameProps::deltaProp>();

			//			t_self::IterEff([&](auto i, auto propid) {
			//				dp.atomic_rep[propid].fetch_add(-aprop.datas[i], std::memory_order_relaxed);
			//				});*/
			//			});
			//	};
			//	return 0;
			//}
		};
		//template<Armor am, t_Dmg dmg, PropID...lo>
		//struct Ammo3 :ETS::b_ammo, n_BasObj::obj<Cmp_unit>
		//{
		//	using df = Ammo3<am, dmg, lo...>;
		//	static constexpr std::array<int, sizeof...(lo)> EffLoc = { (int)lo... };
		//	using t_data = Fix::fix<EffLoc.size()>;
		//	//std::bitset<32> Props;
		//	//std::conditional_t < EffLoc.size() < 4,Fix::fix4,Fix::fix<EffLoc.size()>> datas;
		//	t_data datas;
		//	fix_s Td, Tdsq, CrSum;
		//	ui64 frimpact;
		//	using t_prop = GameProps::ArDm<am, dmg>;
		//	//static ui64 offaudio;
		//	static void* ptr_mesh;
		//	//static constexpr t_Dmg _tDmg = dmg;
		//	//df() {}
		//	template<class F>
		//	static constexpr void IterEff(F f) {
		//		ECS::visit<EffLoc.size()>([f = std::forward<F>(f)](auto I) {
		//			f(I, std::integral_constant<std::size_t, EffLoc[I]>());
		//		});
		//	}
		//	template<class T, class t_self>
		//	static inline void Build(const p_Amc1 p1, T&& tup, t_data&& data_) {
		//		t_self&& re = tup.get<t_self>();
		//		const ui64& physfreamid = p1.physfreamid;
		//		mtrans&& selftrans = tup.get<mtrans>();
		//		re.vel = (p1.tgpos - selftrans.pos4).normalized() * p1.s_vel;
		//		const auto dissq = math::distancesq3(selftrans.pos4, p1.tgpos);
		//		//calculate minimum possiable time impact will occur
		//		re.frimpact = ((math::sqrt(dissq) / p1.s_vel) / Physics::dt) * 0.8 + physfreamid;
		//		auto&& td = p1.td;
		//		re.datas = data_;
		//		re.Td = p1.td;
		//		re.Tdsq = td * td;
		//		re.CrSum = p1.crsum;
		//	}
		//	template<class t_self, class Registry, class Eclass>
		//	static int update(Manager::UpdatePara const* par) {

		//		auto& para = *par;
		//		auto&& [trans, aminfo] = Cmps<Eclass>(para.dfac, para.id).template getref<mtransfrom, t_self>();
		//		if (aminfo.frimpact < para.ct.freamid)
		//			return 1;

		//		mtransfrom& tgtrans = para.ct.template GetEnti<physobj<>>(aminfo.targetid).template get<math::mtransfrom>();
		//		auto& tgpos = tgtrans.pos;
		//		auto&& tgdead = tgtrans.Scale4.isZero();
		//		if (tgdead) {
		//			aminfo._state.SetState(state::died);
		//			return -1;
		//		}
		//		const auto dissq = math::distancesq(trans.pos, tgpos);
		//		if (dissq < aminfo.Tdsq && (math::sqrt(dissq) < (aminfo.CrSum))) {

		//			/*	ECS::visit_convertible<Registry, Objs::Cmp_unit>(aminfo.targetid.tid, [&, &aprop = aminfo](auto I) {
		//					using cmp = typename n_Registry::Registry::t<I>;
		//					using tprop = ECS::derif<cmp, n_BasObj::Ubase>;
		//					GameProps::deltaProp& dp = Cmps<cmp>(para.ct.getArch(aprop.targetid.tid), aprop.targetid.id).template get<GameProps::deltaProp>();

		//					t_self::IterEff([&](auto i, auto propid) {
		//						dp.atomic_rep[propid].fetch_add(-aprop.datas[i], std::memory_order_relaxed);
		//						});
		//					});*/
		//		}
		//		return 0;
		//	}
		//};
		struct stgunprop {
			math::ui64 audioid;
			mtransfrom muzzle;
		};
	}
	namespace n_Objs {
		struct Boxinfo {
			GameProps::equipment equp;
			int num = 1;
		};
		struct acquiredice {
			INT64 fm = -1, ct = 0;
			aint pi;
		};
		/*struct stateinstance:public n_BasObj::obj<ECS::ECSclass<astate,>> {

		};*/
		using Cmps_robj = ECS::ECSclass<mtransform, n_objProp::dRender,
			GameProps::deltaProp, GameProps::BasicProp>;
		//using boxcmp=ECS::ECSclass<Cmps_robj,Physics::
		struct ObjBox : public n_BasObj::rpoolobj<ECS::ECSclass<n_BasObj::stphyobj,n_objProp::dRender,
			GameProps::deltaProp, GameProps::BasicProp,Boxinfo, acquiredice>>
		{
			/*int Build(const n_Component::BuildPara& para, GameProps::equipment& ed) {
				const auto iins = para.id;
				if (iins >= 0) {
					auto&& [trs, bif] = this->getcmps<mtransform, Boxinfo>(iins);
					trs.reset(para.pos.head<3>());
					bif.equp = ed;
					bif.pi() = 0;
				}
				return iins;
			}*/
			using EntityId=GameProps::EntityId;
			std::unordered_map<int, std::array<GameProps::EntityId, 16>> phis;
			aint pct;
			std::mutex _lk;
			ObjBox() {

			}
			ObjBox(ObjBox&&ob) {
				std::memcpy(this, &ob, sizeof(*this));
			}
			ObjBox(const ObjBox& ob) {
				std::memcpy(this, &ob, sizeof(*this));
			}
			int uusf() {
				pool::uusf(*this);
				return 1;
			}
			int pk(int i, const EntityId& fid) {
				auto&& [trs, bif, dice] = this->getcmps<mtransform, Boxinfo, acquiredice>(i);
				//const auto cfm = GameProps::autiosetting::Main().perframeprogess;
				//dice.fm = dice.fm < 0 ? cfm : dice.fm;
				//assert(cfm - dice.fm <= 1);
				////Utility::update_max(dice.pi.t, fa);
				//if (cfm == dice.fm)
				{
					const std::lock_guard lkg(_lk);
					phis[i][dice.pi()++] = fid;
				}
				return -1;
			}
			void init() {

			}
		};
		struct Site {
			int money = 0, level, iRes;

		};
	}
	namespace n_weapon {
		using namespace math::Fix;
		using namespace n_BasObj;
		using n_Component::prefabID;
		using n_BasObj::primaryweapon;
		using n_weapon::fireret;
		struct bulletinfo {
			prefabID pfbid;

			int d;//creating bullet id
			int16 d2[2];

			bulletinfo() { d = -1; }
			inline operator int() const {
				return d2[1];
			}
			inline int16& num() { return d2[1]; }
			inline auto& tid0() { return pfbid.tid0; }
			int operator --() {
				return d2[1]--;
			}
		};

		struct gundesc {
			std::string desc, afire, reload;
			int64_t firerate;
			GameProps::BasicProp gbp;
			int bi;
			float tfreq;
		};
		template<class T>
		struct gunbase0 :primaryweapon
		{
			//using pjt = projectile<Armor::count,t_Dmg::collision,PropID::hp>;
			static inline n_BasObj::pWeprop sprop;
			bulletinfo bullet;
			int treload, audiosrc, audiolength;
			/*STV(int, periodreload)
			STV(gunpara0, spara)*/
		};
		struct gun0 :gunbase0<gun0>//:n_BasObj::obj<gun0>
		{
			t_Dmg dmgtype;
			int cap, dmg;
			static auto& prop() {
				static n_weapon::stgunprop sp;// gunpara0 sp;
				return sp;
			};
			inline void fire(const mtransfrom& pos, mtransform::v4& tgpos, const GameProps::EntityId& tgid, bullet0& bl) {
				fireret rst(this->bullet <= 0 ? wstate::finsh : wstate::reload);
			}
			inline fireret reload(const int& num) {
				this->bullet.num() = num;
				return fireret(wstate::finsh);
			}
			void Seri(void* js);
			/*void operator=(GameProps::Objdesc& od) {
				this->treload = od.prop.get<0>();
				this->audiosrc = od.prop.get<1>();
				this->audiolength = od.prop.get<2>();
			}
			void operator=(GameProps::equipment& od) {
				this->treload = od.prop.get<0>();
				this->audiosrc = od.prop.get<1>();
				this->audiolength = od.prop.get<2>();
			}*/
			void get(GameProps::Objdesc& od) {
				od.prop.get<0>() = this->treload;
				od.prop.get<1>() = this->audiosrc;
				od.prop.get<2>() = this->audiolength;
			}
		};
		struct gun0prop :primaryweapon, n_BasObj::ecsobj<gun0, gundesc>
		{
		};
		struct wepresources :eqpdesc
		{
			std::string shotAudio, reloadaud, finishloadaud;
			float attackvel;
		};
		struct guns0 :n_BasObj::ecsobj<gun0, wepresources>, n_BasObj::beqp
		{
			void Seri(void* js);
			void DeSeri(void* js);
			eqpdesc& GetDesc(int i) { return this->getcmp<wepresources>(i); }
			//gun type name
			std::string tgun;
			//wepresources desc;
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
