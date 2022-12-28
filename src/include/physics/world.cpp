#include"world.h"
//#pragma optimize( "", off )
//#pragma comment(linker, "/STACK:4000000")
namespace pj2 {
	using spocup = Utility::FixAy;
	constexpr int Iv = n_Registry::Registry::size();
	PhysicsWorld* pkp;
	namespace TaskEx
	{
		using namespace tf;
		template <typename C>
		tf::Task parallelforl(Taskflow& tf, n_Registry::Registry::t_Pkgs& pkgs, C&& c0,
			size_t chunk_size0 = 100)
		{
			//constexpr auto c1 = []() {};
			//int totaltask = 0;
			//tf::Task tsks[n_Registry::Registry::size()];

			Task task = tf.emplace(
				[
					&pkgs,
					c1 = std::forward<C>(c0)
				](Subflow& sf) mutable {
					std::array< std::atomic<size_t>, Iv> Counters;
					//std::atomic<T*> currcmps = &ff[0];
					//size_t N = distance(beg, end, chunk_size);
					std::atomic<size_t>  next2(0), end(0);// = curritem.load(std::memory_order_relaxed)->size();;
					std::atomic_bool steping = false;
					size_t W = std::thread::hardware_concurrency();
					ECS::visit_all<n_Registry::Registry>([&](auto IL) {
						constexpr auto I = IL.value;
						using tI = typename n_Registry::Registry::template t<I>;
						constexpr bool cvtst = ((ECS::convertible<tI, n_BasObj::ecsphysobj >()));
						//if constexpr (0)
						if constexpr (cvtst)
						{
							auto& l2 = pkgs.get<I>();

							W = W > 0 ? W : 8;
							//W=std:min(l2)
							for (size_t w = 0; w < W; w++) {

								sf.silent_async([&steping, &end, &next2, &l2, W, &c = c1, &IL, &Counters]
								() mutable {
										auto&& next = std::get<I>(Counters);
										int chunk_size = 100;
										constexpr auto I = IL.value;
										//constexpr auto getele = [&l2, &rt](auto&& i) {return rt(l2[i]); };
										//T* curritem = NULL;
										// end = curritem.load(std::memory_order_relaxed)->size();
										 //= next.load(std::memory_order_relaxed);
										if (!cds::threading::Manager::isThreadAttached())
											cds::threading::Manager::attachThread();
										size_t endf = 0, next3l = 0;
										while (1) {
											size_t s0 = next.fetch_add(chunk_size, std::memory_order_relaxed);
											//endl = std::min(endl, s0+chunk_size-endf);
											while (s0 >= endf)
											{
												if (next3l >= l2.size()) {

													return;
												}
												endf += l2[next3l++].size();
											}
											size_t next2l = next3l - 1, endl = (l2[next2l]).size();
											//auto&& curritem = rt(l2[next2l]);
											s0 = endl - (endf - s0);
											size_t e0 = std::min(endl, (s0 + chunk_size));
											{
												auto&& curr = (l2[next2l]);
												for (size_t x = s0; x < e0; x++) {

													const CollideID cdid(x, I, next2l);
													c(cdid, curr, std::integral_constant<std::size_t, I>(), pkp);
												}
											}
											size_t rem = (s0 + chunk_size - e0);
											while (rem != 0 && ++next2l < l2.size()) {
												auto&& curr = (l2[next2l]);
												endl = curr.size();
												e0 = std::min(endl, rem);
												for (size_t x = 0; x < e0; x++) {
													const CollideID cdid(x, I, next2l);
													c(cdid, curr, std::integral_constant<std::size_t, I>(), pkp);
												}
												rem = (rem - e0);
											}
										}
									});
							}
						}
						});
					sf.join();
				});
			return task;
		}

		inline size_t min(const int& a, const int& b) { return std::min(a, b); }
		template <typename C>
		tf::Task parallelforev(Taskflow& tf, n_Registry::Registry::t_Pkgs& pkgs, C&& c0,
			size_t chunk_size0 = 100)
		{
			return
				tf.emplace(
					[&pkgs, c = std::forward<C>(c0)](Subflow& sf) {

						std::array< std::atomic<size_t>, Iv> Counters;
						size_t W = std::max(1u, std::thread::hardware_concurrency());
						ECS::visit_all<n_Registry::Registry>([&, c](auto IL) {
							constexpr auto I = IL.value;
							using tI = typename n_Registry::Registry::t<I>;
							if constexpr (ECS::convertible<tI, n_BasObj::ecsphysobj>() ||
								ECS::convertible<tI, n_BasObj::ecsvirtualobserve>())
							{
								auto&& next = std::get<I>(Counters);
								std::atomic<size_t>  next2(0), end(0);// = curritem.load(std::memory_order_relaxed)->size();;
								auto& l2 = pkgs.get<I>();
								size_t totalsize = 0;
								for (size_t i = 0; i < l2.size(); i++)
								{
									totalsize += l2[i].size();
								}

								for (size_t w = 0; w < W; w++) {

									sf.silent_async([&next, &end, &next2, &l2, W, &c, &IL]
									() mutable {
											int chunk_size = 100;
											constexpr auto I = IL.value;
											//constexpr auto getele = [&l2, &rt](auto&& i) {return rt(l2[i]); };
											//T* curritem = NULL;
											// end = curritem.load(std::memory_order_relaxed)->size();
											 //= next.load(std::memory_order_relaxed);
											if (!cds::threading::Manager::isThreadAttached())
												cds::threading::Manager::attachThread();
											size_t endf = 0, next3l = 0;
											while (1) {
												size_t s0 = next.fetch_add(chunk_size, std::memory_order_relaxed);
												//endl = std::min(endl, s0+chunk_size-endf);
												while (s0 >= endf)
												{
													if (next3l >= l2.size()) {

														return;
													}
													endf += l2[next3l++].size();
												}
												size_t next2l = next3l - 1, endl = (l2[next2l]).size();
												//auto&& curritem = rt(l2[next2l]);
												s0 = endl - (endf - s0);
												size_t e0 = std::min(endl, (s0 + chunk_size));
												{
													auto&& curr = (l2[next2l]);
													for (size_t x = s0; x < e0; x++) {

														const CollideID cdid(x, I, next2l);
														c(cdid, curr, std::integral_constant<std::size_t, I>());
													}
												}
												size_t rem = (s0 + chunk_size - e0);
												while (rem != 0 && ++next2l < l2.size()) {
													auto&& curr = (l2[next2l]);
													endl = curr.size();
													e0 = std::min(endl, rem);
													for (size_t x = 0; x < e0; x++) {
														const CollideID cdid(x, I, next2l);
														c(cdid, curr, std::integral_constant<std::size_t, I>());
													}
													rem = (rem - e0);
												}
											}
										});
								}
							}
							});
						sf.join();
					});
		}
		template <typename Retriver, typename C>
		tf::Task parallelforlvir(Taskflow& tf, n_Registry::Registry::t_Pkgs& pkgs, C&& c0, Retriver&& RT,
			size_t chunk_size0 = 100)
		{
			//constexpr auto c1 = []() {};
			//int totaltask = 0;
			//tf::Task tsks[n_Registry::Registry::size()];

			Task task = tf.emplace(
				[&pkgs, chunk_size = (chunk_size0),
				c = std::forward<C>(c0), rt = std::forward<Retriver>(RT)
				](Subflow& sf) mutable {

					std::array< std::atomic<size_t>, Iv> Counters;
					//std::atomic<T*> currcmps = &ff[0];
					//size_t N = distance(beg, end, chunk_size);
					std::atomic<size_t> next2(0), end(0);// = curritem.load(std::memory_order_relaxed)->size();;
					std::atomic_bool steping = false;
					size_t W = std::thread::hardware_concurrency();

					ECS::visit_all<n_Registry::Registry>([&](auto IL) {
						constexpr static auto I = IL.value;
						using tI = typename n_Registry::Registry::t<I>;

						if constexpr (ECS::convertible<tI, n_BasObj::ecsvirtualobserve>())
						{
							auto& l2 = pkgs.get<I>();

							W = W > 0 ? W : 8;
							//W=std:min(l2)
							for (size_t w = 0; w < W; w++) {

								sf.emplace([&steping, &end, &next2, &l2, &chunk_size, W, &c, &Counters]
								() mutable {
										auto&& next = std::get<I>(Counters);
										//constexpr auto getele = [&l2, &rt](auto&& i) {return rt(l2[i]); };
										//T* curritem = NULL;
										// end = curritem.load(std::memory_order_relaxed)->size();
										 //= next.load(std::memory_order_relaxed);
										if (!cds::threading::Manager::isThreadAttached())
											cds::threading::Manager::attachThread();
										size_t endf = 0, next3l = 0;
										while (1) {
											size_t s0 = next.fetch_add(chunk_size, std::memory_order_relaxed);
											//endl = std::min(endl, s0+chunk_size-endf);
											while (s0 >= endf)
											{
												if (next3l >= l2.size()) {

													return;
												}
												endf += l2[next3l++].size();
											}
											size_t next2l = next3l - 1, endl = (l2[next2l]).size();
											//auto&& curritem = rt(l2[next2l]);
											s0 = endl - (endf - s0);
											size_t e0 = std::min(endl, (s0 + chunk_size));
											{
												auto&& curr = (l2[next2l]);
												for (size_t x = s0; x < e0; x++) {

													const CollideID cdid(x, I, next2l);
													c(cdid, curr, std::integral_constant<std::size_t, I>());
												}
											}
											size_t rem = (s0 + chunk_size - e0);
											while (rem != 0 && ++next2l < l2.size()) {
												auto&& curr = (l2[next2l]);
												endl = curr.size();
												e0 = std::min(endl, rem);
												for (size_t x = 0; x < e0; x++) {
													const CollideID cdid(x, I, next2l);
													c(cdid, curr, std::integral_constant<std::size_t, I>());
												}
												rem = (rem - e0);
											}
										}
									}).name("pfg_"s + std::to_string(w));
							}
						}
						});
					sf.join();
				});
			return task;
		}

#ifdef test
		template<std::size_t fi, class... _Types>
		constexpr auto getCa(std::tuple<_Types& ...>&& datas) {
			return std::apply([&fi](auto&& ... args) {
				return ECS::CompAccessor(std::forward_as_tuple(std::get<fi>(args)...));
				}, std::move(datas));
		}
		template <typename Retriver, typename C, class... _Types>
		inline Task parallelforl(Taskflow& tf, n_Registry::Registry::t_Pkgs& pkgs, C&& c0, Retriver&& RT,
			size_t chunk_size0 = 100, _Types&&... _Args)
		{
			//int totaltask = 0;
			//tf::Task tsks[n_Registry::Registry::size()];
			constexpr auto getCa = [](auto foi, auto&& datas) {
				//constexpr auto fi = foi.value;
				return std::apply([&foi](auto&& ... args) {

					constexpr auto fi = foi.value;
					return ECS::CompAccessor(std::forward_as_tuple(std::get<fi>(args)...));
					}, std::move(datas));
			};
			Task task = tf.emplace(
				[&pkgs, chunk_size = std::forward<size_t>(chunk_size0), tups,
				c = std::forward<C>(c0), rt = std::forward<Retriver>(RT)
				](Subflow& sf) mutable {

					//std::atomic<T*> currcmps = &ff[0];
					//size_t N = distance(beg, end, chunk_size);
					std::atomic<size_t> next(0), next2(0), end(0);// = curritem.load(std::memory_order_relaxed)->size();;
					std::atomic_bool steping = false;
					size_t W = std::thread::hardware_concurrency();

					ECS::visit_convertible<n_Registry::Registry, n_BasObj::ecsphysobj>([&](auto I) {
						//using te = typename n_Registry::Registry::template t<I>;
						//if constexpr (ECS::is_convertible_v<te, n_BasObj::ecsphysobj>)
						{
							auto& l2 = pkgs.get<I>();
							auto cmps0 = getCa(I, tups);
							W = W > 0 ? W : 8;
							//W=std:min(l2)
							for (size_t w = 0; w < W; w++) {

								sf.emplace([&next, &steping, &end, &next2, &l2, chunk_size, W, &c, &rt,
									&I, &cmps0]() mutable {
										//constexpr auto getele = [&l2, &rt](auto&& i) {return rt(l2[i]); };
										//T* curritem = NULL;
										// end = curritem.load(std::memory_order_relaxed)->size();
										 //= next.load(std::memory_order_relaxed);
										if (!cds::threading::Manager::isThreadAttached())
											cds::threading::Manager::attachThread();
										size_t endf = 0, next3l = 0;
										while (1) {
											size_t s0 = next.fetch_add(chunk_size, std::memory_order_relaxed);
											//endl = std::min(endl, s0+chunk_size-endf);
											while (s0 >= endf)
											{
												if (next3l >= l2.size())
													return;
												endf += rt(l2[next3l++]).size();
											}
											size_t next2l = next3l - 1, endl = rt(l2[next2l]).size();
											//auto&& curritem = rt(l2[next2l]);
											s0 = endl - (endf - s0);
											size_t e0 = std::min(endl, (s0 + chunk_size));
										iter:
											{
												auto&& curr = rt(l2[next2l]);
												for (size_t x = s0; x < e0; x++) {

													const CollideID cdid(x, I, next2l);
													c(cdid, curr, std::integral_constant<std::size_t, I>(), cmps0);
												}
											}
											size_t rem = (s0 + chunk_size - e0);
											while (rem != 0 && ++next2l < l2.size()) {
												auto&& curr = rt(l2[next2l]);
												endl = curr.size();
												e0 = std::min(endl, rem);
												for (size_t x = 0; x < e0; x++) {
													const CollideID cdid(x, I, next2l);
													c(cdid, curr, std::integral_constant<std::size_t, I>(), cmps0);
												}
												rem = (rem - e0);
											}
										}
									}).name("pfg_"s + std::to_string(w));
							}
						}
						sf.join();
						});
				});
			return task;
		}
#endif
	}
	enum ebox {
		pBox = 0, cBox
	};
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

	inline dedupset& Dset() {
		thread_local dedupset lset;
		return lset;
	}
	template<typename T>
	constexpr void LoopGrid(Box2i& bx, T const& f = [](auto) {}) {

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
	inline void CalculStartEndL(const Vector3i& CellSize, const Box2f& bounds, Box2i& outres) {

		const auto& invcell = 1 / CellSize.cast<float>().array();
		outres.Min = bounds.Min.cwiseProduct(invcell.head<2>().matrix()).array().floor().cast<int>();
		outres.Max = bounds.Max.cwiseProduct(invcell.head<2>().matrix()).array().ceil().cast<int>();
	};
	thread_local int ct1;
	using t_lod = Physics::LODShape;
	thread_local dedupset __lset;
	//::: can't detect object that not inherit from n_BasObj::Cmp_unit
	void PhysicsWorld::Init() {
		constexpr int dyid = (int)CollideID::CollidDS::Dynamic;
		constexpr int chunk_size = 100;
		auto&& posmap = _posmap;
		pkp = this;
		ECS::visit_all_convertible<n_Registry::Registry, n_BasObj::ecsphysobj>([&](auto IL) {
			constexpr auto I = IL.value;
			using tI = typename n_Registry::Registry::t<I>;
			auto& pkgs = *_pkgs_ptr;
			std::atomic<size_t> next(0);
			auto& l2 = pkgs.get<I>();
			for (size_t i = 0; i < l2.size(); i++)
			{
				executor.silent_async([&l2, i = i]() {
					auto&& curr = l2[i];
					for (size_t id = 0; id < curr.size(); id++)
					{
						auto&& [collider, transform, b2x, blocks, ads] =
							curr.comps.get<Collid, mtransform, t_lod, spocup, Lavnes>(id);
						Box2i& qb = b2x[ebox::cBox];
						Box2i& obx = b2x[ebox::pBox];
						const  Box2f bbox(transform.pos.head(2), collider.ConvexR);
						CalculStartEndL(Physics::Standardcell, bbox, qb);
						new(&obx)Box2i();
					}
					});
			}
			});
		executor.wait_for_all();
		//static std::shared_ptr<MyObserver> observer = executor.make_observer<MyObserver>("PhysicsWorld Observer");
		int ctcy = 7878;
		//NOTE Vox and function head(2)  will cause compile error
		auto se1 = TaskEx::parallelforl(taskflow, *(t_pkg*)_pkgs_ptr,
			[yf = 75, r = this, kl = this](const auto& cdid, auto& ele, auto I, auto& pthis)
			{
				const auto& id = cdid.id;
				auto&& [collider, transform, b2x, blocks, ads] =
					ele.comps.get<Collid, mtransform, t_lod, spocup, Lavnes>(id);
				Box2i& qb = b2x[ebox::cBox];
				Box2i& obx = b2x[ebox::pBox];
				const  Box2f bbox(transform.pos.head<2>(), collider.ConvexR);
				CalculStartEndL(Physics::Standardcell, bbox, qb);
				if (collider.ConvexR == 0 || (obx == qb))
					return;
				auto& lf = qb.Min;
				auto& tr = qb.Max;
				qCell  xel;

				for (auto i = lf[0]; i <= tr[0]; ++i)
				{
					xel[0] = i;
					for (auto p = lf[1]; p <= tr[1]; ++p)
					{
						xel[1] = p;
						//#ifdef DEBUG
						//						const map_traits::hash_accessor he;
						//						size_t hash = (size_t)(he(xel));
						//						LOGINFO("insert  id={0} thread:{1} xel={2} xel hash={3}, {4} {5}",
						//							cdid, std::thread::id::id(), xel, hash, lf, tr);
						//#endif // DEBUG
						//var pd = _posmap.get(float2(i, p));

						if (obx.Contain(xel)) {
							continue;
						}
						GcQueue::value_type p_vx = NULL;

						{

							//voxel0* p_vx = NULL;
							bool False = false;
							pthis->_posmap.update(xel, [&](auto& item, auto* old) {

								if (old)
								{
									int count = 0;
									while (old->second == NULL) {
										std::this_thread::yield();
										if (count++ > 1000000) {
											ERROR("second={0}", (void*)(old->second));
											std::abort();
										}
									}
									p_vx = old->second;
									item.second = p_vx;
									//std::abort();
								}
								else {
									//p_vx = alloc();

									pthis->Gcq.pop(p_vx);
									item.second = p_vx;
									//All.fetch_add(1, std::memory_order_relaxed);
								}
								auto& val_ = pthis->getvox(p_vx);
								//const Vox vx(cdid);
								//int ko = vx % 30;
								auto irs = val_.insert((cdid));
								if (!irs)
									std::abort();
								//if (obx.Contain(xel)) {
								//	//assert((!vx->insert(cdid)));
								//	if (p_vx.inside)
								//		return;

								//	//std::atomic_bool  inside;
								//	if (!p_vx.empty() && p_vx.inside.compare_exchange_strong(False, true)) {
								//		//_corase[ct1++] = p_vx;  cause compiler error
								//		False = true;
								//	}
								//	return;
								//}
								//if (p_vx.inside.compare_exchange_strong(False, true)) {
								//	//_corase[ct1++] = NULL;
								//	False = true;
								//}
								});
							/*if (False) {
							_corase[ct1++] = p_vx;
							}*/
						}
						if (!blocks.add(p_vx)) [[unlikely]] {
							std::abort();
						}

					}
				}
			});
		auto se2 = TaskEx::parallelforl(taskflow, *(t_pkg*)_pkgs_ptr,
			[&](const auto& cdid, auto& ele, auto I, auto& pthis)
			{
				const auto& id = cdid.id;
				auto&& [collider, transform, b2x, blocks, ads] =
					ele.comps.get<Collid, mtransform, t_lod, spocup, Lavnes>(id);
				Box2i& qb = b2x[ebox::cBox];
				Box2i& obx = b2x[ebox::pBox];
				if (!collider.Active || (obx == qb))
					return;
				const  Box2f bbox(transform.pos.head(2), collider.ConvexR);
				CalculStartEndL(Physics::Standardcell, bbox, qb);
				auto& lf = qb.Min;
				auto& tr = qb.Max;
				auto&& posmap = pthis->_posmap;
				//voxel0* efvx[8];
				/*std::array<voxel0*, 8> efvx;

				uint xxef = 0;
				while (efps > 0)
				{
					xxef = xxef % efps;
					auto& val2 = efvx[xxef];
					if (val2.erase<1>(cdid)) {
						efvx[xxef] = efvx[--efps];
						if (val2.empty()) {
							if (!posmap.erase(xel, [&](auto const& val) {
								pthis->dealloc(val.second);
								pthis->dAll.fetch_add(1, std::memory_order_relaxed);
								})) {
								std::abort();
							}
							return;
						}
					}
					++xxef;
				}*/
				int efps = 0;
				LoopGrid(obx, [&](const auto& xel) {
					{
						if (qb.Contain(xel))
							return;
						bool db = false;
						pthis->_posmap.find(xel, [&](auto& val) {
							auto& val2 = pthis->getvox(val.second);
							bool es = val2.erase<1>(cdid);
							if ((blocks.erase(val.second))) {
								if (!es)es = val2.erase<0>(cdid);
								assert(es);
								if (val2.empty()) {
									if (!posmap.erase(xel, [&](auto const& val) {
										//assert(!getvox(val.second).inside);
										//_gcvoxs.push_back(val.second);
										pthis->dealloc(val.second);
										pthis->dAll.fetch_add(1, std::memory_order_relaxed);
										})) {

										for (size_t dali = Gcq.front,back= Gcq.back; dali < back; dali++)
										{
											if (val.second == Gcq.ele[dali% Gcq.ele.size()]) {
												ERROR(fmt::format("same erased empty voxel0 {0} fail", xel));
												return;
											}
										}
										ERROR(fmt::format("erased empty voxel0 {0} fail", xel));
										std::abort();
									}
									return;
								}
								/*LOGINFO("erase sucessed id={0} thread:{1} xel={2}",
								cdid, std::thread::id::id(), xel);*/
								return;
							}
							/*ERROR("erase fail id={0} thread:{1} xel={2} i={3} p={4} obx={5} new box={6}",
							cdid, std::thread::id::id(), xel, i, p, obx.toString(), bbox.tof4());*/
							ERROR("val2.erase(cdid)={0}", val2.erase<1>(cdid));
							const map_traits::hash_accessor hasher;
							/*LoopGrid(obx, cell, [&](auto& const bxele) {
							ERROR("inside hash {0} bxele={1} xel hash={2}", hasher(bxele), bxele, hasher(xel));
							});*/
							for (size_t i = 0; i < blocks.size(); i++)
							{
								ERROR("block i={0} val={1}", i, blocks[i]);
							}
							for (size_t i = 0; i < val2.size(); i++)
							{
								auto& ele = val2.ele[i];
								var same = false;
								if (ele == cdid)
									same = true;
								ERROR("cdid={0} ele={1} thread:{2} same {3}\
	 st {4} {5} i={6} reinsert={7}",
									cdid, "|ele|", std::thread::id(), same,
									(size_t)ele, (size_t)cdid, i, posmap.insert(xel, &val2));
							}
							std::abort();
							});
					}});
				obx = qb;
			});

		auto se3vir = TaskEx::parallelforlvir(taskflow, *(t_pkg*)_pkgs_ptr,
			[&](const auto& cdid, auto& ele, auto I)
			{
				//using Collid = Physics::VirtCollid;
				const auto& id = cdid.id;
				auto&& [collider, transform, ads] =
					ele.comps.get<Physics::VirtCollid, mtransform, Lavnes>(id);
				/// = b2x[ebox::cBox];
				if (!collider.Active) [[unlikely]]
					return;
				Box2i qb;
				const  Box2f bbox(transform.pos.head<2>(), collider.ConvexR);
				CalculStartEndL(Physics::Standardcell, bbox, qb);
				auto& lf = qb.Min;
				auto& tr = qb.Max;
				//auto&& ard =std::get<I>(_AroundInfo)[cdi.tid][id];
				var trancoll = &collider;
				auto& lset = __lset;
				lset.clear();
				decltype(Box2i::Max)  xel;
				for (auto i = lf[0]; i <= tr[0]; i++)
				{

					xel[0] = i;
					for (auto p = lf[1]; p <= tr[1]; ++p)
					{
						xel[1] = p;
						posmap.cfind(xel, [&](auto& val) {
							auto&& second = val.second;
							assert(second != 0);
							//ct1++;
							auto& val2 = getvox(second);
							//if (val2.size() > 1)
							{
								for (size_t i = 0; i < val2.size(); i++)
								{
									auto&& voxs = (val2)[i];
									if (voxs == cdid)
										continue;
									lset.emplace((val2)[i]);
								}
							}
							});
					}
				}
				using namespace Physics;
				LAVNe ad;
				ads.clear();
				constexpr float uh = -1u >> 1;
				ad.distSq(uh);
				ads.emplace(ad);
				ads[0].distSq(uh);
				for (auto&& voxs : lset)
				{
					tranColl op;
					Utility::MyTuple<mtransform*, Collid*, GameProps::Cmd0*> tpp;
					GetObjl<n_BasObj::Cmp_unit>(*_pkgs_ptr, voxs.cd, tpp);
					auto&& [opts, opco, pst] = tpp.getbind();
					ad.relativePosition = (opts->pos4 - transform.pos4);

					{
						ad.combinedRadius(collider.ConvexR + opco->ConvexR);
						ad.distSq(ad.relativePosition.head<3>().squaredNorm());
						const float skindepth = 0.1;
						float detsq = math::square(ad.combinedRadius()+ skindepth);//detect range inside 0.5
						int pent = (detsq - ad.distSq()) * 1000;
						if (pent > 0 && !ads.full()) {
							ad.cd = voxs.cd;
							ad.relativeVelocity = (opco->vel4 - collider.vel4);
							ad.desiv = opco->V_desire;
							ad.st = pst->cState();
							ad.cmdid = pst->cmdid;
							ad.gid = pst->gid;
							ad.distSq(ad.relativePosition.head<3>().squaredNorm());
							ad.combinedRadius(collider.ConvexR + opco->ConvexR);
							ads.emplace(ad);
						}
						else if (ads.full()) {
							ERROR("ads full id={0}", id);
							for (size_t i = 0; i < ads.size(); i++)
							{
								LAVNe& iads = ads[i];
								ERROR("ads i={0} id={1} dissq={2}", i, iads.cd.id, iads.distSq());
							}
							std::abort();
						}
						/*if (ads.size() > 2)
						{
							if(ad.distSq() < ads[1].distSq())
							{
								if (ad.distSq() < ads[0].distSq())
								{
									ads[1] = ads[0];
									ads[0] = ad;
								}
								else
								ads[1] = ad;
									continue;
							}
						}
						else
						{
							ads.emplace(ad);
							if (ads.size() > 2 && ads[1].distSq() < ads[0].distSq()) {
								std::swap(ads[0], ads[1]);
							}
						}*/
					}
					//const auto& dissq = 900;// T::CheckAround(voxs, cdid, op, PO);

				}
				//ads.Sort();
			}, [](const auto& trt) {return trt; });

		auto se3v2 = TaskEx::parallelforl(taskflow, *(t_pkg*)_pkgs_ptr,
			[](const auto& cdid, auto& ele, auto I, auto& pthis)
			{
				const auto& id = cdid.id;
				auto&& [collider, transform, b2x, blocks, ads] =
					ele.comps.get<Collid, mtransform, t_lod, spocup, Lavnes>(id);
				Box2i& qb = b2x[ebox::cBox];
				Box2i& obx = b2x[ebox::pBox];
				if (!collider.Active) [[unlikely]]
					return;
				//auto&& ard =std::get<I>(_AroundInfo)[cdi.tid][id];
				//auto&& lset = pthis->Dset();
				var trancoll = &collider;
				thread_local dedupset lset;
				lset.clear();
				Utility::MyTuple<mtransform*, Collid*, GameProps::Cmd0*> tpp[32];
				//std::make_tuple()
				//CalculStartEndL(cell, bbox, qb);
				//	chsum += id;
				//const var olderbox = obx;
				for (size_t i = 0; i < blocks.size(); i++)
				{
					auto&& second = blocks[i];
					if (second == 0)
						break;
					//ct1++;
					auto& val2 = pthis->getvox(second);
					//if (val2.size() > 1)
					{
						for (size_t i = 0; i < val2.size(); i++)
						{
							auto&& voxs = (val2)[i];
							if (voxs == cdid)
								continue;
							lset.emplace((val2)[i]);
						}
					}
				}
				using namespace Physics;
				LAVNe ad;
				ads.clear();
				for (auto&& voxs : lset)
				{
					tranColl op;
					Utility::MyTuple<mtransform*, Collid*, GameProps::Cmd0*> tpp;
					pthis->GetObjl<n_BasObj::Cmp_unit>(*(pthis->_pkgs_ptr), voxs.cd, tpp);
					//ECS::convert(tpp, ad.Prop_ptrs);
					auto& opts = tpp.get<mtransform*>();
					auto&& pst = tpp.get< GameProps::Cmd0*>();
					auto& opco = tpp.get<Collid*>();
					ad.relativePosition = (opts->pos4 - transform.pos4);

					ad.combinedRadius(collider.ConvexR + opco->ConvexR);
					ad.distSq(ad.relativePosition.head<3>().squaredNorm());
					float detsq = math::square(ad.combinedRadius() + 0.5);//detect range inside 0.5
					int pent = (detsq - ad.distSq()) * 1000;
					if (pent > 0 && !ads.full()) {
						//ad.p_cust=
						ad.cd = voxs.cd;
						ad.relativeVelocity = (opco->vel4 - collider.vel4);
						ad.desiv = opco->V_desire;
						ad.st = pst->cState();
						ad.cmdid = pst->cmdid;
						ad.gid = pst->gid;
						ad.combinedRadius(collider.ConvexR + opco->ConvexR);
						ad.distSq(ad.relativePosition.head<3>().squaredNorm());
						ads.emplace(ad);
					}
					else if (ads.full()) {
						ERROR("ads full id={0}", id);
						for (size_t i = 0; i < ads.size(); i++)
						{
							LAVNe& iads = ads[i];
							ERROR("ads i={0} id={1} dissq={2}", i, iads.cd.id, iads.distSq());
						}
						std::abort();
					}
					//const auto& dissq = 900;// T::CheckAround(voxs, cdid, op, PO);

				}
				ads.Sort();

			});

		using n_BasObj::ecsvirtualobserve;

		auto integrateMotion = TaskEx::parallelforev(taskflow, *(t_pkg*)_pkgs_ptr,
			[&](const auto& cdid, auto& ele, auto I)
			{
				using tI = typename n_Registry::Registry::t<I>;
				//if constexpr (0)
				if constexpr (ECS::convertible<tI, n_BasObj::ecsphysobj>()) {
					const auto& id = cdid.id;
					auto&& [collider, transform] = ele.comps.get<Collid, mtransform>(id);
					IntegrateMotion(transform, collider, Physics::dt);
				}
				if constexpr (ECS::convertible<tI, n_BasObj::ecsvirtualobserve>()) {

					const auto& id = cdid.id;
					auto&& [collider, transform] = ele.comps.get< Physics::VirtCollid, mtransform>(id);
					IntegrateMotion(transform, collider, Physics::dt);
				}


			});

		se3v2.name("paraller for");
		se1.name("se1");
		se2.name("se2");
		se1.precede(se2);
		se2.precede(se3v2, se3vir);
		integrateMotion.succeed(se3v2, se3vir);
		/*var integrateMotion = parallelloop(taskflow, [&](size_t id, auto& group) {
			const int items = group.size();
			auto& PO = group.Get<Physics::PhysicsObject<>>(id);
			if (!collider.Active)
				return;
			IntegrateMotion(transform, collider, Physics::dt);
			}, DynamicObj, chunk_size);*/

	}
	//thread safe erase
	void PhysicsWorld::erase(CollideID const& cdid)
	{
		auto&& pkgs = *_pkgs_ptr;
		const int sta = (int)cdid.State;
		auto& posmap = _posmap;
		const int id = cdid.id; ECS::visit_convertible<n_Registry::Registry, n_BasObj::ecsphysobj>(cdid.tid0,
			[&](auto IL) {
				constexpr static auto I = IL.value;
				auto& ele = pkgs.get<I>(cdid.tid);
				auto&& [b2x, collider, blocks] = ele.comps.get<t_lod, Collid, spocup>(id);
				collider.Active = 0;
				for (size_t i = 0; i < blocks.size(); i++)
				{
					auto& biv = blocks[i];
					auto& val2 = getvox(biv);
					if (!val2.erase<true>(cdid)) {
						std::abort();
					}
				}
				blocks.clear();
				Box2i& qb = b2x[ebox::cBox];
				LoopGrid(qb, [&](const auto xel) {
					bool es = posmap.erase(xel);
					assert(es);
					});

				qb.Max.setConstant(-Infinityf);
				qb.Min.setConstant(Infinityf);
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

	void PhysicsWorld::Deactive(CollideID& cd) {
		auto& pkgs = *_pkgs_ptr;
		ECS::visit_at<n_Registry::Registry>(cd.tid0, [&](auto I) {
			if constexpr (ECS::convertible<n_Registry::Registry::t<I>, n_BasObj::ecsvirtualobserve>()) {
				return;
			}
			if constexpr (ECS::convertible<n_Registry::Registry::t<I>, n_BasObj::ecsphysobj>()) {
				auto& id = cd.id;
				auto&& [collider, transform, b2x, blks, ads] =
					pkgs.get<I.value>()[cd.tid].comps.get<Collid, mtransform, t_lod, spocup, Lavnes>(id);
				Box2i& qb = b2x[ebox::cBox];
				Box2i& obx = b2x[ebox::pBox];
				new(&obx)Box2i();
				qb = obx;
				assert(blks.size() > 0);
				for (size_t i = 0; i < blks.size(); i++)
				{
					const auto& bi = blks[i];
					if (bi == 0) {
						std::abort();
					}
					auto& val2 = getvox(bi);
					//can't find collid in space -bug
					if (!val2.erase<0>(cd)) {
						for (size_t i3 = 0; i3 < blks.size(); i3++)
						{
							ERROR("erase {0} : {1}", cd.id, getvox(blks[i3]).erase<0>(cd));
						}
						std::abort();
					}
				}
				collider.ConvexR = 0;
				collider.Active = 0;
			}
			else {
				//LOGINFO("delete")
			}});
	}
	inline void PhysicsWorld::Active(CollideID& cd) {
		auto& pkgs = *_pkgs_ptr;
		ECS::visit_at<n_Registry::Registry>(cd.tid0, [&](auto I) {
			if constexpr (ECS::convertible<n_Registry::Registry::t<I>, n_BasObj::ecsphysobj>()) {
				auto& id = cd.id;
				auto&& [collider, transform, b2x, blks, ads] =
					pkgs.get<I.value>()[cd.tid].comps.get<Collid, mtransform, t_lod, spocup, Lavnes>(id);
				Box2i& qb = b2x[ebox::cBox];
				Box2i& obx = b2x[ebox::pBox];
				new(&obx)Box2i();
			std:memset(&qb, 0, sizeof(qb));
				collider.Active = 1;
			}
			else {
				std::abort();
			}});
	}
	void PhysicsWorld::Move() {
		//collpair.clear()
		executor.run(taskflow).wait();
	}
}