#pragma once

#include"Common/Math1.h"
#include "Common/ecs.hpp"
#include "Common/syss.hpp"
#include"Config.h"
#include"Render/ShaderStructs.h"
#include<bitset>
#include"parallel_hashmap/phmap.h"
namespace pj2::AI {
	struct mkfi
	{
		//int size = 0;
		std::vector<int> _memberid;
		constexpr auto size()const {
			return _memberid.size();
		}
	};
	template <typename T, class... _Types>
	struct IAi :GoupController1<T,_Types...>
	{
		std::vector<CollideID> members;
		std::vector<ECS::CompPackageFix<8,CollideID>> _potentialTargets;
		std::vector<WayPoint> Ways;
		std::vector<mkfi> _subGroupinfo;
		phmap::parallel_flat_hash_map<size_t, gProp>* _hmmembers;
		CollideID _target;
		State _state;
		mtransfrom::v4 _tgpos;
		Utility::bit128 _selfmark;
		static int Update(Controller& ct, uint i) {
			ct.GetComp<self>(i).UpdateGroupState(ct);
			return 1;
		}
		void addMember(Utility::span<CollideID> collids) {
			std::sort(collids.begin(), collids.end(), [](const CollideID& a, const CollideID& b) {
				return a.tid == b.tid ? a.id < b.id : a.tid < b.tid;
				});
			members.assign(collids.begin(), collids.end());
			this->_gInfo.resize(members.size());
		}
		using Lavnes = Physics::Lavnes;
		//using LAVNe = Physics0::LAVNe;
		template<class T, class T1>
		auto
			steerForSeparation(const T1& ginfo_,
				const float cosMaxAngle, const mtransfrom::v3 const& forward,
				Lavnes& flock, const float bufdis_ = 0.01f)
		{
			// steering accumulator and count of neighbors, both initially zero
			mtransfrom::v4 steering;
			steering.setConstant(0);
			int neighbors = 0;
			for (size_t i = 0; i < flock.size(); i++)
			{
				auto& flk =std::get<0>(flock.get(i));
				if (flk.state == State::idle) {
					flk.relativeVelocity.setConstant(0);

					//continue;
					//flk.distSq(10000);
				}
				if (ginfo_.state == State::idle || (flk.state != State::idle &&
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
		template<State mode>
		void MoveTo(Controller& ct){
			ECS::Conany* arry_ptr = NULL;
			//if (_state == State::move)
			{
				auto& cps = ct.GetRowComps(T::type);
				auto&& insd = ct.getArch<T>().Cmp<sStruct::instData>();
				for (size_t i = 0, prevtid = -1; i < members.size(); i++)
				{
					auto&& cid = members[i];
					if (!arry_ptr || cid.tid != prevtid) {
						arry_ptr = &(ct.GetRowComps(cid.tid));
					}
					auto& cps = *arry_ptr;
					auto& ut = cps.Get<Unit<>>(cid.id);
					if (!ut.Active())
						continue;

					postp&& ndrep = this->_gInfo[i];
					ndrep.vect = ndrep._tgpos - ut.transform.pos4;
					auto&& [max_v, ACR] = T::AIProp(ut,ndrep);
					//auto max_v = T::MaxSpeed(ut);
					auto max_dp = max_v * Physics::dt;
					//auto ACR = math::square(ut.collider.ConvexR + max_dp);
					mtransfrom::v3 forward = ut.transform.froward();
					auto&& position = ut.transform.pos4();
					auto& neb = ct.filter.Arounds(cid.tid)[cid.id];
					size_t friendneb = 0;
					float cosMaxAngle = 0;
					mtransfrom::v4 separation, cohesion;
					separation.setConstant(0);
					cohesion.setConstant(0);
					if constexpr (mode == State::idle) {
						for (size_t i2 = 0; i2 < neb.size(); i2++)
						{

							auto&& [sv, bid] = neb.get(i2);
							if (Boids::inBoidNeighborhood(sv, cosMaxAngle, forward)) {
								if (_selfmark == bid) {
									cohesion += (sv.relativePosition + position);
									friendneb++;
								}
							}
							Boids::steerForSeparation(*_hmmembers, sv, separation);
						}
					}
					else if constexpr (mode==State::move){
						//steer
						for (size_t i2 = 0; i2 < neb.size(); i2++)
						{

							auto&& [sv, bid] = neb.get(i2);
							if (Boids::inBoidNeighborhood(sv, cosMaxAngle, forward)) {
								if (_selfmark == bid) {
									cohesion += (sv.relativePosition + position);
									friendneb++;
								}
							}
							Boids::steerForSeparation(*_hmmembers, sv, separation);
						}
					}
					else if (mode == State::attackwhenfind) {
						//search around try to find possible target
						for (size_t i2 = 0; i2 < neb.size(); i2++)
						{

							auto&& [sv, bid] = neb.get(i2);
							bool findenemy = false;
							//find around member
							if ((bid & _TargetFilter).any() && sv.distSq() < ndrep.vect[3]) {
								//if(Boids::inBoidNeighborhood(sv,0,))
								findenemy = true;
								auto&& dv = ndrep.vect;
								dv= sv.relativePosition;
								dv[3] = dv.head<3>().squaredNorm();
								ndrep.cid_target = sv.cd;
								ndrep.state.SwitchState(State::attack);
							}
						}
						//get preferred speed and if not find target try get friend target
						//steer
						for (size_t i2 = 0; i2 < neb.size(); i2++)
						{

							auto&& [sv, bid] = neb.get(i2);
							if (Boids::inBoidNeighborhood(sv, cosMaxAngle, forward)) {
								if (_selfmark == bid) {
									cohesion += (sv.relativePosition + position);
									friendneb++;
								}
							}
							const auto& othstate = _hmmembers->find(sv.cd.hash());
							if (othstate != _hmmembers->end()) {
								const auto&& nebstate = othstate->second.get<0>();
								if (nebstate == State::idle) {
									auto&& othsd = othstate->second.get<0>();
									//set relativeVelocity to 0 indicate not perform avoidence
									sv.relativeVelocity.setConstant(0);
								}
								//not find target get friend target
								else
									if (ndrep.cid_target.id == -1 && nebstate.Any(State::attack)) {
										auto&& friendprop = this->_gInfo[othstate->second.get<1>()];
										ndrep.cid_target = friendprop.cid_target;
										ndrep._tgpos = friendprop._tgpos;
										ndrep.vect = ndrep._tgpos - ndrep.pos4;
										ndrep.vect[3] = ndrep.vect.head<3>().squaredNorm();
										ndrep.state.SwitchState(State::move);
									}
							}
							else {
								const auto dis = (flk.distSq() - (flk.combinedRadiussq() + bufdis_));
								separation += -(flk.relativePosition * math::exp(math::min((dis * -32), 10)));
							}
						}
					}
					else if (mode == State::attack ) {
						if(ACR < ndrep.vect[3])
						for (size_t i2 = 0; i2 < neb.size(); i2++)
						{

							auto&& [sv, bid] = neb.get(i2);
							if (Boids::inBoidNeighborhood(sv, cosMaxAngle, forward)) {
								if (_selfmark == bid) {
									cohesion += (sv.relativePosition + position);
									friendneb++;
								}
							}
							Boids::steerForSeparation(*_hmmembers, sv, separation);
						}else{
							(T::Attack(ct,ndrep));
							ut.collider.Velocity.setZero();
							return;
						}
					}
					math::truncate(ndrep.vect.head<3>(), max_v);
					cohesion = friendneb > 0 ? (cohesion / friendneb) - position : cohesion;
					auto normsq = separation.head<3>().squaredNorm();
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
		void PreUpdate(Controller& ct){
			for (size_t i = 0; i < members.size(); i++)
			{
				auto&& cid = members[i];
				if (!arry_ptr || cid.tid != prevtid) {
					arry_ptr = &(ct.GetRowComps(cid.tid));
				}
				auto& cps = *arry_ptr;
				auto& ut = cps.Get<Unit<>>(cid.id);
				if (!ut.Active())
					continue;
				auto& neb = ct.filter.Arounds(cid.tid)[cid.id];
				for (size_t i2 = 0; i2 < neb.size(); i2++)
				{

					auto&& [sv, bid] = neb.get(i2);
					if (Boids::inBoidNeighborhood(sv, cosMaxAngle, forward)) {
						if (_selfmark == bid) {
							cohesion += (sv.relativePosition + position);
							friendneb++;
						}
					}
					const auto& othstate = _hmmembers->find(sv.cd.hash());
					sv.state = othstate != _hmmembers->end() ?State::non:othstate->second.get<0>() == State::idle;
				}
			}
		}
		void UpdateGroupState(Controller& ct) {
			ECS::Conany* arry_ptr = NULL;
			if (_state == State::move)
			{
				auto& cps = ct.GetRowComps(T::type);
				auto&& insd = ct.getArch<T>().Cmp<sStruct::instData>();
				for (size_t i = 0, prevtid = -1; i < members.size(); i++)
				{
					auto&& cid = members[i];
					if (!arry_ptr || cid.tid != prevtid) {
						arry_ptr = &(ct.GetRowComps(cid.tid));
					}
					auto& cps = *arry_ptr;
					auto& ut = cps.Get<Unit<>>(cid.id);
					if (!ut.Active())
						continue;
					postp&& ndrep = this->_gInfo[i];
					ndrep.vect = _tgpos - ut.transform.pos4;
					auto max_v = T::MaxSpeed(ut);
					auto max_dp = max_v * Physics::dt;
					auto ACR = math::square(ut.collider.ConvexR + max_dp);
					math::truncate(ndrep.vect.head<3>(), max_v);
					mtransfrom::v3 forward = ut.transform.froward();
					auto& neb = ct.filter.Arounds(cid.tid)[cid.id];
					size_t friendneb = 0; 
					float cosMaxAngle = 0;
					mtransfrom::v4 separation,cohesion;
					separation.setConstant(0);
					cohesion.setConstant(0);
					//steer
					for (size_t i2 = 0; i2 < neb.size(); i2++)
					{
						
						auto&& [sv,bid ]= neb.get(i2);
						if (Boids::inBoidNeighborhood(sv, cosMaxAngle,forward)) {
							if(_selfmark == bid){
								cohesion += (sv.relativePosition + position);
								friendneb++;
							}
							const auto& othstate = _hmmembers->find(sv.cd.hash());
							//set relativeVelocity to 0 indicate not perform avoidence
							if (othstate != _hmmembers->end()&& othstate->second.get<0>() == State::idle) {
								auto&& othsd = othstate->second.get<0>();
								sv.relativeVelocity.setConstant(0);
							}
							else {
								const auto dis = (flk.distSq() - (flk.combinedRadiussq() + bufdis_));
								separation += -(flk.relativePosition * math::exp(math::min((dis * -32), 10)));
							}
						}
						//auto& sv = std::get<0>(nbd);
						//find around member
						//if (!(std::get<Utility::bit128>(nbd)&_TargetFilter).any()) {
						//	//if(Boids::inBoidNeighborhood(sv,0,))
						//	auto&& dv = sv.relativePosition;
						//	auto sqnG = (cd.trans.pos4 - (dv + ut.transform.pos4));
						//	auto dissq = sqnG.head<3>().squaredNorm();
						//	if (sqnG[3] < cd.TG().vect[3]) {
						//		cd.TG().vect = sqnG;
						//		cd.TG().vect[3] = dissq;
						//		cd.TG().cid_g = sv.cd;
						//	}
						//	dv[3] = dv.head<3>().squaredNorm();
						//	if (dv[3] < ndrep.vect[3]) {
						//		ndrep.vect = dv;
						//		ndrep.cid_g = sv.cd;
						//		//ndrep.pos[3] = sqn;
						//	}
						//}
						//else {

						//}
					}

					auto normsq = separation.head<3>().squaredNorm();
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
			if (_target.State != CollideID::CollidDS::non)
				auto&& tgp = ct.filter.GetPhysObj(_target.tid, _target.id);
			for (size_t i = 0,prevtid=-1; i < members.size(); i++)
			{
				auto&& cid = members[i];
				if (!arry_ptr || cid.tid != prevtid) {
					arry_ptr = &(ct.GetRowComps(cid.tid));
				}
				auto& cps = *arry_ptr;
				auto& ut = cps.Get<Unit<>>(cid.id);
				if (!ut.Active())
					continue;
			}
		}
		void SetTarget(CollideID& tg_) {
			_target = tg_;
		}
		void MoveTo(mtransfrom::v4& pos_) {
			Ways.clear();
			_tgpos = pos_;
			_target.State = CollideID::CollidDS::non;
		}
	};
}