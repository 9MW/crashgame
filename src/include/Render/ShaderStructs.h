#pragma once

namespace sStruct {

	constexpr int MaxCpyStep = 2;
	//#include <AdvancedMath.hpp>
#define EV(num) using float##num=math::Efloat##num
	using namespace math;
	using float4x4 = math::Matrix4f;
	using float4 = math::Efloat4;
	using float4 = math::Efloat4;
	using uint4 = math::uint4;
	using uint2 = math::uint2;
	using int2 = math::int2;
	using int4 = math::vec4<int32>;
	constexpr int u2sz = sizeof(math::uint2);
	EV(3);
	EV(2);
#include <shaders\BasicStructures.fxh>
}