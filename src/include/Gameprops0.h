#pragma once
#define EXPORT 
#include"Common/Math1.h"
#include"Common/ecs.hpp"
#define CAST_INT(v) (int)v

EXPORT namespace pj2::GameProps {

	constexpr int Step1s = 50.0;
	constexpr double fixfeq = 1.0 / Step1s;
	constexpr double desired_frametime = fixfeq;
	constexpr double griphicfeq = 1 / 60.0;
	static float Samplerate = 0.02;
	using t_Arch = ECS::archs<int64_t, ECS::Conany_thsafe>;
	using uint = math::uint;
	using ui64 = math::ui64;
	using i64 = math::i64;
	using RenderIf = int;
	//using EntityId = ECS::EntityId;
	struct EntityId
	{
		/*enum class state :UINT16 {
			Dynamic = 0, Static = 1
		};*/
		enum class CollidDS :UINT16 {
			Dynamic = 0, Static = 1, non
		};
		//ECS::Entity et;
		union
		{
			struct {
				int id;
				UINT16 tid;
				union {
					CollidDS State;
					UINT16 tid0;
				};
			};
			size_t hashnum;
		};

		constexpr EntityId() :EntityId(-1, -1, CollidDS::non) {}
		//EntityId(int id) : id(id), tid(-1), State(CollidDS::Dynamic) {}
		constexpr EntityId(int id, UINT16 tid, CollidDS state) : id(id), tid(tid), State(state) {
		}

		constexpr EntityId(int id, UINT16 tid, UINT16 tid0) : id(id), tid(tid), tid0(tid0) {
		}
		inline  bool operator ==(const EntityId& rhs)
		{
			return reinterpret_cast<const size_t&>(*this) == reinterpret_cast<const size_t&>(rhs);
		}
		/*void operator =(const EntityId& rhs)
		{
			 std::memcpy(this,&rhs,sizeof(EntityId));
		}*/
		inline bool operator >(const EntityId& rhs)const
		{
			return hashnum > rhs.hashnum;
		}
		inline bool operator <(const EntityId& rhs)const
		{
			return hashnum < rhs.hashnum;
		}
		constexpr size_t hash()const {
			return hashnum;
		}
		template<typename OStream>
		friend OStream& operator<<(OStream& os, const EntityId& c)
		{
			return os << "[id=" << c.id << " state=" << (int)c.State << "tid=" << c.tid << "]";
		}
		/*string ToString() {
			return fmt::format("state={0} id={1}", (int)State, id);
		}*/
		inline operator const size_t&()const {
			return hashnum;
		}
	};
	template<class T, math::ui64 sz>
	using array = std::array<T, sz>;
	constexpr int propnum = 8;
	static int _TexSkinnum = 20;
	namespace ENU {

		enum class state :UINT32 {
			nan, died, dieing, move, attack, attackmove, idle, pick, put, deactive,
			autoreload, insreload, PATROL, FOLLOW, change, EnemyDead,
			attackwhenfind, findattack, non, count
			//nan, dieing, died, alive, count
		};
		//must have same arrangement as state
		enum class AnimState :UINT32 {
			dieing = (int)(state::dieing), move, attack, attackmove, idle, pick, put, reload,count 
			//nan, dieing, died, alive, count
		};
		constexpr int AStateCount = 5;
		enum class Armor :int32 {
			nan, t1, t2, t3, count
		};
		enum class t_Dmg :int32 {
			nan, laser, explosion, plasma, collision, cut, chemical, count
		};
		enum class PropID :int {
			speed, feq, energy, hp, visiableRange, attackrangesq, damage, Armor, Actionrangesq, count, nan
		};
		enum class Equip :int {
			bullet1, bullet2, bullet3, injection1, MRL, grenade, rifle_grenade, battery, count
		};
	}
	struct autiosetting {
		math::ui64 perframeprogess;
		static autiosetting& Main() {
			static autiosetting s;
			return s;
		}
	};
	using namespace ENU;
	struct DmgCoeff {
		float _v1 = 1;
	};
	template<Armor am, t_Dmg dmg>
	struct ArDm {
		static constexpr Armor GetArmor() {
			return am;
		}
		static constexpr auto GetDmg() {
			return dmg;
		}
	};

	struct s_Info
	{
		std::array<DmgCoeff, (CAST_INT(Armor::count) - 1)* (CAST_INT(t_Dmg::count) - 1)> v_;
		inline const auto GetDmgCoeff(const int& i) {
			return v_[i];
		}
		template<int i>
		inline const auto GetDmgCoeff() {
			return v_[i];
		}
		template<class Armor, class Dmg>
		constexpr auto CalDmg(const int& dmgn, const int amr) {
			constexpr auto p = (CAST_INT(Armor::GetArmor()) - 1) + (CAST_INT(Dmg::GetDmg()) - 1);
			const DmgCoeff& coef = GetDmgCoeff(p);
			return coef._v1 * dmgn * (dmgn / (dmgn + amr));
		}
	};
	template<class AD0, class AD1>
	struct Form
	{
		constexpr auto CalDmg(const int& dmgn, const s_Info& si) {
			constexpr auto p = (CAST_INT(AD0::GetArmor()) - 1) + (CAST_INT(AD1::GetDmg()) - 1);
			const DmgCoeff& coef = si.GetDmgCoeff<p>();
			return coef._v1 * dmgn;
		}
	};
	struct State
	{
		using ET = int;
		using Actions = GameProps::ENU::state;
		using I = ET&;

		union {
			//GameProps::states2 pcstate;
			state _st;
			array<int16, 2> pcstate;
		};
		int _statetick;
		State() :_statetick(0) {
			_st = state::non;
		}

		inline  State(state a, state b) : _statetick(0) {
			pcstate[0] = (uint16)a;
			pcstate[1] = (uint16)b;
		}
		template<state a, state b>
		static constexpr  auto makeState() {

			return (state)((int)a << 16 | (int)b);
		}
		inline void SetState(const GameProps::ENU::state st) {
			int& sti = (int&)(_st);
			sti <<= 16;
			sti |= (int)st;
			_statetick = 0;
		}
		template<GameProps::ENU::state st>
		inline void SetState() {
			int& sti = (int&)(_st);
			sti <<= 16;
			sti |= (int)st;
			_statetick = 0;
		}
		inline void JumpState(const GameProps::ENU::state st) {
			_st = st;
			/*pcstate[0] = pcstate[1];
			auto&& CState = pcstate[1];
			CState = (int16)st;*/
			_statetick = 0;
		}
		inline void SetStateb(const GameProps::ENU::state st) {
			pcstate[0] = (int16)state::non;
			auto&& CState = pcstate[1];
			CState = (int16)st;
			_statetick = 0;
			/*for (size_t i = 0; i < pcstate.size()-1; i++)
			{
				pcstate[i] = pcstate[i + 1];
			}
			pcstate.back() = st;*/
		}
		inline auto& tick() { return _statetick; }
		inline void tick(const int ag) { _statetick = ag; }
		constexpr GameProps::ENU::state SwiState() {
			_statetick = 0;
			_st = (state)pcstate[1];
			return _st;
		}
		constexpr bool operator==(const State& s)const {
			return s._st == _st;
		}
		constexpr bool operator==(const Actions& s)const {
			return s == _st;
		}
		constexpr operator const Actions() const {
			return _st;
		}
	};
	struct Cmd0
	{
		static constexpr int stateslots = 2;
		static constexpr float trackfreq = 0.1;
		//tagrget updated by group commander,element 3 state max action dissq
		math::mtransform::v4 tpos;
		//two state can coexist,[[discard]]size must be multiple of 8 btye for proper serialization
		array<State, 2>action;
		Utility::Gbit hostile;//hostile
		//target id
		//cmdid : command group id
		//cmstate[0] ; in group id
		EntityId eid; int gid, cmdid;
		array<int,4> cmstate;
		//AI::State state;
		inline ENU::state cState() {
			return action[0];
		}
		template<int I>
		inline State& cState() {
			return std::get<I>(action);
		}
		inline bool Any(const GameProps::ENU::state& s)const {
			//sizeof return byte
			return s == action[0] || s == action[1];
		}
		template<GameProps::ENU::state s>
		inline bool Any()const {
			//sizeof return byte
			return s == action[0] || s == action[1];
		}
		inline bool All(const GameProps::ENU::state& s)const {
			//sizeof return byte
			return s == action[0] && s == action[1];
		}
		template<GameProps::ENU::state s>
		inline bool All()const {
			return s == action[0] && s == action[1];
		}
		inline bool idle() { return All<state::idle>(); }
		template<GameProps::ENU::state s>
		inline void SetState() {
			if constexpr(s!=state::nan)
			cState<0>().SetState<s>();
		}
		template<int I, GameProps::ENU::state s>
		inline void SetState() {
			cState<I>().SetState<s>();
		}
	};
	/*struct sucmd
	{
		Cmd0 c, n;
	};*/
	struct targte
	{
		//Controller* ct;
		//tagrget updated by group commander
		math::mtransform::v4 tpos;
		EntityId eid;
	};
	using states2 = array<ENU::state, 2>;
	/*struct Target
	{
		math::mtransfrom::v4 _tpos;

	};*/
	template<PropID pop>
	struct battery0
	{
		//float t0;
		math::Fix::fix_s dur;
		math::Fix::fix_s dprop;

	};
	struct AudioOff
	{
		ui64 off1;//pos in clips;
		int off0;//offset from state start
	};
	template<int i>
	struct Prop
	{
		using DataType = math::Vector<int, 8>;
		using tsf = Prop<i>;
		using type = DataType::Scalar;
		using Scalar = DataType::Scalar;
		union {
			DataType v8i;
			//static constexpr int size = sizeof(BasicProp);
			std::array<std::atomic<math::Fix::s_fix>, propnum> atomic_rep;
		};
		Prop() {
			std::memset(this, 0, sizeof(*this));
			//constexpr auto jni = sizeof(AudioOff);
		}
	public:
		inline auto& data() {
			return v8i;
		}

		Prop(Prop<i>&& other) {
			std::memcpy(this, &other, sizeof(*this));
		}
		Prop(const Prop<i>& other) {
			std::memcpy(this, &other, sizeof(*this));
		}
		template<int P>
		constexpr auto&& getAtoProp() {
			return std::get<P>(this->atomic_rep);
		}
		constexpr std::atomic<type>& getAtoProp(const int p) {
			return (this->atomic_rep)[p];
		}
		constexpr tsf& operator=(const Prop<i>& other) {
			std::memcpy(this, &other, sizeof(*this));
			return *this;
		}
		const auto cssize() const{ return v8i.size(); }
		/*	template<typename T>
			constexpr void Loop(T&& func) {
				v8i.Loop(func);
				math::Vector4f fk;

			}*/

		constexpr DataType& Drep() { return v8i; }
		inline  DataType& cDrep()const { return v8i; }
		template<ENU::PropID pid >
		inline auto&& getfix() {
			auto&& val = v8i[(int)pid];// .get<(int)pid>();
			{
				return math::Fix::toFixs(val);
			}
		}
		inline auto&& get(const ENU::PropID& pid) {
			auto&& val = v8i[(int)pid];
			/*if (pid == ENU::PropID::attackrangesq) {
				return math::Fix::toFixs(val);
			}*/
			return val;
		}
		template<int pid >
		inline auto& get() {
			using namespace math;
			return v8i[pid];// (math::seq(fix<0>, fix<0>));
		}
		template<ENU::PropID pid > 
		inline auto& get() {
			return v8i[(int)pid];// (math::seq(fix<0>, fix<0>));
		}
		inline operator DataType& () { return v8i; }
		template<int pid >
		inline auto&& get_atomic() {
			return std::get<pid>(this->atomic_rep);
		}
		inline type& operator [](const uint id) {
			//const auto div = std::div(id, 4);
			return v8i[id];
		}
		/*template<typename T>
		constexpr DataType::V4T& getV4(const uint id) {
			return v8i.data[id];
			constexpr int size = sizeof(Prop);
		}
		template<uint i>
		constexpr DataType::V4T& getV4() {
			return v8i.data[i];
		}*/
	};
	using BasicProp = Prop<0>;
	using AnimProp = Prop<2>;
	using deltaProp = Prop<1>;
	inline void apply(BasicProp& bp, deltaProp& dp) {
		bp.v8i += dp.v8i;
	}

	enum class soltDef {
		weapon, armor,ammo, eq1, eq2
	};
	struct IItem {
		EntityId Eqprefid;
		std::array<size_t, 1> cdata;
		int On(BasicProp&) {
			return 0;
		}
		int Use(BasicProp&, Cmd0&) {
			return 0;
		}
		int Off(BasicProp&) {
			return 0;
		}
	};
	struct equipment {
		using t_use =
			Utility::tup<math::mtransform, GameProps::deltaProp>;
		Equip 
			eqtp;
		EntityId Eqprefid;
		int maxstack = 1,num=0;
		//std::string desc;
		math::Fix::fix_s weight, dis;
		/*union {
			GameProps::BasicProp prop;
			int ammoid, ammonum;
		};*/
		equipment() { std::memset(d64, 0, sizeof(d64)); }
		equipment(const equipment& e) {
			std::memcpy(this, &e, sizeof(equipment));
		}
		union {
			math::ui64 d64[3];
			std::array< UINT16, sizeof(d64) / 2> items;//max 65535 bullet x 12;
			//math::ui64 lastactive, cd, num;
		};
		//DataType data;
		/*int(*onpick)(void* const);
		int(*unpick)(void* const);
		int(*use)(void* const);*/
	};
	struct box
	{
		int tid, id, volume, weight;
		equipment eq;
	};
	struct EffDesc {
		equipment eqEve;
	};
	struct Inventory {
		std::array<equipment, 8> items;

	};
	struct UnitProps {
		static constexpr int  attackrangesq = 0, hp = 1, Actionrangesq = 2, visiableRange = 3, feq = 4, damage = 5, energy = 6, Armor = 7;
		int val = -1;
		operator const int()const {
			return val;
		}
	};
	struct StateDescV {
		//using PropType = math::Vector4i;
		int num, time, pd;
		UnitProps category;
		math::mtransform::v4i SourcePos;
		math::i4 Eff;

	};
	struct unitState {
		BasicProp temp;
		const BasicProp temptimec;
		BasicProp temptimer;
		unitState() {}

	};
	struct weaponpara
	{
		math::uint audiolength = 0, audiosrc = 0, freq = -1, bullet, treload;
	};
	struct alignas(16) DrawArgs {
		math::uint32_t NumIndices,
			NumInstances,
			FirstIndexLocation,
			BaseVertex,
			FirstInstanceLocation;
	};
	struct unitdesc
	{
		struct animclip
		{
			std::string Name;
			int start, size, samplerate;
		};
		struct aastate
		{
			animclip anclip;
		};
		int maxcap;
		std::vector<DrawArgs> drawarg;
		std::string Name;
		std::string render;
		GameProps::BasicProp peop;
		std::string weapons;
		std::array<animclip, ENU::AStateCount> anims;
		//math::i4 weaponid;// weapondesc weapon0;
		//weapondesc wep;
	};
	struct ResourceData {
		std::string Name, recpath;
		int rectype;
	};
	struct Objdesc
	{
		std::string Name;
		ResourceData resource;
		GameProps::BasicProp prop;
	};
	using GobjDesc = unitdesc;
	/*struct delProp {
		std::bitset<32> propdisc;
		std::
	};*/
	using ubuilder0 = std::tuple<math::mtransform, int, unitdesc, GameProps::weaponpara>;
	struct Troop {
		std::vector<GameProps::EntityId> units;
		std::vector<GameProps::Cmd0> cmds;
	};
	struct Gprops
	{
		float physicsdt = 0.02;

	};
	inline Gprops& MainProp() {
		static Gprops g;
		return g;
	}
	//struct EffDesc
	//{
	//	INT64 gp;
	//	union {
	//		struct {
	//			BasicProp temp;
	//			const BasicProp temptimec;
	//			BasicProp temptimer;
	//			BasicProp persistent;
	//		};
	//		//16 pointer max
	//		struct {
	//			INT64 Mk;
	//			INT64 Mk1;
	//			Vector4i data0;
	//			Vector4i data1;
	//			Unit<>* target;
	//			Utility::WrapFuncObj<bool, Unit<>&, EffDesc&, Controller&> onDeploy;
	//			Physics::v4p dist;//v3 : pos v1 : radius
	//			//BasicProp persistent;
	//		};


	//	};
	//	inline bool Deployale() {
	//		return onDeploy;
	//	}
	//	EffDesc(bool Deployale = false) {
	//		Mk = Deployale * MarkDeployable;
	//		Mk1 = Mk;
	//	}
	//	/*constexpr bool TryDeployale(Unit<>& unit) {
	//		if (Mk==MarkDeployable)
	//			return onDeploy(unit,*(this));
	//		else {
	//			return false;
	//		}
	//	}*/
	//};
	//struct StateDesc {
	//    //using PropType = math::Vector4i;
	//    int num, time, pd;
	//    UnitProps category;
	//};
}