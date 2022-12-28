#pragma once
//import RenderSet;
//import drawerm;
//import fogRender;
#undef GetObject
#include"common/ecs.hpp"
#include "spdlog/stopwatch.h"
#include "Render/Renders0.h"
#include <AdvancedMath.hpp>
#include"Gameprops0.h"
#include"Task.h"
#include"Render/RenderCommon.h"
#include"Systems/SysSet.h"
#include"physics/world.h"
//#include"Render/drawer.h"
#define STV(type,name) inline static type& name(){static type name;return name;}
//#include"AllType.h"
//#include "physics\physics.hpp"

//-> decltype(result = T::funcNmae, void())
#if __cplusplus <= 201703L
#define ExFunc(funcNmae) template <class T, class T2> static auto Ex##funcNmae(T2& result, int i) -> decltype(result = T::funcNmae, int()){\
    result = T::funcNmae;\
}\
template <class T, class T2> static auto Ex##funcNmae(T2& arg, long i) {\
arg = NULL;\
}\
template <class T, class T2> static auto Ex##funcNmae(T2& arg) {\
    Ex##funcNmae<T>(arg, 0);\
}
#else
#define ExFunc(funcNmae) template <class T, class T2> static auto Ex##funcNmae(T2& result, int i) -> decltype(result = T::funcNmae, int()){\
    result = T::funcNmae;\
}\
template <class T, class T2> static auto Ex##funcNmae(T2& arg, long i) {\
arg = NULL;\
}\
template <class T, class T2> static auto Ex##funcNmae(T2& arg) {\
    Ex##funcNmae<T>(arg, 0);\
}
#endif
namespace Utility {
	using namespace pj2;
	static constexpr auto contain =
		[](const Collid& cd, const mtransfrom pos, const Efloat2 tp) {
		switch (cd.colliderType)
		{
		case ColliderType::Box: {
			const auto& bx = cd.getData<IBoxCollider>();
			const auto loc = pos.Trans2dIv(tp);
			if (bx.Contain(loc))
				return true;
		}
							  break;
		case ColliderType::Circle:
			return (pos.pos.head<2>() - tp).squaredNorm() <= cd.ConvexR * cd.ConvexR;
		default:
			std::abort();
			break;
		}
		return false;
	};
}
namespace pj2 {

	using namespace ECS;
	struct Controller;
	struct Obj
	{
		bool dynamic;

	};
	struct FunctionExtr
	{
		/*template <class T, class T2> static auto ExUpdate(T2& result, int i) -> decltype(result = T::funcNmae, void()) {

			result = T::funcNmae;
		}
		template <class T, class T2> static auto ExUpdate(T2& arg, long i) {
			arg = NULL;
		}
		template <class T, class T2> static auto ExUpdate(T2& arg) {
			ExUpdate<T>(arg, 0);
		}
		template <class T, class T31>
		static void ExUpdate(T31& res) {
			if constexpr (ConCp::Update<T,T31>) {
				res = &(T::Update);
			}
			else {
				res = NULL;
			}
		}*/
		ExFunc(Update)
			ExFunc(Start)
			ExFunc(Die)
			ExFunc(OnCollision)
	};
	//[[discard]]
	namespace GamePropsdis {
#define CAST_INT(v) (int)v
		constexpr int propnum = 8;
		using vec4i = Vector4i;
		constexpr int64_t MarkDeployable = 0x1234567890;
		//constexpr int64_t MarkDeployable = 0x1234567890;
		using uint = math::uint;
		enum class state :int16 {
			nan, dieing, died, alive
		};
		enum class Armor :int32 {
			nan, t1, t2, t3, count
		};
		enum class Dmg :int32 {
			nan, laser, explosion, plasma, collision, cut, chemical, count
		};
		struct DmgCoeff {
			float _v1 = 1;
		};
		struct s_Info
		{
			std::array<DmgCoeff, (CAST_INT(Armor::count) - 1)* (CAST_INT(Dmg::count) - 1)> v_;
			constexpr const auto& GetDmgCoeff()const {
				return v_;
			}
			constexpr  auto& GetDmgCoeff(const Armor& at, const Dmg& dt) {
				return v_[(CAST_INT(at) - 1) + (CAST_INT(dt) - 1)];
			}
		};
		constexpr auto CalDmg(const Armor& at, const Dmg& dt, const int& dmgn, s_Info& si) {
			const DmgCoeff& coef = si.GetDmgCoeff(at, dt);
			return coef._v1 * dmgn;
		}
		union BasicProp
		{
			using DataType = Utility::SimdNums<propnum>;
			DataType v8i;
			//static constexpr int size = sizeof(BasicProp);
			using type = DataType::Scalar;
			int feq, attackrangesq, hp, Actionrangesq, visiableRange, damage, energy, Armor;
			BasicProp() {}
		public:
			inline Utility::SimdNums<propnum> data() {
				return v8i;
			}
			template<typename T>
			constexpr void Loop(T&& func) {
				v8i.Loop(func);
			}
			inline type& operator [](const uint id) {
				//const auto div = std::div(id, 4);
				return v8i[id];// [div.quot] [div.rem] ;
			}
			/*template<typename T>
			constexpr DataType::V4T& getV4(const uint id) {
				return v8i.ptr0[id];
				constexpr int size = sizeof(BasicProp);
			}
			template<uint i>
			constexpr DataType::V4T& getV4() {
				return v8i.ptr0[i];
			}*/
		};
		enum ac {
			targetpick,
			targettransfer, //same as targetpick
			targetdeploy//, targetattack,targetfollow,waypoint
		};

		enum StateEm : int64_t {

		};
		struct unitState {
			BasicProp temp;
			const BasicProp temptimec;
			BasicProp temptimer;
			unitState() {}

		};
		struct Skill {
			unitState State;

		};
	}
	/*struct damgb
	{
		int feq, dt, hp, Actionrangesq,visiableRange, damage, energy, Armor;
	};*/
	using damgb = GameProps::BasicProp;
	struct Groupid
	{
		uint tid = -1, Gid = -1;
	};
	struct Faction
	{
		uint tid = -1, Gid = -1;
	};

	struct UnitProps {
		static constexpr int  attackrangesq = 0, hp = 1, Actionrangesq = 2, visiableRange = 3, feq = 4, damage = 5, energy = 6, Armor = 7;
		int val = -1;
		operator const int()const {
			return val;
		}
	};
	template<typename T = int>
	using physobj = Physics::PhysicsObject<T>;
	/*struct StateDesc {
		using PropType = math::Vector4i;
		int num, time, pd;
		UnitProps category;
		PropType Eff;
	};*/
	struct Controller;
	namespace Manager {
		using  GameProps::t_Arch;
		using Physics0 = pj2::Physics0<t_Arch::t_Con>;
		//using t_Renders = ECS::ECSclass<insRander2,FowRender>;
		using a_st = GameProps::ENU::state;
		/*struct State {

		};*/
		struct UpdatePara {
			int id, tid;
			Controller& ct;
			t_Arch& dfac;
			UpdatePara(Controller& ct, t_Arch& ach, const int& tid_, const int& id_) :ct(ct), dfac(ach), tid(tid_), id(id_) {}
			UpdatePara(Controller* ct, t_Arch& ach, int& tid_, int& id_) :ct(*ct), dfac(ach), tid(tid_), id(id_) {}
			inline auto&& Tup() {
				return std::forward_as_tuple(id, tid, ct, dfac);
			}
		};
		struct RenderPara {
			int id, tid;
			Controller& ct;
			RenderContext rct;
			n_Render::renderbatch* rb;
			std::vector<StateTransitionDesc> Barriers;
		};
		template<class T>
		inline auto Cmps(t_Arch& achs, const std::size_t i) {
			return achs.template getComps<T>(i);
		}
	}

	using Manager::Physics0;
	struct Controller
	{
		using t_Arch = Manager::t_Arch;
		struct funcs {
			//void (*Update)(Controller&);
			int (*Update)(Controller&, const uint);
			int (*OnDelete)(Controller&, const uint);
			void (*Update1)(Controller&);
			void (*start)(void*);
			void (*OnCollision)(const CollideID, const CollideID);
			void (*Die)(void*);
			funcs() {
				Update = NULL;
				start = NULL;
				OnCollision = NULL;
				Die = NULL;
				OnDelete = NULL;
			}
			/*template<class T>
			void Exfunc() {

			}*/
		};
		struct tinfo
		{
			funcs fcs;
			int sz, tid,behid=-1,gid=-1,renderid=-1,modid=-1;
			GameProps::unitdesc::aastate unitinfo;
			//GLTF::
			bool CanRender = false;
		};
		// static Controller Instance;
		tf::Executor executor;
		tf::Taskflow taskflow;
		tf::Taskflow tfcopy;
		Physics0 filter;
		std::vector<std::vector<CollideID>> _Ent2collid;
		std::vector<funcs> Actions;
		pj2::task::Task intargratev;
		//ECS::CompAccessor<> Types;
		std::vector<tinfo> _Types;
		std::vector<std::vector<Entity>> _cid2Ent;
		GameProps::s_Info _sinfo;
		using rtpkg =n_Registry::Registry::t_Pkgs;
		using repkg =Utility::MyTuple<n_Registry::RenderReg::typkg>;
		using syspkg = Utility::MyTuple<n_Registry::SysReg::typkg>; //n_Registry::SysReg::t_Pkgs;
		rtpkg _pkgs;
		n_Registry::ReguRen::t_Pkgs _pkurd;
		n_Registry::SysReg::t_Pkgs _pksys;
		repkg _pkgrs;
		n_Registry::Archtypes _atps;
		RenderContext _rt;
		std::unique_ptr<PhysicsWorld> _PhyWorld;
		//std::vector<std::vector<uint>> _Pool;
		//ECS::timei Current=0;
		Controller(uint num = 10000) :filter(num, _cmp0) {
			init(num);
			//_PhyWorld.Init)()
			cds::Initialize();
		}
		Controller(RenderContext& rt, uint num = 10000) :filter(num, _cmp0) {
			init(num);
			//_PhyWorld.Init)()
			cds::Initialize();
			_rt = rt;
		}
		inline GameProps::EntityId convert(sStruct::seleout& pe) {
			const auto& tif = _Types[pe.tid];
			return GameProps::EntityId(tif.behid, tif.gid, tif.tid);
		}
		int dynum;
		void init(uint num = 1000);
		void EcsReg(int reservenum=10000, const size_t bksize=512);
		//constexpr *(mrtransform&) accessTrans = [&](CollideID const& v) {return wod.getTS(v); };

		/*constexpr auto GetTD( CollideID const& id) {
			assert(id.id >= 0);
			return filter.get(id);
		}
		inline auto& GetCD(CollideID const& id) {
			return filter.getCD(id);
		}
		inline mtransfrom& getTS(const CollideID cd)  { return filter._pos[(int)(cd.State)][cd.id]; }

		template<typename T>
		inline auto GetTD(uint const& id) {
			const auto& phyid = _Ent2collid[T::type][id];
			return filter.get(phyid);
		}
		template<typename T>
		constexpr auto& GetAround(const uint id) {
			const auto& phyid = _Ent2collid[T::type][id];
			return filter._around[phyid.id];
		}
		template<typename T>
		constexpr auto AccessAround(const uint id) {
			const auto& phyid = _Ent2collid[T::type][id];
			constexpr auto accessTrans = [&](CollideID const& v) {return filter.getTS(v); };
			return accessTrans(_Ent2collid[id]);
		}*/
		template<typename T>
		constexpr auto& GetEnti(const int tid, const int id) {
			return GetRowComps(tid).Get<T>(id);
		}
		/*inline mtransfrom& getTrans(const int tid, const int id) {
			return _cmp0[tid].template Get<physobj<>>(id).template get<math::mtransfrom>();;
		}*/

		template<typename T>
		constexpr auto&& GetEnti(const ECS::EntityId cd) {
			return GetEnti<T>(cd.tid, cd.id);
		}
		template<typename T>
		constexpr auto&& GetEnti(const CollideID cd) {
			//assert(T::type == _cid2Ent[(int)cd.State][cd.id].tid);
			//return ECSManager::GetEntity<T>(cd.tid, _cid2Ent[(int)cd.State][cd.id].id);
			return GetEnti<T>(cd.tid, cd.id);
			//return ECSManager::con[cd.tid].Get<T>(cd.id);
		//	return ECSManager::GetCompUnCheck<T>(_cid2Ent[(int)cd.State][cd.id]);
		}
		inline auto GetBox(const math::Efloat3 pos, Physics::PhysicsNum r) {
			Box2i bx;
			const auto min = (pos.array() - r).head<2>();
			const auto max = (pos.array() + r).head<2>();
			bx.Min = Physics::Quantize(min, Physics::cellsize).cast<int>();
			bx.Max = Physics::Quantize(max, Physics::cellsize).cast<int>();
			return bx;
		}
		constexpr auto Getcellsize() {
			return Physics::cellsize;
		}
		/*	inline auto DeQuantize(qCell& voxeli) {
				return (voxeli.cast<Physics::PhysicsNum>() * Physics::Standardcell);
			}
			inline auto GetMindis(math::vec3f pos,qCell& voxeli) {
				auto dis = Physics::Standardcell.cast<Physics::PhysicsNum>().norm() / 2;
				return (voxeli.cast<Physics::PhysicsNum>() * Physics::cellsize);
			}*/
		template<typename T0, typename T1>
		void AccessCellObj(const T0 qcpos, const T1&& resultaccess) {
			filter._posmap.find(qcpos, [&](PosHash::value_type& val) {
				auto& val2 = *val.second;
				resultaccess(val2);
				}
			);
		}
		void Check() {
			for (const auto dd : filter._posmap)
			{

			}
		}
		//template<typename T>
		//constexpr void RegistPhyobj(int reservenum, CollideID::CollidDS state = CollideID::CollidDS::Dynamic) {
		//	constexpr auto isPhyobj = std::is_convertible<T, Physics::PhysicsObject<>&>();
		//	//constexpr auto isPhyobj2 = std::is_base_of_v<T, Physics::PhysicsObject<>&>();
		//	static_assert(isPhyobj,
		//		"can't convert to Physics::PhysicsObject<>>");
		//	ECSManager::RegistEntity<T>(reservenum);
		//	if constexpr (isPhyobj)
		//		filter.Reg(ECSManager::last(), (int)state);
		//	auto& func = Actions.emplace_back();;
		//	Types.push_back(T::type);
		//	FunctionExtr::ExUpdate<T>(func.Update);
		//	FunctionExtr::ExDie<T>(func.Die);
		//	FunctionExtr::ExOnCollision<T>(func.OnCollision);

		//	_Ent2collid.resize(_Ent2collid.size() + 1);
		//}static id_type next() ENTT_NOEXCEPT {
		//std::vector<entt::registry> eRegis;
		std::vector<t_Arch> _Archs;
		std::vector<t_Arch::t_Con> _cmp0;
		//template<typename T>
		//constexpr void Regist(int reservenum, CollideID::CollidDS state = CollideID::CollidDS::Dynamic) {
		//	constexpr auto isPhyobj = std::is_convertible<T, Physics::PhysicsObject<>&>();
		//	ECSManager::RegistEntity<T>(reservenum);
		//	if constexpr (isPhyobj)
		//		filter.Reg(ECSManager::last(), (int)state);
		//	auto& func = Actions.emplace_back();;
		//	//if constexpr (std::is_same<ammu, T>) 
		//	{
		//		//func.Update = T::Update;
		//	}
		//	
		//	Types.push_back(T::type);
		//	FunctionExtr::ExUpdate<T>(func.Update);
		//	FunctionExtr::ExDie<T>(func.Die);
		//	FunctionExtr::ExOnCollision<T>(func.OnCollision);
		//	auto& ack=Archs.emplace_back();
		//}

		template<typename T>
		constexpr uint& Typeid() {
			return T::type;
		}
		//template<typename T, class... _Types>
		//constexpr void Regist(int reservenum, CollideID::CollidDS state = CollideID::CollidDS::Dynamic,
		//	const size_t bksize = 512,
		//	tinfo fc = {}) {
		//	auto& tid = Typeid<T>();
		//	if (tid != ECS::dfid)
		//		return;
		//	assert(tid == Maxid);
		//	tid = Archs.size();
		//	fc.tid = tid;
		//	constexpr auto isPhyobj = std::is_convertible<T, Physics::PhysicsObject<>&>();
		//	auto& ack = Archs.emplace_back();
		//	using em = ECS::ECSclass<_Types...>;
		//	ack.rig<em>(reservenum, tid, bksize);
		//	_cmp0.emplace_back(sizeof(T), tid, reservenum, bksize);
		//	if constexpr (isPhyobj)
		//		filter.Reg(GetRowComps(tid), (int)state);
		//	//auto& func = Actions.emplace_back(fc);;
		//	Actions.emplace_back(fc.fcs);
		//	this->_Types.emplace_back(fc);
		//	const auto clasnum = Actions.size();
		//	_pools.resize(clasnum);
		//	_pooldels.resize(clasnum);
		//	_Activer.resize(clasnum);
		//	if constexpr (std::is_base_of_v<BasObj::Moveobj, T>) {
		//		_MoveObj.emplace_back(tid);
		//	}
		//	//Types.push_back(tid);
		//	/*FunctionExtr::ExUpdate<T>(func.Update);
		//	FunctionExtr::ExDie<T>(func.Die);
		//	FunctionExtr::ExOnCollision<T>(func.OnCollision);*/
		//}
		inline auto& GetComps(const uint tid) {
			return _Archs[tid];
		}
		inline auto& GetRowComps(const uint tid) {
			return  _cmp0[tid];
		}
		template<typename T>
		inline auto& GetRowComps() {
			return  GetRowComps(T::type);
		}
		template<typename T>
		inline auto& GetComp(const uint i) {
			return  GetRowComps(T::type).Get<T>(i);
		}
		/*template<typename T>
		inline auto GetComps() {
			return ECS::Conarray<T,t_Arch::t_Con>(Archs<T>().Cmp<T>());
		}*/
		template<class ...Ts>
		inline auto GetObj(const CollideID& id, std::tuple<Ts*...>& ot) {
			auto& pkgs = _pkgs;
			ECS::visit_at<n_Registry::Registry>(id.tid0, [&](auto I) {
				pkgs.get<I.value>()[id.tid].comps.get(id.id, ot);
				},
				[&ot](auto I) {
					std::memset(&ot, 0, sizeof(ot));
				});
		}
		inline auto& getArch(const uint i) {
			return _Archs[i];
		}
		template<std::size_t I>
		constexpr auto& getArch() {
			return _Archs[I];
		}
		double dt1 = 0, dt0 = 0;
		size_t freamid = 0;

		struct objrep
		{
			math::uint tid, id;
			//xyz and vision range
			math::int4 pos;
			math::uint4 groupids;
			math::uint4 targetfilter;
		};
		struct Did
		{
			Did(size_t i) :id(i) {}
			operator const size_t()const {
				return id;
			}
		private:
			size_t id;
		};
		struct PerTypeInfo
		{
			std::vector<math::uint> deleted;
		};
		std::vector<PerTypeInfo> Typeinfo;

		n_Render::rendermanager _rmgr;

		struct reginfo
		{
			int maxnum;
			int reservenum;
			CollideID::CollidDS state = CollideID::CollidDS::Dynamic;
			size_t bksize = 512;
			tinfo fc = {};
			bool cull = false, isPhyobj;
		};
		struct behaivorgroup
		{
			n_Render::RenderAttr rattr;
			struct Ebehaivorgroup
			{
			};
		};
		std::vector<int>_cullGroup;
		//int _tid = 0;
		//FowRender fowrender;
		//buffer0 transform,buffer1 instance property,buffer2 cull result
		std::vector<n_Render::RenderAttr*>culled;
		std::array<n_Render::RenderAttr, 20> _Renders;
		void Regist(int I);;
		void finishRegist();
		void PreDraw() {

		}
		//using t_ps = std::tuple < mtransform*, sStruct::instData1*>;
		std::vector<sStruct::bufferinfo> so_bufferinfo;
		/*void Cull(RenderContext& rc) {
			using bfif = sStruct::bufferinfo;
			const auto cusz = _cullGroup.size();
			so_bufferinfo.resize(cusz);
			for (size_t i = 0, k = 0; i < cusz; i++)
			{
				const auto& id = _cullGroup[i];
				auto& bfif = so_bufferinfo[i];
				bfif.count = (int)size(id);
				bfif.org = k;
				k += bfif.count;
			}
			constexpr auto tsz = sizeof(mtransfrom);
			constexpr auto inssz = sizeof(sStruct::instData1);
			auto& m_pImmediateContext = rc.m_pImmediateContext;
			MapHelper<mtransfrom>vmtransfrom(m_pImmediateContext, RCull.m_vfTrans, MAP_WRITE, MAP_FLAG_DISCARD);
			MapHelper<sStruct::instData1>vins
			(m_pImmediateContext, RCull.m_vfProps, MAP_WRITE, MAP_FLAG_DISCARD);
			RenderUtil::UpdateBuffer(m_pImmediateContext, RCull.m_soBFinfo, 0, so_bufferinfo, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
			for (size_t i = 0; i < _cullGroup.size(); i++)
			{
				const auto& id = _cullGroup[i];
				const auto& binfo = so_bufferinfo[i];

				auto& ach_ = getArch(id);
				auto& trans = ach_.getrowcp<mtransfrom>();
				auto& insp = ach_.getrowcp<sStruct::instData1>();
				trans.CopyTo(vmtransfrom + (binfo.org * tsz), binfo.count);
				insp.CopyTo(vins + (binfo.org * inssz), binfo.count);
			}
		}*/
		void Render() {


		}
		/*tf::Executor executor;
		tf::Taskflow taskflow;*/
		void Update(void* res = NULL);
		template<class...Ts>
		void execmd(Ts... args);
		static Controller& main();
		void Serialize(ResManager& rm);
		void DeSerialize(ResManager& rm);
		//template<typename T>
		//inline Collid& createCir(Collid&& outcd, T& cp, mtransfrom const& trans, const bool isstatic = false) {
		//	assert(T::type != Maxid);
		//	const auto state = isstatic ? CollideID::CollidDS::Static : CollideID::CollidDS::Dynamic;
		//	const auto num = ECSManager::GetComps<T>().size();
		//	int id;
		//	CollideID ID(-1, T::type, state);
		//	outcd = filter.Creat((outcd), boxSize, trans, ID, state);
		//	ECSManager::GetComps<T>().add(cp).SetCollide(ID);
		//	_Ent2collid[T::type].push_back(ID);
		//	_cid2Ent[(int)ID.State].emplace_back(T::type, num);
		//	return outcd;
		//	//return Entity = { T::type ,num };
		//}
		constexpr size_t GetFId()const {
			return freamid;
		}
		//template<typename T>
		//inline Collid& createBox(Collid&& outcd, const Efloat2 boxSize, T const& cp, mtransfrom const& trans, const bool isstatic = false) {
		//	assert(T::type != Maxid);
		//	const auto state = isstatic ? CollideID::CollidDS::Static : CollideID::CollidDS::Dynamic;
		//	const auto num = ECSManager::GetComps<T>().size();
		//	ECSManager::GetComps<T>().add() = cp;
		//	int id;
		//	outcd = &filter.Creat(outcd, boxSize,trans, id, state);
		//	const CollideID ID(id, T::type ,state);
		//	_Ent2collid[T::type].push_back(ID);
		//	_cid2Ent[(int)ID.State].emplace_back(T::type, num, ID);
		//	return outcd;
		//	//return Entity = { T::type ,num };
		//}
		//template<typename T>
		//inline Collid& createBox(Collid& outcd, const Efloat2 boxSize, T& cp, mtransfrom& trans, const bool isstatic = false) {
		//	assert(T::type != Maxid);
		//	const auto state = isstatic ? CollideID::CollidDS::Static : CollideID::CollidDS::Dynamic;
		//	const auto num = ECSManager::GetComps<T>().size();
		//	if (!_bxPool.empty()) {
		//		auto& bx = *_bxPool.top();
		//		_bxPool.pop();
		//		bx.init(boxSize, outcd);
		//		outcd.data_ptr = &bx;
		//	}
		//	else
		//	{
		//		auto& bx = _bxBlock.emplace_back();
		//		bx.init(boxSize, outcd);
		//		outcd.data_ptr = &bx;
		//	}

		//	return outcd;
		//	//return Entity = { T::type ,num };
		//}

		//
		template<typename T>
		inline Collid& createBox(Collid& outcd, const Efloat2 boxSize, const bool isstatic = false) {
			assert(T::type != Maxid && outcd.data_ptr == NULL);
			const auto state = isstatic ? CollideID::CollidDS::Static : CollideID::CollidDS::Dynamic;
			auto& bx = *(IBoxCollider*)_bxPool.allocate();
			//_bxPool.pop();
			bx.init(boxSize, outcd);
			outcd.data_ptr = &bx;
			/*	if (!_bxPool.empty()) {
					auto& bx = *_bxPool.malloc();
					_bxPool.pop();
					bx.init(boxSize, outcd);
					outcd.data_ptr = &bx;
				}
				else
				{
					auto& bx = *_bxPool.malloc();
					bx.init(boxSize, outcd);
					outcd.data_ptr = &bx;
				}*/

			return outcd;
			//return Entity = { T::type ,num };
		}
		template<typename T>
		constexpr T& Create(const Collid outcd, const Efloat2 boxSize, mtransfrom& trans, const bool isstatic = false) {
			return Create<T>(outcd, trans, boxSize, isstatic);
		}
		/*template<typename T, typename ...Args>
		constexpr T& Create(const Collid outcd, const Efloat2 boxSize, mtransfrom& trans, const bool isstatic = false) {
			return Create<T>(outcd, trans, boxSize, isstatic);
		}*/
		/*template<typename T>
		constexpr T& Create() {
			return GetRowComps<T>().add<T>();
		}*/
		//cds::container::SegmentedQueue<int> dfed;
		std::vector<Utility::MuQue<math::uint>> _pools;
		//create pool at next frame
		std::vector<Utility::MuQue<math::uint>> _pooldels;

		std::vector<math::ui64> _MoveObj;
		std::mutex _mutex;
		std::vector<std::vector<bool>> _Activer;
		bool OnCreate = false;
		inline void readyforuse(const int tid, const int id) {
			/*if (_pools.size() < tid + 1) [[unlikely]] {
				_pools.resize(tid + 1);
			}*/
			//_pools.emplace_back();
			_pools[tid].push(id);
			_Activer[tid][id] = false;
		}
		//template<typename T, typename... Args>
		//inline auto Create(const int tid) {
		//	/*if (_pools.size() < tid + 1) [[unlikely]] {
		//		_pools.resize(tid + 1);
		//	_pooldels.resize(tid + 1);
		//	}*/
		//	OnCreate = true;
		//	auto&& rowcmp = GetRowComps<T>();
		//	auto&& vt = _pools[tid];
		//	auto&& tif = _Types[tid].sz;
		//	math::uint id;
		//	auto&& ach = getArch(tid);
		//	if (vt.try_pop(id)) {
		//		_pooldels[tid].push(id);
		//		assert(!_Activer[tid][id]);
		//		_Activer[tid][id] = true;
		//	}
		//	_Activer[tid].emplace_back(false);
		//	id = ach.size();
		//	_pooldels[tid].push(id);
		//	rowcmp.addrow();
		//	getArch(tid).add();
		//	constexpr auto isPhyobj = std::is_convertible<T, Physics::PhysicsObject<>&>();
		//	if constexpr (isPhyobj) {
		//		filter.OnAdd(GetRowComps<T>());

		//	}
		//	return id;
		//}
		inline int size(const int& tid) const {
			return _Activer[tid].size();
		}

		//template<typename ...Ts>
		//inline int Create(const GameProps::EntityId& tid, std::vector<GameProps::EntityId>& tins);
		template<typename ...Ts>
		inline int Create(const Ts&...arg);
		template<class T0, typename ...Ts>
		inline int Create(Ts&...arg);
		//template<typename T>
		//inline auto Create(const int tid);
		template<typename T, typename... Args>
		constexpr T& Emplace(Args&&... args) {
			//std::make_tuple()
			/*if constexpr (std::is_base_of_v<T, FixTimeObj<T>>) {
				Conany& d = GetRowComps<T>();
				return *new(T::RingAlloctor.allocate(d))T(*this, std::forward<Args>(args)...);
			}*/
			auto&& gp = GetRowComps<T>();
			auto const p = gp.Get(Create<T>(this->Typeid<T>()));
			auto&& nv = *new(p)T(std::forward<Args>(args)...);
			constexpr auto isPhyobj = std::is_convertible<T, Physics::PhysicsObject<>&>();
			/*if constexpr (isPhyobj) {
				filter.OnAdd<T>(gp);
			}*/
			return nv;
		}

		template<typename T>
		constexpr void reserve(std::size_t nm) {
			static_assert(!std::is_base_of_v<T, FixTimeObj<T>>);
			//std::make_tuple()
			/*if constexpr (std::is_base_of_v<T, FixTimeObj<T>>) {
				Conany& d = GetRowComps<T>();
				return *new(T::RingAlloctor.allocate(d))T(*this);
			}*/
			(GetRowComps<T>().reserve(nm));
		}

		template<typename T, typename... Args>
		constexpr T& EmplaceR(Args&&... args) {
			Conany& d = GetRowComps<T>();
			return *new(T::RingAlloctor.allocate(d))T(*this, std::forward<Args>(args)...);
		}
		template<typename T>
		constexpr T& GetorCreate() {
			auto& rowComp = GetRowComps<T>();
			return  rowComp.size() == 0 ? GetRowComps<T>().add<T>() : rowComp.last<T>();
		}
		template<typename T>
		T& Create(const Collid outcd, T& data, mtransfrom& trans) {
			static_assert(std::is_convertible<T, Physics::PhysicsObject<>>::value);
			auto& vss = GetRowComps<T>();
			auto& last = (Physics::PhysicsObject<>)vss.add<T>(data);
			last.collider = outcd;
			last.transform = trans;
			return reinterpret_cast<T&>(last);
		}
		Physics::PhysicsObject<>& GetPhyObj(const Vox id) {
			return GetRowComps(id.cd.tid).Get<Physics::PhysicsObject<>>(id.cd.id);
		}
		/*template<typename T>
		inline void Delete(const uint Entityid, const CollideID cdid) {
			Util::removeResize(_Ent2collid[T::type], Entityid);
			Util::removeResize(_cid2Ent[(int)cdid.State], cdid.id);
			filter.erase(cdid);
		}*/
		//using BasicStr = Unit<>;
		template<typename T>
		bool Deletes(const T&& Matcher, const math::Vector3f trs) {
			//CollideID cd;
			return false;
			//ECSManager::erase(cd.tid, cd.id);
		}
		//std::vector<IBoxCollider> _bxBlock;
		//std::stack<uint> _bxPool;
		lfpAlloc::Pool<sizeof(IBoxCollider), 8192> _bxPool;// = *new boost::object_pool<IBoxCollider>(100, 29);
		template<class ...Ts>
		bool Delete(Ts&...args);
		template<class T0,class Ti>
		int Delete(T0& arg,Ti&i) {
			static_assert(!ECS::convertible<T0, n_BasObj::ecsphysobj>());
			auto& vecunit = arg;
			vecunit.Delete(i);
			return 1;
		}
		inline void DeleteR(const CollideID cd) {

		}
		/*template<typename T>
		inline void Delete(const uint id) {
			const auto tid = T::type;
			const auto cdid = _Ent2collid[tid][id];
			ECSManager::erase<T>(id);
			Util::removeResize(_Ent2collid[tid], id);
			Util::removeResize(_cid2Ent[(int)cdid.State], cdid.id);
			filter.erase(cdid);
		}*/
		/*template<size_t... Is, typename T>
		void foo(indices<Is...>)
		{
			auto list = { (T<Is>().doStuff(), 0)... };
		}*/
		~Controller()
		{
			cds::Terminate();
		}
		using cvetb = std::array<int, 4>;
		using t_sinout = sStruct::seleout;

		//! assume di is ordered
		inline void ToObjSpace(std::vector<t_sinout>& di,
			std::vector<cvetb>& odesc, std::vector<int>& dout) {
			Controller& cont = *this;
			auto& table = cont._Types;
			int i = 0, pbid;
			while (i < di.size())
			{
				auto& tr = odesc.emplace_back();
				auto& tbe = table[di[i].tid];
				tr[0] = i;
				tr[2] = tbe.behid;
				tr[3] = tbe.gid;
				do {
					auto& die = di[i];
					dout.emplace_back(die.itid);
					i++;
					if (i >= di.size() && die.tid != di[i].tid)
						break;
				} while (true);
				tr[1] = i - tr[0];
			}
		}
		inline int addtid(int gid,int bid=0) {
			tinfo& t = _Tidtable[tidcounter];
			t.gid = gid;
			t.behid = bid;
			t.tid = tidcounter;
			return tidcounter++;
		}
		inline tinfo gettidmap(const int tid) {
			return _Tidtable[tid];
		}
		private:
		std::array<tinfo,512> _Tidtable;
		int tidcounter = 0;
	};

	//gc_type Controller::hpGC;

}