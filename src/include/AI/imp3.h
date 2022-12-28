#pragma once

//#pragma optimize("", off)
#include "Common/ecs.hpp"
//#include "Common/syss.hpp"
#include"Config.h"
#include"algorithm0.h"
#include"Render/ShaderStructs.h"
#include<bitset>
//export module AI;
namespace pj2::AI {
	constexpr float W_SP(0.6f);
	constexpr float W_AR(0.5f);
	constexpr float W_CH(0.15f);

	struct flstocks
	{
		using k_ = std::size_t;
		using v_ = math::Vector2f;
		static auto&& fkmap() {
			static phmap::parallel_flat_hash_map<k_, v_> fkmap(4096);
			return fkmap;
		}
		inline static auto&& GetGoal(k_& k_) {
			return fkmap().find(k_);
		}
	};
	//struct Boid3 : Unit<Boid3>
	//{
	//	float av = math::DegToRad(240), lv = 3;

	//	//comp 0 rep cd,1 rep if ready
	//	math::uint2 maincd;
	//	units::StateDescV atdesc;
	//	template<class T>
	//	constexpr static void Moveto(T& sf, mtransfrom::v4& tv, sStruct::instData& renderstate) {
	//		renderstate.color = { 0,1,0,1 };
	//		auto& sff = reinterpret_cast<Boid3&>(sf);
	//		const auto forward = sff.transform.forward();
	//		//const auto distrotate=math::AngleAxisf()
	//		const auto qvt = math::quaternion::FromTwoVectors(forward, tv);
	//		math::AngleAxisf qd(qvt);
	//		const auto adis = (sff.transform.rotation.angularDistance(qvt * sff.transform.rotation));
	//		sff.collider.AngularVelocity = adis > sff.av ? qd.axis() * sff.av : qd.axis() * adis;
	//		sff.collider.Velocity = tv.normalized() * lv;
	//		sff.collider.Velocity *= adis > math::DegToRad(10);

	//		//const auto angledis = math::AngleTo(forward, tv);
	//		//sff.
	//	}
	//	template<class T>
	//	constexpr static void tick(Controller& ct, Unit<>& tg, sStruct::instData& renderstate) {

	//	}
	//	template<class T>
	//	constexpr static const auto& MaxSpeed(T& tg) {
	//		return reinterpret_cast<self&>(tg).lv;
	//	}
	//	template<class T, class T2>
	//	constexpr static const auto SetSpeed(T& tg, T2& sped, sStruct::instData& renderstate) {
	//		reinterpret_cast<self&>(tg).collider.Velocity.head<2>() = sped.head<2>();
	//		renderstate.color = { 0,1,0,1 };
	//	}
	//	template<class T>
	//	constexpr static void Attack(T& sf, Unit<>& tg, sStruct::instData& renderstate) {
	//		renderstate.color = { 1,0,0,1 };
	//		auto& tv = tg.transform.pos;
	//		auto& sff = reinterpret_cast<Boid3&>(sf);
	//		auto& cpos4 = sff.transform.pos4;
	//		const auto forward = sff.transform.forward();
	//		const auto qvt = math::quaternion::FromTwoVectors(forward, tv);
	//		math::AngleAxisf qd(qvt);

	//		const auto adis = (sff.transform.rotation.angularDistance(qvt * sff.transform.rotation));
	//		sff.collider.AngularVelocity = adis > sff.av ? qd.axis() * sff.av : qd.axis() * adis;
	//		sff.collider.Velocity *= 0;
	//		if (adis > math::DegToRad(5))
	//			return;
	//		if (sf.maincd[1] == 0) {
	//			sf.maincd[1] = sf.maincd[0];
	//			units::StateDescV& asdesc = sf.atdesc;
	//			tg.ApplyState(asdesc);
	//		}
	//		//constexpr int vf = sizeof(math::Vector3i);
	//		//const auto angledis = math::AngleTo(forward, tv);
	//		//sff.
	//	}
	//	template<class T>
	//	constexpr static void Reached(T& sf, sStruct::instData& renderstate) {
	//		renderstate.color = { 0,0,1,1 };
	//		auto& tv = tg.transform.pos;
	//		auto& sff = reinterpret_cast<Boid3&>(sf);
	//		auto& cpos4 = sff.transform.pos4;
	//		const auto forward = sff.transform.forward();
	//		const auto qvt = math::quaternion::FromTwoVectors(forward, tv);
	//		math::AngleAxisf qd(qvt);

	//		const auto adis = (sff.transform.rotation.angularDistance(qvt * sff.transform.rotation));
	//		sff.collider.AngularVelocity = adis > sff.av ? qd.axis() * sff.av : qd.axis() * adis;
	//		sff.collider.Velocity *= 0;
	//		if (adis > math::DegToRad(5))
	//			return;
	//		if (sf.maincd[1] == 0) {
	//			sf.maincd[1] = sf.maincd[0];
	//			units::StateDescV& asdesc = sf.atdesc;
	//			tg.ApplyState(asdesc);
	//		}
	//		//constexpr int vf = sizeof(math::Vector3i);
	//		//const auto angledis = math::AngleTo(forward, tv);
	//		//sff.
	//	}
	//};
	namespace GoupControl {
		template <typename T, math::uint sz = 11>
		struct Gnode
		{

			//using postype = mtransfrom::v4;
			math::uint activer = 0, startid = 0, tick = -1;
			std::bitset<sz> active;
			postp tginfo_;
			mtransfrom trans;
			//std::array<postp, sz> Poss;
			Gnode() {
				active = 0u;
			}
			//constexpr auto size() { return sz - 1; }
			static constexpr auto size() {
				return sz;
			}
			constexpr auto Alived() {
				return active.count();
			}
			inline postp& TG() {
				return tginfo_;
			}
			/*inline auto& operator [](const std::size_t i) const {
				return Poss[i];
			}
			inline void MarkActive(const std::size_t i, bool v) {
				active[i] = v;
			}*/
			//std::array<int, sz> eleState;
		};
	}
	//TODO: multi class dynamic size goup
	template <typename T, class... _Types>
	struct GoupController0 : ECS::Component0<GoupController0<T>> {
		using self = GoupController0 < T>;
		using Commander = GoupControl::Gnode<T>;

		mtransfrom::v4 _target;
		mtransfrom _trans;
		Utility::bitmark<> TragetFilter;
		uint _start = 0, _num = 0, tick = 0;
		std::vector<Commander> _gNodes;
		std::vector<postp> _gInfo;
		float separationRadius = 5.0f;
		float separationAngle = -0.707f;
		float separationWeight = 0.6f;
		float alignmentRadius = 7.5f;
		float alignmentAngle = 0.7f;
		float alignmentWeight = 8.0f;
		int _nebth = 5, _nebsz = 4;
		const float cohesionRadius = 0.2f;
		const float cohesionAngle = -0.15f;
		float cohesionWeight = 0.2f;
		//ECS::CompAccessor<sStruct::instData> insd;
		GoupController0() {
		}
		GoupController0(Controller& ct, mtransfrom trans, float unitrad = 4, math::uint num_ = 11 * 4, Controller::tinfo fc = {}) {
			constexpr auto gg = sizeof(std::tuple<int, math::i4>);
			auto n1md = num_ % Commander::size();
			auto n1num = n1md == 0 ? num_ / Commander::size() : num_ / Commander::size() + 1;
			fc.CanRender = true;
			assert(false, "Regist function discarded");
			//ct.Regist<T, _Types...>(num_ * 2, CollideID::CollidDS::Dynamic, 32, fc);
			_gNodes.resize(n1num);
			auto&& cps = ct.GetRowComps(T::type);
			cps.reserve(cps.size() + num_);
			size_t row = math::sqrt(num_);
			_num = num_;
			mtransfrom::v4 off = { row * 0.5f,row * 0.5f, 0,0 };
			off = -off;
			//off *= unitrad*2;
			_start = cps.size();
			_gInfo.resize(n1num * Commander::size());
			for (size_t i = 0, i1 = 0; i < _gNodes.size(); i++)
			{
				//auto ndid = i / num_;
				auto& n1 = _gNodes[i];
				//n1.active.flip();
				//size_t ed = std::min(row, num_ - i);
				//if(i+1<_gNodes.size())
				for (size_t i5 = 0; i5 < n1.size(); i5++)
				{

					n1.active[i5] = i1 < num_;
					auto&& tes = ct.Emplace<T>();
					tes.transform = trans;
					tes.transform.pos4 += off;
					tes.transform.pos4[0] += i1 / row;
					tes.transform.pos4[1] += i1 % row;
					tes.transform.pos4.head<2>() *= unitrad * 3;
					tes.basic.Actionrangesq = unitrad + 1;
					ICircleCollider::CreatCollid(1, unitrad, false, tes.collider);
					i1++;
				}
			}
			_trans = trans;
		}
		mtransfrom::v3 AdjectV(mtransfrom::v4 const& newv_, mtransfrom::v3 const& cv_, float speed, float maxv, float maxav) {
			const auto&& newv = newv_.head<3>();
			const auto&& cv = cv_.head<3>();
			const float maxAdjustedSpeed = maxv * 0.3;
			auto cosdis = math::dot(newv, cv);
			if ((speed > maxAdjustedSpeed) || newv.isApproxToConstant(0, math::Epsilon))
			{
				return newv;
			}
			else
			{
				const float range = speed / maxAdjustedSpeed;
				const float cosine = math::interpolate(pow(range, 20), 1.0f, -1.0f);
				//return limitMaxDeviationAngle(force, cosine, forward());
				const auto& basis = cv;
				const auto& cosineOfConeAngle = cosine;
				const auto& source = newv;
				// immediately return zero length input vectors
				float sourceLength = source.norm();
				// measure the angular diviation of "source" from "basis"
				const auto direction = source / sourceLength;
				float cosineOfSourceAngle = math::dot(direction, basis);

				// Simply return "source" if it already meets the angle criteria.
				// (note: we hope this top "if" gets compiled out since the flag
				// is a constant when the function is inlined into its caller)
				if (true)
				{
					// source vector is already inside the cone, just return it
					if (cosineOfSourceAngle >= cosineOfConeAngle) return source;
				}
				else
				{
					// source vector is already outside the cone, just return it
					if (cosineOfSourceAngle <= cosineOfConeAngle) return source;
				}

				// find the portion of "source" that is perpendicular to "basis"
				mtransfrom::v3 perp;
				perp.setConstant(0);
				(math::rotate90(basis, perp));// math::rotate source.perpendicularComponent(basis);
				// normalize that perpendicular
				auto unitPerp = math::dot(perp, source) > 0 ? perp.normalized() : -perp.normalized();

				// construct a new vector whose length equals the source vector,
				// and lies on the intersection of a plane (formed the source and
				// basis vectors) and a cone (whose axis is "basis" and whose
				// angle corresponds to cosineOfConeAngle)
				float perpDist = math::sqrt(1 - (cosineOfConeAngle * cosineOfConeAngle));
				auto c0 = basis * cosineOfConeAngle;
				auto c1 = (unitPerp)*perpDist;
				auto vh = (c0 + c1) * sourceLength;
				if (vh.squaredNorm() > (maxv * maxv)) {
					return vh.normalized() * maxv;
				}
				return vh;
			}
		}
		template <typename T>
		constexpr void LoopSubnods(T&& f) {
			for (size_t i = 0; i < _num; i++)
			{
				auto h = i / Commander::size();
				auto cmopid = _start + i;
				auto& n = _gNodes[h];
				f(n, i % Commander::size(), cmopid);
			}
		}
		template <typename... Args >
		auto& Emplace(Controller& ct, Args&&... args) {
			ct.Emplace<T>(args...);
		}
		static int Update(Controller& ct, uint i) {
			//ct.GetComps<self>(i).UpdateGroupState(ct);
			return 1;
		}
		template <typename T>
		void SetTarget(T& vect_) {
			assert(false);
			//Utility::EleSet(_target, vect_);
		}
		template <typename T>
		constexpr auto VMTo(Utility::span<T> ojs) {
			for (auto&& tm : ojs) {

			}
			for (size_t i = 0; i < ojs.size(); i++)
			{

			}
		}
		void SetTarget(postp& v) {}
		/*constexpr auto GroupExist(CollideID&& cd) {
			math::Contain(_start, _num, cd.id)?-1,cd.id-_start;
		}*/
		template<class T, class T1>
		auto
			steerForSeparation(const T1& ginfo_,
				const float cosMaxAngle, const mtransfrom::v3 const& forward,
				T& flock, const float bufdis_ = 0.01f)
		{
			// steering accumulator and count of neighbors, both initially zero
			mtransfrom::v4 steering;
			steering.setConstant(0);
			int neighbors = 0;

			for (size_t i = 0; i < flock.size(); i++)
			{
				auto& flk = flock[i];
				if (_gInfo[flk.cd.id].state == State::idle) {
					flk.relativeVelocity.setConstant(0);

					//continue;
					//flk.distSq(10000);
				}
				if (ginfo_.state == State::idle || (_gInfo[flk.cd.id].state != State::idle &&
					Boids::inBoidNeighborhood(flk, cosMaxAngle, forward)))
				{
					// add in steering contribution
					// (opposite of the offset direction, divided once by distance
					// to normalize, divided another time to get 1/d falloff)
					const auto dis = (flk.distSq() - (flk.combinedRadiussq() + bufdis_));
					steering += -(flk.relativePosition * math::exp(math::min((dis * -32), 10)));

					// count neighbors
					++neighbors;
				}
			}

			return steering;
		}
		void UpdateGroupState(Controller& ct) {
			if (tick++ % 3 != 0)
				return;
			auto& cps = ct.GetRowComps(T::type);
			auto& ards = ct.filter.Arounds(T::type);
			auto&& insd = ct.getArch<T>().getcp<sStruct::instData>();
			//calculate group Center
			constexpr auto GetCpid = [](auto&& nid, auto&&) {};
			for (size_t i = 0, i1 = 0; i < _gNodes.size(); i++)
			{
				constexpr auto ast = Commander::size();

				Commander& cNode = _gNodes[i];
				cNode.activer = 0;
				for (size_t i9 = 0; i9 < ast && i1 < _num; i9++, i1++)
				{
					auto& ut = cps.Get<Unit<>>(i9 + i * ast + _start);
					cNode.active[i9] = ut.Active();
					if (!cNode.active[i9])
						continue;
					//auto& ut1 = ct.GetEnti<Unit<>>(sv.cd);
					cNode.activer++;
					//cNode.Poss[i9].forword.head<3>() = ut.transform.froward();
					auto& atm = _gInfo[i1]; //cNode.Poss[i9];
					atm.pos4 = ut.transform.pos4;
					cNode.trans.pos4 += atm.pos4;
				}
				if (cNode.activer == 0)
					continue;
				cNode.trans.pos4 /= cNode.activer;
				cNode.TG().pos4 = _target - cNode.trans.pos4;
				cNode.TG().pos4[3] = math::sqnorm(cNode.TG().pos4.head<3>());
			}
			//search around try to find possible target
			for (size_t i = _start, i1 = 0, Commid = 0; i < _num; i++, i1++)
			{
				const auto ast = Commander::size();
				Commid = i / ast;
				const auto modu = i % ast;
				/*Commander* p_cmd;
				if (i >= _gNodes[Commid].size())
					p_cmd = &_gNodes[++Commid];*/
				Commander& cd = _gNodes[Commid];
				if (cd.activer == 0 || !cd.active[modu]) {
					continue;
				}
				auto renderstate = insd[i];
				auto& ut = cps.Get<Unit<>>(i);
				postp& ndrep = _gInfo[i1];// cd.Poss[modu];
				if (ndrep.cid_target.id != -1) {
					Unit<>& tg = ct.GetEnti<Unit<>>(ndrep.cid_target);
					if (tg.Active())
					{
						ndrep.vect = tg.transform.pos4 - ndrep.pos4;
						ndrep._tgpos = tg.transform.pos4;
						ndrep.state.SetCst(State::move);
						ndrep.vect[3] = ndrep.vect.head<3>().squaredNorm();
					}
					else
					{
						ndrep.vect.setConstant(mtransfrom::Maxv4s);
						ndrep.cid_target.id = -1;
					}
				}
				ndrep.tick = -1;
				/*if (cd.eleState[(int)ndrep.states] > cd.eleState[(int)Ai::State::move]) {
					T::tick(ct, ut, renderstate);
					ndrep.tick = 1;
					continue;
				}*/
				//cd.activer++;
				auto& neb = ards[i];
				for (size_t i2 = 0; i2 < neb.size(); i2++)
				{
					auto&& [sv] = neb.get(i2);
					/*auto& sv =(nbd).get<0>(i2);
					auto& nbd =(nbd).get<1>(i2);*/
					//find target
					//if (TragetFilter.AndAny(std::get<Utility::bit128>(nbd)))
					{
						auto&& dv = sv.relativePosition;
						auto sqnG = (cd.trans.pos4 - (dv + ut.transform.pos4));
						auto dissq = sqnG.head<3>().squaredNorm();
						if (sqnG[3] < cd.TG().vect[3]) {
							cd.TG().vect = sqnG;
							cd.TG().vect[3] = dissq;
							cd.TG().cid_target = sv.cd;
						}
						dv[3] = dv.head<3>().squaredNorm();
						if (dv[3] < ndrep.vect[3]) {
							ndrep.vect = dv;
							ndrep.cid_target = sv.cd;
							//ndrep.pos[3] = sqn;
						}
					}
				}
				//size_t f = _start;
			}
			/*auto&& getState = [&](CollideID&& i_) {
				if (i_.tid == cps.Gattid() && i_.id >= _start && i_.id < _start + _num) {
					r
			}
			};*/
			auto stid = _start;
			constexpr auto checknan = [](auto& a) {
				return !(a[0] == a[0]);
			};
			//set speed
			for (size_t i = 0, i1 = 0; i < _gNodes.size(); i++)
			{
				Commander& nnn = _gNodes[i];
				for (size_t i2 = 0; i2 < nnn.size() && i1 < _num; i1++, i2++)
				{
					//Commander& nnn = ndrep[i2];
					postp& ndrep = _gInfo[i1];//nnn.Poss[i2];
					if (!nnn.active[i2])
						continue;
					const auto abid = stid + i1;
					Unit<>& ut = cps.Get<Unit<>>(abid);
					const auto forward = ut.transform.froward();
					auto&& renderdata = insd[abid];
					auto&& neb = ards[abid];
					auto max_v = T::MaxSpeed(ut);
					auto max_dp = max_v * Physics::dt;
					auto ACR = math::square(ut.collider.ConvexR + max_dp);
					const auto maxvel = T::MaxSpeed(ut);// *Physics::dt;
					const auto maxvelsq = maxvel * maxvel;
					constexpr auto cosMaxAngle = 0.0f;
					auto& neighbors = neb;
					//not find target around
					if (ndrep.cid_target.id == -1) {
						ndrep.vect = _target - ndrep.pos4;
						if (ndrep._tgpos != _target) {
							ndrep._tgpos = _target;
							ndrep.state.SetCst(State::move);
						}
						constexpr auto slowRadius = 10.0f;
						constexpr auto slowRadiussq = slowRadius * slowRadius;
						auto& dssq = ndrep.vect[3];
						dssq = ndrep.vect.head<3>().squaredNorm();
						//	ndrep.vect.head<3>() = slowRadiussq > dssq ? ndrep.vect.head<3>()* dssq/slowRadius:
					}
					math::truncate(ndrep.vect.head<3>(), max_v);
					ndrep.vect.head<3>() *= 0.5;
					auto separation = steerForSeparation(ndrep, separationAngle, forward,
						neighbors);
					auto cohesion = Boids::steerForCohesion(forward, ut.transform.pos4,
						cohesionAngle,
						neighbors);
					//forward.squaredNorm()
					auto normsq = separation.head<3>().squaredNorm();
					auto& ut_speed = ut.collider.Velocity;
					if (normsq > maxvelsq) {
						separation *= maxvel / math::sqrt(normsq);
					}
					normsq = cohesion.head<3>().squaredNorm();
					if (normsq > maxvelsq) {
						cohesion *= maxvel / math::sqrt(normsq);
					}
					if (ndrep.vect[3] > ACR && ndrep.state != State::idle) {
						/*if (ndrep.state == State::idle) {
							ndrep.vect.setConstant(0);
						}
						else*/
						int u = 0;
						for (size_t i3 = 0; i3 < neb.size(); i3++)
						{
							auto& other = neb[i3];
							if (math::Contain(_start, _num, other.cd.id) &&
								_gInfo[other.cd.id - _start].state == State::idle &&
								_gInfo[other.cd.id - _start]._tgpos == ndrep._tgpos &&
								//neb.size()>=4&&
								(other.distSq() - other.combinedRadiussq()) < 1) {
								//meet a member reach  to goal
								u++;
							}
						}
						if (u > _nebth && neb.size() > _nebsz) {
							ndrep.vect.setConstant(0);
							ut.collider.Velocity.setConstant(0);
							ndrep.state.SetCst(State::idle);
						}
						//const int& cst = ndrep.states;
						//if (nnn.eleState[cst] < nnn.eleState[(int)Ai::State::move]) [[unlikely]]
						{
							// apply weights to components (save in variables for annotation)
							const auto separationW = separation.head<3>() * separationWeight;
							//const auto alignmentW = alignment * alignmentWeight;
							const auto cohesionW = cohesion.head<3>() * cohesionWeight;
							//ndrep.vect.head<3>().normalize();
							ndrep.vect.head<3>() += separationW + cohesionW;
							ut.collider.vel4.head<3>() = AdjectV(ndrep.vect, forward, ut_speed.norm(), maxvel, 0);
							assert(math::ckftvalid(ndrep.vect.head<2>()[0]));
							//same as T::Moveto(ut, ndrep.vect);
							T::SetSpeed(ut, //ut.collider.Velocity.head<2>(),
								AI::RVO::computeNewVelocity(ct, ut.collider.Velocity.head<2>(),
									neb,
									maxvel, ndrep.vect.head<2>()),
								renderdata
							);
							if (!ut.collider.Velocity.isApproxToConstant(0, math::Epsilon))
								ut.transform.rotation *= math::quaternion::FromTwoVectors(forward, ut.collider.Velocity);
						}
					}
					else// if (nnn.eleState[i2] < nnn.eleState[(int)Ai::State::attack])
					{
						ndrep.state.SetCst(State::idle);
						//ndrep.vect.setConstant(0); 
						const auto separationW = separation.head<3>();
						//const auto alignmentW = alignment * alignmentWeight;
						//const auto cohesionW = cohesion.head<3>() * 0;
						//ndrep.vect.head<3>().normalize();
						ndrep.vect.head<3>() = separationW;
						ut.collider.vel4.head<3>() = AdjectV(ndrep.vect, forward, ut_speed.norm(), maxvel, 0);
						math::truncate(ut.collider.Velocity, maxvel);
						if (!ut.collider.Velocity.isApproxToConstant(0, math::Epsilon))
							ut.transform.rotation *= math::quaternion::FromTwoVectors(forward, ut.collider.Velocity);
						ut.transform.rotation.normalize();
						/*T::SetSpeed(ut,
							AI::AVO::computeNewVelocity(ct, ut.collider.Velocity.head<2>(), neb,
								maxvel, ndrep.vect.head<2>()), renderdata
						);*/
						//ndrep.states.SetCst(Ai::State::attack);
						T::Reached(reinterpret_cast<T&>(ut), renderdata);
					}
				}
			}
		}

		void DispatchCmd(span<CollideID> e) {
			std::sort([](CollideID& a, CollideID& b) {return a < b; }, e.begin(), e.end());

		}
		template<class T, class T1>
		constexpr auto& tupget(T1&& tup) { return *std::get<T*>(tup); }

		void reSetNeb(int i) {
			_nebth = i;
			for (postp& v : _gInfo)
			{
				v.state.SetCst(State::move);
			}
		}
	};
	/*template<class L>
	struct flocks
	{
		std::vector<pj2::CollideID> members;
		phmap::parallel_flat_hash_map<pj2::CollideID,>
		Utility::FixAy<Cdid, 64> _reached;
		void Update(Controller& ct) {

			for (size_t i = 0; i < members.size(); i++)
			{
				auto&& k = members[i];
				auto& ards = ct.filter.Arounds(k.tid)[k.id];
				for (size_t i1 = 0; i1 < ards.size(); i1++)
				{
					auto&& k1 = ards[i1];
				}
			}
		}
	};*/
}