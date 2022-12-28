#pragma once
#include "Common/ecs.hpp"
//#include "Common/syss.hpp"
#include"Config.h"
#include"algorithm0.h"
#include"Render/ShaderStructs.h"
#include<bitset>
namespace pj2::AI {

	struct GMgr1 {

		float separationRadius = 5.0f;
		float separationAngle = -0.707f;
		float separationWeight = 0.6f;
		float alignmentRadius = 7.5f;
		float alignmentAngle = 0.7f;
		float alignmentWeight = 8.0f;
		int _nebth = 3, _nebsz = 4;
		const float cohesionRadius = 0.2f;
		const float cohesionAngle = -0.15f;
		float cohesionWeight = 0.2f;
		void FindSpeed(GameProps::Cmd0& cmd,
			mtransform& transform,
			Collid& collider,
			GameProps::BasicProp& bp,
			Physics::Lavnes& Neb);
		template<int i,class ...Ts>
		void FindSpeed(GameProps::Cmd0& cmd,
			mtransform& transform,
			Collid& collider,
			GameProps::BasicProp& bp,
			Physics::Lavnes& Neb,Ts&... args);
	};
	/*constexpr auto _Pred = [](LAVNe& a, EntityId& b) {
		return a.cd < b;
	}; */
	template<int i, class ...Ts>
	void FindSpeed(GameProps::Cmd0& cmd,
		mtransform& transform,
		Collid& collider,
		GameProps::BasicProp& bp,
		Physics::Lavnes& Neb, Ts&... args);
	template <class... _Types1>
	auto&& getprop(int id) {
		return Utility::MyTuple<_Types1...>();
	}
#define TGET(tname,tup,resu)  tname& resu = *std::get<tname*>(tup)
	//template<class F,class F1 >
	//void update(int i,F&& fvel,F1&& farrive) {
	//	using EntityId = GameProps::EntityId;
	//	constexpr auto checknan = [](auto& a) {
	//		return !(a[0] == a[0]);
	//	};
	//	using tcmd = GameProps::Cmd0;
	//	using state = GameProps::state;
	//	using Physics::LAVNe;
	//	
	//	std::tuple<tcmd*, Collid*, mtransform*, sStruct::instData*, Physics::Lavnes*> cmps;
	//	TGET(tcmd, cmps, cmd);
	//	TGET(mtransform, cmps, transform);
	//	TGET(Physics::Lavnes, cmps, neb);
	//	TGET(Collid, cmps, collider);

	//	float separationRadius = 5.0f;
	//	float separationAngle = -0.707f;
	//	float separationWeight = 0.6f;
	//	float alignmentRadius = 7.5f;
	//	float alignmentAngle = 0.7f;
	//	float alignmentWeight = 8.0f;
	//	int _nebth = 5, _nebsz = 4;
	//	const float cohesionRadius = 0.2f;
	//	const float cohesionAngle = -0.15f;
	//	float cohesionWeight = 0.2f;
	//	//set speed
	//	//for (size_t i = 0, i1 = 0; i < _gNodes.size(); i++)
	//	{
	//		//Commander& nnn = _gNodes[i];
	//		//for (size_t i2 = 0; i2 < nnn.size() && i1 < _num; i1++, i2++)
	//		{
	//			//Commander& nnn = ndrep[i2];
	//			//postp& ndrep = _gInfo[i1];//nnn.Poss[i2];
	//			//if (!nnn.active[i2])
	//			//	continue;
	//			//const auto abid = stid + i1;
	//			//Unit<>& ut = cps.Get<Unit<>>(abid);
	//			const auto forward = transform.froward();
	//			//auto&& neb = ards[abid];
	//			const auto maxvel = collider.MaxVelocity;// T::MaxSpeed(ut);
	//			auto max_dp = maxvel * Physics::dt;
	//			const auto maxvelsq = maxvel * maxvel;
	//			constexpr auto cosMaxAngle = 0.0f;
	//			auto& neighbors = neb;
	//			mtransform::v4 separation;
	//			mtransform::v4 vect = cmd.tpos - transform.pos4;
	//			const auto& ACR = cmd.tpos[3];// math::square(collider.ConvexR + max_dp);
	//			math::truncate(vect.head<3>(), maxvel);
	//			vect[3] = vect.head<3>().squaredNorm();
	//			using fixs = Fix::fix_s;
	//			if (cmd.Any<state::attackwhenfind>()) {
	//				Fix::fix_s mindis = Fix::fix_s::max();
	//				size_t imin;
	//				separation = steerForSeparation(cmd, separationAngle, forward,
	//					neighbors, [&](LAVNe& neb1, auto& i) {
	//						return neb1.Prop_ptrs.get<0>()->gid == cmd.gid;
	//						if (cmd.hostile[neb1.Prop_ptrs.get<0>()->gid]) {
	//							imin = i;
	//							mindis = std::min(mindis, fixs(neb1.distSq()));
	//							return false;
	//						}
	//						return true;
	//					});
	//				if (Fix::fix_s(cmd.tpos[3]) > mindis) {
	//					cmd.SetState<state::findattack>();
	//					const auto& closest = neighbors[imin];
	//					cmd.tpos = transform.pos4 + closest.relativePosition;
	//					cmd.eid = closest.cd;
	//				}
	//			}
	//			else
	//				separation = steerForSeparation(cmd, separationAngle, forward, neighbors,
	//					[&cmd](const LAVNe& neb1,const auto& i) {
	//						return neb1.Prop_ptrs.get<0>()->gid == cmd.gid;
	//					});
	//			auto cohesion = Boids::steerForCohesion(forward, transform.pos4,
	//				cohesionAngle,
	//				neighbors, [&](LAVNe& neb1, auto& i) {
	//					return neb1.Prop_ptrs.get<0>()->gid == cmd.gid; });
	//			//forward.squaredNorm()
	//			auto normsq = separation.head<3>().squaredNorm();
	//			auto& ut_speed = collider.Velocity;
	//			if (normsq > maxvelsq) {
	//				separation *= maxvel / math::sqrt(normsq);
	//			}
	//			normsq = cohesion.head<3>().squaredNorm();
	//			if (normsq > maxvelsq) {
	//				cohesion *= maxvel / math::sqrt(normsq);
	//			}
	//			if (vect[3] > ACR &&
	//				cmd.Any(state::move)) {
	//				/*if (ndrep.state == State::idle) {
	//					vect.setConstant(0);
	//				}
	//				else*/
	//				int u = 0;
	//				//for (size_t i3 = 0; i3 < neb.size(); i3++)
	//				//{
	//				//	auto& other = neb[i3];
	//				//	auto& othcmd = other.Prop_ptrs.get<0>();
	//				//	const bool friendly = cmd.gid == othcmd->gid;
	//				//	if (friendly &&
	//				//		othcmd->idle() &&
	//				//		cmd.cmdid == othcmd->cmdid &&
	//				//		//neb.size()>=4&&
	//				//		(other.distSq() - other.combinedRadiussq()) < 1) {
	//				//		//meet a member reach  to goal
	//				//		u++;
	//				//	}
	//				//}
	//				if (u > _nebth && neb.size() > _nebsz) {
	//					vect.setConstant(0);
	//					collider.Velocity.setConstant(0);
	//					//ndrep.state.SetCst(State::idle);
	//					farrive(vect);
	//					return;
	//				}
	//				//const int& cst = ndrep.states;
	//				//if (nnn.eleState[cst] < nnn.eleState[(int)Ai::State::move]) [[unlikely]]
	//				{
	//					// apply weights to components (save in variables for annotation)
	//					const auto separationW = separation.head<3>() * separationWeight;
	//					//const auto alignmentW = alignment * alignmentWeight;
	//					const auto cohesionW = cohesion.head<3>() * cohesionWeight;
	//					//vect.head<3>().normalize();
	//					vect.head<3>() += separationW + cohesionW;
	//					const auto& adis = AI::LookAtDirection(vect, transform, collider);// AdjectV(vect, forward, ut_speed.norm(), maxvel, 0);
	//					collider.vel4 = adis == 0 ? vect : vect.Zero();
	//					assert(math::ckftvalid(vect.head<2>()[0]));
	//					const auto finvel = AI::RVO::computeNewVelocity(collider.Velocity.head<2>(), collider.Velocity.head<2>(),
	//						neb,
	//						maxvel, vect.head<2>());
	//					fvel(finvel);
	//				}
	//			}
	//			else// if (nnn.eleState[i2] < nnn.eleState[(int)Ai::State::attack])
	//			{
	//				//vect.setConstant(0); 
	//				const auto separationW = separation.head<3>();
	//				//const auto alignmentW = alignment * alignmentWeight;
	//				//const auto cohesionW = cohesion.head<3>() * 0;
	//				//vect.head<3>().normalize();
	//				vect.head<3>() = separationW;
	//				//collider.vel4.head<3>() = AdjectV(vect, forward, ut_speed.norm(), maxvel, 0);
	//				math::truncate(collider.Velocity, maxvel);
	//				farrive(vect);
	//			}
	//		}
	//	}
	//}
}