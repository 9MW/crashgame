//export module imp3;

export module AI;
#include"Common/Math1.h"
#include "Common/ecs.hpp"
#include "Common/syss.hpp"
#include"AI/Config.h"
#include"Render/ShaderStructs.h"
#include<bitset>
namespace pj2::AI {
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
	struct  postp {
		mtransfrom::v4 vect;
		mtransfrom::v4 pos4;
		//mtransfrom::v4 forword;
		mtransfrom::v4 av;
		CollideID cid;
		Ai::State states;
		int tick;
	};
	namespace GoupControl {
		template <typename T, math::uint sz = 11>
		struct Gnode
		{

			//using postype = mtransfrom::v4;
			math::uint activer = 0, startid = 0, tick = -1;
			std::bitset<sz> active;
			mtransfrom trans;
			std::array<postp, sz> Poss;
			//constexpr auto size() { return sz - 1; }
			static constexpr auto size() {
				return sz;
			}
			inline postp& Last() {
				return Poss.back();
			}
			inline postp& TG() {
				return Last();
			}
			inline auto& operator [](const std::size_t i) const {
				return Poss[i];
			}
			std::array<int, sz> eleState;
		};
	}
	export
	template <typename T, class... _Types>
	struct GoupController0 : ECS::Component0<GoupController0<T>> {
		using self = GoupController0 < T>;
		using Commander = GoupControl::Gnode<T>;
		mtransfrom::v4 _target;
		mtransfrom _trans;
		Utility::bitmark<> TragetFilter;
		uint _start = 0, _num = 0;
		std::vector<Commander> _gNodes;
		//ECS::CompAccessor<sStruct::instData> insd;
		GoupController0() {
		}
		GoupController0(Controller& ct, mtransfrom trans, int unitrad = 4, math::uint num_ = 11 * 4) {
			auto n1num = num_ % Commander::size() == 0 ? num_ / Commander::size() : num_ / Commander::size() + 1;
			ct.Regist<T, _Types...>(num_ * 2);
			_gNodes.reserve(n1num);
			_gNodes.resize(n1num);
			auto&& cps = ct.GetRowComps(T::type);
			cps.reserve(cps.size() + num_);
			size_t row = math::sqrt(num_);
			_num = num_;
			mtransfrom::v4 off = { row * 0.5f,row * 0.5f, 0,0 };
			off = -off;
			off *= unitrad * 2;
			_start = cps.size();
			for (size_t i = 0; i < num_; i += row)
			{
				size_t ed = std::min(row, num_ - i);
				for (size_t ie = 0; ie < ed; ie++)
				{

					auto&& tes = ct.Emplace<T>();
					tes.transform = trans;
					tes.transform.pos4 = trans.pos4 + off;
					tes.transform.pos4[0] += i * unitrad;
					tes.transform.pos4[1] += ie * unitrad;
					ICircleCollider::CreatCollid(1, unitrad, false, tes.collider);
				}
			}
			_trans = trans;
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
			ct.GetComps<self>(i).UpdateGroupState(ct);
			return 1;
		}
		template <typename T>
		void SetTarget(T& vect_) {
			Utility::EleSet(_target, vect_);
		}
		constexpr auto VMTo(mtransfrom::v4 vect_, mtransfrom::v4 _target) {

		}
		void UpdateGroupState(Controller& ct) {
			auto& cps = ct.GetRowComps(T::type);
			auto& ards = ct.filter.Arounds(T::type);
			auto&& insd = ct.getArch<T>().Cmp<sStruct::instData>();
			//calculate group Center
			constexpr auto GetCpid = [](auto&& nid, auto&&) {};
			for (size_t i = 0; i < _gNodes.size(); i++)
			{
				constexpr auto ast = Commander::size();

				Commander& cNode = _gNodes[i];
				cNode.activer = 0;
				for (size_t i9 = 0; i9 < ast; i9++)
				{
					auto& ut = cps.Get<Unit<>>(i9 + i * ast + _start);
					cNode.active[i9] = ut.Active();
					if (!ut.Active())
						continue;

					//auto& ut1 = ct.GetEnti<Unit<>>(sv.cd);
					cNode.activer++;
					//cNode.Poss[i9].forword.head<3>() = ut.transform.froward();
					auto& atm = cNode.Poss[i9];
					atm.av.head<3>() = ut.collider.AngularVelocity;
					atm.pos4 = ut.transform.pos4;
					cNode.trans.pos4 += atm.pos4;
				}
				if (cNode.activer == 0)
					continue;
				cNode.trans.pos4 /= cNode.activer;
				cNode.TG().pos4 = _target - cNode.trans.pos4;
				cNode.TG().pos4[3] = math::sqnorm(cNode.TG().pos4.head<3>());
			}
			//search around
			for (size_t i = _start; i < _num; i++)
			{
				constexpr auto ast = Commander::size();
				const auto Commid = i / ast;
				Commander& cd = _gNodes[Commid];
				if (cd.activer == 0) {
					//i += ast - 1;
					continue;
				}
				auto renderstate = insd[i];
				auto& ut = cps.Get<Unit<>>(i);
				const auto modu = i % ast;
				postp& ndrep = cd.Poss[modu];
				if (ndrep.cid.id != -1) {
					Unit<>& tg = ct.GetEnti<Unit<>>(ndrep.cid);
					if (ut.Active()) {
						ndrep.vect = tg.transform.pos4 - ndrep.pos4;
						ndrep.vect[3] = ndrep.vect.head<3>().squaredNorm();
					}
					else
					{
						ndrep.vect.setConstant(mtransfrom::Maxv4s);
						ndrep.cid.id = -1;
					}
				}
				ndrep.tick = -1;
				if (cd.eleState[modu] > cd.eleState[(int)Ai::State::move]) {
					T::tick(ct, ut, renderstate);
					ndrep.tick = 1;
					continue;
				}
				//cd.activer++;
				auto& neb = ards[i];
				for (size_t i2 = 0; i2 < neb.size(); i2++)
				{
					auto& sv = neb[i2];
					if (TragetFilter.AndAny(sv.b128)) {
						auto&& dv = sv.relativePosition;
						auto sqnG = (cd.trans.pos4 - (dv + ut.transform.pos4));
						auto dissq = sqnG.head<3>().squaredNorm();
						if (sqnG[3] < cd.TG().vect[3]) {
							cd.TG().vect = sqnG;
							cd.TG().vect[3] = dissq;
							cd.TG().cid = sv.cd;
						}
						dv[3] = dv.head<3>().squaredNorm();
						if (dv[3] < ndrep.vect[3]) {
							ndrep.vect = dv;
							ndrep.cid = sv.cd;
							//ndrep.pos[3] = sqn;
						}
					}
				}
				//size_t f = _start;
			}

			//set speed
			for (size_t i = 0; i < _gNodes.size(); i++)
			{
				Commander& nnn = _gNodes[i];
				auto stid = i * Commander::size() + _start;
				for (size_t i2 = 0; i2 < Commander::size(); i2++)
				{
					//Commander& nnn = ndrep[i2];
					postp& ndrep = nnn.Poss[i2];
					if (!nnn.active[i2])
						continue;
					//not find target around
					if (ndrep.cid.id == -1) {
						ndrep.vect = _target - ndrep.pos4;
						ndrep.vect[3] = ndrep.vect.head(3).squaredNorm();
					}
					const auto abid = stid + i2;
					Unit<>& ut = cps.Get<Unit<>>(abid);
					auto renderdata = insd[abid];
					if (ndrep.vect[3] > ut.basic.Actionrangesq) {
						//const int& cst = ndrep.states;
						//if (nnn.eleState[cst] < nnn.eleState[(int)Ai::State::move]) [[unlikely]]
						{
							ndrep.states.SetCst(Ai::State::move);
							auto&& neb = ards[abid];
							//same as T::Moveto(ut, ndrep.vect);
							T::SetSpeed(ut,
								::AI::AVO::computeNewVelocity(ct, ut.collider.Velocity.head<2>(), neb,
									T::MaxSpeed(ut), ndrep.vect.head<2>()), renderdata
							);
						}
					}
					else if (nnn.eleState[i2] < nnn.eleState[(int)Ai::State::attack]) {
						//ndrep.states.SetCst(Ai::State::attack);
						T::Reached(reinterpret_cast<T&>(ut), renderdata);
					}
				}
			}
		}
	};
}