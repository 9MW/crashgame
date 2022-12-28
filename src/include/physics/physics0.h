#pragma once

#include "common/Math1.h"
namespace pj2::Physics{
	static constexpr float dt = 0.02;
	using math::mtransform;
	inline void IntegrateLinearMotion(math::mtransform& motionData, const mtransform::v4 speed, const float Timestep = Physics::dt)
	{
			motionData.pos4 += speed * Timestep;
	}

	inline auto IntegrateVel(const mtransform::v4 pos4, const mtransform::v4 speed, const float Timestep = Physics::dt)
	{
		return pos4 + speed * Timestep;
	}
}