#pragma once
#include"Common/Math1.h"
#include"physics/physics.hpp"
#include"Gameprops0.h"
namespace pj2::AI::RVO {

	using Vector2 = math::Vector2f;
	constexpr auto sqr = [](const auto& a) { return a * a; };
	constexpr auto  RVO_EPSILON = math::Epsilon;
	constexpr auto absSq = [](const auto& a) { return a.dot(a); };
	constexpr auto normalize = math::normalize;
	struct  Line {
		math::Vector2f  point, direction;
	};
	inline float det(const Vector2& vector1, const Vector2& vector2)
	{
		return vector1.x() * vector2.y() - vector1.y() * vector2.x();
	}
	constexpr auto checknan = [](auto& a) {
		return !(a[0] == a[0]);
	};
	/*constexpr float operator*(const Vector2& vector1, const Vector2& vector2)
	{
		return math::dot(vector1, vector2);
	}*/
	bool linearProgram1(const std::vector<Line>& lines, size_t lineNo, float radius, const Vector2& optVelocity, bool directionOpt, Vector2& result);

	size_t linearProgram2(const std::vector<Line>& lines, float radius, const Vector2& optVelocity, bool directionOpt, Vector2& result);

	void linearProgram3(const std::vector<Line>& lines, size_t numObstLines, size_t beginLine, float radius, Vector2& result);
	struct AVOpara
	{
		bool directionOpt = false;
	};
	using LAVNe = Physics::LAVNe;
	/// <summary>
	/// 
	/// </summary>
	/// <param name="unit"></param>
	/// <param name="agentNeighbors_"></param>
	/// <param name="maxSpeed_"></param>
	/// <param name="prefVelocity_"></param>
	/// <param name=""></param>
	/// <param name="timeHorizon_:updates per sec"></param>
	/// <returns></returns>
	template<class T>
	auto computeNewVelocity(
		Vector2 Currvelocity_, T& agentNeighbors_,
		float maxSpeed_, const Vector2& prefVelocity_,
		AVOpara avoArg = {},
		const float timeHorizon_ = 50)
	{
		using Vector2 = math::Vector2f;
		thread_local std::vector<Line> orcaLines_;
		orcaLines_.clear();
		const float invTimeHorizon = 1.0f / timeHorizon_;
		//Vector2 Currvelocity_ = unit.collider.Velocity.head<2>();
		for (size_t i = 0; i < agentNeighbors_.size(); ++i) {
			LAVNe& other = agentNeighbors_[i];
			auto&& relativeVelocity = -other.relativeVelocity.head<2>();
			if (relativeVelocity.isConstant(0))
				continue;
			auto&& relativePosition = other.relativePosition.head<2>();
			const float distSq = other.distSq();// math::dot(relativePosition, relativePosition);
			const float combinedRadius = other.combinedRadius() + 0.2;
			const float combinedRadiusSq = math::square(combinedRadius);

			Line line;
			Vector2 u;

			if (distSq > combinedRadiusSq) {
				/* No collision. */
				const Vector2 w = relativeVelocity - invTimeHorizon * relativePosition;
				/* Vector from cutoff center to relative velocity. */
				const float wLengthSq = absSq(w);

				const float dotProduct1 = math::dot(w, relativePosition);

				if (dotProduct1 < 0 && sqr(dotProduct1) > combinedRadiusSq * wLengthSq) {
					/* Project on cut-off circle. */
					const float wLength = std::sqrt(wLengthSq);
					const Vector2 unitW = w / wLength;

					line.direction = Vector2(unitW.y(), -unitW.x());
					u = (combinedRadius * invTimeHorizon - wLength) * unitW;
				}
				else {
					/* Project on legs. */
					const float leg = std::sqrt(distSq - combinedRadiusSq);

					if (det(relativePosition, w) > 0) {
						/* Project on left leg. */
						line.direction = Vector2(relativePosition.x() * leg - relativePosition.y() * combinedRadius, relativePosition.x() * combinedRadius + relativePosition.y() * leg) / distSq;
					}
					else {
						/* Project on right leg. */
						line.direction = -Vector2(relativePosition.x() * leg + relativePosition.y() * combinedRadius, -relativePosition.x() * combinedRadius + relativePosition.y() * leg) / distSq;
					}

					const float dotProduct2 = math::dot(relativeVelocity, line.direction);

					u = dotProduct2 * line.direction - relativeVelocity;
				}
			}
			else {
				/* Collision. Project on cut-off circle of time timeStep. */
				const float invTimeStep = 1.0f / Physics::dt;

				/* Vector from cutoff center to relative velocity. */
				const Vector2 w = relativeVelocity - invTimeStep * relativePosition;

				const float wLength = w.norm();
				const Vector2 unitW = w / wLength;

				line.direction = Vector2(unitW.y(), -unitW.x());
				u = (combinedRadius * invTimeStep - wLength) * unitW;
			}

			line.point = Currvelocity_ + 0.5f * u;
			orcaLines_.push_back(line);
		}
		size_t lineFail = linearProgram2(orcaLines_, maxSpeed_, prefVelocity_, avoArg.directionOpt, Currvelocity_);
		if (lineFail < orcaLines_.size()) {
			linearProgram3(orcaLines_, 0, lineFail, maxSpeed_, Currvelocity_);
			assert(math::ckftvalid(Currvelocity_[0]));
		}
		/*if (checknan(Currvelocity_)) {
			LOGERROR("eer");
		}*/
		return Currvelocity_;
	}
}
namespace pj2::AI {
	inline auto LookAtDirection(const mtransform::v4& direction, mtransform& trans, Collid& cd)
	{
		constexpr bool fixz = true;
		using Scalar = mtransform::v4::Scalar;
		using Vector3 = mtransform::v3;
		assert(direction.norm() > 0.001f);
		{
			const auto cos = math::acos(direction.head<3>().normalized().dot(trans.froward()));
			auto adis = std::abs(cos);
			//attention £º do not swap cross direction
			if (adis > (cd.MaxAngularVeocity * Physics::dt))
				cd.AngularVelocity = ((trans.froward().cross(direction.head<3>())).normalized() * cd.MaxAngularVeocity);
			else
			{
				cd.AngularVelocity.setZero();
				adis = 0;
			}
			return adis;
		}
	}
	template<class ForwardIt, class T, class Compare>
	ForwardIt lower_bound(ForwardIt first, ForwardIt last, const T& value, Compare comp)
	{
		std::binary_search();
		ForwardIt it;
		typename std::iterator_traits<ForwardIt>::difference_type count, step;
		count = std::distance(first, last);

		while (count > 0) {
			it = first;
			step = count / 2;
			std::advance(it, step);
			if (comp(*it, value)) {
				first = ++it;
				count -= step + 1;
			}
			else
				count = step;
		}
		return first;
	}
	template <class _FwdIt, class _Ty, class _Pr>
	auto binary_search(_FwdIt _UFirst,const _FwdIt _ULast, const _Ty& _Val, _Pr _Pred) {
		// test if _Val equivalent to some element
		assert(_UFirst!= _ULast);
		_UFirst = lower_bound(_UFirst, _ULast, _Val, _Pass_fn(_Pred));
		return (_UFirst != _ULast && !_Pred(_Val, *_UFirst))?_UFirst:NULL;
	}
	//fun return friendly or not
	template<class F>
	inline auto
		steerForSeparation(const GameProps::Cmd0& ginfo_,
			const float& cosMaxAngle, const mtransfrom::v3 const& forward,
			Physics::Lavnes& flock, F&& fun, const float bufdis_ = 0.01f)
	{
		using GameProps::state;
		// steering accumulator and count of neighbors, both initially zero
		math::mtransform::v4 steering;
		steering.setConstant(0);
		int neighbors = 0;

		for (size_t i = 0; i < flock.size(); i++)
		{
			auto& flk = flock[i];
			auto& ocmd = flk.st;
			if (fun(flk, i))
				if (ginfo_.All(state::idle) || (ocmd!=(state::idle)
					//&&Boids::inBoidNeighborhood(flk, cosMaxAngle, forward)
					))
				{
					// add in steering contribution
					// (opposite of the offset direction, divided once by distance
					// to normalize, divided another time to get 1/d falloff)
					const auto dis = (flk.distSq() - (flk.combinedRadiussq() + bufdis_));
					steering += -(flk.relativePosition * math::exp(math::min((dis * -32), 10)));

					// count neighbors
					++neighbors;
				}
			else
			{
				// remove cllision speed vector
				const auto dis = (flk.distSq() - (flk.combinedRadiussq() + bufdis_));
				const auto& a = steering.head<3>(), & b = flk.relativePosition.head<3>();
				const auto vb = b.normalized();
				const auto& dr = a.dot(vb);
				if (Fix::fix_s (dr) > Fix::fix_s((int)0))
					steering.head<3>() -= (vb*dr);
				//Physics::RemoveVector(steering.head<3>(), flk.relativePosition.head<3>());
			}
		}

		return steering;
	}
}