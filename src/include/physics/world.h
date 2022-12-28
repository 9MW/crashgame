#pragma once

#include "Common\Math1.h"
#include"Common/ecs.hpp"
#include"Common/Utility.h"
#include<execution>
#include "cds/container/lazy_list_dhp.h"
#include <cds/container/feldman_hashmap_dhp.h>
#include "physics/physics.hpp"
#include "common/span.hpp"
#include"AllType.h"
#include"Common/thObser.h"
#include <cds/init.h>
namespace pj2 {

	//typedef cds::gc::DHP gc_type;
	//static gc_type hpGC_;
	using voxele = CollideID;
	using namespace math;
	using Physics::aroundele;
	//template<class T>
	//export

	class PhysicsWorld
	{
		struct tranColl {
			Collid* collider;
			mtransfrom* transform;
			tranColl(
				Collid& coll,
				mtransfrom& trans
			) :collider(&coll), transform(&trans) {}
			tranColl() {}
			inline void setnull() {
				collider = NULL;
				transform = NULL;
			}
		};
		//constexpr auto CalculStartEnd= Physics::CalculStartEnd;
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
		typedef cds::gc::DHP gc_type;
		//static gc_type hpGC;
		template<class T1>
		using spa = Utility::spa<T1>;
		using voxel = spa<voxele>;
		using gridPtr = voxel*;
		using qCell = math::int2;
		using PosHash = cds::container::FeldmanHashMap<gc_type, qCell, gridPtr, map_traits>;
		template<class T, class T1>
		constexpr int getAddress(const T& coord0, const T1& halfMapsize) {
			const auto& coord = coord0 + halfMapsize.head<2>().cast<int>();
			return coord[0] + coord[1] * halfMapsize[1];
		}
#define TGET(tname,tup,resu)  tname& resu = std::get<tname&>(tup)
		//const T& _objcolle;
	public:
		tf::Executor executor;
		using t_pkg = n_Registry::Registry::t_Pkgs;
		t_pkg* _pkgs_ptr;
		using Lavnes = Physics::Lavnes;
		struct mapir
		{
			std::size_t gid, b;
		};
		using voxel0 = spa<voxele>;
		using gridPtr0 = voxel0*;
		using PosHash0 = cds::container::FeldmanHashMap<gc_type, qCell, gridPtr0, map_traits>;
		using GcQueue = Utility::RingQue<math::uint, 1024 * 256>;
		GcQueue Gcq;
		/*template<class C, class... _Types>
		inline auto parallelloop(tf::Taskflow& tf, C&& f, std::vector<math::ui64>& gids, const math::ui64 chunksize, _Types&&... _Args) {
			if constexpr (sizeof...(_Types) == 0) {
				return TaskEx::parallelfor(tf, std::forward<C>(f), [&](auto&& gid) {return &this->Objects[gid]; },
					gids, chunksize);
			}
			return TaskEx::parallelfor(tf, std::forward<C>(f), [&](auto&& gid) {return &this->Objects[gid]; },
				gids, chunksize, std::forward<_Types>(_Args)...);
		}*/
		/// <summary>
		/// insert->remove->deduplicate per entity->solve constrain
		/// </summary>

		void Move();
		void Init();
		inline auto GetObj(const CollideID& id, tranColl& res) {
			//const auto& c = *(t_pkg*)_pkgs_ptr;
			res.setnull();
			ECS::visit_convertible<n_Registry::Registry, n_BasObj::ecsphysobj>(id.tid0, [&](auto I) {
				//n_Registry::Registry::t_Pkgs& pkgs = *_pkgs_ptr;
				auto tup = _pkgs_ptr->get<I.value>()[id.tid].comps[id.id];
				TGET(Collid, tup, collider);
				TGET(mtransfrom, tup, transform);
				new(&res)tranColl(collider, transform);
				});
		}

		template<class T>
		void Move() {
			using t_obj = Physics::eobj;
			ECS::visit_all_convertible<T, t_obj>([&](auto I) {
				using ECl = T::template t<I>;
				});
		}
		void erase(const CollideID& id);
		inline voxel0& getvox(math::uint const i) {
			return voxel0s[i];
		}
		std::vector<std::atomic<math::uint>> PMaps;
		std::vector<voxel0> voxel0s;
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
		PhysicsWorld(size_t dynum, t_pkg& p_pkgs, math::uint4 mapsize = { 1024,1024,1,1 },
			size_t GcQueueCapacity = 1024 * 256, size_t stanum = 0) :
			_manfu(dynum * 6), _pkgs_ptr(&p_pkgs), _corase(10 * dynum),// Gcq(GcQueueCapacity),
			solvepair(10 * dynum), PMaps(mapsize.prod()), _mapsize(mapsize)
		{
			cds::Initialize();
			voxel0s.resize(GcQueueCapacity);
			//remain 0 unused for compatible with NULL pointer
			for (size_t i = 1; i < voxel0s.size(); i++)
			{
				Gcq.enqueue(i);
			}
			cds::threading::Manager::attachThread();
			cpunum = std::thread::hardware_concurrency();
			Init();
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
		/*auto& Arounds(std::size_t i) {
			return _AroundInfo[(_Didmap[i]).gid];
		}*/
		//template<typename T>//, typename T1>
		//void QuaryPos(const qCell& pr, const T&& Matcher) {
		//	auto& pword = this;
		//	if (!pword->_posmap.cfind(posi, [&](auto& val) {
		//		auto& val2 = pword->getvox(val.second);
		//		for (size_t ipk = 0; ipk < val2.size(); ipk++)
		//		{
		//			auto& elei = val2[ipk];
		//			Utility::MyTuple<mtransform*, Collid*, GameProps::Cmd0*> tpp;
		//			GetObjl<n_BasObj::Cmp_unit>(pkgs, elei, tpp);

		//		}
		//		})) {

		//		std::abort();
		//	}
		//}

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

				//_Voxids.resize(dg.size());
			}
		}
		template<typename T>
		void OnAdd(T const& gp) {

			//if (Phyclassinfo[gp.Gattid()].rep == (int)CollideID::CollidDS::Dynamic) {
			//	auto&& kv = (_Didmap.find(gp.Gattid())->second);
			//	if (_AroundInfo[kv.gid].size() < gp.size()) {
			//		//LocalFil[did].resize(gp.size());
			//		_AroundInfo[kv.gid].resize(gp.size());
			//		_Voxids[kv.gid].resize(gp.size());
			//	}
			//}
		}
		//auto&& Arounds(){}
		void ShowHash() {

			LOGINFO(typeid(PosHash0).name());
			/*for (const auto& kv : _posmap)
			{
				auto key = kv.first.transpose();
				const voxel0& vex = getvox(kv.second);
				for (size_t i = 0; i < vex.count; i++)
				{
					const Vox vo = vex.ele[i];
					LOGINFO("key={0} v={1}", key, vo.cd);
				}
			}*/
		}
		inline voxel0& LookUp(const qCell& xel) {
			const auto& vx = _posmap.get(xel)->second;
			return getvox(vx);
		}
		void Clear() {

		}

		template<class Tcv, class ...Ts>
		inline auto GetObjl(PhysicsWorld::t_pkg& _pkgs, const CollideID& id, std::tuple<Ts*...>& ot) {
			//const auto& c = *(t_pkg*)_pkgs_ptr;
			auto& pkgs = _pkgs;
			ECS::visit_at<n_Registry::Registry>(id.tid0, [&](auto I) {
				if constexpr (ECS::convertible<n_Registry::Registry::t<I>, Tcv>())
					pkgs.get<I.value>()[id.tid].comps.get(id.id, ot);
				else {
					std::abort();
				}
				},
				[&ot](auto I) {
					std::memset(&ot, 0, sizeof(ot));
				});
		}
		/*template<typename T, typename T1>
		void GetRadiusObj(const T rad, const math::Efloat3 pos, T1 resultaccess) {
			Utility::circle([&](const auto a, const auto b)
				{
					filter._posmap.find(qCell(a, b), [&](Physics0::PosHash0::value_type& val) {
						auto& val2 = *val.second;
						for (size_t i = 0; i < val2.size(); i++)
						{
							const var& ele = val2.ele[i];
							resultaccess(ele);
						}
						}
					);
				}, pos[0], pos[1], std::round(rad / Physics::cellsize));
		}*/
		//not thread safe function
		void Deactive(CollideID& cd);
		void Active(CollideID& cd);
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
		~PhysicsWorld() {

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

		std::array<std::vector<std::vector<Lavnes>>, t_pkg::se()>_AroundInfo;

		std::vector<ECS::CompAccessor<std::vector<Lavnes>, std::vector<Utility::bit128>>> _Ar3;
		AppendArray<contact> _manfu;
		AppendArray<Paircid0> solvepair;
		tf::Taskflow taskflow;
		std::vector<gridPtr0>  _corase;
		uint cpunum;
		std::stack<uint>_BoxPool;
		std::atomic_llong dAll = 0, All = 0;
		//PosHash0 _posmap;
		//static thread_local lfpAlloc::Pool<sizeof(voxel0), 128> pools_;
		//static thread_local dedupset lset;
		inline dedupset& Dset() {
			thread_local dedupset lset;
			return lset;
		}
	};
}