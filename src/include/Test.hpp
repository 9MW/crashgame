#pragma once
//#include "physics/physics.hpp"
#include "common/Math1.h"
#include "common/span.hpp"
#include <unordered_set>
//#include "physics/collisioin.h"
#include <chrono>
#include "cds/container/lazy_list_dhp.h"
#include <cds/container/feldman_hashmap_dhp.h>
#include <cds/init.h> // for cds::Initialize and cds::Terminate
#include <cds/gc/hp.h>
#include "taskflow/taskflow.hpp"
#include <iostream>
#include <taskflow\core\task.hpp>
#include<execution>
#include <Common\ecs.hpp>
//#include "entitys/CM.h"
#include"Common/syss.hpp"
#include"AI/imp3.h"
//import "AI";
//#include"ecs.hpp"
namespace pj2 {
	//using namespace Physics;
	using namespace nonstd;
	//
	//typedef tbb::concurrent_unordered_multimap<Vector2f, int, matrix_hash<Vector2f>>
	//MyMultiMap;
	//tbb::concurrent_unordered_map<int, int> mm;


	// maximum mumber of lines the output console should have
	static const WORD MAX_CONSOLE_LINES = 10000;
	//#ifdef _DEBUG
	void RedirectIOToConsole()
	{
		int hConHandle;
		long lStdHandle;
		CONSOLE_SCREEN_BUFFER_INFO coninfo;
		FILE* fp;

		// allocate a console for this app
		AllocConsole();

		// set the screen buffer to be big enough to let us scroll text
		GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &coninfo);
		coninfo.dwSize.Y = MAX_CONSOLE_LINES;
		SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), coninfo.dwSize);

		// redirect unbuffered STDOUT to the console
		lStdHandle = (long)GetStdHandle(STD_OUTPUT_HANDLE);
		hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
		fp = _fdopen(hConHandle, "w");
		*stdout = *fp;
		setvbuf(stdout, NULL, _IONBF, 0);

		// redirect unbuffered STDIN to the console
		lStdHandle = (long)GetStdHandle(STD_INPUT_HANDLE);
		hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
		fp = _fdopen(hConHandle, "r");
		*stdin = *fp;
		setvbuf(stdin, NULL, _IONBF, 0);

		// redirect unbuffered STDERR to the console
		lStdHandle = (long)GetStdHandle(STD_ERROR_HANDLE);
		hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
		fp = _fdopen(hConHandle, "w");
		*stderr = *fp;
		setvbuf(stderr, NULL, _IONBF, 0);

		// make cout, wcout, cin, wcin, wcerr, cerr, wclog and clog
		// point to console as well
		ios::sync_with_stdio();
	}

	//#endif
	template <typename T>
	void setVector(std::vector<T> v, nonstd::span<T> s)
	{
		v.data = s.data();
		v.size = s.size();
	}

	template <class T>
	class ay
	{
	public:
		// constants and types

		T* p;
		int size;
		inline void operator=(span<T> sp)
		{
			p = &sp[0];
			size = sp.size();
		}
		inline T& operator[](int i) { return p[i]; }
	};
	template<typename T>
	double  timeMeasure(const T fc)
	{
		var t1 = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now()).time_since_epoch();
		fc();
		var t2 = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now()).time_since_epoch();
		var dt = t2 - t1;
		double dtd = dt.count() * 0.001;
		return dtd;
	}
	//void testTaskflow() {
	//	Vector2f p1;
	//	
	//	for (size_t i = 0; i < 10; i++)
	//	{
	//		Box2d b2d(Vector2f(0, 0), i);
	//		
	//		INFO("m {0}, p1 {1}",timeMeasure([&]() {
	//			LoopGrid(b2d, Vector2f(1, 1), [&](const auto p) {
	//			p1 += p;
	//			}); }), p1);
	//		INFO("s {0} p1 {1}",timeMeasure([&]() {
	//			CalculStartEnd(Vector2f(1, 1), b2d);
	//			const var& lf = b2d.Min;
	//			const var& tr = b2d.Max;
	//			decltype(Box2d::Max)  xel;
	//			for (float i = lf[0]; i <= tr[0]; i++)
	//			{
	//
	//				xel[0] = i;
	//				for (float p = lf[1]; p <= tr[1]; ++p)
	//				{
	//					xel[1] = p;
	//					p1 += xel;
	//				}
	//			}}),p1);
	//	}
	//}
	void testThreadPoolAlloc() {
		lfpAlloc::Pool<sizeof(math::Vector2cd), 128> pools_;
	}
	void taskgra()
	{

		tf::Executor executor;
		tf::Taskflow tf0;
		var t1 = tf0.emplace([]() {
			for (size_t i = 0; i < 10; i++)
				LOGINFO("task1");
			});
		var t2 = tf0.emplace([]() {
			for (size_t i = 0; i < 10; i++)
				LOGINFO("task2");
			});
		var t3 = tf0.emplace([]() {
			LOGINFO("task3");
			});
		LOGINFO("unlinked");
		{
			executor.run(tf0).wait();
		}
		t1.precede(t3);
		t2.precede(t3);
		LOGINFO("linked");

		executor.run(tf0).wait();
	}
	void copyTest() {
		int num = 100000;
		std::vector<int> index(num);
		{
			std::vector<pj2::mtransfrom> trans(num), trans2(num);
			for (size_t i = 0; i < num; i++)
			{
				index[i] = std::rand();
				trans[i].pos[0] = i;
			}
			LOGINFO("start copytest");
			auto m1 = timeMeasure([&]() {
				for (size_t i = 0; i < 10000; i++)
				{
					trans2[i] = trans[index[i]];
				}
				});
			auto m2 = timeMeasure([&]() {
				for (size_t i = 0; i < 10000; i++)
				{
					trans2[i] = trans[i];
				}
				});
			INFO("ti={0}", m1);
			INFO("t2={0}", m2); }

		std::vector<int> ids1(num), ids2(num);
		for (size_t i = 0; i < num; i++)
		{
			index[i] = std::rand();
			ids1[i] = i;
		}
		LOGINFO("start copytest");
		auto m1 = timeMeasure([&]() {
			for (size_t i = 0; i < 10000; i++)
			{
				ids2[i] = ids1[index[i]];
			}
			});
		auto m2 = timeMeasure([&]() {
			for (size_t i = 0; i < 10000; i++)
			{
				ids2[i] = ids1[i];
			}
			});
		INFO("ti={0}", m1);
		INFO("t2={0}", m2);
	}
	void t0() { LOGINFO("execute t0"); }
	ay<Collid> ss2;
	ay<mtransfrom> mtrans2;
	std::unordered_set<float> map;
	std::vector<contact> ct;
	void copyvsref(std::vector<std::pair<int, int>> testp)
	{
		contact lt;
		LOGINFO("start ref test");
		timeMeasure([&]() {
			for (size_t i = 0; i < testp.size(); i++)
			{
				const var& pa = testp[i];
				const int f = pa.first;
				const int e = pa.second;
				Collid& c2 = ss2[e], & c1 = ss2[f];
				mtransfrom& t2 = mtrans2[e], & t1 = mtrans2[f];
				BoxBox2(&lt, c1, c2, t1, t2, 0.2);
			}
			});
		LOGINFO("start copy test");
		//timeMeasure([&]() {
		//	for (size_t i = 0; i < testp.size(); i++)
		//	{
		//		const var& pa = testp[i];
		//		const int f = pa.first;
		//		const int e = pa.second;
		//		Collid c2 = ss2[e], c1 = ss2[f];
		//		mtransfrom t2 = mtrans2[e], t1 = mtrans2[f];
		//		BoxBox2(&lt, c1, c2, t1, t2, 0.2);
		//		ss2[e] = c2;
		//		ss2[f] = c1;
		//		mtrans2[e] = t2;
		//		mtrans2[f] = t1;
		//	}
		//	});
		//LOGINFO("sequenence");
		//timeMeasure([&]() {
		//	for (size_t i = 1; i < testp.size(); i++)
		//	{
		//		//const var& pa = testp[i];
		//		const int f = i - 1;
		//		const int e = i;
		//		Collid c2 = ss2[e], c1 = ss2[f];
		//		mtransfrom t2 = mtrans2[e], t1 = mtrans2[f];
		//		BoxBox2(&lt, c1, c2, t1, t2, 0.2);
		//		ss2[e] = c2;
		//		ss2[f] = c1;
		//		mtrans2[e] = t2;
		//		mtrans2[f] = t1;
		//	}
		//	});
	}
	void locality(int num = 10000) {
		std::vector<uint> idx;
		idx.resize(num * 2);
		std::vector<Vector4f> ps(num), ps2(num);
		for (size_t i = 0; i < ps.size(); i++)
		{
			ps[i][0] = std::rand();
			idx[i] = std::rand() % idx.size();
		}
		LOGINFO("t1={0}", timeMeasure([&]() {
			for (size_t i = 0; i < ps.size(); i++)
			{
				var& e = ps[i];
				e[1] = i % (uint)e[0];
			}
			}));
	}
	void atomic_test() {
		std::atomic_llong mull(0), sum(0);
		std::vector<std::thread> threads;
		for (size_t i = 0; i < 4; i++)
		{

			threads.emplace_back(std::thread([&]() {

				for (size_t pi = 0; pi < 100000; pi++)
				{
					var a = mull.fetch_add(1, std::memory_order_relaxed);
					sum.fetch_add(a, std::memory_order_relaxed);
					if (pi % 10000 == 0)
						INFO("thread {0} running a={1}", std::this_thread::get_id(), a);
				}
				}));
		}
		for (auto& th : threads)
			th.join();
		INFO("thread:{0} pi:{1} sum:{2}", std::this_thread::get_id(), mull, sum);
	}
	void testPopBack() {
		std::vector<voxel*> ppp, mm;
		for (size_t i = 0; i < 10; i++)
		{
			ppp.emplace_back(new voxel());
			var& de = *ppp[i];
			de[0].id = i;
			mm.emplace_back(ppp[i]);
			ppp.pop_back();
		}
		for (size_t i = 0; i < 10; i++)
		{
			voxele ve = (*(mm[i]))[0];
			LOGINFO("mm i:{0} id:{1}", i, ve);
		}
	}
	void simdTest() {
		const int item = 1000000;
		std::vector<Paircid> ay, re;
		ay.resize(item);
		re.resize(item);
		for (size_t i = 0; i < ay.size(); i++)
		{
			ay[i].v2ll[0] = std::rand();
		}
		/*var tm = timeMeasure([&]() {
			for (size_t i = 0; i < ay.size(); i++)
			{
				re[i].e1 = ay[i] == re[i];
			}
			});
		INFO(" t1 {0}ms", tm);
		tm = timeMeasure([&]() {
			for (size_t i = 0; i < ay.size(); i++)
			{
				re[i].e1 = ay[i].v2ll == re[i].v2ll;
			}
			});
		INFO(" t2 {0}ms", tm);*/
	}
	void sortTest() {
		const int item = 100000;
		AppendArray<Paircid> re(item);
		std::vector<Paircid> ay;
		ay.resize(item);
		for (size_t i = 0; i < ay.size(); i++)
		{
			ay[i].v2ll[0] = std::rand();
			ay[i].v2ll[1] = std::rand() % 2 == 0;
			/*re[i].e1 = ay[i] == re[i];
			re[i].e1 = ay[i].v2ll == re[i].v2ll;*/
		}
		var tm = timeMeasure([&]() { std::sort(ay.begin(), ay.end(), [](const Paircid& l, const Paircid& r) {
			var b2 = l.v2ll.array() < r.v2ll.array();
			return b2[0] != 0 ? b2[0] : b2[1];
			}); });
		INFO(" sort {0}ms", tm);
		//std::vector<std::thread> threads;
		// tm = timeMeasure([&]() {
		//	for (size_t i = 0; i < 4; i++)
		//	{
		//		//bx::simd_test_all_xyzw
		//		threads.emplace_back(std::thread([&]() {
		//			int cunk = (item / 4),s=i*cunk;
		//			HashCollision::deDup(ay, s, math::min(s+cunk, item), re);
		//			}));
		//	}
		//	for (auto& th : threads)
		//		th.join();
		//	});

		//INFO(" dedup {0}ms", tm);
		/*for (size_t i = 0; i < ay.size(); i++)
		{
			ay[i].v2ll[0] = std::rand();
			ay[i].v2ll[1] = std::rand();
		}
		 tm = timeMeasure([&]() { std::sort(std::execution::par,ay.begin(), ay.end()); });
		INFO(" sort para {0}ms", tm);
		for (size_t i = 0; i < 1000; i++)
		{
			INFO("y i {0} ={1}", i, ay[i]);
		}*/
	}
	void unionTest() {
		CollideID cd;
		Paircid pd(cd, cd);
		pd.e1.id = 0;
		pd.e1.State = CollideID::CollidDS::non;
		pd.v2ll[1] = 100;
		pd.v2ll[0] = 100;
		pd.e2.id = 1000;
		pd.e1.id = 0;
		pd.e1.State = CollideID::CollidDS::Dynamic;
	}
	void findvsHash() {
		std::vector<int> com;
		com.resize(1000);

		unordered_set<int> st;
		for (size_t i = 0; i < com.size(); i++)
		{
			com[i] = std::rand();
			st.insert(com[i]);
		}
		st.insert(12);
		com[200] = 12;
		bool f = false;
		var ts = timeMeasure([&]() {f = st.find(12) != std::end(st); });
		INFO("set find={0} ms f={1}", ts, f);
		for (size_t i = 0; i < 5; i++)
		{

			com[(i + 1) * 200] = 12;
			f = false;
			var st = com.begin(), ed = com.end();
			var ts = timeMeasure([&]() {
				if (std::find(st, ed, 12) != std::end(com))
				{
					f = true;
					return;
				}
				});
			INFO(" find={0} ms f={1} ", ts, f);
			com[(i + 1) * 200] = 25;
		}
	}
	template<uint i>
	struct ecsTemp :ECS::Component0<ecsTemp<i>>
	{
		static uint id;
	};
	template<uint i>
	uint ecsTemp<i>::id;

	template <std::size_t start, std::size_t ... Is, typename F>
	void ctf_helper(std::index_sequence<Is...>, F f)
	{
		(f(Is), ...);
		((std::cout << ecsTemp<Is>::id << ' '), ...);
		//(f(Is), ...);
	}

	template <std::size_t start, std::size_t ... Is>
	void Register(std::index_sequence<Is...>)
	{
		((ecsTemp<Is>::id = Is), ...);
		((std::cout << ecsTemp<Is>::id << ' '), ...);
		//(f(Is), ...);
	}
	template <std::size_t start, std::size_t end, typename F, typename ... Ts>
	void compile_time_for(F f)
	{
		ctf_helper<start>(std::make_index_sequence<end - start>{}, f);
	}
	template <std::size_t start, std::size_t end>
	void compile_time_for(ECS::ECSManager& Emanager)
	{
		//Register<start>(std::make_index_sequence<end - start>{});
	}
	union H
	{
		struct {
			Vector4f f4;
		};
		union a
		{
			struct {
				Vector4f f4;
			};
		} physid;
	};
	int& testref(int& in) {
		return in;
	}
	void uniontest() {
		H u = { Vector4f::Zero() };
		u.f4[0] = 10;
		u.physid.f4[0] = 100;
		LOGINFO("sizeof H={0}", sizeof(H));
	}
	Controller cont;
	static vector<math::Efloat3> id2d;
	void ecstest1(int num = 100, int CreateDesType = 0) {
		math::Efloat3 v22;
		math::Efloat3 v1;
		cont.filter.ShowHash();
		constexpr int maxIns = 1000;
		auto v2 = v1;
		v2[2] = 0;
		int row = std::sqrt(num);

		std::random_device rd; // 真随机数生成器（但有些编译器没有实现），用于生成伪随机数生成器的种子
		std::mt19937 eng(rd()); // 使用梅森旋转法作为伪随机数生成器，随机程度较好
		std::uniform_int_distribution<> distrib(0, 1);
		if (id2d.size() > 0) {

			std::shuffle(id2d.begin(), id2d.end(), eng);
			LOGINFO("filter size={0}", cont.filter._posmap.size());
			for (size_t i = 0; i < id2d.size(); i++)
			{
				v2 = id2d[i];
				CollideID cd(2, 0, CollideID::CollidDS::Dynamic);
				cont.Delete(cd);
				/*if (!cont.Deletes([&](const Vox cd) {
					auto& cd1 = cont.GetPhyObj(cd);
					if (Utility::contain(cd1.collider, cd1.transform, v2.head<2>())) {
						LOGINFO("delete cd={0} \n", cd.cd);
						return true;
					}
					return false;
					}, v2))
				{
					cont.filter.ShowHash();
					LOGERROR("v2={0}", v2.transpose());
					std::abort();
				}*/
			}
			id2d.clear();
			return;
		}

		for (size_t i = 0; i < num; i++)
		{
			v2 = { (float)(i % row),i / (float)row,0 };
			v2 *= 3;
			v2.head<2>() -= math::Efloat2(row, row);
			id2d.push_back(v2);
			bool findobj = false;
			/*cont.Delete([&](const Vox cd) {
				auto& cd1 = cont.GetPhyObj(cd);
				if (Utility::contain(cd1.collider, cd1.transform, v2.head<2>())) {
					findobj = true;
					LOGINFO("delete cd={0} \n", cd.cd);
					return true;
				}
				return false;
				}, v2);*/
				/*if (!findobj) {
					Collid cd;
					cd.colliderType = ColliderType::Box;
					cd.Acceleration = 1;
					cd.MaxAngularVeocity = math::PI / 6;
					cd.MaxV(2);
					cd.InvMass = 1 / 300;
					if (distrib(eng)) {
						using bg = Unit<bug00>;
						if (bg::type == -1) {
							cont.RegistPhyobj<bg>(maxIns);
						}
						static int id = 0;
						auto& bg0 = cont.Create<bg>(cd, math::float2(2, 2), mtransfrom(v2[0], v2[1], 0));
						bg0.basic.feq = id++;

						auto& ff = cont.GetRowComps(bg::type).last<Physics::PhysicsObject<>>();
						auto pos = ff.transform.pos.transpose();
						LOGINFO("insert={0} \n", pos);
					}
					else
					{
						using bg = Unit<bug2>;
						bg f();
						if (bg::type == -1)
							cont.RegistPhyobj<bg>(maxIns);
						static int id = 0;
						auto& bg0 = cont.Create<bg>(cd, math::float2(2, 2), mtransfrom(v2[0], v2[1], 0));
						bg0.basic.feq = id++;
						auto& ff = cont.GetRowComps(bg::type).last<Physics::PhysicsObject<>>();
						LOGINFO("insert={0} \n", ff.transform.pos.transpose());
					}
					LOGINFO("insert={0} \n", v2.transpose());
				}*/
				//const float dd =math::disR(v1[2] ,(v1[2]+v2[2]));
		}
		id2d.resize(num);
		/*Collid tempcd;
		ECS::ECSManager::con.push_back(ECS::Conany());
		ECS::ECSManager::RegistEntity<GmUnit::bug0>(num);

		for (size_t i = 0; i < num; i++)
		{
			Unitbase<IType::bug0> bug;
			bug.animid = i;
			cont.create(tempcd, ColliderType::Box, bug, mtransfrom(), false);
		}
		auto bugs = ECS::ECSManager::GetComps<GmUnit::bug0>();
		for (size_t i = 0; i < bugs.size(); i++)
		{
			cont.Delete<GmUnit::bug0>(i);
		}*/
	}
	namespace FlockTy {

		/*using bg1 = Unit<bug00>;
		using bg2 = Unit<bug2>;
		using bC1 = BoidGoupController<bg1>;
		using Gb3 = AI::GoupController0<pj2::Boid3, sStruct::instData>;
		using bC2 = BoidGoupController<bg2>;*/
	}

	void AppendArrayTest() {
		AppendArray<int> aa;
		std::unordered_map<int, int> cheset;
		std::vector<std::thread> tt(4);
		int perTh = 10000;
		std::vector<int> tt1(tt.size() * perTh);
		std::vector<int> tt2(tt.size() * perTh);
		for (size_t i = 0; i < tt2.size(); i++)
		{
			tt2[i] = i;
		}

		{
			constexpr int perTh = 4;
			constexpr int bg = 0;
			constexpr int ed = 4;
			tf::Taskflow tfk;
			tf::Executor tfex;
			//auto task1=TaskEx::parallelfor(tfk,bg, ed, [&](const auto i0) {
			//	//LOGINFO("insert i0={0} THREAD ID={1}", i0, std::this_thread::get_id());

			//	for (size_t i = i0 * perTh; i < i0 * perTh + perTh; i++)
			//	{
			//		aa.push_back(i);
			//	}
			//	});
			//

			//auto task2 = TaskEx::parallelfor(tfk, bg, ed, [&](const auto i0) {
			//	//LOGINFO("rem i0={0} THREAD ID={1}", i0, std::this_thread::get_id());

			//		for (size_t i = i0 * perTh; i < i0 * perTh + perTh; i++)
			//		{
			//			if (!aa.erase(i))
			//				std::abort();
			//		}
			//	});
			//task1.second.precede(task2.first);
			for (size_t i4 = 0; i4 < 100000; i4++)
			{
				LOGINFO("loop i={0}", i4);

				//for (size_t i = 0; i < tt.size(); i++)
				//{
				//	tt[i] = std::thread([&](size_t i0) {
				//		//aa.push_back(&tt2[i0*perTh], perTh);
				//		//for (size_t i5 = 0; i5 < 1000; i5++)
				//		{
				//			for (size_t i = i0 * perTh; i < i0 * perTh + perTh; i++)
				//			{
				//				//LOGINFO("insert i={0} i0={1} THREAD ID={2}", i, i0,std::this_thread::get_id());
				//				aa.push_back(i);
				//			}
				//		}
				//		}, i);
				//}
				tfex.run(tfk).wait();
				//for (size_t i = 0; i < tt.size(); i++)
				//{
				//	tt[i] = std::thread([&](size_t i0) {
				//		//aa.push_back(&tt2[i0*perTh], perTh);
				//		//for (size_t i5 = 0; i5 < 1000; i5++)
				//		{

				//			//LOGINFO("aac={0}",aa.size());
				//			for (size_t i = i0 * perTh; i < i0 * perTh + perTh; i++)
				//			{
				//				if (!aa.erase(i))
				//					std::abort();
				//			}
				//		}
				//		}, i);
				//}
				//for (size_t i = 0; i < tt.size(); i++)
				//{
				//	tt[i].join();
				//}
			}

			for (size_t i = 0; i < tt1.size(); i++)
			{
				assert(tt1[i] != -1);
			}
		}

		{
			for (size_t i = 0; i < tt.size(); i++)
			{
				tt[i] = std::thread([&](size_t i0) {

					INFO("thread id={0}", std::this_thread::get_id());
					//aa.push_back(&tt2[i0*perTh], perTh);
					for (size_t i = i0 * perTh; i < i0 * perTh + perTh; i++)
					{
						aa.push_back((int)i);
					}
					}, i);
			}
			std::memset(tt1.data(), -1, tt1.size() * sizeof(int));
			for (size_t i = 0; i < tt.size(); i++)
			{
				tt[i].join();
			}
			for (size_t i = 0; i < tt1.size(); i++)
			{
				auto& reco = tt1[aa[i]];
				if (reco != -1)
					std::abort();
				reco = 0;
			}
			for (size_t i = 0; i < tt1.size(); i++)
			{
				assert(tt1[i] != -1);
			}
		}
		{
			aa.clear();
			for (size_t i = 0; i < tt.size(); i++)
			{
				tt[i] = std::thread([&](size_t i0) {
					INFO("thread id={0} Gp", std::this_thread::get_id());
					for (size_t i = 0; i < perTh; i += 100)
					{

						aa.push_back(&tt2[i0 * perTh + i], 100);
					}
					}, i);
			}
			std::memset(tt1.data(), -1, tt1.size() * sizeof(int));
			for (size_t i = 0; i < tt.size(); i++)
			{
				tt[i].join();
			}
			for (size_t i = 0; i < tt1.size(); i++)
			{
				auto& reco = tt1[aa[i]];
				if (reco != -1)
					std::abort();
				reco = 0;
			}
		}
		LOGINFO("aa.size() i={i}", aa.size());
	}
	namespace UnitTest {
		int instannum = 100;
		void quaternion() {
			math::quaternion q(math::AngleAxisf(math::DegToRad(30), math::Vector3f(0, 0, 1)));
			math::quaternion q2(math::AngleAxisf(math::DegToRad(90), math::Vector3f(0, 0, 1)));
			auto q2v = math::quaternion::FromTwoVectors(math::Vector3f(0.5, 0.5, 0), math::Vector3f(1, 0, 0));
			auto y = math::Vector3f(1, 0, 0);
			auto yx = q2v * y;
			LOGINFO("yx={0} yx2={1}", yx.transpose(), (q2 * y).transpose());
			auto red = q.angularDistance(q2v);
			auto a1 = math::RadToDeg(red);
			red = q2v.angularDistance(q);
			a1 = math::RadToDeg(red);
			red = q2v.angularDistance(q2);
			a1 = math::RadToDeg(red);
			red = q.angularDistance(q2);
			auto a2 = math::RadToDeg(red);
			auto a3 = math::RadToDeg(q.angularDistance(q2v * q));
			auto a4 = math::RadToDeg(q.angularDistance(q * q2v));
		}
		void entttest() {
			//entt::registry registry, registry2;
			//using position = math::vec4<float>;
			//using pos16 = std::array<math::vec4<float>, 50>;
			//using velocity = math::Vector<INT64, 2>;
			//const auto group2 = registry.group<position, velocity, comp<0>, comp<1>, comp<2>, pos16, posi<2>>();
			//nonstd::span<std::tuple< position, velocity, pos16, comp<0>, comp<1>, comp<2>, posi<2>>> obj;
			//std::vector<std::tuple< position, velocity, pos16, comp<0>, comp<1>, comp<2>, posi<2>>> obj1(1000);
			//obj = obj1;
			//for (std::uint64_t i = 0; i < obj.size(); i++) {
			//	const auto entity = registry.create();
			//	registry.emplace<position>(registry2.create());
			//	posi<2>& po2 = std::get<posi<2>>(obj[i]);
			//	po2.dd[0] = (float)i;
			//	registry.emplace<posi<2>>(entity) = po2;
			//	registry.emplace<velocity>(entity);
			//	registry.emplace<pos16>(entity);
			//	registry.emplace<comp<0>>(entity);
			//	registry.emplace<comp<1>>(entity);
			//	registry.emplace<comp<2>>(entity);
			//}

			//auto cp0s = group2.raw<comp<0>>();
			//auto cp01 = group2.raw<position>();
			//for (size_t i = 0; i < group2.size(); i++)
			//{
			//	auto&& [c0, p0] = std::make_tuple(cp0s[i], cp01[i]);
			//	c0.x = 9799;
			//	p0[3] = 9797;
			//	const auto& [vel, pos] = group2.get<velocity, posi<2>>(group2[i]);
			//	vel[0] = i;
			//	vel[1] = 2 << i;
			//	pos[0] = i;
			//	cp0s[i].x = std::rand();
			//	pos[3] = std::rand();
			//}
			////group.
			//auto test = [&](auto&& func) {
			//	timer timer;
			//	func();
			//	timer.elapsed();
			//};
			//auto pp = group2.raw<position>();
			//auto p2 = group2.raw<posi<2>>();
			//test([&]() {
			//	for (size_t i = 0; i < obj.size(); i++) {
			//		pp[i] = pp[i].cross3(p2[i].dd);
			//	}
			//	});
			//test([&]() {
			//	for (size_t i = 0; i < obj.size(); i++) {
			//		auto& cobj = obj[i];
			//		auto& po1s3 = std::get<position>(cobj);
			//		auto& po1s = std::get<posi<2>>(cobj);
			//		po1s3 = po1s3.cross3(po1s.dd);
			//	}
			//	});
			//for (size_t i = 0; i < group2.size(); i++)
			//{
			//	const auto& [pos, cpo0] = group2.get<velocity, comp<0>>(group2[i]);
			//	LOGINFO("vel={0}, vel={1}", pos.transpose(), cpo0.x);
			//}
		}
		void flocktest() {
			using namespace FlockTy;
#ifndef DEBUG
			instannum = 1000;
#endif // DEBUG
			int block1 = 0;
			int block2 = 0,numpg=100,scl=10;
			int maxIns = instannum,gnum=1;
			instannum = 0;
			int row = std::sqrt(instannum);
			std::random_device rd; // 真随机数生成器（但有些编译器没有实现），用于生成伪随机数生成器的种子
			std::mt19937 eng(rd()); // 使用梅森旋转法作为伪随机数生成器，随机程度较好
			std::uniform_int_distribution<> distrib(0, 1);
			//if (bg1::type == -1)
			//{
			//	cont.Regist<ammu>(2);
			//	//int (*Update)(Controller&, const uint)  = NULL;
			//	//cont.Regist<Boid3>();
			//	cont.Regist<bg1>(maxIns);
			//	cont.Regist<bg2>(maxIns);
			//	cont.Regist<FlockTy::bC1>(2);
			//	cont.Regist<FlockTy::bC2>(2);
			//	Controller::tinfo fc;
			//	fc.fcs.Update = FlockTy::Gb3::Update;
			//	//fc.CanRender = true;
			//	cont.Regist<FlockTy::Gb3>(2,CollideID::CollidDS::Dynamic,32,fc);
			//	//auto& amu=cont.EmplaceR<ammu>();
			//	//auto fc = [](void* ins, Paircid0& pir) {};
			//	////amu.RegistDesEv((void*)&amu, fc);
			//	//bool ex = amu.Eve;
			//}
			//GraphicUtil::quad qud;
			//gnum = math::min(qud.Vertexs.size(), gnum);
			//for (size_t i = 0; i < gnum; i++)
			//{
			//	auto v = qud.Vertexs[i]*scl;
			//	auto& gc3=cont.Emplace<FlockTy::Gb3>(cont,mtransfrom::Build(v),1.414,numpg);
			//	mtransfrom::v4 _target;
			//	instannum += numpg;
			//	/*Utility::EleSet(_target, -v);
			//	_target.head<3>()=v.cast< mtransfrom::v4::Scalar>();*/
			//	gc3.SetTarget(-v);
			//}
			//Utility::bitmark di1(0);
			//Utility::bitmark di2(1);
			//auto& bCon1 = cont.GetorCreate<bC1>();
			//auto& bCon2 = cont.GetorCreate<bC2>();

			/*bCon1.blocksize = block1;
			bCon2.blocksize = block2;*/
			int sign = -1;
			//for (size_t i = 0; i < instannum; i++)
			//{
			//	math::Efloat3 v2 = { (float)(i % row),i / (float)row,0 };
			//	v2 *= 3;
			//	v2.head<2>() -= math::Efloat2(row, row);
			//	sign = -sign;
			//	v2.head<2>() += sign * Vector2f(12, 12);
			//	bool findobj = false;
			//	/*cont.Delete([&](const Vox cd) {
			//		auto& cd1 = cont.GetPhyObj(cd);
			//		if (Utility::contain(cd1.collider, cd1.transform, v2.head<2>())) {
			//			findobj = true;
			//			LOGINFO("delete cd={0} \n", cd.cd);
			//			return true;
			//		}
			//		return false;
			//		}, v2);*/
			//		//if (!findobj) 
			//	{
			//		Collid cd;
			//		cd.colliderType = ColliderType::Box;
			//		cd.Acceleration = 1;
			//		cd.MaxAngularVeocity = math::PI / 6;
			//		cd.MaxV(4);
			//		cd.SetMass(10);
			//		//cont.Emplace();
			//		/*if (distrib(eng))
			//		{
			//			using bg = Unit<bug00>;
			//			static int id = 0;
			//			auto& bg0 = cont.Create<bg>(cd, math::float2(2, 2), mtransfrom(v2[0], v2[1], 0));
			//			bg0.basic.feq = id++;
			//			bg0.bitgroupid = di1;
			//			auto& ff = cont.GetRowComps(bg::type).last<Physics::PhysicsObject<>>();
			//			auto pos = ff.transform.pos.transpose();
			//			block1++;
			//			//LOGINFO("insert={0} \n", pos);
			//		}
			//		else
			//		{
			//			using bg = Unit<bug2>;
			//			static int id = 0;
			//			auto& bg0 = cont.Create<bg>(cd, math::float2(2, 2), mtransfrom(v2[0], v2[1], 0));
			//			bg0.basic.feq = id++;
			//			bg0.bitgroupid = di2;
			//			auto& ff = cont.GetRowComps(bg::type).last<Physics::PhyObj>();
			//			block2++;
			//			LOGINFO("insert={0} \n", ff.transform.pos.transpose());
			//		}*/
			//	}
			//}
		}
		template<class T>
		void Measure(const T fc, const std::string id = "measure") {
			var t1 = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now()).time_since_epoch();
			fc();
			var t2 = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now()).time_since_epoch();
			var dt = t2 - t1;
			//LOGINFO("{0} = {1} ms",55,  dt * 0.001);
			spdlog::info("{0} {1} ms", id, (double)(dt.count() * 0.001));
		}
		bool operator==(const aroundele& lhs, const aroundele& rhs) {
			return std::get<1>(lhs) == std::get<1>(rhs);
		}
		void parallelfortest() {
			using tsT = size_t;
			std::vector <ECS::Conany*>ta;
			int subz = 400;
			/*tf::Executor executor;
			tf::Taskflow taskflow;
			taskflow.clear();
			auto t1 = TaskEx::parallelfor(taskflow, [&](const auto i, ECS::Conany& buk) {
				buk.Get<tsT>(i) = buk.size()* buk.tid+ i;
			}, ta, 100);
			auto t2 = TaskEx::parallelfor(taskflow, [&](const auto i, ECS::Conany& buk) {
				assert(buk.Get<tsT>(i) ==( buk.size() * buk.tid + i));
			}, ta, 100);
			t1.precede(t2);
			for (size_t i = 0; i < 100; i++)
			{
				executor.run(taskflow).wait();
			}*/
		}
		void DeDupTest() {
			AppendArray<aroundele> dl;
			std::vector<aroundele> fixed;
			struct KeyEqual
			{
				inline std::size_t operator()(const aroundele& lhs, const aroundele& rhs)const {
					return reinterpret_cast<const size_t>(std::get<1>(lhs)) == reinterpret_cast<const size_t>(std::get<1>(rhs));
				}
			};
			struct hasher
			{
				inline std::size_t operator()(aroundele const& s) const noexcept
				{
					return reinterpret_cast<const size_t>(std::get<1>(s));
				}
			};
			std::random_device rd; // 真随机数生成器（但有些编译器没有实现），用于生成伪随机数生成器的种子
			std::mt19937 eng(rd()); // 使用梅森旋转法作为伪随机数生成器，随机程度较好
			std::uniform_int_distribution<> distrib(0, 1);
			std::unordered_set<aroundele, hasher, KeyEqual> ua;
			for (size_t i = 0; i < 39; i++)
			{
				if (i % 2 == 0)
					fixed.emplace_back(std::make_tuple(Vox(i - 1), (void*)(i - 1)));
				else
					fixed.emplace_back(std::make_tuple(Vox(i), (void*)i));
			}
			int accallynum = 0;
			KeyEqual ke;
			for (size_t i = 0; i < fixed.size(); i++)
			{
				for (size_t i2 = i + 1; i2 < fixed.size(); i2++)
				{
					if (ke(fixed[i], fixed[i2])) {
						accallynum++;
					}

				}
			}
			auto fix2 = fixed;
			std::sort(fix2.begin(), fix2.end(), [](const aroundele& lhs, const aroundele& rhs) {
				return reinterpret_cast<const size_t>(std::get<1>(lhs)) < reinterpret_cast<const size_t>(std::get<1>(rhs));
				});
			for (size_t i = 0; i < 1000; i++)
			{
				dl.clear();
				dl.push_back(fixed.data(), fixed.size());
				Measure([&]() {dl.DeDup1(ua); }, "DeDup1");
				std::shuffle(fixed.begin(), fixed.end(), eng);
			}
			std::sort(&dl[0], &dl[dl.size() - 1], [](const aroundele& lhs, const aroundele& rhs) {
				return reinterpret_cast<const size_t>(std::get<1>(lhs)) < reinterpret_cast<const size_t>(std::get<1>(rhs));
				});
			for (size_t i = 0; i < 1000; i++)
			{
				dl.clear();
				dl.push_back(fixed.data(), fixed.size());
				Measure([&]() {
					dl.DeDup2([](const aroundele& lhs, const aroundele& rhs) {
						return reinterpret_cast<const size_t>(std::get<1>(lhs)) < reinterpret_cast<const size_t>(std::get<1>(rhs));
						}, ke);
					}, fmt::format("DeDup2 i={0} ", i));
				std::shuffle(fixed.begin(), fixed.end(), eng);
			}
		}
	}

	shared_ptr< tf::TFProfObserver>observer;
	inline void Start(int num = 1)
	{
		return;

		/*{
			std::vector<Vector2i> sop;
			sop.reserve(100000);
			int r = 30;
			for (size_t i0 = 30; i0 < 100; i0++)
			{
				sop.clear();
				LOGINFO("r={0}", i0);
				auto t = timeMeasure([&]() {
					Utility::circle(sop, 0, 0, i0);
					});
				LOGINFO("t1={0} ms size={1}", t, sop.size());
				r = i0;
				sop.clear();
				t = timeMeasure([&]() {
					const auto l1 = -r;
					const auto l22 = r + 1;
					auto i = l1;
					for (; i < l22; i++)
					{
						for (auto i2 = l1; i2 < l22; i2++)
						{
							sop.emplace_back(i, i2);
						}
					}
					});

				LOGINFO("t2={0} ms size={1}", t, sop.size());
			}}*/
		Vector2f f2;
		f2[0] = 12;
		ecstest1();
		//LOGINFO(" group tr sz={0} et={1} bp {2} cp {3}", f2[0], f2[1],po1::_pool.capacity(),po2::_pool.capacity());
		//ecstest<ECS::Group>();
		cds::Initialize();
		//cds::gc::DHP hpGC;
		//rcugc ruc;
		// If main thread uses lock-free containers
		// the main thread should be attached to libcds infrastructure
		cds::threading::Manager::attachThread();
		timeMeasure(t0);
		//	var bxs = static_cast<IBoxCollider*>(malloc(sizeof(IBoxCollider) * num));
		Collid tcd;
		int outid;
		//IOFormat CommaInitFmt(FullPrecision, DontAlignCols);
		//for (size_t i = 0; i < num; i++)
		//{
		//	mtransfrom v;

		//	v.rotation.setIdentity();
		//	float f = (i * 3);
		//	const var colnum = 10;
		//	var mod = i % colnum;
		//	v.pos = Vector3f((i / colnum) * 3, mod * 3, 0);
		//	var bxcd=wod.Creat(ColliderType::Box,v, outid);
		//	//var bxcd = IBoxCollider::Creat(Vector2f(2, 2), 10, false);
		//	bxcd.InvMass = 0;
		//	bxcd.detectRange = 0;
		//	bxcd.Velocity.setZero();
		//	bxcd.AngularVelocity.setZero();
		//	bxcd.Velocity = (v.pos.normalized()) * 0.5;
		//	//LOGINFO("i {0} inv {1} {2}", i, ss[i].InvMass, f);
		//}
		///*var &cd0 = wod.getCD(CollideID(0, CollideID::CollidDS::Dynamic));
		//cd0.Velocity[1] = 0;
		//cd0.InvMass = 1;*/
		//Vector2f cdidff(11, 2);
		//var& posmp = wodptr->_posmap;
		//posmp.insert(cdidff, new voxel());
		//LOGINFO("_posmap size={0}", posmp.size());
		//posmp.erase(cdidff);
		//LOGINFO("_posmap size={0}", posmp.size());
		//observer = (HashCollision::instance->executor.make_observer<tf::ExecutorObserver>());
	}

	//template<typename T>
	//inline size_t tbb_hasher(const T& t) {
	//	return static_cast<size_t>(t) * internal::hash_multiplier;
	//}
	////const var testcellsize = [](const var a, const var b, var& d, var& e) {
	//
	//	Vector2f CELL(2, 2);
	//	Vector2f s(-11, 0), p;
	//	Box2d box(Vector2f(1.1, 2.2), 1);
	//	CalculStartEndIterationInternal(CELL, box, s, p);
	//	LOGINFO("v1={0} v2={1}", fmt::join(s, ","), fmt::join(p, ","));
	//};
	void exit()
	{
		std::ofstream outfile("test.json");
		outfile << observer->dump() << std::endl;;
		outfile.close();
		spdlog::shutdown();
		cds::threading::Manager::detachThread();
		cds::Terminate();
	}
	
}