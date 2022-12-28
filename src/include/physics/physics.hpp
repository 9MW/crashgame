#pragma once
//module;
#include "common/Math1.h"
#include"Common/ecs.hpp"
#include"Log/log.h"
#include"Gameprops0.h"
//#include"cds/container/lazy_list_dhp.h"
//#include <cds/container/feldman_hashmap_dhp.h>
//#include <cds/init.h>       // for cds::Initialize and cds::Terminate
//#include <cds/gc/hp.h> 
#include<immintrin.h>
#include"physics0.h"
#include"Common/Utility.h"
//export module physicsfc;

namespace pj2
{
	using namespace math;
	using mtransfrom = math::mtransform;
	using mtransform = mtransfrom;
	//#include <Common\ecs.hpp>
	constexpr float MaxObjVelocity = 5, maxLoop = 32, EpsilonSq = math::Epsilon * math::Epsilon;
	using namespace Eigen;
	template<typename T>
	using Ref = Eigen::Ref<T>;
	using uint = math::uint;

	struct Paircid0;
	struct Vox;
	template<typename T>
	using AppendArray=Utility::AppendArray<T>;
	template<typename T>
	struct 
		Box2
	{
		/*union{
			math::float4 all;

		}; */
		using self = Box2<T>;
		using eleType = T;
		using scalar = typename T::Scalar;
		using v4 = math::Vector<scalar, 4>;

		T Max, Min;
		Box2() : Max(T::Constant(-math::Infinityf)), Min(T::Constant(math::Infinityf)) {}
		Box2(T center, float halfextend)
		{
			Min = center.array() - halfextend;
			Max = center.array() + halfextend;
		}
		Box2 operator+(const T off)
		{
			return Box2f(Min += (T)off, Max += (T)off);
		}
		bool operator==(const self& b)
		{
			return Map<v4>((scalar*)this) == Map<v4>((scalar*)&b);
		}
		bool operator!=(const self& b)const
		{
			return Map<v4>((scalar*)this) != Map<v4>((scalar*)&b);
		}
		Box2(const v4&& v) {
			Map<v4>((scalar*)this) = v;
		}
		Box2(T&& min, T&& max)
		{
			Min = min;
			Max = max;
		}
		std::string toString() { return fmt::format("max {0} min{1}", Max.transpose(), Min.transpose()); }
		inline bool Contain(const T p)const {
			return ((p.array() <= Max.array()) && (p.array() >= Min.array())).all();
		}
	};
	//
	struct Box2f :Box2<math::Efloat2> {
		using uni = math::Efloat4;
		math::Ref<uni> tof4() {
			return Map<uni>((scalar*)this);
		}
		using Box2<eleType>::Box2;
	};
	struct Box2i :Box2<math::int2> {
		using uni = Vector4i;
		inline math::Ref<Vector4i> to4i() {
			return Map<uni>((scalar*)this);
		}
		inline void operator=(Box2f& f) {
			this->to4i() = f.tof4().cast<int>();
		}
		inline bool operator==(Box2i& f) {
			return this->to4i() == f.to4i();
		}
		Box2i() {
			Max.setConstant(-Infinityi); Min.setConstant(Infinityi);
		}
		inline void reset() { Max.setConstant(-Infinityi); Min.setConstant(Infinityi); }
		using Box2<eleType>::Box2;
	};
	//struct mtransfrom;
	//struct mtransfrom;
	namespace Physics
	{

		using LODShape = std::array<Box2i, 2>;
		struct bPhys {};
		using eobj = bPhys;

		using CollideID = GameProps::EntityId;
		using PhysicsNum = float;
		using PhysTime = int64_t;
		using v4p = math::vec4<PhysicsNum>;
		static constexpr int cellsize = 2;
		static  math::ui64 frameid = 0;
		static const Vector3i Standardcell((Vector3i() << cellsize, cellsize, 0).finished());
		static const Vector3f Standardcellf((Vector3f() << cellsize, cellsize, 0).finished());
		using aroundele = std::tuple<Vox, void*>;
		using namespace ECS;
		using CollEve = Utility::WrapFuncObj<void, const Paircid0>;
		template<typename T>
		inline auto Quantize(const T& v, const int const cellsize) { return(v.array() / cellsize).round(); }
		template<typename T>
		inline auto DeQuantize(const T& v, const int const cellsize)
		{
			return(v * T::Constant(cellsize)).array().round();
		}
		inline auto GetPhysTime(float t) {
			return (PhysTime)(t / dt);
		}
		inline auto GetErTime(PhysTime t) {
			return t * dt;
		}
		inline auto GetErTime(int t) {
			return t * dt;
		}
		//remove subvector along vector b 
		/*template<typename T>
		inline auto RemoveVector(T& a,T& b) {
			auto vb = b.normalized();
			const auto& dr = a.dot(vb);
			a -= (dr>0?dr * vb:vb.Zero());
		}*/
		enum class PhyProp :int
		{
			transform, velocity
		};
		enum class ColliderType :uint8_t
		{
			Box, Circle
		};
		struct VirtCollid
		{
				using v3 = math::Vector3f;
			union {
				v3 Velocity;
				mtransform::v4 vel4;
			};
			v3
				//DisPlace,
				AngularVelocity;// , FinalAngularVelocity;
			//CollideID ID;
			union {
				bool IsTrigger, Active = true;
				int Activei;
			};
			float Acceleration, AngularAcceleration, MaxVelocity, MaxAngularVeocity;// , stepsq;
			
			float InvMass, InvInertia;
			float detectRange;
			/*void Mass(float v) {
				 InvMass=1/v;
			}*/
			float ConvexR;
			void MaxV(const float v) {
				MaxVelocity = v;// MaxVelocitySq = v * v; stepsq = math::square(v * Physics::dt);
			}
			constexpr void SetMass(const float mass) { InvMass = 1 / mass; }
			VirtCollid() :  vel4(0, 0, 0, 0), AngularVelocity(0, 0, 0) {
				ConvexR = 0.5;
			}
			VirtCollid(const VirtCollid& cs) {
				std::memcpy(this, &cs, sizeof(cs));
			}
			void operator=(const VirtCollid& cs) {
				std::memcpy(this, &cs, sizeof(cs));
			}
			template<typename T>
			static T& Build() {
			}

		};
		struct SCollid {
			v4f halfext_CR;
		};
		struct Collid
		{
			using v3 = math::Vector3f;
			union {
				v3 Velocity;
				mtransform::v4 vel4;
			};
			mtransform::v4 V_desire;
			v3 AngularVelocity;// , FinalAngularVelocity;
			ColliderType colliderType;
			//CollideID ID;
			union {
				bool IsTrigger, Active = true;
				int Activei;
			};
			float Acceleration, AngularAcceleration, MaxVelocity, MaxAngularVeocity;// , stepsq;
			/*float friction;*/
			float detectRange, InvInertia, InvMass;
			/*void Mass(float v) {
				 InvMass=1/v;
			}*/
			void* data_ptr;
			float ConvexR;
			void MaxV(const float v) {
				MaxVelocity = v;// MaxVelocitySq = v * v; stepsq = math::square(v * Physics::dt);
			}
			constexpr void SetMass(const float mass) { InvMass = 1 / mass; }
			template<typename T>
			T& getData()const {
				return  *reinterpret_cast<T*>(data_ptr);//T::get(data);
			}
			Collid() : data_ptr(NULL), vel4(0, 0, 0, 0), AngularVelocity(0, 0, 0) {}
			Collid(const Collid& cs) {
				std::memcpy(this, &cs, sizeof(Collid));
			}
			void operator=(const Collid& c) {
				std::memcpy(this, &c, sizeof(Collid));
			}
			//Collid(const Collid& c) { std::memcpy( this ,&c,sizeof(Collid)); }
			/*template<typename T>
			const T& getData() const{
				return T::get(data);
			}*/
			template<typename T>
			static T& Build() {
			}

		};

		template<typename T = int>
		struct  PhysicsObject :Component0<T> {
			Utility::bit128 bitgroupid;
			Collid collider;
			mtransform transform;
			Box2i cBox, pBox;
			AppendArray<aroundele> _arounds;
			CollEve Eve;
			constexpr operator PhysicsObject<>& () {
				return reinterpret_cast<PhysicsObject<>&> (*this);;
			}
			//template<class J>
			//constexpr J& get() {
			//	if constexpr (std::is_same_v<Collid, J>) {
			//		return collider;
			//	}
			//	if constexpr (std::is_same_v<mtransfrom, J>) {
			//		return transform;
			//	}
			//	else {
			//		static_assert(false, "unexpected input");
			//	}
			//	//return J();
			//}
			inline math::Fix::fix_s ConvexR() {
				return math::Fix::fix_s(collider.ConvexR);
			}
			virtual void OnCollision(Paircid0& pir) {}
			/*inline operator PhyObj () {
				return reinterpret_cast<PhyObj> (*this);;
			}*/
		};
		using pobj = PhysicsObject<>;
		/*	template<class J>
			template J& PhysicsObject<int>::get<J>();*/
		struct LAVNe {
			/*
				const Vector2 relativePosition = other->position_ - position_;
				const Vector2 relativeVelocity = velocity_ - other->velocity_;
				*/
			using scl = float;
			//Utility::bit128 b128;
			math::Vector4f relativePosition, relativeVelocity,desiv;
			int gid,cmdid;
			scl  _distSq;
			GameProps::ENU::state st;
			//Utility::MyTuple<GameProps::Cmd0*> Prop_ptrs;
			CollideID cd;// mtransfrom::v4::Scalar;
			const auto& ccombinedRadius() const{
				return relativeVelocity[3];
			}
			inline auto& combinedRadius() {
				return relativeVelocity[3];
			}
			inline auto combinedRadiussq()const {
				return ccombinedRadius() * ccombinedRadius();
			}
			const auto& distSq() const {
				return _distSq;
			}
			
			/*template<class T>
			inline void SetProp_ptrs(T&& tin) {
				ECS::convert(tin, Prop_ptrs);
			}*/
			inline void combinedRadius(const scl& v) {
				combinedRadius() = v;
			}
			inline void distSq(const scl& v) {
				_distSq = v;
			}
			LAVNe(const LAVNe& lm) {
				std::memcpy(this, &lm, sizeof(lm));
			}
			LAVNe() {}
		};
		template<math::ui64 sz>
		struct alignas(16) Lavnesg
		{
			std::size_t c;
			template <class... _Types>
			void emplace(_Types&&... _Args) {
				if (c >= sz) {
					std::abort();
				}
				//auto&& tup = datas[c++];
				datas.set(c++, std::forward_as_tuple(_STD forward<_Types>(_Args)...));// std::forward<_Types>(_Args)...)
					//tup = std::forward_as_tuple(_Args...);
			}
			ECS::CompPackageFix<sz, LAVNe> datas;
			/*	template<size_t I = 0, typename... Tp>
	constexpr void Looptup(auto&& tp,auto&& tp2){
		std::get<I>(tp) = std::get<I>(tp2);
		if constexpr (I + 1 != sizeof...(Tp))
			Looptup<I + 1>(t);
	}*/
			const inline auto size() const {
				return c;
			}
			constexpr void clear() {
				c = 0;
			}
			constexpr auto&& get(const math::ui64 i) {
				return datas.get(i);
			}
			/*inline auto data() {
				return ay;
			}*/

			/*LAVNe& operator[](std::size_t i) {
				return ay[i];
			}*/
			LAVNe& operator[](std::size_t i)  {
				return datas.get<0>(i);
			}
			inline void Sort() {
				/*insertionSort(datas,
					[&](auto&& a, auto&& b)
					{return std::get<0>(a).distSq() < std::get<0>(datas.get(b)).distSq(); });*/
					/*std:sort(ay.begin(), ay.begin()+c,
						[](const auto& a, const auto& b) {return a.distSq() < b.distSq(); });*/
			}
			bool full() {
				return c >= sz;
			}
		};
		using Lavnes = Lavnesg<16>;
	};
	
	using Physics::CollideID;
	using Physics::Collid;
	using Physics::ColliderType;
	template<typename T>
	inline auto Quantize(const math::Efloat2& v, const T& cellsize) { return(v.cwiseQuotient(cellsize)).array().round().cast<int>(); }

	inline auto Quantize(const math::Efloat2& v, const int const cellsize) { return(v / cellsize).array().round(); }
	inline auto Quantize(const float v, const int cellsize) { return (int)(std::round(v / cellsize)); }
	struct Vox
	{
		union {
			size_t num;
			CollideID cd;
		};
		//Physics::PhyObj* PO;
		/*operator CollideID& () {
			return cd;
		}*/
		Vox(const CollideID&& cd) :cd(cd) {}
		Vox(const CollideID& cd) :cd(cd) {}
		Vox(const size_t num) :num(num) {}
		Vox() {}
		inline bool operator >(const Vox& rhs)
		{
			return num > rhs.num;
		}
		inline bool operator <(const Vox& rhs)
		{
			return num < rhs.num;
		}
		void operator =(const Vox& v) {
			cd = v.cd;
			//PO = v.PO;
		}
		bool operator ==(const Vox el) const {
			return num == el.num;;
		}


		inline auto& hash () const{
			return (size_t&)cd;
		}
		inline operator CollideID& () {
			return cd;
		}
		bool operator ==(const CollideID el) {
			return cd.hashnum == el.hashnum;
		}
		operator size_t&() {
			return (size_t&)cd;
		}
		template<typename OStream>
		friend OStream& operator<<(OStream& os, const Vox& c)
		{
			return os << c.cd;
		}
	};

	const auto dynamicid = (int)CollideID::CollidDS::Dynamic, statid = (int)CollideID::CollidDS::Static;
	template<typename T>
	struct AssetReference
	{
		T Value;
		AssetReference(T& v) :Value(v) {}
	};

	template<class T>
	inline void CalculStartEnd(T const& CellSize, const Box2f& bounds, Box2i& outres) {
		outres.Min = Quantize(bounds.Min, CellSize.head<2>().cast<float>()).round().cast<int>();
		outres.Max = Quantize(bounds.Max, CellSize.head<2>().cast<float>()).round().cast<int>();
	};
	//template<typename T, uint PoolSize = 100>
	//struct staticPool {
	//	static std::vector<T> _pool;

	//	struct constructor {
	//		constructor() { _pool.reserve(PoolSize); }
	//	};

	//	static constructor cons;
	//	static T& create(uint& id) {
	//		id = _pool.size();
	//		_pool.resize(id + 1);
	//		return _pool[id];
	//	}
	//	inline static uint create() {
	//		uint  id;
	//		id = _pool.size();
	//		_pool.resize(id + 1);
	//		return id;
	//	}
	//	inline static T& Created() {
	//		const auto id = _pool.emplace_back();
	//		_pool.resize(id + 1);
	//		return pool.emplace_back();
	//	}
	//	inline static uint create(T& pa) {
	//		uint  id;
	//		id = _pool.size();
	//		_pool.emplace_back(pa);
	//		return id;
	//	}
	//	inline static uint create(T&& pa) {
	//		uint  id;
	//		id = _pool.size();
	//		_pool.emplace_back(pa);
	//		return id;
	//	}
	//	static T& get(uint i) {
	//		return _pool[i];
	//	}
	//};

	//template<typename T, uint PoolSize>
	//std::vector<T> staticPool<T, PoolSize>::_pool;
	template<typename T, uint size>
	struct AtomicArray {
	private:
		T S0[size];
		std::atomic_llong counter;
	public:
		AtomicArray(size_t reserve,const size_t& size = 0) : counter(0)
		{
			S0.resize(reserve);
			counter = size;
		}
		T& operator[](size_t i) {
			assert(i < counter);
			return S0[i];
		}
		var begin() {
			return S0.begin();
		}
		var end() {
			return S0.end();
		}
		void enqueue(T&& i) {
			S0[counter++] = i;
		}
		void enqueue(T& i) {
			S0[counter++] = i;
		}
		void add(T& i) {
			S0[counter++] = i;
		}
		const size_t size()const {
			return counter;
		}
		size_t& sz() {
			return counter;
		}
		void clear() {
			counter = 0;
		}
	};
	struct Paircid0;


	struct Paircid0 {
		Vox a, b;
		Physics::PhysicsObject<>* pa, * pb;
		Paircid0() {}
		//Paircid0(Vox a, Vox b, Physics::PhyObj* pa, Physics::PhyObj* pb) :a(a), b(b), pa(pa), pb(pb) {}
		Paircid0(Vox a, Vox b, Physics::PhysicsObject<>* pa, Physics::PhysicsObject<>* pb) :a(a), b(b), pa(pa), pb(pb) {}
		//Paircid0(const Paircid0& v) :a(v.a), b(v.b) {}
		void operator =(const Paircid0& v) {
			a = v.a;
			b = v.b;
			pa = v.pa;
			pb = v.pb;
		}
		bool operator !=(const Paircid0& v) {
			return !(a.cd == b.cd);
		}
		inline Paircid0 Swap() const {
			return Paircid0(b, a, pb, pa);
		}
		inline bool operator <(const Paircid0& rhs)const
		{
			return a.num == rhs.a.num ? b.num < rhs.b.num : a.num < rhs.a.num;
		}
	};
	template<typename T>
	struct Pool {
		std::vector<T> _pool;

		Pool(uint re) { _pool.reserve(re); }

		T& create(uint& id) {
			id = _pool.size();
			_pool.resize(id + 1);
			return _pool[id];
		}
		T& operator [](uint i) {
			return _pool[i];
		}
	};
	union alignas(16) Paircid
	{
		struct {
			CollideID e1, e2;
		};

		//std::aligned_storage<16, 16> alignment_only;

		Eigen::Matrix<int64_t, 2, 1> v2ll;
		//Paircid(CollideID& e1, CollideID& e2):e1(e1),e2(e2){}
		Paircid() {}
		Paircid(CollideID e1, CollideID e2) :e1(e1), e2(e2) {}
		Paircid(const Paircid& v) {
			this->v2ll = v.v2ll;
		}
		void operator=(const Paircid& v) {
			this->v2ll = v.v2ll;
		}
		bool operator!=(const Paircid& h) {
			//return bx::simd_test_all_xyzw(bx::simd_cmpneq(bx::simd_ld(this), bx::simd_ld(&h)));
			return v2ll != h.v2ll;
		}
		int operator<(const Paircid& r) {
			return v2ll[0] == r.v2ll[0] ? v2ll[1] < r.v2ll[1] : v2ll[0] < r.v2ll[0];
		}
		int operator>(const Paircid& r) {
			return v2ll[0] == r.v2ll[0] ? v2ll[1] > r.v2ll[1] : v2ll[0] > r.v2ll[0];
		}
		//private:
		bool operator==(const Paircid& h) {
			//return bx::simd_test_all_xyzw(bx::simd_cmpneq(s12,h.s12));
			//return vfi==(h.vfi);
			return std::memcmp(this, &h, 16);
		}
		Vector4i vfi;
	};

	struct ICircleCollider
	{
		// ref Collid MyCollider.Head {get{float (ICircleCollider* ts= &this){ return ref ts->Head; } } }

		static void CreatCollid(float mass, float r, bool IsTrigger, Collid& c, bool active = true) {
			c.IsTrigger = IsTrigger;
			c.detectRange = mass;
			c.ConvexR = r;
			c.colliderType = ColliderType::Circle;
			c.Active = active;
			AssetReference<Collid> blob(c);
		}

	};
	struct IBoxCollider //:staticPool<IBoxCollider, 5200>
	{
		static const int m_count = 4;
		math::Efloat2 m_vertices[m_count];
		const math::Efloat2& max = m_vertices[0];
		math::Efloat2 normals[m_count];

		void init(const math::Efloat2& size, Collid& cd, const bool IsTrigger = false) {

			IBoxCollider& box = *this;
			math::Efloat2 a = size * 0.5f, b = a.cwiseProduct(Efloat2(-1, 1)), c = -a, d = Efloat2(a[0], -a[1]);

			var& m_vertices = box.m_vertices;
			//box.m_vertices[0][0] = 100;
			m_vertices[0] = (a);
			m_vertices[1] = (b);
			m_vertices[2] = (c);
			m_vertices[3] = (d);
			var sizesq = a.squaredNorm();
			cd.InvInertia = 12 / (sizesq)*cd.InvMass;
			cd.ConvexR = math::sqrt(sizesq);
			cd.colliderType = ColliderType::Box;
			var& m_normals = box.normals;

			m_normals[2] = Efloat2(0.0f, -1.0f);
			m_normals[3] = Efloat2(1.0f, 0.0f);
			m_normals[0] = Efloat2(0.0f, 1.0f);
			m_normals[1] = Efloat2(-1.0f, 0.0f);
		}
		inline void Resize(Collid& cd, const math::Efloat2& size = math::Efloat2::Zero()) {

			if (m_vertices[0] == size * 0.5f)
				return;
			IBoxCollider& box = *this;
			math::Efloat2 a = size * 0.5f, b = a.cwiseProduct(Efloat2(-1, 1)), c = -a, d = Efloat2(a[0], -a[1]);

			var m_vertices = box.m_vertices;
			//box.m_vertices[0][0] = 100;
			m_vertices[0] = (a);
			m_vertices[1] = (b);
			m_vertices[2] = (c);
			m_vertices[3] = (d);
			cd.ConvexR = a.norm();
		}
		bool Contain(const math::Efloat2 p)const {
			const bool sm = (p.array() <= m_vertices[0].array()).all();
			const bool bg = (p.array() >= m_vertices[2].array()).all();
			return sm && bg;
		}
		/*IBoxCollider& get() {
			return _pool[0];
		}*/
		~IBoxCollider() {

		}
		var operator[](int i) {
			return m_vertices[i];
		}
	};
	void CalculCollision(float restitution, Collid& ca, Collid& cb,
		const math::Efloat3& impact, mtransfrom& transforma, mtransfrom& transformb, const math::Efloat3& normala);
	inline mtransfrom Mul(const mtransfrom& a, const mtransfrom& b) {
		return mtransform(a.pos + b.pos, a.rotation * b.rotation);
	}
	inline mtransfrom B2A(const mtransfrom& a, const mtransfrom& b) {
		mtransfrom result;
		const var inva = a.rotation.inverse();
		result.rotation = inva * b.rotation;
		result.pos = inva * (b.pos - a.pos);

		return result;
	}
	inline const math::Efloat3& Trans2d(const math::Efloat3& tg, const mtransfrom& q)
	{
		return q.pos.head<3>() + q.rotation * tg.head<3>();
	}

	inline math::Efloat2 Trans2d(const math::Efloat2& tg, const mtransfrom& q)
	{
		return q.pos.head<2>() + math::mulf2q(q.rotation, tg);
	}

	inline const math::Efloat2 Trans2d(const mtransfrom& q, const math::Efloat2& tg)
	{
		return Trans2d(tg, q);
	}
	template<typename T>
	inline math::Efloat2 Trans2dIv(const mtransfrom& q, T const& tg)
	{
		var t = tg - q.pos.head<2>();
		return  math::mulf2q(t, q.rotation.inverse());
	}
	inline math::Efloat2 Trans2d(const math::Efloat2& tg, const math::Efloat2& pos, const math::quaternion& rotation)
	{
		const math::Efloat3 f(tg[0], tg[1], 0);
		return pos + (rotation * f).head<2>();
	}
	inline math::quaternion IntegrateAngularVelocity(math::Efloat3 angularVelocity, float timestep)
	{

		math::Efloat3 halfDeltaAngle = angularVelocity * timestep * 0.5f;
		math::quaternion q;
		q.w() = 1;
		q.vec() = halfDeltaAngle;
		return q;
	}
	inline void IntegrateOrientation(math::quaternion& orientation, math::Efloat3 angularVelocity, float timestep)
	{
		math::quaternion dq = IntegrateAngularVelocity(angularVelocity, timestep);
		orientation = (orientation * dq).normalized();
	}
	/// Perform the cross product on a vector and a scalar. In 2D this produces
	/// a vector.
	inline auto b2Cross(const math::Efloat2& a, float s)
	{
		return  math::Efloat2(s * a[1], -s * a[0]);
	}
	/// Perform the cross product on two vectors. In 2D this produces a scalar.
	inline float b2Cross(const math::Efloat2& a, const math::Efloat2& b)
	{
		return a[0] * b[1] - a[1] * b[0];
	}

	inline void CalculCollision(float restitution, Collid& ca, Collid& cb, const math::Efloat2& impact, mtransfrom& transforma, mtransfrom& transformb, math::Efloat2& normala) {
		const math::Efloat3 i(impact[0], impact[1], 0), n(normala[0], normala[1], 0);
		CalculCollision(restitution, ca, cb, i, transforma, transformb, n);
	}

	inline float b2FindMaxSeparation(int32* edgeIndex,
		const IBoxCollider* poly1, const mtransfrom& xf1,
		const IBoxCollider* poly2, const mtransfrom& xf2)
	{
		int32 count1 = poly1->m_count;
		int32 count2 = poly2->m_count;
		const var n1s = poly1->normals;
		const var v1s = poly1->m_vertices;
		const var v2s = poly2->m_vertices;
		mtransfrom xf = B2A(xf2, xf1);

		int32 bestIndex = 0;
		float maxSeparation = -math::Infinityf;
		for (int32 i = 0; i < count1; ++i)
		{
			// Get poly1 normal in frame2.
			math::Efloat2 n = math::mulf2q(n1s[i], xf.rotation);
			var v1 = Trans2d(v1s[i], xf);

			// Find deepest point for normal i.
			float si = math::Infinityf;
			for (int32 j = 0; j < count2; ++j)
			{
				float sij = math::dot(n, v2s[j] - v1);
				if (sij < si)
				{
					si = sij;
				}
			}

			if (si > maxSeparation)
			{
				maxSeparation = si;
				bestIndex = i;
			}
		}

		*edgeIndex = bestIndex;
		return maxSeparation;
	}
	// Sutherland-Hodgman clipping.
	inline int32  b2ClipSegmentToLine(math::Efloat2 vOut[2], const math::Efloat2 vIn[2],
		const math::Efloat2& normal, float offset, int32 vertexIndexA)
	{
		// Start with no output points
		int32 numOut = 0;

		// Calculate the distance of end points to the line
		float distance0 = math::dot(normal, vIn[0]) - offset;
		float distance1 = math::dot(normal, vIn[1]) - offset;

		// If the points are behind the plane
		if (distance0 <= 0.0f) vOut[numOut++] = vIn[0];
		if (distance1 <= 0.0f) vOut[numOut++] = vIn[1];

		// If the points are on different sides of the plane
		if (distance0 * distance1 < 0.0f)
		{
			// Find intersection point of edge and plane
			float interp = distance0 / (distance0 - distance1);
			vOut[numOut] = vIn[0] + interp * (vIn[1] - vIn[0]);

			// VertexA is hitting edgeB.
		  /*vOut[numOut].id.cf.indexA = static_cast<uint8>(vertexIndexA);
			vOut[numOut].id.cf.indexB = vIn[0].id.cf.indexB;
			vOut[numOut].id.cf.typeA = b2ContactFeature::e_vertex;
			vOut[numOut].id.cf.typeB = b2ContactFeature::e_face;*/
			++numOut;
		}

		return numOut;
	}
	inline void b2FindIncidentEdge(math::Efloat2 c[2],
		const IBoxCollider* poly1, const mtransfrom& xf1, const int32 edge1,
		const IBoxCollider* poly2, const mtransfrom& xf2)
	{
		const var normals1 = poly1->normals;

		int32 count2 = poly2->m_count;
		const var* vertices2 = poly2->m_vertices;
		const var* normals2 = poly2->normals;

		assert(0 <= edge1 && edge1 < poly1->m_count);
		const var worldnomal = math::mulf2q(xf1.rotation, normals1[edge1]);
		// Get the normal of the reference edge in poly2's frame.
		var normal1 = math::mulf2q(xf2.rotation.inverse(), worldnomal);

		// Find the incident edge on poly2.
		int32 index = 0;
		float minDot = math::Infinityf;
		for (int32 i = 0; i < count2; ++i)
		{
			float dot = math::dot(normal1, normals2[i]);
			if (dot < minDot)
			{
				minDot = dot;
				index = i;
			}
		}

		// Build the clip vertices for the incident edge.
		int32 i1 = index;
		int32 i2 = i1 + 1 < count2 ? i1 + 1 : 0;

		c[0] = Trans2d(xf2, vertices2[i1]);

		c[1] = Trans2d(xf2, vertices2[i2]);

	}

	struct contact
	{
		Collid* box1, * box2;
		mtransfrom* xfa, * xfb;
		math::Efloat2 points[2];	///< the points of contact
		math::Efloat2 localNormal;								///< not use for Type::e_points
		math::Efloat2 localPoint;								///< usage depends on manifold type
		//Type type;
		int32 pointCount;
	};
	const float EP = 0.001, Epsional2 = EP * 2;
	inline void CirBox(contact* manifold, Collid& cir, Collid& bx,
		mtransfrom& xfa, mtransfrom& xfb, float dt) {
		assert(bx.colliderType == ColliderType::Circle);
		int32 normalIndex = 0;
		float separation = -math::Infinityf;
		const	float radius = EP + cir.ConvexR;
		var& polygonA = bx.getData<IBoxCollider>();
		int32 vertexCount = polygonA.m_count;
		const var* vertices = polygonA.m_vertices;
		const var* normals = polygonA.normals;
		const var cLocal = vertices[0];//Trans2dIv(xfb, xfa.pos);
		for (int32 i = 0; i < vertexCount; ++i)
		{
			float s = math::dot(normals[i], cLocal - vertices[i]);

			if (s > radius)
			{
				// Early out.
				return;
			}

			if (s > separation)
			{
				separation = s;
				normalIndex = i;
			}
		}

		// Vertices that subtend the incident face.
		int32 vertIndex1 = normalIndex;
		int32 vertIndex2 = vertIndex1 + 1 < vertexCount ? vertIndex1 + 1 : 0;
		var v1 = vertices[vertIndex1];
		var v2 = vertices[vertIndex2];

		float percent;
		var pjp = math::ProjectClamp((decltype(v1))xfa.pos.head<2>(), v1, v2, percent);
		// If the center is inside the polygon ...
		if (separation < math::Epsilon)
		{
			manifold->pointCount = 1;

			manifold->localNormal = normals[normalIndex];
			manifold->localPoint = 0.5f * (v1 + v2);
			manifold->points[0] = xfa.pos.head<2>();
			return;
		}
		const var pjpcir = xfa.pos.head<2>() - pjp;
		float s = math::dot(pjpcir, pjpcir);
		if (s > radius * radius)
		{
			return;
		}

		manifold->pointCount = 1;
		manifold->localNormal = normals[vertIndex1];
		manifold->localPoint = pjp;
		manifold->points[0] = xfa.pos.head<2>();
	}
	inline void CirCir(contact* manifold, Collid& cir1, Collid& cir2,
		mtransfrom& xfa, mtransfrom& xfb, float dt)
	{
		var o = xfa.pos - xfb.pos;
		var d = math::dot(o, o);
		/*var constrainr = math::square(cir1.ConvexR + cir2.ConvexR);
		if (d > constrainr)
			return false;*/
		manifold->localNormal = o.normalized().head<2>();
		manifold->localPoint = xfa.pos.head<2>() + manifold->localNormal * cir1.ConvexR;
		manifold->pointCount = 1;
		manifold->points[0] = xfb.pos.head<2>();
	}
	inline void BoxBox2(contact* manifold, Collid& box1, Collid& box2,
		mtransfrom& xfa, mtransfrom& xfb, float dt) {

		manifold->pointCount = 0;
		var* poly1 = &box1.getData<IBoxCollider>();
		var* poly2 = &box2.getData<IBoxCollider>();
		int32 edgeA = 0;
		float separationA = b2FindMaxSeparation(&edgeA, poly1, xfa, poly2, xfb);
		if (separationA > EP)
			return;

		int32 edgeB = 0;
		float separationB = b2FindMaxSeparation(&edgeB, poly2, xfb, poly1, xfa);
		if (separationB > EP)
			return;
		int edge1 = edgeA;
		var xf1 = &xfa, xf2 = &xfb;
		bool filp = false;
		if (separationB > separationA)
		{
			std::swap(poly1, poly2);
			std::swap(xf1, xf2);
			manifold->box1 = &box2;
			manifold->box2 = &box1;
			manifold->xfa = xf1;
			manifold->xfb = xf2;
			edge1 = edgeB;
			filp = true;
		}
		else
		{

			manifold->xfa = xf1;
			manifold->xfb = xf2;
			manifold->box1 = &box1;
			manifold->box2 = &box2;
		}
		math::Efloat2 incidentEdge[2];
		b2FindIncidentEdge(incidentEdge, poly1, *xf1, edge1, poly2, *xf2);
		int32 count1 = poly1->m_count;
		const var* vertices1 = (*poly1).m_vertices;

		int32 iv1 = edge1;
		int32 iv2 = edge1 + 1 < count1 ? edge1 + 1 : 0;

		var v11 = vertices1[iv1];
		var v12 = vertices1[iv2];

		var localTangent = (v12 - v11).normalized();

		const decltype(localTangent) localNormal = b2Cross(localTangent, 1.0f);
		const decltype(localTangent) planePoint = 0.5f * (vertices1[iv1] + vertices1[iv2]);

		var tangent = math::mulf2q(xf1->rotation, localTangent);
		var normal = b2Cross(tangent, 1.0f);

		v11 = Trans2d(*xf1, v11);
		v12 = Trans2d(*xf1, v12);

		// Face offset.
		float frontOffset = math::dot(normal, v11);

		// Side offsets, extended by polytope skin thickness.
		float sideOffset1 = -math::dot(tangent, v11) + Epsional2;
		float sideOffset2 = math::dot(tangent, v12) + Epsional2;

		// Clip incident edge against extruded edge1 side edges.
		math::Efloat2 clipPoints1[2];
		math::Efloat2 clipPoints2[2];
		int np;

		// Clip to box side 1
		np = b2ClipSegmentToLine(clipPoints1, incidentEdge, -tangent, sideOffset1, iv1);

		if (np < 2)
			return;

		// Clip to negative box side 1
		np = b2ClipSegmentToLine(clipPoints2, clipPoints1, tangent, sideOffset2, iv2);

		if (np < 2)
		{
			return;
		}

		// Now clipPoints2 contains the clipped points.
		manifold->localNormal = localNormal;
		manifold->localPoint = planePoint;
		//LOGINFO("x1.py {0} x2.py {1}", xf1->pos[1], xf2->pos[1]);
		int32 pointCount = 0;
		for (int32 i = 0; i < 2; ++i)
		{
			float separation = math::dot(normal, clipPoints2[i]) - frontOffset;

			if (separation <= Epsional2)
			{
				var* cp = manifold->points + pointCount;
				*cp = Trans2dIv(*xf2, clipPoints2[i]);

				++pointCount;
			}
		}

		manifold->pointCount = pointCount;
	}
	const float b2_baumgarte = 0.2f, b2_linearSlop = 0.005f, b2_maxLinearCorrection = 0.2f;
	const math::IOFormat CommaInitFmt(2, DontAlignCols);
	// Sequential solver.
	inline bool SolvePositionConstraints(contact* manifold)
	{
		float minSeparation = 0.0f;

		//for (int32 i = 0; i < m_count; ++i)
		{
			//b2ContactPositionConstraint* pc = m_positionConstraints + i;

			float mA = manifold->box1->InvMass;
			float iA = manifold->box1->InvInertia;
			float mB = manifold->box2->InvMass;
			float iB = manifold->box2->InvInertia;
			int32 pointCount = manifold->pointCount;
			auto cA = (*manifold->xfa).pos.head<2>();
			var& aA = (*manifold->xfa).rotation;

			auto cB = (*manifold->xfb).pos.head<2>();
			var& aB = (*manifold->xfb).rotation;
			/*if (pointCount == 1)
				LOGERROR("count =1");*/
				// Solve normal constraints
			for (int32 j = 0; j < pointCount; ++j)
			{
				var xfA = *manifold->xfa, xfB = *manifold->xfb;
				/*xfA.q.Set(aA);
				xfB.q.Set(aB);
				xfA.p = cA - Trans2d(xfA.q, localCenterA);
				xfB.p = cB - Trans2d(xfB.q, localCenterB);*/


				var normal = math::mulf2q(xfA.rotation, manifold->localNormal);
				var planePoint = Trans2d(xfA, manifold->localPoint);

				var clipPoint = Trans2d(xfB, manifold->points[j]);
				var separation = math::dot(clipPoint - planePoint, normal) - 0.02f;
				/*var clipPoint = Trans2d(xfA, manifold->points[j]);
				var separation = math::dot(clipPoint - planePoint, normal) - 0.02f;*/
				var point = clipPoint;
				var rA = point - cA;
				var rB = point - cB;

				// Track max constraint error.
				minSeparation = math::min(minSeparation, separation);

				// Prevent large corrections and allow slop. b2_baumgarte * (separation + b2_linearSlop);//
				float C = math::clamp(b2_baumgarte * (separation + b2_linearSlop), -b2_maxLinearCorrection, 0.0f);

				// Compute the effective mass.
				float rnA = b2Cross(rA, normal);
				float rnB = b2Cross(rB, normal);
				float K = mA + mB + iA * rnA * rnA + iB * rnB * rnB;

				// Compute normal impulse
				float impulse = K > 0.0f ? -C / K : 0.0f;

				var P = impulse * normal;
				/*LOGINFO("impluse={0} separation={1} planePoint.y={2} localPoint.y={3} clip.y={4}",
					impulse,separation,planePoint[1],manifold->localPoint[1], clipPoint[1]);*/
				cA -= mA * P;
				aA = AngleAxisf(-iA * b2Cross(rA, P), math::Efloat3::UnitZ()) * aA;

				cB += mB * P;
				aB = AngleAxisf(iB * b2Cross(rB, P), math::Efloat3::UnitZ()) * aB;
			}


		}

		// We can't expect minSpeparation >= -b2_linearSlop because we don't
		// push the separation above -b2_linearSlop.
		return minSeparation >= -3.0f * b2_linearSlop;
	}
	template<class CLD>
	inline  void IntegrateMotion(mtransfrom& motionData, CLD& cd, const float Timestep)
	{
		{
			/*cd.Velocity += cd.Acceleration + cd.DisPlace;
			cd.Acceleration = 0;
			cd.AngularVelocity += cd.AngularAcceleration;
			cd.DisPlace = cd.AngularAcceleration = 0f;*/
			// center of mass
			motionData.pos += cd.Velocity * Timestep;

			// orientation
			if (!cd.AngularVelocity.isZero())
				IntegrateOrientation(motionData.rotation, cd.AngularVelocity, Timestep);
		}
	}
	inline void CalculContact(contact* manifold, Collid& box1, Collid& box2,
		mtransfrom& xfa, mtransfrom& xfb, float dt) {
		switch (box1.colliderType)
		{
		case ColliderType::Box:
			switch (box1.colliderType)
			{
			case ColliderType::Box:
				BoxBox2(manifold, box1, box2, xfa, xfb, dt);
				break;
			case ColliderType::Circle:
				CirBox(manifold, box1, box2, xfa, xfb, dt);
				break;
			};
			break;
		case ColliderType::Circle:
			switch (box1.colliderType)
			{
			case ColliderType::Box:
				CirBox(manifold, box1, box2, xfa, xfb, dt);
				break;
			case ColliderType::Circle:
				CirCir(manifold, box1, box2, xfa, xfb, dt);
				break;
			};
			break;
		default:
			std::abort();
		}
	}
}
//template<typename T> T& Collid::getData() {
//	return T::get(data);
//}