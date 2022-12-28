#pragma once
#include"Common/Math1.h"
#include"physics/physics.hpp"
#include"Gameprops0.h"
//#include"Common/syss.hpp"
#include<vector>
namespace pj2::AI {
	using CollideID = Physics::CollideID;
	using mtransfrom = math::mtransform;
	using AiSIn = Utility::MyTuple<>;
	using State = GameProps::State;
	//using CollideID = math::mtransfrom;
	struct  postp {
		mtransfrom::v4 vect;
		mtransfrom::v4 pos4;
		//mtransfrom::v4 forword;
		mtransfrom::v4 _tgpos;
		CollideID cid_target;
		State state;
		int tick;// , goalid = 0;
		postp() {
			vect.setConstant(-1);
		}
		constexpr bool operator==(State& s)const {
			return s == state;
		}
	};
#define STAU static inline auto
		struct gProp {
			std::tuple<State,int> datas;
			template<size_t I = 0>
			constexpr auto&& get() {
				return std::get<I>(datas);
			}
		};
		struct WayPoint
		{
			mtransfrom::v4 point;
		};

		class Boids {
			using Lavnes = Physics::Lavnes;
			
		public:
			constexpr static auto inBoidNeighborhood = [](const auto& v, const auto& cosMaxAngle,// const auto& minDistance,
				mtransfrom::v3 const& forward) {
					const auto& offset = v.relativePosition;
					const auto& distanceSquared = v.distSq();
					// otherwise, test angular offset from forward axis
					const auto unitOffset = offset / sqrt(distanceSquared);
					const float forwardness = math::dot(forward, unitOffset.template head<3>());
					return forwardness > cosMaxAngle;
					// definitely in neighborhood if inside minDistance sphere
					//if (distanceSquared < (minDistance * minDistance))
					//{
					//	return true;
					//}
					//else
					//{
					//	// definitely not in neighborhood if outside maxDistance sphere
					//	if (distanceSquared > (maxDistance * maxDistance))
					//	{
					//		return false;
					//	}
					//	else
					//	{
					//		// otherwise, test angular offset from forward axis
					//		const auto unitOffset = offset / sqrt(distanceSquared);
					//		const float forwardness = math::dot(forward, unitOffset);
					//		return forwardness > cosMaxAngle;
					//	}
					//}
			};
			template<class T>
			STAU
				steerForSeparation(//const float maxDistance,
					const float cosMaxAngle, const mtransfrom::v3 const& forward,
					const T& flock, const float bufdis_ = 0.01f)
			{
				// steering accumulator and count of neighbors, both initially zero
				mtransfrom::v4 steering;
				steering.setConstant(0);
				int neighbors = 0;
				for (size_t i = 0; i < flock.size(); i++)
				{
					auto& flk = flock[i];
					if (inBoidNeighborhood(flk, cosMaxAngle, forward))
					{
						// add in steering contribution
						// (opposite of the offset direction, divided once by distance
						// to normalize, divided another time to get 1/d falloff)
						const auto dis = (flk.distSq() - (flk.combinedRadiussq() + bufdis_));
						steering += -(flk.relativePosition * math::exp((dis) * -20));

						// count neighbors
						++neighbors;
					}
				}

				// divide by neighbors, then normalize to pure direction
				// bk: Why dividing if you normalize afterwards?
				//     As long as normilization tests for @c 0 we can just call normalize
				//     and safe the branching if.
				/*
				if (neighbors > 0) {
					steering /= neighbors;
					steering = steering.normalize();
				}
				*/
				return steering;
			}
			template<class T,class T2>
			STAU 
				steerForSeparation(const T2&& gmap_ ,
					const T& flk, math::mtransform::v4&& separation,const float bufdis_ = 0.01f)
			{
				const auto& othstate = gmap_->find(flk.cd.hash());
				//set relativeVelocity to 0 indicate not perform avoidence
				if (othstate != gmap_->end() && othstate->second.get<0>() == State::idle) {
					auto&& othsd = othstate->second.get<0>();
					flk.relativeVelocity.setConstant(0);
				}
				else {
					const auto dis = (flk.distSq() - (flk.combinedRadiussq() + bufdis_));
					separation += -(flk.relativePosition * math::exp(math::min((dis * -32), 10)));
				}
			}

			// ----------------------------------------------------------------------------
			// Cohesion behavior: to to move toward center of neighbors

			using Lavnes = Physics::Lavnes;
			//using LAVNe = Physics::LAVNe;

			template<class T>
			STAU steerForCohesion(const mtransfrom::v3 const& forward, mtransfrom::v4& position,
				const float cosMaxAngle,
				const T& flock) {
				mtransfrom::v4 steering(0, 0, 0, 0);
				int neighbors = 0;
				for (size_t i = 0; i < flock.size(); i++)
				{
					auto&& flk = std::get<0>(flock.get(i));
					auto&& bst= std::get<1>(flock.get(i));
					if (inBoidNeighborhood(flk, cosMaxAngle, forward))
					{
						// add in steering contribution
						// (opposite of the offset direction, divided once by distance
						// to normalize, divided another time to get 1/d falloff)
						steering += (flk.relativePosition + position);

						// count neighbors
						++neighbors;
					}
				}
				// divide by neighbors, subtract off current position to get error-
				// correcting direction, then normalize to pure direction
				if (neighbors == 0) [[unlikely]] {
					return steering;
				}
				steering /= (float)neighbors;
				steering -= position;
				return steering;
			}

			template<class T, class T1>
			STAU steerForCohesion(const T1&& inGroupf_, const mtransfrom::v3 const& forward, mtransfrom::v4& position,
				const float cosMaxAngle,
				const T& flock) {
				mtransfrom::v4 steering(0, 0, 0, 0);
				int neighbors = 0;
				for (size_t i = 0; i < flock.size(); i++)
				{
					auto&& flk = std::get<0>(flock.get(i));
					auto&& bst = std::get<1>(flock.get(i));
					if (inBoidNeighborhood(flk, cosMaxAngle, forward))
					{
						// add in steering contribution
						// (opposite of the offset direction, divided once by distance
						// to normalize, divided another time to get 1/d falloff)
						steering += (flk.relativePosition + position);

						// count neighbors
						++neighbors;
					}
				}
				// divide by neighbors, subtract off current position to get error-
				// correcting direction, then normalize to pure direction
				if (neighbors == 0) [[unlikely]] {
					return steering;
				}
				steering /= (float)neighbors;
				steering -= position;
				return steering;
			}

			// ----------------------------------------------------------------------------
			// Alignment behavior: steer to head in same direction as neighbors


			template<class T>
			STAU
				steerForAlignment(const float maxDistance,
					const float cosMaxAngle, mtransfrom::v4& cvel, const mtransfrom::v3 const& forward,
					const T& flock)
			{
				// steering accumulator and count of neighbors, both initially zero
				mtransfrom::v3 steering(0, 0, 0);
				int neighbors = 0;
				for (size_t i = 0; i < flock.size(); i++)
				{
					auto& flk = flock[i];
					if (inBoidNeighborhood(flk, cosMaxAngle, forward))
					{
						// add in steering contribution
						// (opposite of the offset direction, divided once by distance
						// to normalize, divided another time to get 1/d falloff)
						steering += (flk.relativeVelocity + cvel).head<3>();

						// count neighbors
						++neighbors;
					}
				}

				// divide by neighbors, subtract off current heading to get error-
				// correcting direction, then normalize to pure direction
				if (neighbors > 0) steering = ((steering / (float)neighbors) - forward).normalized();

				return steering;
			}


			STAU steerForArrive() {}
		};
		template <typename T, class... _Types>
		struct GoupController1 : ECS::Component0<GoupController1<T>> {
			//using Commander = GoupControl::Gnode<T>;

			mtransfrom::v4 _target;
			mtransfrom _trans;
			Utility::bit128 _TargetFilter;
			uint _start = 0, _num = 0, tick = 0;
			//std::vector<Commander> _gNodes;
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
			GoupController1() {
			}
			//GoupController1(Controller& ct, mtransfrom trans, float unitrad = 4, math::uint num_ = 11 * 4, Controller::tinfo fc = {}) {
			//	constexpr auto gg = sizeof(std::tuple<int, math::i4>);
			//	auto n1md = num_ % Commander::size();
			//	auto n1num = n1md == 0 ? num_ / Commander::size() : num_ / Commander::size() + 1;
			//	fc.CanRender = true;
			//	ct.Regist<T, _Types...>(num_ * 2, CollideID::CollidDS::Dynamic, fc);
			//	_gNodes.resize(n1num);
			//	auto&& cps = ct.GetRowComps(T::type);
			//	cps.reserve(cps.size() + num_);
			//	size_t row = math::sqrt(num_);
			//	_num = num_;
			//	mtransfrom::v4 off = { row * 0.5f,row * 0.5f, 0,0 };
			//	off = -off;
			//	//off *= unitrad*2;
			//	_start = cps.size();
			//	_gInfo.resize(n1num * Commander::size());
			//	for (size_t i = 0, i1 = 0; i < _gNodes.size(); i++)
			//	{
			//		//auto ndid = i / num_;
			//		auto& n1 = _gNodes[i];
			//		//n1.active.flip();
			//		//size_t ed = std::min(row, num_ - i);
			//		//if(i+1<_gNodes.size())
			//		for (size_t i5 = 0; i5 < n1.size(); i5++)
			//		{

			//			n1.active[i5] = i1 < num_;
			//			auto&& tes = ct.Emplace<T>();
			//			tes.transform = trans;
			//			tes.transform.pos4 += off;
			//			tes.transform.pos4[0] += i1 / row;
			//			tes.transform.pos4[1] += i1 % row;
			//			tes.transform.pos4.head<2>() *= unitrad * 3;
			//			tes.basic.Actionrangesq = unitrad + 1;
			//			ICircleCollider::CreatCollid(1, unitrad, false, tes.collider);
			//			i1++;
			//		}
			//	}
			//	_trans = trans;
			//}
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
			/*template <typename T>
			void SetTarget(T& vect_) {
				Utility::EleSet(_target, vect_);
			}*/
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
			void reSetNeb(int i) {
				_nebth = i;
				for (postp& v : _gInfo)
				{
					v.state.SetCst(State::move);
				}
			}
		};
};