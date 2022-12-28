#pragma once
//#include"tbb/concurrent_unordered_map.h"
#include"collisioin.h"
#undef DEBUG
namespace pj2 {
	struct physicsUV
	{
		using float3 = math::Efloat3;
		std::vector<Collid> _cirPool;
		std::vector<int>_BoxPool;
		const int dsid = (int)CollideID::CollidDS::Dynamic;
		const int ssid = (int)CollideID::CollidDS::Static;
		float _currTime;
		std::vector<Collid> _colliders[2];
		std::vector<mtransfrom> _pos[2];
		std::vector<spa<voxele>> _around;
		std::vector<Box2i> _prevBox;
		std::vector<std::pair<voxele, voxele>> _collpair;
		PosHash _posmap;
		//constexpr mtransfrom& getTS( CollideID& cd) { return _pos[(int)(cd.State)][cd.id]; }
		//constexpr Collid& getCD( CollideID cd) { return _colliders[(int)(cd.State)][cd.id]; }

		inline mtransfrom& getTS(const CollideID cd) { return _pos[(int)(cd.State)][cd.id]; }
		inline Collid& getCD(const CollideID cd) { return _colliders[(int)(cd.State)][cd.id]; }
		inline mtransfrom& getTS(const CollideID* cd) { return _pos[(int)(cd->State)][cd->id]; }
		constexpr auto get(CollideID const& cd) {
			const auto state = (int)(cd.State);
			return std::tie(_pos[state][cd.id], _colliders[state][cd.id]);
		}
		constexpr auto operator[](const CollideID cd)
		{
			return std::make_pair(&getTS(cd), &getCD(cd));
		}
		int DMNUM()
		{
			return _colliders[dsid].size();
		}
		std::vector<Collid>& dcs()
		{
			return _colliders[dsid];
		}
		std::vector<mtransfrom>& dts()
		{
			return _pos[dsid];
		}
		physicsUV(int dynum, int stanum = 0)
		{
			_colliders[dsid].reserve(dynum);
			_colliders[ssid].reserve(stanum);
			_pos[dsid].reserve(dynum);
			_pos[ssid].reserve(stanum);
			_collpair.reserve(dynum);
		}
		Collid& CreatCir(const Collid&& cd, const mtransfrom& trs, CollideID& id, CollideID::CollidDS state = CollideID::CollidDS::Dynamic)
		{
			const int sta = (int)state;
			auto& coll = _colliders[sta];
			auto& mpos = _pos[sta];
			id.id = coll.size();
			_prevBox[id.id].Max[0] = id.tid;
			coll.emplace_back(cd);
			Collid& cd1 = coll[id.id];
			//cd.ID.id = id2;
			mpos.push_back(trs);
			return cd1;
		}
		Collid& Creat(const Collid&& cd, const math::Efloat2 boxSize, mtransfrom&& trs, CollideID& id, CollideID::CollidDS state = CollideID::CollidDS::Dynamic)
		{

			const int sta = (int)state;
			auto& coll = _colliders[sta];
			auto& mpos = _pos[sta];
			id.id = coll.size();
			_prevBox[id.id].Max[0] = id.tid;
			coll.emplace_back(cd);
			Collid& cd1 = coll[id.id];
			//cd.ID.id = id2;
			mpos.push_back(trs);
			if (_BoxPool.size() > 0) {
				auto sz = _BoxPool.size() - 1;
				cd1.data = _BoxPool[sz];
				_BoxPool.resize(sz);
			}
			else
			{
				std::abort();
			}

			cd1.getData<IBoxCollider>().Resize(cd1, boxSize);
			return cd1;
		}
		Collid& Creat(const Collid& cd, const math::Efloat2 boxSize, const mtransfrom& trs, CollideID& id, CollideID::CollidDS state = CollideID::CollidDS::Dynamic)
		{
			const int sta = (int)state;
			auto& coll = _colliders[sta];
			auto& mpos = _pos[sta];
			id.id = coll.size();
			_prevBox[id.id].Max[0] = id.tid;
			coll.emplace_back(cd);
			Collid& cd1 = coll[id.id];
			//cd.ID.id = id2;
			mpos.push_back(trs);
			if (_BoxPool.size() > 0) {
				auto sz = _BoxPool.size() - 1;
				cd1.data = _BoxPool[sz];
				_BoxPool.resize(sz);
			}
			else
			{
				std::abort();
			}

			cd1.getData<IBoxCollider>().Resize(cd1, boxSize);
			return cd1;
		}
		void lineCast(const float3 start, const float3 end, std::vector<CollideID>& result, int maxnum = 1) {
			const auto v1 = (start - end).head<2>().cwiseQuotient(cell);

		}
		void erase(CollideID const& cdid)
		{

			const int sta = (int)cdid.State;
			const int id = cdid.id;
			var& cds = _colliders[sta];
			var& trs = _pos[sta];
			/*if (cdid.State == CollideID::CollidDS::Dynamic) {
				Box2i& qb = _prevBox[cdid.id];
				var& lf = qb.Min;
				var& tr = qb.Max;
				qCell  xel;
				PosHash& posmap = _posmap;
				for (auto i = lf[0]; i <= tr[0]; ++i)
				{
					xel[0] = i;
					for (auto p = lf[1]; p <= tr[1]; ++p)
					{
						xel[1] = p;
						posmap.find(xel, [&](PosHash::value_type& val) {
							var& val2 = *val.second;

							if ((val.second->erase(cdid))) {
								if (val2.empty()) {
									if (!posmap.erase(xel, [&](PosHash::value_type& val) {
										assert(!val.second->inside);
										delete(val.second);
										})) {
										ERROR(fmt::format("erased empty voxel {0} fail", xel));
										std::abort();
									}
									return;
								}
								return;
							}
							val2.erase(cdid);
							const map_traits::hash_accessor hasher;
							for (size_t i = 0; i < val2.size(); i++)
							{
								var& ele = val2.ele[i];
								var same = false;
								if (ele == cdid)
									same = true;
								ERROR("cdid={0} ele={1} thread:{2} same {3} st {4} {5} i={6} reinsert={7}",
									cdid, ele.id, std::thread::id(), same,
									(size_t)ele, (size_t)cdid, i, posmap.insert(xel, &val2));
							}
							std::abort();
							});
					}

				}
				var& d = _prevBox[cdid.id];
				d.Max.setConstant(-Infinityf);
				d.Min.setConstant(Infinityf);
				Util::removeResize(_prevBox, cdid.id);

			}*/
			const var last = cds.size() - 1;
			//cds[last].ID = cdid;
			auto& cd = getCD(cdid);
			if (cd.colliderType == ColliderType::Box) {
				_BoxPool.push_back(cd.data);
				cd.data = -1;
			}
			Util::removeResize(cds, cdid.id);
			Util::removeResize(trs, id);
		}

	};
	template<typename T>
	struct DCte
	{

		std::vector<T> old;
		std::vector<T> curr;
		DCte(int size) {
			old.resize(size);
			curr.resize(size);
		}
	};
	struct HashCollision :physicsUV
	{
		using float3 = math::Efloat3;
		using raytype = math::Efloat2;
	private:
		AppendArray<Paircid> _apr;
		std::vector<voxel*>  _corase;
		DCte<raytype> _Rays;

		std::vector<Box2i> _currentBox;
	public:
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
		//thread_local static std::vector<voxel*> _gcvoxs;
		static HashCollision* instance;
		coarse cfilter;
		physicsUV* worldptr;
		AppendArray<contact> _manfu;
		tf::Executor executor;
		tf::Taskflow taskflow;
		const UINT cpunum = std::thread::hardware_concurrency();
		using i9 = int;
		//const auto vCcdThreshold=physic
		//const size_t reserveFactor = 10;
		/*voxel& newVox() {
			return voxs.emplace_back(v);
		}*/
		HashCollision(size_t dynum, size_t stanum = 0) :physicsUV(dynum, stanum),
			_apr(10 * dynum), _manfu(dynum), _Rays(dynum)
		{
			cds::Initialize();
			cds::threading::Manager::attachThread();
			worldptr = this;
			_around.resize(dynum);
			_corase.resize(dynum * 8);
			_prevBox.resize(dynum);
			_currentBox.resize(dynum);
			//
			//TaskEx::parallelfor(taskflow, 0, (int)_prevBox.size(), [&](int i) {
			//	var& d = _prevBox[i];
			//	d.Max.setConstant(-Infinityf);
			//	d.Min.setConstant(Infinityf);
			//	}, _prevBox.size() / 8);
			////init voxs per thread
			//TaskEx::parallelfor(taskflow, 0, (int)executor._threads.size(), [&](int i) {
			//	cds::threading::Manager::attachThread();
			//	//_gcvoxs.reserve(dycount / std::thread::hardware_concurrency() * gridmaxsize * 8);
			//	}, 1);
			//taskflow.clear();
			//voxs.reserve(dycount/std::thread::hardware_concurrency() * gridmaxsize*8);
			int ii = 0;
			i9 ij = ii;
			instance = this;
		}
		//template<typename A>
		inline void collp(physicsUV& wd, Paircid& pir, contact& result) {
			var paircdid0 = wd[pir.e1];
			var paircdid1 = wd[pir.e2];
			var& ta = *paircdid0.first;
			var& tb = *paircdid1.first;
			var& collida = *paircdid0.second, & collidb = *paircdid1.second;
			CalculContact(&result, collida, collidb, ta, tb, Physics::dt);
		}

		template<typename A, typename B>
		void F1(B& a, int s, int e, AppendArray<A>& result) {
			contact ct;
			if (s == 0) {
				var& curr = a[s];
				collp(*worldptr, curr, ct);
				if (ct.pointCount > 0)
					result.enqueue(ct);
				s++;
				if (s >= e)
					return;
			}
			var& prev = a[s - 1];
			while (s < e)
			{
				var& curr = a[s];
				if (prev != curr)
				{
					collp(*worldptr, curr, ct);
					if (ct.pointCount > 0)
						result.enqueue(ct);
					prev = curr;
				}
				s++;
			}

		}
		void erase(const CollideID cdid) {
			Box2i qb;
			if (cdid.State == CollideID::CollidDS::Dynamic)
				qb = _currentBox[cdid.id];
			else
			{
				const auto state = (int)cdid.State;
				CalculStartEnd(cell, Box2f(_pos[state][cdid.id].pos.head<2>(), _colliders[state][cdid.id].ConvexR), qb);
			}
			PosHash& posmap = (*this).worldptr->_posmap;
			{
				qCell  xel;
				{
					var& lf = qb.Min;
					var& tr = qb.Max;
					for (auto i = lf[0]; i <= tr[0]; ++i)
					{
						xel[0] = i;
						for (auto p = lf[1]; p <= tr[1]; ++p)
						{
							xel[1] = p;
							posmap.find(xel, [&](PosHash::value_type& val) {
								var& val2 = *val.second;

								if ((val.second->erase(cdid))) {
									if (val2.empty()) {
										if (!posmap.erase(xel, [&](PosHash::value_type& val) {
											assert(!val.second->inside);
											delete(val.second);
											})) {
											ERROR(fmt::format("erased empty voxel {0} fail", xel));
											std::abort();
										}
										return;
									}
									return;
								}
								/*ERROR("erase fail id={0} thread:{1} xel={2} i={3} p={4} obx={5} new box={6}",
									cdid, std::thread::id::id(), xel, i, p, obx.toString(), bbox.tof4());*/
								val2.erase(cdid);
								const map_traits::hash_accessor hasher;
								/*LoopGrid(obx, cell, [&](auto& const bxele) {
									ERROR("inside hash {0} bxele={1} xel hash={2}", hasher(bxele), bxele, hasher(xel));
									});*/
								for (size_t i = 0; i < val2.size(); i++)
								{
									var& ele = val2.ele[i];
									var same = false;
									if (ele == cdid)
										same = true;
									ERROR("cdid={0} ele={1} thread:{2} same {3} st {4} {5} i={6} reinsert={7}",
										cdid, ele.id, std::thread::id(), same,
										(size_t)ele, (size_t)cdid, i, posmap.insert(xel, &val2));
								}
								std::abort();
								});
						}

					}
				}
			}
			const int sta = (int)cdid.State;
			const int id = cdid.id;
			var& cds = _colliders[sta];
			var& trs = _pos[sta];
			const var last = cds.size() - 1;
			CalculStartEnd(cell, Box2f(trs[last].pos.head<2>(), cds[last].ConvexR), qb);
			//cds[last].ID = cdid;
			auto& cd = getCD(cdid);
			if (cd.colliderType == ColliderType::Box) {
				_BoxPool.push_back(cd.data);
				cd.data = -1;
			}
			LoopBox([&](const auto coord) {
				posmap.find(coord, [&](PosHash::value_type& val) {
					auto& va = *val.second;
					for (size_t i = 0; i < va.size(); i++)
					{
						auto& vae = va[i];
						if (vae.id == last && vae.State == cdid.State) {
							vae.id = cdid.id;
							break;
						}
					}
					});
				}, qb);
			Util::removeResize(cds, id);
			Util::removeResize(trs, id);
			if (cdid.State == CollideID::CollidDS::Dynamic) {
				_prevBox[id] = _prevBox[last];
				_prevBox[last].reset();
			}
		}
		void Move();
		/// <summary>
		/// solve collision
		/// </summary>
		void update() {

		}
	};
	//thread_local std::vector<voxel*> HashCollision::_gcvoxs;
	HashCollision* HashCollision::instance = NULL;
	/// <summary>
	/// se1:remove collider from exit cell
	///se2:insert to new occupied cell,allocate new cell if necessary
	//se3:get potential collider pair and remove duplicated pair 
	/// </summary>
	void HashCollision::Move()
	{
		constexpr int dyid = (int)CollideID::CollidDS::Dynamic;
		var& collpair = _apr;
		collpair.clear();
		var& world = *(this->worldptr);
		PosHash& posmap = world._posmap;
		const int items = world._pos[dyid].size();

		_around.resize(items);
		if (_currentBox.size() != _prevBox.size()) {
			_currentBox.resize(_prevBox.size());
		}
		//	std::atomic_llong chsum = 0;
		static std::atomic_llong ct1 = 0, ct2 = 0;

		ct1 = ct2 = 0;
		//LOGINFO("start Move");
		if (!taskflow.empty())
		{
			executor.run(taskflow).wait();
			//INFO("csum={0}", chsum);
			return;
		}
		var se1 = taskflow.emplace([&](tf::Subflow& sub) {

			TaskEx::parallelfor(
				sub, 0, items,
				[&](int id) {
					_around[id].clear();
					//LOGINFO("paraller for id={0}", id);
					const CollideID cdid(id, -1, CollideID::CollidDS::Dynamic);
					var trancoll = &(world._colliders[dyid][id]);
					Box2f bbox(world._pos[dyid][id].pos.head<2>(), trancoll->ConvexR);

					var& obx = _prevBox[id];
					Box2i& qb = _currentBox[id];
					CalculStartEnd(cell, bbox, qb);
					//	chsum += id;
					var& lf = obx.Min;
					var& tr = obx.Max;
					//const var olderbox = obx;
					qCell  xel;
					LoopGrid(obx, [&](const auto xel) {
						{
							if (qb.Contain(xel))
								return;
							bool es = false;
							posmap.find(xel, [&](PosHash::value_type& val) {
								var& val2 = *val.second;
								
								if ((val2.erase(cdid))) {
									if (val2.empty()) {
										if (!posmap.erase(xel, [&](PosHash::value_type& val) {
											assert(!val.second->inside);
											es = true;
											//_gcvoxs.push_back(val.second);
											delete(val.second);
											})) {
											ERROR(fmt::format("erased empty voxel {0} fail", xel));
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
								ERROR("val2.erase(cdid)={0}", val2.erase(cdid));
								const map_traits::hash_accessor hasher;
								/*LoopGrid(obx, cell, [&](auto& const bxele) {
									ERROR("inside hash {0} bxele={1} xel hash={2}", hasher(bxele), bxele, hasher(xel));
									});*/
								for (size_t i = 0; i < val2.size(); i++)
								{
									var& ele = val2.ele[i];
									var same = false;
									if (ele == cdid)
										same = true;
									ERROR("cdid={0} ele={1} thread:{2} same {3}\
 st {4} {5} i={6} reinsert={7}",
										cdid, ele.id, std::thread::id(), same,
										(size_t)ele, (size_t)cdid, i, posmap.insert(xel, &val2));
								}
								std::abort();
								});
							if (es && posmap.contains(xel))
								/*	posmap.erase(xel);
								if(posmap.contains(xel))*/
								std::abort();
						}});
				},
				items / 10); });

		var se2 = taskflow.emplace([&](tf::Subflow& sub) {
			TaskEx::parallelfor(
				sub, 0, items,
				[&](int id) {
					//LOGINFO("paraller for id={0}", id);
					CollideID cdid(id, -1, CollideID::CollidDS::Dynamic);
					var trancoll = &(world._colliders[dyid][id]);
					//Box2f bbox(world._pos[dyid][id].pos.head<2>(), trancoll->ConvexR);
					Box2i& qb = _currentBox[id];
					var& olderbox = _prevBox[id];
					var& lf = qb.Min;
					var& tr = qb.Max;
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
													//var pd = posmap.get(float2(i, p));

							if (!posmap.find(xel, [&](PosHash::value_type& val) {
								const var& vx = val.second;
								if (olderbox.Contain(xel)) {
									//assert((!vx->insert(cdid)));
									if (vx->inside)
										return;
									if (!vx->empty() && !vx->inside) {
										vx->inside = true;
										_corase[ct1++] = val.second;
									}
									return;
								}

								//assert(!val.second->contain(cdid));
								//assert(vx != NULL);
								while (vx == NULL) {

								}
								cdid.tid = _prevBox[cdid.id].Max[0];
								vx->insert(cdid);
								if (!vx->inside) {
									vx->inside = true;
									_corase[ct1++] = val.second;
									//if (needtest.enqueue(val.second))
								}
								//if (vx.insert(cdid)) {
								//	if (!ept && !vx.inside) {
								//		vx.inside = true;
								//		_corase[ct1++] = val.second;
								//		//if (needtest.enqueue(val.second))
								//	}
								//}
								//else
								//{
								//	std::abort();
								//}
								}))
							{
								voxel* p_vx;
								bool False = false;
								posmap.update(xel, [&](auto& item, auto* old) {

									if (old)
									{
										std::abort();
									}
									else {
										p_vx = new voxel;
										item.second = p_vx;
									}
									//auto& val = *vx;
									if (olderbox.Contain(xel)) {
										//assert((!vx->insert(cdid)));
										if (p_vx->inside)
											return;

										//std::atomic_bool  inside;
										if (!p_vx->empty() && p_vx->inside.compare_exchange_strong(False, true)) {
											//_corase[ct1++] = p_vx;  cause compiler error
											False = true;
										}
										return;
									}
									cdid.tid = _prevBox[cdid.id].Max[0];
									p_vx->insert(cdid);
									if (p_vx->inside.compare_exchange_strong(False, true)) {
										//_corase[ct1++] = NULL;
										False = true;
									}
									});
								if (False) {
									_corase[ct1++] = p_vx;
								}
							}
								//if (!posmap.insert_with((xel), [&](PosHash::value_type& val) {
								//	//voxel* clist=NULL;
								//	//if (_gcvoxs.size() == 0)
								//	/*else
								//	{
								//		clist = _gcvoxs[_gcvoxs.size() - 1];
								//		_gcvoxs.pop_back();
								//	}*/
								//	//assert(clist->empty());
								//	/*if (val.second != NULL)
								//		std::abort();*/
								//	val.second = ((new voxel()));
								//	val.second->inside = false;
								//	val.second->insert(cdid);
								//	}
								//)) {
								//	if (!posmap.find(xel, [&](PosHash::value_type& val) {
								//		bool fa = false;
								//		while (val.second == NULL) {}
								//		var& vx = *val.second;
								//		const	var ept = vx.empty();
								//		assert(!vx.contain(cdid));
								//		vx.insert(cdid);
								//		if (!ept && !vx.inside) {
								//			vx.inside = true;
								//			_corase[ct1++] = val.second;
								//			//if (needtest.enqueue(val.second))
								//		}

								//		})) {
								//		std::abort();
								//	}
								//}
						}
					}
					olderbox = qb;
				},
				items / 10); });
		const var subnum = cpunum;
		std::vector<tf::Task> tasks;
		//taskflow.parallel_for()
		tasks.resize(subnum);
		var se3 = taskflow.emplace([&](tf::Subflow& sub) {

			//LOGINFO("ct1={0} mapsize{1}", ct1, posmap.size());
			TaskEx::parallelfor(sub, 0, (int)(std::ref(ct1)).get(), [&](int id) {
				const gridPtr& gp = _corase[id];
				var& gpref = *gp;
				gpref.inside = false;
				for (var i = gpref.begin(); i != gpref.end(); ++i)
				{
					var pi = i;
					++pi;
					voxele iv = gpref[i];
					if (iv.State == CollideID::CollidDS::Dynamic) {
						auto& _aa = _around[iv.id];
						//constexpr auto comp = [&](const voxele& a, const voxele& bvoxele) {return };
						const auto remain = _aa.remain();
						if (remain < gpref.size())
							_aa.count = 0;

						for (var i1 = gpref.begin(); i1 != gpref.end(); ++i1)
						{
							_aa.insert(gpref[i1 % gridmaxsize]);
							//if (_aa.remain() > 2)
							//{
							//	_aa.insert(gpref[i1]);
							//}
							//else
							//{
							//	_aa.count = 0;
							//}
						}
						//_aa.erase(i);
					}
					//ar.add(iv);
					for (; pi != gpref.end(); ++pi)
					{
						const var piv = gpref[pi];
						var pir = Paircid(iv, piv);
						//var cont = math::CantorPloynomialUnOrdered((size_t)(iv), (size_t)(piv));
						//if (mset.insert(cont))
						{
							var paircdid0 = world[piv];
							var paircdid1 = world[iv];
							const var dissq = (paircdid0.first->pos - paircdid1.first->pos).squaredNorm();
							const var& collida = *paircdid0.second, & collidb = *paircdid1.second;
							const var dis = dissq - math::square(paircdid0.second->ConvexR + paircdid1.second->ConvexR);
							//if (dis < collidb.detectRange)

							if (dis < 0)//&& mset.insert(cont))
							{
								if (pir.v2ll[0] > pir.v2ll[1])
									std::swap(pir.v2ll[0], pir.v2ll[1]);
								collpair.enqueue(pir);
							}
						}
					}
				}
				}, 10);
			});
		var t2 =
			taskflow.emplace([&](tf::Subflow& sub) {
			if (collpair.size() == 0)
				return;
			std::sort(std::execution::par, &collpair[0], &collpair[0] + collpair.size());
			const var cuk = collpair.size() / cpunum;
			for (size_t i = 0; i < cpunum; i++)
			{
				const auto  s = i * cuk;
				sub.emplace([=, &collpair]() {
					F1(collpair, s, math::min(s + cuk, collpair.size()), _manfu);
					});
			}

				});

		var solvecontact = taskflow.emplace([&](tf::Subflow& sub) {
			if (_manfu.size() == 0)
				return;
			TaskEx::parallelfor(sub, 0, (int)_manfu.size(), [&](int id) {
				//auto& pair = _manfu[id];
				int i = 0;
				while (!SolvePositionConstraints(&_manfu[id]) && i++ < 4)
				{
				}
				}, 10);

			});
		se1.precede(se2);
		se2.precede(se3);
		se3.name("paraller for");
		se3.precede(t2);
		t2.precede(solvecontact);
		executor.run(taskflow).wait();
	}
}