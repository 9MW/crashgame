#pragma once
#include "PCH.h"
#include "Eigen/Dense"
#include "CSP2CPP.h"
#include <cstring>
#include <cctype>
#include <cstdlib>
#include <cstdio>
#include<bitset>
//#include<bx/math.h>
#include <float.h>
#include <limits>
#include"Common/span.hpp"
#include <initializer_list>
#include "math0.h"
//#include"boost/align/aligned_allocator.hpp"

//#include <corecrt_math.h>
//struct Efloat2
//{
//    Vector2f v;
//    float& x = v[0], y = v[1];
//    float& operator[](int i) {
//        return v[i];
//    }
//   explicit operator Vector2f& () {
//        return v;
//    }
//};
//   float& PositiveInfinityf = (std::kInfinity);

namespace math
{
	inline float Dot(Efloat2& a, Efloat2& b) {
		return a.dot(b);
		i4 a1, a2;

	}
	inline auto Determinant2D = [](const auto& u, const auto& v)
	{
		return u[0] * v[1] - u[1] * v[0];
	};
	template<class T>
	inline auto DotSelf(T&& v) {
		return v.dot(v);
	}
	namespace Fiximp {
		struct fb
		{

		};
	}
	namespace Fix {
		//using f3 = i4;
		using s_fix = int;
		constexpr int bdk = 4;
		constexpr int percisioni = 1000;
		/*struct fix3 {
			Eigen::Vector4i data;
			fix3(frep&& d) :data(d) {}
			inline fix3 operator-(const fix3& t) {
				return fix3(data - t.data);
			}
		};*/
		
		struct fix_s
		{
			s_fix d;
			/*inline  operator int() {
				return (d);
			}
			explicit operator float() const {
				return (float)d * Invpercisioni;
			}*/

			inline float tof ()const { return (float)d * Invpercisioni; }
			inline operator s_fix&()  {
				return d;
			}
			fix_s(const fix_s& i) {
				*this = i;
			}
			fix_s() {}
			//fix_s():d(0) {}
			fix_s(const float& i) {
				d = i * percisioni;
			}
			inline void operator =(const float ii) {
				d = (ii * percisioni);
				//return *this;
			}
			inline void operator +=(const s_fix& ii) {
				d +=ii ;
				//return *this;
			}
			/*inline bool operator >(float ii) {
				return (d >(s_fix)(ii * percisioni));
			}*/
			inline auto max() { return fix_s(std::numeric_limits<s_fix>::max()); }
			constexpr bool operator >(const fix_s& ii) {
				return (d > (ii.d));
			}
			constexpr bool operator <(const fix_s& ii) {
				return (d < (ii.d));
			}
			//cast to float prevent overflow
			fix_s operator*(const fix_s t) {
				return (t.tof() * tof());
			}
			inline static fix_s make(s_fix&& i) {
				return fix_s(i);
			}
		private:
			inline fix_s(const s_fix& i) {
				this ->d= i;
			}
			/*fix_s& operator=(int& t) {
				return ((float)t * (float)(*this));
			}*/
			/*protected :
				fix_s(s_fix i) {
					d = i;
				}*/
		};
		inline fix_s& toFixs(int& v) {
			static_assert(sizeof(fix_s) == sizeof(int), "need readjust");
			return reinterpret_cast<fix_s&>(v);
		}
		constexpr bool operator <(const fix_s& ii, const fix_s& r) {
			return ii.d < (r.d);
		}
		inline bool operator <(const float& ii, const fix_s& f) {
			return fix_s(ii) < f;
		}
		/*inline bool operator -(const float& ii, const fix_s& f) {
			return (ii * percisioni) < f.d;
		}*/
		template<int sz, class T = int>
		struct fix :Fiximp::fb {
			using frep = fix<sz,T>;
			using t_self = fix<sz>;
			using d_self = Eigen::Vector<T, sz>;
			//std::conditional_t < EffLoc.size() < 4, fix4, fix4[2] > datas;
			//std::array<frep, sz / bdk + sz % bdk == 0 ? 0 : 1> data;
			Eigen::Vector<T, sz>data;
			//frep data;
			inline fix(frep&& d) {
				std::memcpy(this, &d, sizeof(*this));
			}
			inline fix(const frep& d) {
				std::memcpy(this, &d, sizeof(*this));
			}
			inline fix() {}

			//inline fix(d_self&& d) {
			//	data = f;// std::memcpy(this, &d, sizeof(*this));
			//}
			inline fix(const d_self& d) {
				data = d;// std::memcpy(this, &d, sizeof(*this));
			}
			template<int i>
			using ev = typename Eigen::Vector<float, i>;
			inline fix(const ev<4>& d) {
				//if constexpr(!std::is_base_of_v<Fiximp,ev>)
				data.template head<4>() = (d * percisioni).template cast<s_fix>();
				/*else {
				data.template head <ev::size()>() = d.data;
				}*/
				/*else {
					static_assert(false, "unexpect in");
				}*/

			}
			//using ev1 = Eigen::Vector3f;
			inline fix(ev<3>& d) {
				data.template head <3>() = (d * percisioni).template cast<s_fix>();
			}
			static constexpr int size() {
				return sz;
			}
			constexpr s_fix& operator[](const ui64 i) {
				assert(i < sz);
				return (data.data())[i];
			}
			inline t_self& operator=(const t_self& t) {
				std::memcpy(this, &t, sizeof(*this));
				return *this;
			}
			constexpr inline t_self operator-(const t_self& t) {
				/*t_self result;
				for (size_t i = 0; i < data.size(); i++)
				{
					result.data[i] = data[i] - t.data[i];
				}*/
				return t_self(data-t.data);
			}
			inline t_self operator+(const t_self& t) {
				return t_self(data + t.data);
			}
		};
		/*template<class T>
		struct fix<Eigen::Vector<T,sz>> {
			using ev = Eigen::Vector<T, sz>;
			using t_self = fix<sz>;
			frep data;
			inline fix(ev& d){
				data.template head <ev::size()>= d.template cast<s_fix>();
			}
			inline t_self operator-(const t_self& t) {
				return t_self(data - t.data);
			}
			inline t_self operator+(const t_self& t) {
				return t_self(data + t.data);
			}
		};*/
		using fix4 = fix<4>;
		using fix3 = fix<3>;
		constexpr auto One() { return percisioni; }
		inline fix4 toFix(const vec4<float> Tin) {
			Vector4i&& v4 = (Tin * percisioni).template cast<int>();
			return fix4(v4);
		}
		inline fix4::d_self::Scalar toFix(const float& Tin) {
			return (int)(Tin * percisioni);
		}
		inline void toFix(const vec4<float> Tin, Vector4i& v4) {
			 v4= (Tin * percisioni).template cast<int>();
		}
		inline vec4<float> toFloat(const vec4<s_fix>& Tin) {
			return Tin.cast<float>()* Invpercisioni;
		}
		inline float toFloat(const s_fix& Tin) {
			return Tin * Invpercisioni;
		}
		inline s_fix FixDot(fix3& a, fix3& b) {
			return (a.data.cwiseProduct(b.data).template cast<float>().template head<3>().array() * Invpercisioni).prod();
		}
		inline auto FixDot3(v4f& a, v4f& b) {
			return fix_s((a.cwiseProduct(b).template cast<float>().template head<3>().array() * Invpercisioni).prod());
		}
	}
	//template<class T,class T1>
	/*inline vec3<int> toFix(const vec3<float> Tin) {
		return (Tin * percisioni).cast<int>();
	}*/
	using namespace std;

	class v464 {
		using scalar = int64_t;
		using v4rep = vec4<scalar>;
		static constexpr scalar percision = 1000000;
		static constexpr float percisionInv = (float)1 / 1000000;
		v4rep V4;
		constexpr v4rep& ToVector() {
			return V4;
		}
	public:
		v464(Eigen::Vector4f ag) {
			ag *= percision;
			V4 = ag.cast<scalar>();
		}
		Eigen::Vector3f ToV3f() {
			return ((V4).cast<float>() * percisionInv).template head<3>();
		}
		v464(Eigen::Vector3f ag) {
			ag *= percision;
			V4.template head<3>() = ag.cast<scalar>();
		}
		float dissqTo(const Eigen::Vector3f p) {
			return (p - ToV3f()).squaredNorm();
		}
		bool Approx(const Eigen::Vector3f p, scalar maxdis = 1) {
			return (v464(p).ToVector().template head<3>() - V4.template head<3>()).template head<3>().squaredNorm() <= 1;
		}
		scalar toScalar(float v) {
			return (scalar)(v * percision);
		}
		scalar operator[](int i) {
			return V4[i];
		}
		void operator =(Eigen::Vector4f ag) {
			ag *= percision;
			V4 = ag.cast<scalar>();
			int f = std::sqrt(19);
		}

		//Eigen::Vector3f operator =(v464 v) {
		//	//ag *= percision;
		//	Eigen::Vector3f rv = v.to.cast<float>();
		//	
		//	return rv;
		//}
	};
	template <typename T>
	static auto dot2(T& a, T& b)
	{
		var c = a - b;
		return a.dot(b);
	}
	inline Eigen::Ref<Vector3f> tof3(v4f& a) { return a.head<3>(); }
	/*Efloat2 mul(quaternion& q, float3& v)
	{
		var t = 2 * cross(q.value.xyz, v);
		return v + q.value.w * t + cross(q.value.xyz, t);
	}*/
	static const auto disR = [](const auto x, const auto y) { return  x > y ? x - y : y - x; };
	static const auto normalized = [](const auto x) { return  x.normalized(); };
	template <typename T>
	inline var any(const T& a)
	{

		return a.any();
	}
	/*template<class T>
	struct DicRes
	{
		T quot, rem;
		DicRes(T q, T r) :quot(q), rem(r) {}
	};
	constexpr auto div = [](auto&& x, auto&& y) {return DicRes(x / y, x % y); };
	inline var sqrt(float v)
	{
		return std::sqrt(v);
	}*/

	const var CantorPloynomialUnOrdered = [](var const& a, var const& b) {
		const var apb = a + b;
		return apb * apb + a * b;
	};

	static const  auto Contain = [](auto& a, auto& dis_, auto& v)
	{
		return v < a + dis_ && v >= a;
	};
	template <typename T>
	constexpr inline void set(T& a, float b)
	{
		T::Constant(b);
	}
	template<typename Ty>
	inline constexpr Ty Clamp01(const Ty& _a)
	{
		return std::max(std::min(_a, (Ty)1), (Ty)0);
	}
	constexpr inline float min(float a, float b)
	{
		return std::min(a, b);
	}
	template<class T> inline T interpolate(float alpha, const T& x0, const T& x1)
	{
		return x0 + ((x1 - x0) * alpha);
	}
	static const auto ckftvalid = [](const auto f) {
		return !(std::isnan(f) || std::isinf(f));
	};
	template <typename T, class T1>
	inline auto rotate90(T const& ffj, T1& result_) {
		result_[0] = ffj[1];
		result_[1] = -ffj[0];
	}
	/*template <typename T>
	constexpr inline T& min(T& a, T& b)
	{
		return T::min(a, b);
	}*/
	/*constexpr  auto min = [](auto& a, auto& b) {
		return std::min(a, b);
	};*/
	const auto dot = [](const auto& a, const auto& b) { return a.dot(b); };
	static const auto doth3 = [](const auto& a, const auto& b) { return a.template head<3>().dot(b.template head<3>()); };
	inline auto cross(const Efloat3& a, const Efloat3& b) {
		return a.cross(b);
	};

	const auto distancesq = [](auto& a, auto& b) { const var m = a - b; return m.dot(m); };
	template<class T, typename t_in = Eigen::Vector<T, 4>>
	constexpr auto distancesq3(const t_in& a, const t_in& b)
	{
		const auto m = a - b;
		return m.cwiseProduct(m).template head<3>().prod();
	}

	const auto normalize = [](auto& a) { return a.normalized(); };
	//template<std::size_t n>
	static const auto Hd3 = [](auto& a) { return a.template head<3>(); };
	static const auto square = [](const auto a) { return a * a; };
	static const auto truncate = [](auto&& vec_a, const auto& mag) {
		const auto norm = vec_a.norm();
		if (norm > mag) {
			vec_a = vec_a * mag / norm;
		}
	};
	static const auto sqnorm = [](const auto a) { return a.squaredNorm(); };
	static const auto RadToDeg = [](const auto& a) {
		return a / (2 * PI) * 360;
	};
	static const auto DegToRad = [](const auto& a) {
		return  a * (PI / 180);
	};
	static const auto AngleTo = [](const auto& source, const auto& dest)
	{
		if (source == dest) {
			return 0;
		}
		//double dot; Vector3.Dot(ref source, ref dest, out dot);
		return std::acoshf(dot(source, dest));
	};
	static const auto SignedAngleTo = [](const auto& source, const auto& dest)
	{
		const auto angle = source.AngleTo(dest);
		const auto cross = source.cross3(dest);
		//double dot; Vector3.Dot(ref cross, ref planeNormal, out dot);
		return dot(cross, Vector3f(0, 0, 1)) < 0 ? -angle : angle;
	};

	static const auto AngleTim = [](const auto& source, const auto& dest, const auto& aspeed)
	{
		const auto adis = AngleTo(source, dest);
		return adis / aspeed;
	};
	//constexpr auto min = [](auto& a, auto& b) {return std::min(a, b); };
	/*inline  auto mul(const quaternion& a, const Efloat2& b){
		return b * a;
	}*/
	//inline	Efloat2 mulf2q(const Efloat2& v, const quaternion& q)
	//{
	//	var t = 2 * Efloat2(q.z() * v.y(), -q.z() * v.x());// cross(q.value.xyz, v);
	//	return v + q.w() * t + Efloat2(q.z() * t.y(), -q.z() * t.x());
	//}
	//inline Efloat2 mulf2q(const Efloat2& v, const quaternion& q)
	//{
	//	var t = 2 * Efloat2(-q.z() * v.y(), q.z() * v.x()); // cross(q.value.xyz, v);
	//	return v + q.w() * t + Efloat2(-q.z() * t.y(), q.z() * t.x());
	//}
	inline Efloat2 mulf2q(const Efloat2& v, const quaternion& q)
	{
		const var t = 2 * Efloat2(-q.z() * v.y(), q.z() * v.x()); // cross(q.value.xyz, v);
		return v + q.coeffs()[3] * t + Efloat2(-q.z() * t.y(), q.z() * t.x());
	}
	inline Efloat2 mulf2q(const quaternion& q, const Efloat2& v)
	{
		return mulf2q(v, q);
	}

	template <typename T>
	inline T ProjectUnrestrict(const T& targetpoint, const T& b, const T& c, float& percent)
	{
		var cb = b - c;
		T ca = targetpoint - c;
		var dot = math::dot(cb, ca);
		//var projleg = dot / cb.magnitude;
		percent = dot / cb.squaredNorm();
		//percent = dot /(cb.x()*cb.x()+cb.y()*cb.y());
		ca = c + percent * cb;
		// cb = c + cb.normalized * projleg;
		return ca; //power by 2 is due to we want ratio of cb length
	}
	template <typename T>
	inline T ProjectClamp(const T& targetpoint, const T& b, const T& c, float& percent)
	{
		var cb = b - c;
		T ca = targetpoint - c;
		var dot = math::dot(cb, ca);
		//var projleg = dot / cb.magnitude;
		percent = dot / cb.squaredNorm();

		ca = c + std::clamp<float>(percent, 0, 1) * cb;
		// cb = c + cb.normalized * projleg;
		return ca; //power by 2 is due to we want ratio of cb length
	}
	struct mtransform
	{
		using v3 = math::Efloat3;
		using v2 = math::Vector2f;
		using v4 = math::Efloat4;
		using v4i = math::int4;
		using spnum = v3::Scalar;
		static constexpr float Maxv4s = std::numeric_limits<float>::max();
		union
		{
			v4 pos4;
			v3 pos;
		};

		union
		{
			v3 Scale;
			v4 Scale4;
		};
		math::quaternion rotation;
		mtransform(const mtransform& hh) {
			this->pos = hh.pos;
			this->Scale4 = hh.Scale4;
			this->rotation = hh.rotation;
		}
		mtransform() {
			rotation.setIdentity();
			pos.setIdentity();
			Scale.setConstant(1);
		};
		mtransform(float a, float s, float d) {
			pos = { a,s,d };
			rotation.setIdentity();
			Scale = { 1,1,1 };
		}
		mtransform(const v3 pos3) {
			pos4.head<3>() = pos3;
			pos4[3] = 1;
			rotation.setIdentity();
			Scale = { 1,1,1 };
		}
		template<class T>
		static inline mtransform Build(T&& pos) {
			return mtransform(pos.cast<v4::Scalar>());
		}
		void reset(const v3 pos) {
			this->pos = pos;
			rotation.setIdentity();
			Scale = { 1,1,1 };
		}
		void reset(const v4i pos) {
			this->pos4 = pos.cast<float>();
			rotation.setIdentity();
			Scale = { 1,1,1 };
		}
		inline const v3 froward() {
			return rotation * v3(0, 1, 0);
		}
		mtransform(const Affine3f af) {
			Eigen::Matrix3f mr, ms;
			af.computeRotationScaling(&mr, &ms);
			pos = af.translation();
			rotation = mr;
			Scale = ms.diagonal().template head<3>();
		}

		void set(const Affine3f af) {
			new (this)mtransform(af);
		}
		template<typename T>
		inline v2 Trans2dIv(const T tg)
		{
			var t = tg - pos.head<2>();
			return  math::mulf2q(t, rotation.inverse());
		}
		template<typename T>
		inline const v2 Trans2dIv(const T tg)const
		{
			var t = tg - pos.head<2>();
			return  math::mulf2q(t, rotation.inverse());
		}
		mtransform& operator=(const mtransform& hh) {
			this->pos = hh.pos;
			this->Scale4 = hh.Scale4;
			this->rotation = hh.rotation;
			return *this;
		}
		mtransform(v3 pos, math::quaternion b) :pos(pos), rotation(b) {}
		inline auto toworld(const v4& point) { return (rotation * point.cwiseProduct(Scale4).head<3>()) + pos; }
		//inline auto toworld(const v3& point) { return (rotation * point.cwiseProduct(Scale4).head<3>()) + pos; }
		/*var ToString() {
			auto p1 = pos[0];
			return fmt::format("pos({0}),rotation{1}", pos.transpose(), FmQuar(rotation));
		}*/
		/* void ToString()
		{
			LOGINFO("pos={0} rotation={1}", pos, rotation);
		}*/
	};
} // namespace math
namespace Utility
{
	using sz = std::size_t;
	/*template<typename T, std::size_t Alignment>
	using aligned_alloc = boost::alignment::aligned_allocator<T, Alignment>;*/
	template<class T>
	constexpr auto AddRelaxed(T& t, const int r) {
		return t.fetch_add(r, std::memory_order_relaxed);
	}
	/*inline math::quaternion IntegrateAngularVelocity(math::Efloat3 angularVelocity, float timestep)
	{

		math::Efloat3 halfDeltaAngle = angularVelocity * timestep * 0.5f;
		math::quaternion q;
		q.w() = 1;
		q.vec() = halfDeltaAngle;
		return q;
	}*/

	template<class T>
	using span = nonstd::span<T>;
	template<class T>
	auto makespan(nonstd::span<T> ds) { return ds; }
	inline void appendTo(span<char> res,  char const* array2) {
		char* newArray = res.data();
		auto l2 = std::strlen(array2);
		strcat_s(newArray, l2, array2);
	}
	inline void append(span<char> res,  char const* array1,  char const* array2) {
		char* newArray = res.data();
		strcpy_s(newArray, std::strlen(array1), array1);
		appendTo(res, array2);
	}
	inline const char* append(char const* array1,  char const* array2) {
		std::string s1(array1), s2(array2);
		return (s1 + s2).c_str();
		/*auto l2 = std::strlen(array2);
		auto newsize = std::strlen(array1) + l2 + 1;
		char* newArray = new char[newsize];
		append(span<char>(newArray, newsize), array1, array2);
		return newArray;*/
	}
	/*template<class T0, class T1>
	constexpr void EleSet(T0& t, const T1& r) {
		t.head<T1::SizeAtCompileTime>() = r.cast<T0::Scalar>();
	}*/
	using namespace math;
	using namespace nonstd;
	using Efloat2 = math::Efloat2;
	template<int size = 4, typename T = int>
	struct SimdNums {
		using Scalar = T;
		using V4T = math::vec4<T>;
		static constexpr int sz = 4;
#define PJ2_EIGEN_OP(op)									  \
		constexpr t_self operator op(const t_self d) {	  \
		t_self res;									  \
		for (size_t i = 0; i < size / 4; i++)	  \
		{res[i] = data[i] op d.data[i];}			  \
		return res;}
		using t_self = SimdNums<size, T>;
		static_assert(
			!std::is_empty<T>::value,
			"T should be n non-empty class"
			);
		static_assert(
			!std::is_empty<T>::value,
			"size must be multiple of 4"
			);
		std::array<V4T, (math::uint)size / sz> data;
		constexpr t_self operator +(const t_self d) {
			t_self res;
			for (size_t i = 0; i < size / 4; i++)
			{
				res[i] = data[i] + d.data[i];
			}

			return res;
		}
		constexpr void operator +=(const t_self d) {
			for (size_t i = 0; i < data.size(); i++)
			{
				data[i] += d.data[i];
			}
		}
		template<typename T>
		constexpr void Loop(T&& func) {
			for (auto&& i : data)
			{
				func(i);
			}
		}

		template<typename T>
		constexpr void BinaryOp(T&& func, t_self& a, t_self& b) {
			for (size_t i = 0; i < data.size(); i++)
			{
				func(a.data[i], b.data[i]);
			}
		}
		template<typename T>
		constexpr void BinaryOp(T&& func,t_self& b) {
			BinaryOp(std::forward<T>(func), *this, b);
		}
		/*inline auto ptr0() {
			return data[0].data();
		}*/
		template<int I>
		constexpr auto& get() {
			return data[I / sz][I % sz];
		}
		Scalar& operator[](int I) {
			return data[I / sz][I % sz];
		}
		SimdNums() {

			Loop([](auto& v) {
				v.setConstant(-1);
				});
		}
		SimdNums(int i) {

			Loop([](auto& v) {
				v.setConstant(0);
				});
		}
		/*constexpr void operator ++(const Tt d) {
			for (size_t i = 0; i < size / 4; i++)
			{
				math::vec4<T>& d = data[i];
				d+=math::vec4<T>::Ones();
			}
		}
		constexpr void operator --(const Tt d) {
			for (size_t i = 0; i < size / 4; i++)
			{
				math::vec4<T>& d = data[i];
				d -= math::vec4<T>::Ones();
			}
		}*/
		constexpr bool AnyPositive() {

			for (size_t i = 0; i < size / 4; i++)
			{
				math::vec4<T>& d = data[i];
				if ((d > 0).any())
					return true;
			}
			return false;
		}
		void each() {}
		PJ2_EIGEN_OP(*)
			PJ2_EIGEN_OP(*= )
			//#undef OP;
	};

	/// <summary>
	/// a smaller than b
	/// </summary>
	template<int bitwidth = 128>
	struct alignas(16) bitmark
	{
		using t = unsigned long  long;
		using self = bitmark<bitwidth>;
		//t  a, b;
		union
		{
			struct {
				t a, b;
			};
			std::array<t, bitwidth / 64> data;
		};
		static constexpr size_t size = bitwidth / 8;

		bitmark(const t  a = 0, const t  b = 0) {
			if constexpr (bitwidth >= 128) {
				std::memset(this, 0, sizeof(data));
				auto& ad = data[0];
				auto& bd = data[1];
				ad = a;
				bd = b;
			}
		}
		bitmark(const std::initializer_list<uint> l0) :a(0), b(0) {
			auto& a = data[0];
			auto& b = data[1];
			for (const auto bit : l0)
			{
				if (bit >= bitwidth)
					a |= ((t)1) << (bit - bitwidth);
				else
				{
					b |= ((t)1) << (bit);
				}
			}
		}
		/*template <int ... Is, class T>
		constexpr void Loop( std::integer_sequence<int, Is...> const&, T&& f)
		{
			f(Is);
		}*/
		template < class T, int ... ins>
		constexpr void Loop(T&& f, std::integer_sequence<int, ins...> const&)
		{
			((f(ins)), ...);
		}
		bitmark operator |(const self lf) const {
			if constexpr (bitwidth == 128)
				return bitmark(lf.a | a, lf.b | b);
			else {
				self se;
				for (size_t i = 0; i < data.size(); i++)
				{
					se.data[i] = data[i] | lf.data[i];
				}
				return se;
			}
		}

		void operator |=(const self lf) {
			if constexpr (bitwidth == 128) {
				a |= lf.a;
				b |= lf.b;
			}
			else {
				self se;
				for (size_t i = 0; i < data.size(); i++)
				{
					data[i] |= lf.data[i];
				}
			}
		}
		bitmark operator &(const self lf) const {
			if constexpr (bitwidth == 128)
				return bitmark(lf.a & a, lf.b & b);
			else {
				self se;
				se.Loop(
					[&]() {
						for (size_t i = 0; i < data.size(); i++)
						{
							se.data[i] = data[i] & lf.data[i];
						}
					},
					std::make_integer_sequence<int, size>{});
				return se;
			}
		}

		bool AndAny(const self lf) const {
			if constexpr (bitwidth == 128)
				return (lf.a & a) > 0 || (lf.b & b) > 0;
			else {
				self se;
				se.Loop(
					[&]() {
						for (size_t i = 0; i < data.size(); i++)
						{
							if (data[i] & lf.data[i] > 0)return true;
						}
					},
					std::make_integer_sequence<int, size>{});
				return false;
			}
		}
		bitmark operator ^(const self lf) const {
			if constexpr (bitwidth == 128)
				return bitmark(lf.a ^ a, lf.b ^ b);
			else {
				self se;
				se.Loop(
					[&]() {
						for (size_t i = 0; i < data.size(); i++)
						{
							se.data[i] = data[i] ^ lf.data[i];
						}
					},
					std::make_integer_sequence<int, size>{});
				return se;
			}
		}
		constexpr bool Is0() const {
			return a == b == 0;
		}

		//inline void* data() { reinterpret_cast<void*>(this); }
	   /*operator uint ()const {
		   se.Loop<size>(
			   std::make_integer_sequence<int, size>{},
			   [&]() {
				   for (size_t i = 0; i < data.size(); i++)
				   {
					   se.data[i] = data[i] | lf.data[i];
				   }
			   });
		   for (uint i = 0; i < 64; i++)
		   {
			   if (a & (((t)1) << (i)) > 0)
				   return i;
		   }
		   for (uint i = 0; i < 64; i++)
		   {
			   if (b & (((t)1) << (i)) > 0)
				   return i + 64;
		   }
		   std::abort();
	   }*/
		bool operator ==(const self op) {
			return std::memcmp(this, &op, size);
		}
		/*void operator =(const uint op) {
			if (op >= bitwidth) {
				a& (((t)1) << (i));
			}
		}*/
		template<int id>
		constexpr void Set() {
			static_assert(id < bitwidth * 2, "id Too big");
			std::memset(this, 0, sizeof(self));
			if (id < bitwidth) {
				a &= (((t)1) << (id));
			}
			else {
				b &= (((t)1) << (id - bitwidth));
			}
		}
		/*static gp128 Get() {
			static uint count = 0;
			assert(count < 128);
			return gp128({ count++ });
		}*/
	};
	using bit128 = std::bitset<128>; //bitmark<>;
	using Gbit = std::bitset<64>;
	inline constexpr float CantorPolynomial(float a, float b)
	{
		return ((((a + b) * (a + b) + 3 * a + b) * 0.5));
	}
	inline float CantorPolynomialOrderLess(float a, float b)
	{
		return ((a + b) * (a + b) + a * b);
	}
	template <typename T>
	inline T Rotate90(T& t)
	{
		T v = t;
		v.x() = t.y();
		v.y() = -t.x();

		return v;
	}
	template <typename T>
	constexpr void Rotateself90(T& t)
	{
		t = Rotate90(t);
	}
	//template <typename T>
	//inline void discreteSpiral(T&& func,const uint NUMBER_OF_POINTS ) {
	//	using v3i = math::Vector3i;
	//	// a vector - direction in which we move right now
	//	v3i step = { 1,0,0 }, cur = {0,0,0};
	//	// length of current segment
	//	int segment_length = 1;
	//	func(cur);
	//	int segment_passed = 0;
	//	for (int k = 0; k < NUMBER_OF_POINTS; ++k) {
	//		// make a step, add 'direction' vector (di, dj) to current position (i, j)
	//		
	//		cur += step;
	//		++segment_passed;
	//		func(cur);
	//		if (segment_passed == segment_length) {
	//			// done with current segment
	//			segment_passed = 0;

	//			// 'rotate' directions
	//			Rotate90(step);

	//			// increase segment length if necessary
	//			if (step[1] == 0) {
	//				++segment_length;
	//			}
	//		}
	//	}
	//}
	template <typename T>
	inline void discreteSpiral(T&& func, const uint NUMBER_OF_POINTS) {
		using v3i = math::Vector3i;
		// a vector - direction in which we move right now
		v3i step = { 1,0,0 }, cur = { 0,0,0 };
		// length of current segment
		int segment_length = 1;
		func(cur, 0);
		int segment_passed = 0;
		for (int k = 1; k < NUMBER_OF_POINTS; ++k) {
			// make a step, add 'direction' vector (di, dj) to current position (i, j)

			cur += step;
			++segment_passed;
			func(cur, k);
			if (segment_passed == segment_length) {
				// done with current segment
				segment_passed = 0;

				// 'rotate' directions
				Rotateself90(step);

				// increase segment length if necessary
				if (step[1] == 0) {
					++segment_length;
				}
			}
		}
	}

	template <typename T, typename T1>
	inline void CirSpiral(T&& func, T1&& turnfunc, const uint NUMBER_OF_POINTS) {
		using v3i = math::Vector3i;
		// a vector - direction in which we move right now
		v3i step = { 1,0,0 }, cur = { 0,0,0 };
		// length of current segment
		int segment_length = 1;
		func(cur, 0);
		int segment_passed = 0;
		for (int k = 1; k < NUMBER_OF_POINTS; ++k) {
			// make a step, add 'direction' vector (di, dj) to current position (i, j)

			cur += step;
			++segment_passed;
			func(cur, k);
			if (segment_passed == segment_length) {
				// done with current segment
				segment_passed = 0;
				do {
					// 'rotate' directions
					Rotateself90(step);

					if (step[1] == 0) {
						++segment_length;
					}
				} while (!turnfunc(cur, step, segment_length));
				//auto nex=cur +step;
				//if(nex)
				// increase segment length if necessary
			}
		}
	}
	inline float Determinant(const math::Efloat2& u, const Efloat2& v)
	{

		Matrix2f n;
		n << u, v;
		return n.determinant();
		/*  var uv = u(1,0) * v(2.0);
		  var vu = v(1,0) * u(2.0);
		  var re = uv - vu;
		  return re;*/
	}
	template <typename T>
	struct BlobAssetReference
	{
		struct
		{
			T v;
			operator const T& () {
				return &v;
			}

		} Value;
	};
	//inline void DiscreteLine(const math::Efloat4 line, const math::Efloat2 cell, std::vector<math::int2>& res) {
	//	/*const auto  s = line.head<2>().cast<int>();
	//	const auto d = line.tail(2).cast<int>();*/
	//	math::Efloat4 ff;
	//	if (line.head<2>() == line.tail<2>())
	//		return;
	//	ff.head<2>() = cell;
	//	ff.tail(2) = cell;

	//	auto ds = line.cwiseQuotient(ff).array().round().cast<int>();
	//	const auto  s = line.head<2>();
	//	const auto d = line.tail(2);
	//	auto voxelDistance = (ds.head<2>() - ds.tail(2)).cwiseAbs().array().sum() + 1;
	//	//	// This id of the first/current voxel hit by the ray.
	// //// Using floor (round down) is actually very important,
	// //// the implicit int-casting will round up for negative numbers.
	//	Eigen::Vector2i current_voxel = (s.cwiseQuotient(cell)).array().round().cast<int>();

	//	// The id of the last voxel hit by the ray.
	//	// TODO: what happens if the end point is on a border?
	//	Eigen::Vector2i last_voxel = (d.cwiseQuotient(cell)).array().round().cast<int>();

	//	// Compute normalized ray direction.
	//	auto ray = d - s;
	//	//ray.normalize();

	//	// In which direction the voxel ids are incremented.
	//	double stepX = (ray[0] >= 0) ? 1 : -1; // correct
	//	double stepY = (ray[1] >= 0) ? 1 : -1; // correct

	//	// Distance along the ray to the next voxel border from the current position (tMaxX, tMaxY, tMaxZ).
	//	double next_voxel_boundary_x = (current_voxel[0] + stepX) * cell[0]; // correct
	//	double next_voxel_boundary_y = (current_voxel[1] + stepY) * cell[1]; // correct

	//	// tMaxX, tMaxY, tMaxZ -- distance until next intersection with voxel-border
	//	// the value of t at which the ray crosses the first vertical voxel boundary
	//	double tMaxX = (ray[0] != 0) ? (next_voxel_boundary_x - s[0]) / ray[0] : DBL_MAX; //
	//	double tMaxY = (ray[1] != 0) ? (next_voxel_boundary_y - s[1]) / ray[1] : DBL_MAX; //

	//	// tDeltaX, tDeltaY, tDeltaZ --
	//	// how far along the ray we must move for the horizontal component to equal the width of a voxel
	//	// the direction in which we traverse the grid
	//	// can only be FLT_MAX if we never go in that direction
	//	double tDeltaX = (ray[0] != 0) ? cell[0] / ray[0] * stepX : DBL_MAX;
	//	double tDeltaY = (ray[1] != 0) ? cell[1] / ray[1] * stepY : DBL_MAX;

	//	//if (current_voxel[2] != last_voxel[2] && ray[2] < 0) { diff[2]--; neg_ray = true; }
	//	res.push_back(current_voxel);
	//	int traversedVoxelCount = 0;
	//	while (++traversedVoxelCount < voxelDistance) {
	//		if (tMaxX < tMaxY) {
	//			current_voxel[0] += stepX;
	//			tMaxX += tDeltaX;
	//		}
	//		else {
	//			current_voxel[1] += stepY;
	//			tMaxY += tDeltaY;
	//		}
	//		res.push_back(current_voxel);
	//	}
	//}
	template<typename T>
	inline void DiscreteLine(const math::Efloat4 line, const math::Efloat2 cell, const T&& quiry) {
		/*const auto  s = line.head<2>().cast<int>();
		const auto d = line.tail(2).cast<int>();*/
		math::Efloat4 ff;
		if (line.head<2>() == line.tail(2))
			return;
		ff.head<2>() = cell;
		ff.tail(2) = cell;

		auto ds = line.cwiseQuotient(ff).array().round().cast<int>();
		const auto  s = line.head<2>();
		const auto d = line.tail(2);
		auto voxelDistance = (ds.head<2>() - ds.tail(2)).cwiseAbs().array().sum() + 1;
		//	// This id of the first/current voxel hit by the ray.
	 //// Using floor (round down) is actually very important,
	 //// the implicit int-casting will round up for negative numbers.
		Eigen::Vector2i current_voxel = (s.cwiseQuotient(cell)).array().round().cast<int>();

		// The id of the last voxel hit by the ray.
		// TODO: what happens if the end point is on a border?
		Eigen::Vector2i last_voxel = (d.cwiseQuotient(cell)).array().round().cast<int>();

		// Compute normalized ray direction.
		auto ray = d - s;
		//ray.normalize();

		// In which direction the voxel ids are incremented.
		double stepX = (ray[0] >= 0) ? 1 : -1; // correct
		double stepY = (ray[1] >= 0) ? 1 : -1; // correct

		// Distance along the ray to the next voxel border from the current position (tMaxX, tMaxY, tMaxZ).
		double next_voxel_boundary_x = (current_voxel[0] + stepX) * cell[0]; // correct
		double next_voxel_boundary_y = (current_voxel[1] + stepY) * cell[1]; // correct

		// tMaxX, tMaxY, tMaxZ -- distance until next intersection with voxel-border
		// the value of t at which the ray crosses the first vertical voxel boundary
		double tMaxX = (ray[0] != 0) ? (next_voxel_boundary_x - s[0]) / ray[0] : DBL_MAX; //
		double tMaxY = (ray[1] != 0) ? (next_voxel_boundary_y - s[1]) / ray[1] : DBL_MAX; //

		// tDeltaX, tDeltaY, tDeltaZ --
		// how far along the ray we must move for the horizontal component to equal the width of a voxel
		// the direction in which we traverse the grid
		// can only be FLT_MAX if we never go in that direction
		double tDeltaX = (ray[0] != 0) ? cell[0] / ray[0] * stepX : DBL_MAX;
		double tDeltaY = (ray[1] != 0) ? cell[1] / ray[1] * stepY : DBL_MAX;

		//if (current_voxel[2] != last_voxel[2] && ray[2] < 0) { diff[2]--; neg_ray = true; }
		if (quiry(current_voxel))
			return;
		int traversedVoxelCount = 0;
		while (++traversedVoxelCount < voxelDistance) {
			if (tMaxX < tMaxY) {
				current_voxel[0] += stepX;
				tMaxX += tDeltaX;
			}
			else {
				current_voxel[1] += stepY;
				tMaxY += tDeltaY;
			}
			if (quiry(current_voxel))
				return;
		}
	}

	inline void plot4points(std::vector<math::int2>& buffer, int cx, int cy, int x, int y)
	{
		int cy0 = cy + y;
		for (auto i = cx - x; i < cx + x + 1; i++)
		{
			buffer.emplace_back(i, cy0);
		}
		if (y != 0) {
			cy0 = cy - y;
			for (auto i = cx - x; i < cx + x + 1; i++)
			{
				buffer.emplace_back(i, cy0);
			}
		}
	}

	template<class T>
	inline void circle(const T buffer, int cx, int cy, int radius)
	{
		int error = -radius;
		int x = radius;
		int y = 0;

		while (x >= y)
		{
			int lastY = y;

			error += y;
			++y;
			error += y;

			int cy0 = cy + lastY;
			for (auto i = cx - x; i < cx + x + 1; i++)
			{
				buffer(i, cy0);
			}
			if (lastY != 0) {
				cy0 = cy - lastY;
				for (auto i = cx - x; i < cx + x + 1; i++)
				{
					buffer(i, cy0);
				}
			}
			if (error >= 0)
			{
				if (x != lastY) {
					cy0 = cy + x;
					for (auto i = cx - lastY; i < cx + lastY + 1; i++)
					{
						buffer(i, cy0);
					}
					if (x != 0) {
						cy0 = cy - x;
						for (auto i = cx - lastY; i < cx + lastY + 1; i++)
						{
							buffer(i, cy0);
						}
					}
				}

				error -= x;
				--x;
				error -= x;
			}
		}
	}
	const auto VoxelBox2d = [](auto bx, auto dis, const auto fun) {
		auto  xel = bx.Min;
		{
			var& lf = bx.Min;
			var& tr = bx.Max;
			for (auto i = lf[0]; i <= tr[0];)
			{
				xel[0] = i;
				for (auto p = lf[1]; p <= tr[1]; )
				{
					xel[1] = p;
					if (fun(xel)) {
						return;
					}
					p += dis[1];
				}
				i += dis[0];
			}
		}
	};
	template<class T>
	span<T> ff((T*)NULL, 0);
	template<class T, class T2>
	inline void circle(span<T> re, const T2 pos, float radius) {

		constexpr auto PI_F = math::PI_F;
		for (auto ctr = 0; ctr < re.size(); )
		{
			//convert degree to rads
			auto degree = ((float)ctr / re.size()) * (2 * PI_F);

			// work out circle point co-ordinates
			re[ctr][0] = pos[0] + (radius * sin(degree));
			re[ctr][1] = pos[1] - (radius * cos(degree));
			re[ctr][3] = 0;
			//re[ctr] = re[ctr-1];
			ctr++;
			degree = ((float)ctr / re.size()) * (2 * PI_F);

			// work out circle point co-ordinates
			re[ctr][0] = pos[0] + (radius * sin(degree));
			re[ctr][1] = pos[1] - (radius * cos(degree));
			re[ctr][3] = 0;
			//ctr++;
			re[ctr + 1] = re[ctr];
			ctr += 2;
		}
	}
	inline bool Approch0(float a)
	{
		return std::abs(a) < Epsilon;
	}
	inline bool doIntersect(const Efloat2& a, const Efloat2& b, const Efloat2& c, const Efloat2& d,
		bool& para, Efloat2& intersection, float MinEndPointVec, Efloat2& ab, Efloat2& cd, Efloat2& ac, Efloat2& normal)
	{
		ab = b - a;
		cd = d - c;
		ac = c - a;
		float delta = Determinant(ab, cd);
		intersection = INFINITY * Efloat2(1, 1);
		//bool std = b3(1,0);
		if (Approch0(delta)) // delta=0，表示两线段重合或平行
		{
			para = true;
			return false;
		}
		para = false;
		float namenda = Determinant(ac, ab) / delta;
		if (namenda > 1 || namenda < 0)
		{
			return false;
		}

		float miu = Determinant(ac, cd) / delta;

		if (miu > 1 || miu < 0)
		{
			return false;
		}
		intersection = a + miu * ab;
		normal(0, 0);

		Efloat2 dv;
		var Pab = Rotate90(ab);
		var Pcd = Rotate90(cd);
		if (namenda > 0.5)
		{
			dv = d - intersection;
		}
		else
		{
			dv = c - intersection;
		}
		MinEndPointVec = std::abs(Pab.dot(dv));
		if (miu > 0.5)
		{

			dv = b - intersection;
			float Dab = std::abs(Pab.dot(dv));

			if (MinEndPointVec < Dab)
			{
				normal = Pab;
			}
			else
			{
				MinEndPointVec = Dab;
				normal = Pcd;
			}
		}
		else
		{
			dv = a - intersection;
			float Dab = std::abs(Pab.dot(dv));
			if (MinEndPointVec < Dab)
			{
				normal = Pab;
			}
			else
			{
				MinEndPointVec = Dab;
				normal = Pcd;
			}
		}
		return true;
	}

} // namespace Utility
//
//namespace Allocs {
//
//	enum class Alignment : size_t
//	{
//		Normal = sizeof(void*),
//		SSE = 16,
//		AVX = 32,
//	};
//
//
//	namespace detail {
//		void* allocate_aligned_memory(size_t align, size_t size) {
//			return _mm_malloc(align, size);
//		}
//		void deallocate_aligned_memory(void* ptr) noexcept { _mm_free(ptr); }
//	}
//
//
//	template <typename T, Alignment Align = Alignment::AVX>
//	class AlignedAllocator;
//
//
//	template <Alignment Align>
//	class AlignedAllocator<void, Align>
//	{
//	public:
//		typedef void* pointer;
//		typedef const void* const_pointer;
//		typedef void              value_type;
//
//		template <class U> struct rebind { typedef AlignedAllocator<U, Align> other; };
//	};
//
//
//	template <typename T, Alignment Align>
//	class AlignedAllocator
//	{
//	public:
//		typedef T         value_type;
//		typedef T* pointer;
//		typedef const T* const_pointer;
//		typedef T& reference;
//		typedef const T& const_reference;
//		typedef size_t    size_type;
//		typedef ptrdiff_t difference_type;
//
//		typedef std::true_type propagate_on_container_move_assignment;
//
//		template <class U>
//		struct rebind { typedef AlignedAllocator<U, Align> other; };
//
//	public:
//		AlignedAllocator() noexcept
//		{}
//
//		template <class U>
//		AlignedAllocator(const AlignedAllocator<U, Align>&) noexcept
//		{}
//
//		size_type
//			max_size() const noexcept
//		{
//			return (size_type(~0) - size_type(Align)) / sizeof(T);
//		}
//
//		pointer
//			address(reference x) const noexcept
//		{
//			return std::addressof(x);
//		}
//
//		const_pointer
//			address(const_reference x) const noexcept
//		{
//			return std::addressof(x);
//		}
//
//		pointer
//			allocate(size_type n, typename AlignedAllocator<void, Align>::const_pointer = 0)
//		{
//			const size_type alignment = static_cast<size_type>(Align);
//			void* ptr = detail::allocate_aligned_memory(alignment, n * sizeof(T));
//
//			if (ptr == nullptr) {
//				throw std::bad_alloc();
//			}
//
//			return reinterpret_cast<pointer>(ptr);
//		}
//
//		void
//			deallocate(pointer p, size_type) noexcept
//		{
//			return detail::deallocate_aligned_memory(p);
//		}
//
//		template <class U, class ...Args>
//		void
//			construct(U* p, Args&&... args)
//		{
//			::new(reinterpret_cast<void*>(p)) U(std::forward<Args>(args)...);
//		}
//
//		void
//			destroy(pointer p)
//		{
//			p->~T();
//		}
//	};
//
//
//	template <typename T, Alignment Align>
//	class AlignedAllocator<const T, Align>
//	{
//	public:
//		typedef T         value_type;
//		typedef const T* pointer;
//		typedef const T* const_pointer;
//		typedef const T& reference;
//		typedef const T& const_reference;
//		typedef size_t    size_type;
//		typedef ptrdiff_t difference_type;
//
//		typedef std::true_type propagate_on_container_move_assignment;
//
//		template <class U>
//		struct rebind { typedef AlignedAllocator<U, Align> other; };
//
//	public:
//		AlignedAllocator() noexcept
//		{}
//
//		template <class U>
//		AlignedAllocator(const AlignedAllocator<U, Align>&) noexcept
//		{}
//
//		size_type
//			max_size() const noexcept
//		{
//			return (size_type(~0) - size_type(Align)) / sizeof(T);
//		}
//
//		const_pointer
//			address(const_reference x) const noexcept
//		{
//			return std::addressof(x);
//		}
//
//		pointer
//			allocate(size_type n, typename AlignedAllocator<void, Align>::const_pointer = 0)
//		{
//			const size_type alignment = static_cast<size_type>(Align);
//			void* ptr = detail::allocate_aligned_memory(alignment, n * sizeof(T));
//			if (ptr == nullptr) {
//				throw std::bad_alloc();
//			}
//
//			return reinterpret_cast<pointer>(ptr);
//		}
//
//		void
//			deallocate(pointer p, size_type) noexcept
//		{
//			return detail::deallocate_aligned_memory(p);
//		}
//
//		template <class U, class ...Args>
//		void
//			construct(U* p, Args&&... args)
//		{
//			::new(reinterpret_cast<void*>(p)) U(std::forward<Args>(args)...);
//		}
//
//		void
//			destroy(pointer p)
//		{
//			p->~T();
//		}
//	};
//
//}
// class math
//{
//public  :
//	template<typename NUM>
//	static inline NUM dot(NUM& a, NUM& b) {
//		return a.dot(b);
//	}
//private:
//
//};

//
//However, you may be able to use Vector4f class to perform Vector3f operationsand make use of vectorization
//if you can carefully ensure that the last component is always zero..Moreover, Matrix2f internally stores 4 floats,
//and vectorization will automatically take place if the expression can be seen as a 1D vector of 4 elements(e.g., adding two Matrix2f).
