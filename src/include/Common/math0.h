#pragma once
#include "Eigen/Dense"

typedef signed char int8;
typedef signed short int16;
typedef signed int int32;
typedef unsigned char uint8;
typedef unsigned short uint16;
constexpr int Infinityi = std::numeric_limits<int>::max();

namespace math
{

	using namespace Eigen;
	using uint = uint32_t;
	using ui64 = uint64_t;
	template<typename T, uint size>
	using Vector = Eigen::Matrix<T, size, 1>;
	template<typename T>
	using vec4 = Vector<T, 4>;
	using v4f = vec4<float>;
	using Efloat2 = Eigen::Vector2f;
	using Efloat3 = Eigen::Vector3f;
	using Efloat4 = Eigen::Vector4f;
	//using float4x4 = Matrix4f;
	using int2 = Vector2i;
	using uint4 = Vector<uint, 4>;
	using int4 = Vector<int, 4>;
	using i64 = int64_t;
	using uint2 = Vector<uint, 2>;
	using f2 = math::Vector2f;
	using i4 = vec4<int>;
	//Eigen::vector
	//using v464 = Vector<int64_t, 4>;
	using quaternion = Eigen::Quaternionf;
	constexpr int percisioni = 1000;
	constexpr uint ui32max = std::numeric_limits<uint>::max();
	//constexpr uint ui32msax = -1;
	constexpr float Invpercisioni = (float)1 / percisioni;
	constexpr float Invpercisionisq = Invpercisioni * Invpercisioni;
	constexpr int iji = Invpercisioni * (500 * 590);
	constexpr const float Infinityf = INFINITY;
	static const Efloat2 Infinityf2 = Efloat2::Constant(Infinityf);
	constexpr float Epsilon = 1e-5f;
	static constexpr double PI = 3.14159265358979323846;
	static constexpr double PI2 = PI * 2;
	static constexpr float  PI_F = 3.1415927f;
	const Efloat3 Zero3f(0, 0, 0);
	const auto Epsilon1 = 1 + Epsilon;
	const auto Epsilon0 = 0 - Epsilon;
}