#pragma once
#include "Common\Math1.h"
#include"Common/ecs.hpp"
#include"Common/Utility.h"
#include<execution>
#include <string>

#include "cds/container/lazy_list_dhp.h"
#include <cds/container/feldman_hashmap_dhp.h>
#include <cds/init.h> // for cds::Initialize and cds::Terminate
#include <cds/gc/hp.h>
#include <cds/container/segmented_queue.h>
#include <cds/container/feldman_hashmap_rcu.h>
#include <cds/urcu/general_buffered.h>
#include <cds/container/feldman_hashset_dhp.h>
#include "taskflow/taskflow.hpp"
#include "physics/physics.hpp"
//#include <bx/bx.h>
#include "common/span.hpp"
#include <cds\container\impl\feldman_hashset.h>
#include <ecs.hpp>
#include<lfpAlloc/Pool.hpp>
#include<parallel_hashmap/phmap.h>
#include"Common/Task.h"
//import physicsfc;
//using namespace tbb::interface5;
//struct PhysicsWorld {
//
//    concurrent_unordered_multimap<math::float2, int> posmap;
//};
namespace pj2 {


	typedef cds::gc::DHP gc_type;
	static gc_type hpGC;
	using voxele = CollideID;
	using namespace math;
	using rcugc = cds::urcu::gc<cds::urcu::general_buffered<>>;
	const int gridmaxsize = 30;
	template <typename T>
	struct matrix_hash : public cds::container::feldman_hashset::traits
	{
		inline std::size_t operator()(T const& matrix) const
		{
			// Note that it is oblivious to the storage order of Eigen matrix (column- or
			// row-major). It will give you the same hash value for two different matrices if they
			// are the transpose of each other in different storage order.
			size_t seed = 0;
			for (size_t i = 0; i < matrix.size(); ++i)
			{
				auto elem = *(matrix.data() + i);
				seed ^= std::hash<typename T::Scalar>()(elem); // + 0x9e3779b9 + (seed << 6) + (seed >> 2);
			}
			return seed;
		}

		struct hash_accessor
		{
			size_t operator()(T& matrix) const
			{
				size_t seed = 0;
				for (size_t i = 0; i < matrix.size(); ++i)
				{
					auto elem = *(matrix.data() + i);
					seed ^= std::hash<typename T::Scalar>()(elem); // + 0x9e3779b9 + (seed << 6) + (seed >> 2);
				}
				return seed;
			}
		};
	};


	//using Vox= CollideID;
	template<class T>
	using spa = Utility::spa<T>;

	struct map_traits : public cds::container::feldman_hashmap::traits
	{
		//typedef matrix_hash<math::float2> hash;

		struct hash_accessor
		{
			/*size_t operator()(math::Efloat2& matrix) const
			{
				return reinterpret_cast<size_t&>(matrix);
			}*/
			const size_t& operator()(const math::int2& matrix) const
			{
				return reinterpret_cast<const size_t&>(matrix);
			}
		};
	};
	typedef cds::container::FeldmanHashMap<pj2::rcugc, math::Efloat2, int>
		map_type;
	struct voxeleHashAccessor : public cds::container::feldman_hashset::traits
	{
		//using hash_accessor = map_traits::hash_accessor;
		struct hash_accessor
		{
			template <typename T>
			size_t operator()(T const& src) const
			{
				return ((size_t)src);
			}
		};
		/*size_t hash_accessor(voxele const& src) const
		{
			return std::hash<size_t>()((size_t)src);
		}*/
	};
	struct list_traits : public cds::intrusive::lazy_list::traits
	{
		// my_data comparing functor
		struct compare
		{
			template <typename S, typename Q>
			bool operator()(const typename Q::m_Value& d, const Q& s)
			{

				return d == s;
			}
		};
	};
	using conlist = cds::container::LazyList<gc_type, voxele>;
	//using vox = cds::container::FCDeque<voxele>;
	//using voxPtr = vox*;
	using mSet = cds::container::FeldmanHashSet<gc_type, voxele, voxeleHashAccessor>;
	struct Setchecks :mSet
	{
		bool inside;
		Setchecks(int segnum) :mSet(), inside(false)
		{}
		Setchecks() :mSet(), inside(false) {}
	};
	//struct Quechecks :conQueue<voxele>
	//{
	//	bool inside;
	//	Quechecks(int segnum) :SegmentedQueue(segnum), inside(false)
	//	{}
	//};
	//export{
	using voxel = spa<voxele>;
	using gridPtr = voxel*;
	//math::Efloat2 cell(4, 4);
	//using conQueue = cds::container::SegmentedQueue<gc_type, T>;
	//
	//template <typename T>
	//using FeldmanHashSet = cds::container::FeldmanHashSet<gc_type, T, voxeleHashAccessor>;
	//using segmentqueue = conQueue<gridPtr>;
	using qCell = math::int2;
	using PosHash = cds::container::FeldmanHashMap<gc_type, qCell, gridPtr, map_traits>;
	//};
	//struct Box2i {
	//	using T = qCell;
	//	T Max, Min;
	//};
	//using box2i = Box2<int2>;
	struct coarse {
		thread_local static std::vector<gridPtr> gd;
	};

	const int maxaround = 32;

	template<typename T>
	struct FixAy
	{
		T _da[maxaround];
		size_t counter;
		constexpr void add(T const& t) {
			assert(counter < maxaround);
			_da[counter++] = t;
		}
		FixAy() :counter(0) {}
		T& operator[](size_t i) {
			return _da[i];
		}
		constexpr void erase(int i) {
			_da[i] = _da[--counter];
		}
		constexpr size_t size() {
			return counter;
		}
		constexpr size_t remain() {
			return maxaround - counter;
		}
		void clear() {
			counter = 0;
		}
	};
	//namespace{}
	//using namespace std;

	//inline void CalculStartEnd(math::float2 const& CellSize, const Box2d& bounds) {
	//	bounds.Min = (bounds.Min.cwiseQuotient(CellSize)).array().round();
	//	bounds.Max = ((bounds.Max.cwiseQuotient(CellSize)).array().round());
	//	//bounds.Max += math::float2(1, 1);
	//};
	// Structure that defines hashing and comparison operations for user's type.
	//export
	template<typename T>
	constexpr void LoopGrid(Box2i& bx, T const& f) {

		const var& lf = bx.Min;
		const var& tr = bx.Max;

		decltype(Box2i::Max)  xel;
		for (auto i = lf[0]; i <= tr[0]; i++)
		{

			xel[0] = i;
			for (auto p = lf[1]; p <= tr[1]; ++p)
			{
				xel[1] = p;
				f(xel);
			}
		}
	}
	//export
	template<class T>
	constexpr void LoopBox(const T&& fun, const Box2i bx) {
		qCell  xel;
		{
			var& lf = bx.Min;
			var& tr = bx.Max;
			for (auto i = lf[0]; i <= tr[0]; ++i)
			{
				xel[0] = i;
				for (auto p = lf[1]; p <= tr[1]; ++p)
				{
					xel[1] = p;
					fun(xel);
				}
			}
		}
	}
	namespace poshsstr {
		struct PosFilterFunc
		{

			static auto&& CheckAround(const Vox& vv, const Vox& vm, Physics::PhysicsObject<>& pj, Physics::PhysicsObject<>& pm) {
				return (pj.transform.pos4 - pm.transform.pos4).squaredNorm();
			}
		};

		struct Pyprop
		{
			int rep = -1;
		};
	};
	struct MyObserver : public tf::ObserverInterface {

		struct timer final {
			timer() {}
			void start0() {
				start = std::chrono::system_clock::now();
			}
			void elapsed(std::string&& nm = "") {
				auto now = std::chrono::system_clock::now();
				LOGINFO("cost {0} sec {1}", std::chrono::duration<double>(now - start).count(), nm);
			}
			auto stop() {
				auto now = std::chrono::system_clock::now();
				return  std::chrono::duration<double>(now - start).count();
			}
		private:
			std::chrono::time_point<std::chrono::system_clock> start;
		};
		timer t;
		MyObserver(const std::string& name) {
			std::cout << "constructing observer " << name << '\n';
		}

		void set_up(size_t num_workers) override final {
			std::cout << "setting up observer with " << num_workers << " workers\n";
		}

		void on_entry(tf::WorkerView w, tf::TaskView tv) override final {
			t.start0();
		}

		void on_exit(tf::WorkerView w, tf::TaskView tv) override final {
			INFO("{0} cost {1} ms", tv.name(), t.stop() * 1000);
		}
	};
	using Physics::aroundele;
	template<class T>
	//export
	class Physics0
	{
		using voxele0 = Vox;
		template<class T, class T1>
		constexpr int getAddress(const T coord0, const T1 halfMapsize) {
			const auto coord = coord0 + halfMapsize.head<2>().cast<int>();
			return coord[0] + coord[1] * halfMapsize[1];
		}
		//const T& _objcolle;
	public:
		using Lavnes = Physics::Lavnes;
		struct mapir
		{
			std::size_t gid, b;
		};
		using voxel0 = spa<voxele0>;
		using gridPtr0 = voxel0*;
		using PosHash0 = cds::container::FeldmanHashMap<gc_type, qCell, gridPtr0, map_traits>;
		/*struct myTraits : public cds::container::vyukov_queue::traits {
			typedef cds::atomicity::item_counter    item_counter;
		};*/
		typedef cds::container::VyukovMPMCCycleQueue< math::uint > GcQueue;
		//GcQueue Gcq;
		Utility::RingQue<math::uint, 1024 * 256> Gcq;
		template<class C, class... _Types>
		inline auto parallelloop(tf::Taskflow& tf,C&& f,std::vector<math::ui64>& gids, const math::ui64 chunksize, _Types&&... _Args) {
			if constexpr (sizeof...(_Types) == 0) {
				return TaskEx::parallelfor(tf, std::forward<C>(f), [&](auto&& gid) {return &this->Objects[gid]; },
					gids, chunksize);
			}
			return TaskEx::parallelfor(tf, std::forward<C>(f), [&](auto&& gid) {return &this->Objects[gid]; },
				gids,chunksize, std::forward<_Types>(_Args)...);
		}
		/// <summary>
		/// insert->remove->deduplicate per entity->solve constrain
		/// </summary>
		
		void Move() {}
		template<class T>
		void Move(){
			using t_obj = Physics::eobj;
			ECS::visit_all_convertible<T, t_obj>([&](auto I) {
				using ECl = T::template t<I>;
				});
		}
		inline void erase(const CollideID& id);
		inline void erase(const int& tid, const int& id);
		void erase(CollideID&& id);
		//void Move1();
		
		inline voxel0& getvox(math::uint const i) {
			return voxel0s[i];
		}
		inline auto& DynamicObjs() {
			constexpr int dyid = (int)CollideID::CollidDS::Dynamic;
			return PhysObj[dyid];
		}
		inline auto& FixObjs() {
			constexpr int dyid = (int)CollideID::CollidDS::Static;
			return PhysObj[dyid];
		}
		std::vector<std::atomic<GcQueue::value_type>> PMaps;
		std::vector<voxel0> voxel0s;
		//T& _Cont;
		//PosHash0 _posmap;
		/*struct _posmap1k
		{
			uint c;
			std::atomic_uint32_t sz;
			operator math::uint() {
				return c;
			}
		};*/
		using _posmap1k = uint;
		cds::container::FeldmanHashMap<gc_type, qCell, _posmap1k, map_traits> _posmap;
		math::uint4 _mapsize;

		//static const math::Efloat2 cell((Eigen::Vector2f() << cellsize, cellsize).finished());
		//static const math::Efloat2(4, 4) cell;
		Physics0(size_t dynum, std::vector<T>& Object,math::uint4 mapsize = { 1024,1024,1,1 },
			size_t GcQueueCapacity = 1024 * 256, size_t stanum = 0) :
			_manfu(dynum * 6), Objects(Object), _corase(10 * dynum),// Gcq(GcQueueCapacity),
			solvepair(10 * dynum), PMaps(mapsize.prod()), _mapsize(mapsize)
		{
			cds::Initialize();
			/*for (auto&& tb : PMaps) {
				tb = std::numeric_limits<math::uint>::max();
			}*/
			gridPtr0 vx = NULL;
			voxel0s.resize(GcQueueCapacity);
			//remain 0 unused for compatible with NULL pointer
			for (size_t i = 1; i < voxel0s.size(); i++)
			{
				Gcq.enqueue(i);
			}
			cds::threading::Manager::attachThread();
			//TaskEx::parallelfor(taskflow, 0, (int)executor._threads.size(), [&](int i) {
			//	cds::threading::Manager::attachThread();
			//	//_gcvoxs.reserve(dycount / std::thread::hardware_concurrency() * gridmaxsize * 8);
			//	}, 1);
			//taskflow.clear();
			cpunum = std::thread::hardware_concurrency();
		}
		template<typename T>//, typename T1>
		void QuaryAround(const math::Efloat4 pr, const T&& Matcher) {
			/*const auto v2i = Physics::Quantize(pos.head<2>(), Physics::cellsize).cast<int>();
			const auto halfMapsize = _mapsize / 2;
			auto& acpos = PMaps[getAddress(v2i.head<2>(), halfMapsize)];*/
			Box2i qb;
			Box2f bbox(pr.head<2>(), pr[3]);
			const auto halfMapsize = _mapsize / 2;
			CalculStartEnd(Physics::Standardcell, bbox, qb);
			LoopBox([&](const auto xel) {
				/*const auto& dis0 = qb - xel;
					const auto mindis=*/
				auto& acpos = PMaps[getAddress(xel, halfMapsize)];
				if (acpos) {
					var& vx = (voxel0s[acpos.load(std::memory_order_relaxed)]);
					Matcher(vx);
				}
				}, qb);
		}
		//template<typename T>//, typename T1>
		auto& Arounds(std::size_t i) {
			return _AroundInfo[(_Didmap[i]).gid];
		}
		template<typename T>//, typename T1>
		void QuaryPos(const math::Efloat4 pr, const T&& Matcher) {
			/*const auto v2i = Physics::Quantize(pos.head<2>(), Physics::cellsize).cast<int>();
			const auto halfMapsize = _mapsize / 2;
			auto& acpos = PMaps[getAddress(v2i.head<2>(), halfMapsize)];*/
			Box2i qb;
			Box2f bbox(pr.head<2>(), pr[3]);
			const auto halfMapsize = _mapsize / 2;
			const auto xel = Quantize(pr.head<2>(), Physics::Standardcellf.head<2>()).cast<int>();
			/*const auto& dis0 = qb - xel;
				const auto mindis=*/
			auto& acpos = PMaps[getAddress(xel, halfMapsize)];
			if (acpos) {
				var& vx = (voxel0s[acpos.load(std::memory_order_relaxed)]);
				Matcher(vx);
			}
		}

		//template<typename T, typename T1>
		//bool erase(const T&& Matcher, const T1&& postmatch, const math::Efloat3 pos) {
		//	//auto& pos = trs.pos;
		//	const auto v2i = Physics::Quantize(pos.head<2>(), Physics::cellsize).cast<int>();
		//	using vtype = PosHash0::value_type;

		//	auto& posmap = _posmap;
		//	return _posmap.find(v2i, [&](vtype& val) {
		//		auto& val0 = *val.second;
		//		CollideID deletarget;
		//		for (size_t i0 = 0; i0 < val0.size(); i0++)
		//		{
		//			auto ele = val0[i0];
		//			if (Matcher(ele)) {
		//				auto& poj = GetObj(ele);
		//				LOGINFO("cbox {0} address={1} ele{2}", GetObj(ele).cBox.toString(), (size_t)&poj, ele.cd);
		//				const auto bx = poj.cBox;
		//				const auto lf = bx.Min;
		//				const auto tr = bx.Max;
		//				deletarget = ele.cd;
		//				qCell  xel;
		//				for (auto i = lf[0]; i <= tr[0]; ++i)
		//				{
		//					xel[0] = i;
		//					for (auto p = lf[1]; p <= tr[1]; ++p)
		//					{
		//						xel[1] = p;
		//						if (!_posmap.find(xel, [&](vtype& val) {
		//							var& val2 = getvox(val.second);

		//							if ((val2.erase(ele))) {
		//								if (val2.empty()) {
		//									if (!_posmap.erase(xel, [&](vtype& val) {
		//										assert(!val.second->inside);
		//										dealloc(val.second);
		//										})) {
		//										ERROR(fmt::format("erased empty voxel0 {0} fail", xel));
		//										std::abort();
		//									}
		//									return;
		//								}
		//								return;
		//							}
		//							/*ERROR("erase fail id={0} thread:{1} xel={2} i={3} p={4} obx={5} new box={6}",
		//								cdid, std::thread::id::id(), xel, i, p, obx.toString(), bbox.tof4());*/
		//							val2.erase(ele);
		//							const map_traits::hash_accessor hasher;
		//							/*LoopGrid(obx, cell, [&](auto& const bxele) {
		//								ERROR("inside hash {0} bxele={1} xel hash={2}", hasher(bxele), bxele, hasher(xel));
		//								});*/
		//							for (size_t i = 0; i < val2.size(); i++)
		//							{
		//								var& ele0 = val2.ele[i];
		//								var same = false;
		//								if (ele0 == ele)
		//									same = true;
		//								ERROR("xel={0} ele={1} thread:{2} same {3} st {4} {5} i={6} reinsert={7}",
		//									xel.transpose(), (size_t)ele0.cd, std::thread::id(), same,
		//									(size_t)ele.cd, deletarget, i, _posmap.insert(xel, &val2));
		//							}
		//							ERROR("cbox {0} address={1} ele{2}", GetObj(ele).cBox.toString(), (size_t)&poj, ele.cd.id);
		//							std::abort();
		//							}))
		//						{
		//							ERROR("cbox {0}", GetObj(ele).cBox.toString());
		//							std::abort();
		//						}
		//					}

		//				}
		//				poj.pBox.reset();
		//				postmatch(ele, poj);
		//				break;
		//			}

		//		}
		//		if (deletarget.id == -1)
		//			return false;
		//		/*auto& Phyoj = *tg;
		//		auto& cd0 = Phyoj.PO->collider;
		//		if (cd0.colliderType == ColliderType::Box) {
		//			_BoxPool.push(cd0.data);
		//			cd0.data = -1;
		//		}*/
		//		auto& GP = Objects[deletarget.tid];
		//		auto last = deletarget;
		//		last.id = GP.size() - 1;
		//		if (last.id == deletarget.id) {
		//			GP.RemoveLast();
		//			return true;
		//		}
		//		auto& qb = GP.last<Physics::PhysicsObject<>>();
		//		LoopBox([&](const auto coord) {
		//			posmap.find(coord, [&](vtype& val) {
		//				auto& va = *val.second;
		//				for (size_t i = 0; i < va.size(); i++)
		//				{
		//					auto& vae = va[i];
		//					if (vae.cd == last) {
		//						vae.cd = deletarget;
		//						return;
		//					}
		//				}
		//				std::abort();
		//				});
		//			}, qb.cBox);
		//		Objects[deletarget.tid].SwapRmove(deletarget.id);
		//		//assert(deletarget.id==GP.Get<Physics::PhyObj>(deletarget.id).)
		//		});
		//}
		template<typename T>
		//inline bool erase(const T&& cdid, const T1&& postmatch, const math::Efloat3 pos);
		inline void Reg(T& gp, int dyid = (int)CollideID::CollidDS::Dynamic) {
			auto& dg = PhysObj[dyid];
			//std::pair<math::uint, int> ti2(dg.size(), dyid);
			_Didmap[gp.Gattid()] = { dg.size(),(size_t)dyid };
			dg.emplace_back(gp.Gattid());
			if (dyid == (int)CollideID::CollidDS::Dynamic) {
				//LocalFil.resize(dg.size());
				_AroundInfo.resize(dg.size());
				_Voxids.resize(dg.size());
			}
			//ECS::getCompid<>
			if (Phyclassinfo.size() < gp.Gattid() + 1)
				Phyclassinfo.resize(gp.Gattid() + 1);
			auto&& Phyprop = Phyclassinfo[gp.Gattid()];
			Phyprop.rep = dyid;
		}
		inline auto&& GetPhysObj(const int& tid, const int& id) {
			//auto& vp = _Didmap[tid];
			auto&& group =Objects[tid];
			return group.Get<Physics::PhysicsObject<>>(id);
		}
		template<typename T>
		void OnAdd(T const& gp) {

			if (Phyclassinfo[gp.Gattid()].rep == (int)CollideID::CollidDS::Dynamic) {
				auto&& kv = (_Didmap.find(gp.Gattid())->second);
				if (_AroundInfo[kv.gid].size() < gp.size()) {
					//LocalFil[did].resize(gp.size());
					_AroundInfo[kv.gid].resize(gp.size());
					_Voxids[kv.gid].resize(gp.size());
				}
			}
		}
		//auto&& Arounds(){}
		void ShowHash() {
			for (const auto kv : _posmap)
			{
				auto key = kv.first.transpose();
				const voxel0& vex = getvox(kv.second);
				for (size_t i = 0; i < vex.count; i++)
				{
					const Vox vo = vex.ele[i];
					LOGINFO("key={0} v={1}", key, vo.cd);
				}
			}
		}
		void Clear() {

		}
		//Physics::PhyObj& GetObj(const Vox id) { return Objects[id.cd.tid].Get<Physics::PhyObj>(id.cd.id); }
		Physics::PhysicsObject<>& GetObj(const Vox id) {
			return Objects[id.cd.tid].Get<Physics::PhysicsObject<>>(id.cd.id);
		}
		inline Physics::PhysicsObject<>& GetObj(const uint tid, const uint id) {
			return Objects[tid].Get<Physics::PhysicsObject<>>(id);
		}
		void CreatStatic(Physics::PhysicsObject<>& pj, const uint tid, const math::Efloat2 boxSize = { 0 ,0 })
		{
			{
				auto& posmap = _posmap;
				Box2f bbox(pj.transform.pos.head<2>(), pj.collider.ConvexR);
				Box2i& qb = pj.cBox;
				CalculStartEnd(Physics::Standardcell.head<2>(), bbox, qb);
				const CollideID cdid(Objects[tid].size(), tid, CollideID::CollidDS::Dynamic);
				//voxel0* p_vx = NULL;
				//LoopGrid(qb, [&](const auto xel) {
				//	p_vx = NULL;
				//	bool False = false;
				//	posmap.update(xel, [&](auto& item, auto* old) {

				//		if (old)
				//		{
				//			p_vx = old->second;
				//			item.second = p_vx;
				//			//std::abort();
				//		}
				//		else {
				//			p_vx = new std::decay<decltype(*p_vx)>::type;
				//			item.second = p_vx;
				//		}
				//		p_vx->insert(Vox(cdid));
				//		});
				//	});
			}
		}
		inline void Deactive(const uint tid,const uint id){
			/*if (!_Didmap.contains(tid))
				return;*/
			auto& physicsobj = GetObj(tid,id);
			using vtyep = PosHash0::value_type;
			LoopGrid(physicsobj.cBox, [&](const auto xel) {
				auto& posmap = _posmap;
				posmap.find(xel, [&](auto& val) {
					auto& val2 = getvox(val.second);
					const std::lock_guard<std::mutex>guard(_mutex);
					if ((val2.eraseunsafe(std::make_tuple(tid, id), [=](const auto& e1,const auto&) {
						//auto&& [tid, id] = e0;
						return e1.cd.tid == tid && e1.cd.id == id;
						}))) {
						guard.~lock_guard();
						if (val2.empty()) {
							if (!posmap.erase(xel, [&](auto& val1) {
								//assert(!val.second->inside);
								//_gcvoxs.push_back(val.second);
								dealloc(val1.second);
								})) {
								ERROR(fmt::format("erased empty voxel0 {0} fail", xel));
								std::abort();
							}
							return;
						}
						return;
					}
					std::abort();
					});
				});
		}
		//void Deactive(const Vox pj) {
		//	auto& physicsobj = GetObj(pj.cd);
		//	using vtyep = PosHash0::value_type;
		//	LoopGrid(physicsobj.cBox, [&](const auto xel) {
		//		auto& posmap = _posmap;
		//		posmap.find(xel, [&](auto& val) {
		//			auto& val2 = getvox(val.second);
		//			if ((val2.erase(pj.cd))) {
		//				if (val2.empty()) {
		//					if (!posmap.erase(xel, [&](vtyep& val) {
		//						assert(!val.second->inside);
		//						//_gcvoxs.push_back(val.second);
		//						dealloc(val.second);
		//						})) {
		//						ERROR(fmt::format("erased empty voxel0 {0} fail", xel));
		//						std::abort();
		//					}
		//					return;
		//				}
		//				return;
		//			}
		//			std::abort();
		//			});
		//		});
		//}
		~Physics0() {

			//using vtyep = PosHash0::value_type;
			//for (auto& kv:_posmap)
			//{
			//	if (!_posmap.erase(kv, [&](vtyep& val) {
			//		dealloc(val.second);
			//		})) {
			//		assert("cell leaked");
			//	}
			//}
		}
		/*struct tidcov
		{
			std::vector<std::size_t> _cvtab;
            operator*
		};*/
		std::vector<T>& Objects;
		std::atomic_int itor = 0;
	private:
		std::mutex _mutex;
		void resize(Utility::sz i) {

		}

		template<class T>
		inline void dealloc(T const& v) {
			Gcq.push(v);
			//LocalFil.emplace_back(phmap::flat_hash_set<Paircid0>())
		}
		template<typename A, typename B>
		void filter1(AppendArray<Paircid0>& a, int s, int e, AppendArray<A>& result, const B getPhyobj) {
			contact ct;
			if (s == 0) {
				Paircid0& curr = a[s];
				auto& PoA = *curr.pa;
				auto& PoB = *curr.pb;
				if (PoA.Eve)
				{
					PoA.Eve(std::move(curr));
				}
				else if (PoB.Eve) {
					PoB.Eve(curr.Swap());
				}
				if (!PoA.collider.IsTrigger && !PoB.collider.IsTrigger) {
					CalculContact(&ct, PoA.collider, PoB.collider, PoA.transform, PoB.transform, Physics::dt);
					if (ct.pointCount > 0)
						result.enqueue(ct);
				}
				s++;
				if (s >= e)
					return;
			}
			var& prev = a[s - 1];
			while (s < e)
			{
				const var& curr = a[s];
				if (prev != curr)
				{
					auto& PoA = *curr.pa;
					auto& PoB = *curr.pb;
					if (PoA.Eve)
					{
						PoA.Eve(std::move(curr));
					}
					else if (PoB.Eve) {
						PoB.Eve(curr.Swap());
					}
					if (!PoA.collider.IsTrigger && !PoB.collider.IsTrigger) {
						CalculContact(&ct, PoA.collider, PoB.collider, PoA.transform, PoB.transform, Physics::dt);
						if (ct.pointCount > 0)
							result.enqueue(ct);
					}
					prev = curr;
				}
				s++;
			}
		}
		std::vector<math::ui64> PhysObj[2];
		//std::vector<T*> PhysObj[2];
		std::vector<poshsstr::Pyprop> Phyclassinfo;
		struct Hash {
			size_t operator()(const Vox& p) const {
				return p.num;
			}
		};
		struct Eq {
			bool operator()(const Vox& a,
				const Vox& b) const {
				return a == b;
			}
		};
		using dedupset = phmap::flat_hash_set<Vox, Hash, Eq>;
		phmap::flat_hash_map<math::uint, mapir> _Didmap;
		//std::vector<std::vector<dedupset>> LocalFil;
		//std::vector<std::vector<int>> AroundInfoL;
		std::vector<std::vector<Lavnes>> _AroundInfo;
		std::vector<ECS::CompAccessor<std::vector<Lavnes>, std::vector<Utility::bit128>>> _Ar3;
		//store per object space span id
		std::vector<std::vector<Utility::FixAy>> _Voxids;
		AppendArray<contact> _manfu;
		AppendArray<Paircid0> solvepair;
		tf::Executor executor;
		tf::Taskflow taskflow;
		std::vector<gridPtr0>  _corase;
		uint cpunum;
		std::stack<uint>_BoxPool;
		std::atomic_llong dAll = 0, All = 0;
		//static thread_local lfpAlloc::Pool<sizeof(voxel0), 128> pools_;
		//static thread_local dedupset lset;
		inline dedupset& Dset() {
			thread_local dedupset lset;
			return lset;
		}
	};
	/*template<class T>
	inline void Physics0<T>::Move() {}*/
	/*template<class T>
	thread_local Physics0<T>::dedupset Physics0<T>::lset(64);*/
	//using Physics0 = Physics<>;
	//thread_local lfpAlloc::Pool<sizeof(Physics0::voxel0), 128> pools_;
}
//#include"Collision2.cpp"