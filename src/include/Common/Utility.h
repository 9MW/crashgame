#pragma once
#include"common/Math1.h"
#include <functional>
#include <queue>
#include <stack>
#include<thread>
#include<mutex>
#include"Log/log.h"
namespace Utility {
	template<int i>
	using constexi = std::integral_constant<int, i>;
#define f(I)if constexpr(I<IMAX) fun(std::integral_constant<std::size_t, I>());
	template<class F, class I, std::size_t  IMAX=100>
	inline void Switch100(F&& fun, const I& i, std::integral_constant<std::size_t, IMAX>()) {
		assert(i < 100);
		switch (i) { case 0:f(0); break; case 1:f(1); break; case 2:f(2); break; case 3:f(3); break; case 4:f(4); break; case 5:f(5); break; case 6:f(6); break; case 7:f(7); break; case 8:f(8); break; case 9:f(9); break; case 10:f(10); break; case 11:f(11); break; case 12:f(12); break; case 13:f(13); break; case 14:f(14); break; case 15:f(15); break; case 16:f(16); break; case 17:f(17); break; case 18:f(18); break; case 19:f(19); break; case 20:f(20); break; case 21:f(21); break; case 22:f(22); break; case 23:f(23); break; case 24:f(24); break; case 25:f(25); break; case 26:f(26); break; case 27:f(27); break; case 28:f(28); break; case 29:f(29); break; case 30:f(30); break; case 31:f(31); break; case 32:f(32); break; case 33:f(33); break; case 34:f(34); break; case 35:f(35); break; case 36:f(36); break; case 37:f(37); break; case 38:f(38); break; case 39:f(39); break; case 40:f(40); break; case 41:f(41); break; case 42:f(42); break; case 43:f(43); break; case 44:f(44); break; case 45:f(45); break; case 46:f(46); break; case 47:f(47); break; case 48:f(48); break; case 49:f(49); break; case 50:f(50); break; case 51:f(51); break; case 52:f(52); break; case 53:f(53); break; case 54:f(54); break; case 55:f(55); break; case 56:f(56); break; case 57:f(57); break; case 58:f(58); break; case 59:f(59); break; case 60:f(60); break; case 61:f(61); break; case 62:f(62); break; case 63:f(63); break; case 64:f(64); break; case 65:f(65); break; case 66:f(66); break; case 67:f(67); break; case 68:f(68); break; case 69:f(69); break; case 70:f(70); break; case 71:f(71); break; case 72:f(72); break; case 73:f(73); break; case 74:f(74); break; case 75:f(75); break; case 76:f(76); break; case 77:f(77); break; case 78:f(78); break; case 79:f(79); break; case 80:f(80); break; case 81:f(81); break; case 82:f(82); break; case 83:f(83); break; case 84:f(84); break; case 85:f(85); break; case 86:f(86); break; case 87:f(87); break; case 88:f(88); break; case 89:f(89); break; case 90:f(90); break; case 91:f(91); break; case 92:f(92); break; case 93:f(93); break; case 94:f(94); break; case 95:f(95); break; case 96:f(96); break; case 97:f(97); break; case 98:f(98); break; case 99:f(99); break; }
	}
	template<std::size_t  IMAX, class F, class I >
	inline void Switch100(F&& fun, const I& i) {
		assert(i < 100);
		switch (i) { case 0:f(0); break; case 1:f(1); break; case 2:f(2); break; case 3:f(3); break; case 4:f(4); break; case 5:f(5); break; case 6:f(6); break; case 7:f(7); break; case 8:f(8); break; case 9:f(9); break; case 10:f(10); break; case 11:f(11); break; case 12:f(12); break; case 13:f(13); break; case 14:f(14); break; case 15:f(15); break; case 16:f(16); break; case 17:f(17); break; case 18:f(18); break; case 19:f(19); break; case 20:f(20); break; case 21:f(21); break; case 22:f(22); break; case 23:f(23); break; case 24:f(24); break; case 25:f(25); break; case 26:f(26); break; case 27:f(27); break; case 28:f(28); break; case 29:f(29); break; case 30:f(30); break; case 31:f(31); break; case 32:f(32); break; case 33:f(33); break; case 34:f(34); break; case 35:f(35); break; case 36:f(36); break; case 37:f(37); break; case 38:f(38); break; case 39:f(39); break; case 40:f(40); break; case 41:f(41); break; case 42:f(42); break; case 43:f(43); break; case 44:f(44); break; case 45:f(45); break; case 46:f(46); break; case 47:f(47); break; case 48:f(48); break; case 49:f(49); break; case 50:f(50); break; case 51:f(51); break; case 52:f(52); break; case 53:f(53); break; case 54:f(54); break; case 55:f(55); break; case 56:f(56); break; case 57:f(57); break; case 58:f(58); break; case 59:f(59); break; case 60:f(60); break; case 61:f(61); break; case 62:f(62); break; case 63:f(63); break; case 64:f(64); break; case 65:f(65); break; case 66:f(66); break; case 67:f(67); break; case 68:f(68); break; case 69:f(69); break; case 70:f(70); break; case 71:f(71); break; case 72:f(72); break; case 73:f(73); break; case 74:f(74); break; case 75:f(75); break; case 76:f(76); break; case 77:f(77); break; case 78:f(78); break; case 79:f(79); break; case 80:f(80); break; case 81:f(81); break; case 82:f(82); break; case 83:f(83); break; case 84:f(84); break; case 85:f(85); break; case 86:f(86); break; case 87:f(87); break; case 88:f(88); break; case 89:f(89); break; case 90:f(90); break; case 91:f(91); break; case 92:f(92); break; case 93:f(93); break; case 94:f(94); break; case 95:f(95); break; case 96:f(96); break; case 97:f(97); break; case 98:f(98); break; case 99:f(99); break; }
	}
#undef f
	using namespace ::Utility;
	template <class... _Types>
	struct tup {
		using t_tup =
			std::tuple<_Types...>;
		tuple<_Types&&...> d;
		tup(t_tup&& tp) :d(std::forward<t_tup&&>(tp)) {}
		tup(_Types&&... _Args) :d(std::forward<_Types>(_Args)...) {}
		/* template<math::ui64 i>
		 constexpr auto&& get() {
			 return std::get<i>(d);
		 }*/
		template<class T>
		constexpr auto&& get() {
			return ((std::get<T&>(d)));
		}
		template<std::size_t I1, std::size_t I2, std::size_t... I>
		constexpr auto get() {
			return std::forward_as_tuple(std::get<I1>(d), std::get<I2>(d), std::get<I>(d)...);
		}
		template<std::size_t I1>
		constexpr auto&& get() {
			return (std::get<I1>(d));
		}
		//class T0, , class T1, 
	  /*  template <class... _Types1, math::ui64 kjk=sizeof...(_Types1) >
		constexpr auto get() {
			return std::make_tuple(std::move(std::get<_Types1>(d))...);
		}*/
		template <class... _Types1>
		constexpr auto getref() {
			//if constexpr (sizeof ...(_Types1) > 1)
			return std::forward_as_tuple(get<_Types1>()...);
			/* else
				 return std::get<0>(d);*/
		}
		constexpr t_tup& getBind() {
			return d;
		}
		static constexpr auto size() {
			return sizeof...(_Types);
		}
		/*private:
			template <class... _Types1>
			constexpr auto getref1() {
				return std::forward_as_tuple((std::get<_Types1&>(d))...);
			}*/
	};
	template <class... _Types>
	constexpr  tup<_Types...> make_tup(_Types&&... _Args) { // forward arguments in a tuple
		return tup<_Types...>(std::forward<_Types>(_Args)...);
	}
	template<class ...Ts>
	struct MyTuple :std::tuple<Ts...>
	{
		using tuple = std::tuple<Ts...>;
		constexpr MyTuple(const tuple& ut) { new (this)tuple(ut); }
		//constexpr MyTuple(tuple&& ut) { new (&ut)tuple(ut); }
		MyTuple() = default;
		//using std::tuple<Ts...>::tuple<Ts...>;
		template<class T>
		constexpr auto&& get() {
			return ((std::get<T>(*this)));
		}


		template <typename... Ts, typename std::enable_if_t<sizeof...(Ts) >= 2, bool> = true>
		constexpr auto get() {
			return std::forward_as_tuple(std::get<Ts>(*this)...);
		}
		/*template <
			typename... Ts,
			typename std::enable_if_t<sizeof...(Ts) >= 2, bool> = true>
		constexpr auto&& get() {
			return std::forward_as_tuple(std::get<Ts>(*this)...);
		}*/
		template <class... _Types1>
		constexpr auto getrefs() {
			return std::forward_as_tuple(std::get<_Types1>(*this)...);
		}
		template<ui64 I>
		constexpr auto&& get() {
			return ((std::get<I>(*this)));
		}
		inline operator tuple& () { return *this; }
		inline tuple& getbind() { return *this; }
		constexpr static int tuplesize() { return sizeof...(Ts); }
	};
	template<class ...Ts>
	struct MyTuple<std::tuple<Ts...>> :MyTuple<Ts...> {};
	template<typename T>
	inline void update_max(std::atomic<T>& atom, const T val)
	{
		for (T atom_val = atom;
			atom_val < val &&
			!atom.compare_exchange_weak(atom_val, val, std::memory_order_relaxed);
			);
	}
	//using namespace std;
	template<typename T>
	constexpr void removeResize(T& v, int i) {
		const auto end = v.size() - 1;
		v[i] = v[end];
		v.resize(end);
	}

	template<class Tdis, class ...Ts>
	constexpr void copytup(std::tuple<Ts...>& src, Tdis& dis) {
		//dis = src;
		((std::get<Ts&>(dis) = std::get<Ts>(src)), ...);
	}
	template<class ...Ts, class TD>
	constexpr void copy(TD& dest, Ts& ...args) {
		((std::get<Ts&>(dest) = args), ...);
	}
	template<typename T, typename Cmp, int N>
	class float_priority_queue {
	private:
		std::priority_queue<T, std::vector<T>, Cmp> priq_;
		Cmp cmp_;
	public:
		float_priority_queue(Cmp cmp)
			: priq_(cmp), cmp_(cmp)
		{}

		void push(T t) {
			if (priq_.size() == N) {
				if (cmp_(t, priq_.top())) {
					priq_.pop();
					priq_.push(t);
				}
			}
			else {
				priq_.push(t);
			}
		}

		std::array<T, N> dump() {
			std::array<T, N> ary;
			int i = 0;
			while (!priq_.empty()) {
				ary[i] = priq_.top();
				priq_.pop();
				i++;
			}
			return ary;
		}
	};
	struct atomll
	{
		atomic_llong c;
		atomll(const atomll& a) { std::memcpy(this, &a, sizeof(atomll)); }
	};
	template<class T>
	inline auto VecByteSize(std::vector<T>& vv) {
		return vv.size() * sizeof(T);
	}
	//template<class T, size_t _Size>
	struct FixAy
	{
		using T = int;
		static constexpr int _Size = 30;
		std::array<T, _Size> datas;
		std::atomic_int counter = 0,map=0;
		//std::atomic_bool _Wlock;
		//int map;
		//T d[31];
		//T sz;
		FixAy() {
			if constexpr (std::is_convertible_v<T, int>) {
				std::memset(this, 0, sizeof(*this));
			}
		}
		FixAy(const FixAy&l) {
			std::memcpy(this, &l, sizeof(*this));
		}
		FixAy& operator=(const FixAy& l) {
			std::memcpy(this, &l, sizeof(*this));
			return *this;
		}
		constexpr T& operator[](const std::size_t& i) {
			return datas[i];
		}
		bool add(const T& v);
		inline auto& size()const {
			return counter._Storage._Value;
		}
		inline bool full() {
			return datas[datas.size() - 1] != 0;
		}
		inline void clear() {
			std::memset(datas.data(), 0, sizeof(datas));
		}
		bool erase(const T& v);
	};
	template<typename T, int gridmaxsize = 32>
	struct spa
	{
		//std::mutex g_pages_mutex;
		std::atomic_long count,map;
		T ele[gridmaxsize];
		//int ctunsafe = 0;
		std::atomic_bool  inside, removeing;
		spa() :inside(false), removeing(false) {
			count.store(0, std::memory_order_relaxed);
		}
		//spa<T>* init() {
		//	return new (this)spa<T>;
		//	/*inside.store(false);
		//	count.store(0, std::memory_order_relaxed);
		// 	std::constr g_pages_mutex
		//	return this;*/
		//}
		spa(const spa& sp) :removeing(false) {
			count.store(sp.count, std::memory_order_relaxed);
			std::memcpy((void*)&sp, this, sizeof(spa));
			sp.inside._Storage._Value = ((bool)(this->inside));
		}
		inline T& operator[](size_t i) {
			assert(i < gridmaxsize&& i >= 0);
			return (ele[i]);
		}
		constexpr void swaperase(const int i) {
			//const std::lock_guard<std::mutex> lock(g_pages_mutex);
			ele[i] = ele[count.fetch_add(-1, std::memory_order_relaxed)];
		}
		spa& operator=(spa& a) {
			/*std::memcpy(this->ele, a.ele, sizeof(voxele) * a.count);
			this->count =(long) a.count;*/
			return *this;
		}
		template<bool unsafe = false, typename A>
		inline bool erase(const A& t);
		//{
		//	if constexpr (unsafe) {
		//		int i = 0, i2 = count.load(std::memory_order_relaxed);
		//	Restart:
		//		for (; i < i2; i++)
		//		{
		//			if (ele[i] == t) {

		//				bool resz = false;
		//				while (!removeing.compare_exchange_weak(resz, true)) {
		//					resz = false;
		//					if (i >= count) {
		//						i = 0;
		//						goto Restart;
		//					}
		//					std::this_thread::yield();
		//				}

		//				if (i >= count) {
		//					removeing = false;
		//					i = 0;
		//					goto Restart;
		//				}
		//				//counter.fetch_add(-1,std::memory_order_relaxed);
		//				ele[i] = ele[count - 1];
		//				count.fetch_add(-1, std::memory_order_release);
		//				/*LOGINFO("i={0} counter={1} t={2} i2={3} thread id={4} removeing={5} resz={6}",
		//					i, counter, t, i2, std::this_thread::get_id(), removeing, resz);*/
		//				removeing = false;
		//				//assert(!removeing);
		//				return true;
		//			}
		//		}
		//	}
		//	else
		//	{
		//		for (size_t i = 0; i < count; i++)
		//		{
		//			if (ele[i] == t) {
		//				ele[i] = ele[count - 1];
		//				count._Storage._Value--;// .fetch_add(-1, std::memory_order_relaxed);
		//				return true;
		//			}
		//		}
		//	}
		//	return false;
		//}
		template<typename A, class B>
		inline bool eraseunsafe(const A& t, B&& eq) {
			for (size_t i = 0; i < count; i++)
			{
				if (eq(ele[i], t)) {
					ele[i] = ele[count - 1];
					count._Storage._Value--;// .fetch_add(-1, std::memory_order_relaxed);
					return true;
				}
			}
			return false;
		}
		void clear() {
			count = 0;
		}
		constexpr int begin() {
			return 0;
		}
		inline int remain() {
			return gridmaxsize - count;
		}
		inline int size() {
			return count;
		}
		int end() {
			return count;
		}
		static constexpr int _Size = gridmaxsize;
		bool empty() const {
			return count.load(std::memory_order_relaxed) == 0;
		}
		inline bool contain(const  T v)const {
			for (size_t i = 0; i < count; i++)
			{
				if (v == ele[i])
					return true;
			}
			return false;
		}
		inline bool insert(const T& v);
		using sz = size_t;
		template <typename... Args>
		sz emplace_back(Args&&... args) {
			const auto countl = count.fetch_add(1, std::memory_order_relaxed);
			//assert(countl < gridmaxsize&& countl >= 0);
			new (ele + countl) T(std::forward<Args>(args)...);
			return countl < gridmaxsize;
		}

	};
	template<class T, bool safe = false>
	struct MuQue
	{
		std::vector<T> da;
		std::mutex _mutex;
		MuQue(const size_t reserv = 256) {
			da.resize(reserv);
		}
		MuQue(const MuQue<T>& v) {
			this->da = v.da;
		}
		void push(T&& r) {
			if constexpr (!safe) {
				auto pi = size++;
				if (pi < da.size()) {
					da[pi] = r;
					return;
				}
				const std::lock_guard<std::mutex>guard(_mutex);
				da.emplace_back(r);
				return;
			}
			da.emplace_back(r);
		}

		void push(const T& r) {
			if constexpr (!safe) {
				auto pi = size++;
				if (pi < da.size()) {
					da[pi] = r;
					return;
				}
				const std::lock_guard<std::mutex>guard(_mutex);
				da.emplace_back(r);
				return;
			}
			da.emplace_back(r);
		}
		bool try_pop(T& r) {
			if constexpr (!safe) {
				//const std::lock_guard<std::mutex>guard(_mutex);
				auto i = size--;
				if (i >= 0) {
					r= da[i];
					return true;
				}
				return false;
			}
			if (da.size() == 0)
				return false;
			r = da.back();
			da.pop_back();
			return true;
		}
		template<class T1>
		void for_each(T1&& fc) {
			for (auto&& item : da)
			{
				fc(item);
			}
		}
		void clear() {
			da.clear();
		}
	protected:
		std::atomic_int64_t size = 0;
	};

	template<class T>
	struct DyQue
	{
		std::vector<T> da;
		DyQue(const size_t cap = 256) :size(size) {
			da.resize(cap);
		}
		DyQue(const DyQue<T>& v) {
			this->da = v.da;
			std::abort();
		}
		bool push(T& r) {
			auto p = size++;
			if (p >= da.capacity()) [[unlikely]] {
				return false;
			}
			da.emplace_back(r);
			return true;
		}
		bool try_pop(T& r) {
			auto p = size--;
			if (size == 0)
				return false;
			r = da[p];
			return true;
		}
		/*template<class T1>
		void for_each(T1&& fc) {
			for (auto&& item : da)
			{
				fc(item);
			}
		}*/
		void clear() {
			size = 0;
		}
	protected:
		std::atomic_int64_t size;
	};
	template<typename T, std::size_t size>
	struct RingQue
	{
		using value_type = T;
		//std::mutex g_pages_mutex;
		std::atomic_int64_t front, back, sz;
		std::array<T, size>ele;
		std::atomic_bool  inside, removeing;
		RingQue() :inside(false), removeing(false) {
			front = 0; back = 0; sz = 0;
			front.store(0, std::memory_order_relaxed);
		}
		//spa<T>* init() {
		//	return new (this)spa<T>;
		//	/*inside.store(false);
		//	count.store(0, std::memory_order_relaxed);
		// 	std::constr g_pages_mutex
		//	return this;*/
		//}
		/*RingQue(const RingQue<T,size>& sp) :removeing(false) {
			front.store(sp.front, std::memory_order_relaxed);
			std::memcpy((void*)&sp, this, sizeof(spa));
			sp.inside._Storage._Value = ((bool)(this->inside));
		}*/
		constexpr void pop(T& res) {
			auto i = back.fetch_add(1, std::memory_order_relaxed);
			assert(i <= front);
			res = ele[i % ele.size()];
		}
		constexpr bool try_pop(T& res) {
			auto i = back.fetch_add(1, std::memory_order_relaxed);
			if (i <= front)return false;
			res = ele[i % ele.size()];
			return true;
		}
		constexpr void push(T res) {
			auto i = front.fetch_add(1, std::memory_order_relaxed);
			assert(i >= back);
			ele[i % ele.size()] = res;
		}
		/*constexpr void push(T res) {
			auto i = front.fetch_add(1, std::memory_order_relaxed);
			assert(i >= back);
			ele[i % ele.size()] = res;
		}*/
		/*inline T& operator[](int i) {
			assert(i < gridmaxsize&& i >= 0);
			return (ele[i]);
		}*/
		constexpr void swaperase(const int i) {
			//const std::lock_guard<std::mutex> lock(g_pages_mutex);
			ele[i] = ele[front.fetch_add(-1, std::memory_order_relaxed)];
		}
		RingQue& operator=(RingQue& a) {
			return *this;
		}
		void clear() {
			back = 0;
			front = 0;
		}
		constexpr void enqueue(T& res) {
			push(res);
		}
		template<class T1>
		constexpr void enqueue(T1 res) {
			push(res);
		}
		constexpr int begin() {
			return 0;
		}
		inline int size() {
			return front - back;
		}
		int end() {
			return front;
		}

		bool empty() const {
			return front.load(std::memory_order_relaxed) == 0;
		}
		inline bool contain(const  T v)const {
			for (size_t i = 0; i < front; i++)
			{
				if (v == ele[i])
					return true;
			}
			return false;
		}
		constexpr void insert(const T v) {
			assert(size() < Capa() && front >= 0);
			ele[front.fetch_add(1, std::memory_order_relaxed)] = v;
			//return(count < gridmaxsize&& count >= 0);
		}
		constexpr auto Capa() {
			return ele.size();
		}
		constexpr void emplace_back(T& v) {
			enqueue(v);
		}
		/*template <typename... Args>
		sz emplace_back(Args&&... args) {
			const auto countl = front++;
			assert(countl < gridmaxsize&& countl >= 0);
			new (ele + countl) T(std::forward<Args>(args)...);
			return countl;
		}*/

	};
	template<typename T, typename... Args>
	class WrapFuncObj {
		T(*f)(Args...);
	public:
		WrapFuncObj(T(*t)(Args...)) {
			f = t;
		}
		WrapFuncObj() {
			f = NULL;
		}
		T operator()(Args&&... args) {
			return (*f)(std::forward<Args>(args)...);
		}
		inline operator const bool() {
			return f;
		}
		void operator =(T(*t)(Args...)) {
			f = t;
		}
	};

	template<typename T>
	struct AppendArray {
	private:
		std::vector<T> S0;
		std::atomic_size_t  counter;
		std::atomic_bool resizeing, removeing;
		std::mutex g_pages_mutex;
	public:
		AppendArray(size_t reserve = 32) : counter(0), resizeing(false), removeing(false)
		{
			S0.resize(reserve);
		}
		T& operator[](size_t i) {
			//assert(i < counter);
			return S0[i];
		}
		var begin() {
			return S0.begin();
		}
		var end() {
			return S0.end();
		}
		constexpr void resize() {
			bool resz = false;
			//LOGINFO(" i={0} counter={1}", S0.size(), counter);
			if (resizeing.compare_exchange_strong(resz, true)) {
				//LOGINFO("aa.size() i={0} counter={1}", S0.size(), counter);
				if (counter >= S0.size())
					S0.resize(counter * 2);
				resizeing = false;
			}
			while (resizeing)
			{
				std::this_thread::yield();
			}
		}
		constexpr void CheckCapacity() {
			while (counter >= S0.size()) {
				resize();
			}
		}
		constexpr void swaperase(const int i) {
			S0[i] = S0[counter];// .load(std::memory_order_relaxed)];
		}
		inline bool erase(const T& t) {

			int i = 0, i2 = counter.load(std::memory_order_relaxed);
		Restart:
			for (; i < i2; i++)
			{
				if (S0[i] == t) {

					bool resz = false;
					while (!removeing.compare_exchange_weak(resz, true)) {
						resz = false;
						if (i >= counter) {
							i = 0;
							goto Restart;
						}
						std::this_thread::yield();
					}

					if (i >= counter) {
						removeing = false;
						i = 0;
						goto Restart;
					}
					//counter.fetch_add(-1,std::memory_order_relaxed);
					S0[i] = S0[counter - 1];
					counter.fetch_add(-1, std::memory_order_relaxed);
					/*LOGINFO("i={0} counter={1} t={2} i2={3} thread id={4} removeing={5} resz={6}",
						i, counter, t, i2, std::this_thread::get_id(), removeing, resz);*/
					removeing = false;
					//assert(!removeing);
					return true;
				}
			}
			return false;
		}
		void push_back(const T para) {
			const int ip = counter.fetch_add(1, std::memory_order_relaxed);
			CheckCapacity();
			S0[ip] = para;
		}
		void push_back(const T* para, const uint size) {
			int ip = counter.fetch_add(size, std::memory_order_relaxed);
			/*while (!counter.compare_exchange_weak(ip,(counter+size)))
			{
			}*/
			CheckCapacity();
			/*for (size_t i = 0; i < size; i++)
			{
				S0[ip++] = para[i];
			}*/
			//LOGINFO(" ip={0} counter={1}", ip, counter);
			std::memcpy(&S0[ip], para, size * sizeof(T));
		}
		//void push_back(const span<T> para) {
		//	const uint size = para.size();
		//	int ip = counter.fetch_add(size, std::memory_order_relaxed);
		//	/*while (!counter.compare_exchange_weak(ip,(counter+size)))
		//	{
		//	}*/
		//	CheckCapacity();
		//	/*for (size_t i = 0; i < size; i++)
		//	{
		//		S0[ip++] = para[i];
		//	}*/
		//	//LOGINFO(" ip={0} counter={1}", ip, counter);
		//	std::memcpy(&S0[ip], para, size * sizeof(T));
		//}
		void enqueue(T&& i) {
			CheckCapacity();
			S0[counter++] = i;
		}
		constexpr void enqueue(const T& i) {
			push_back(i);
		}
		template<class Tset>
		constexpr void DeDup1(Tset& s) {
			const auto cou = counter.load(std::memory_order_relaxed);
			for (size_t i = 0; i < cou; i++)
			{
				const auto curr = S0[i];
				s.insert(curr);
			}
			counter.store(s.size(), std::memory_order_relaxed);
			uint i = 0;
			for (const auto v : s)
			{
				S0[i++] = v;
			}
			s.clear();
		}
		template<class KeyEqual, class comp>
		constexpr void DeDup2(const comp cpm, const KeyEqual keq) {

			std::sort(&S0[0], &S0[counter], cpm);
			int dd = 0;
			for (size_t i = 1; i < counter; i++)
			{
				const auto curr = S0[i];
				if (!keq(S0[dd], S0[i]) && (i - dd) > 1) {
					S0[++dd] = curr;
				}
			}
			counter.store(dd, std::memory_order_relaxed);
		}
		void lock_push_back(const T* para, const uint size) {
			bool resz = false;
			while (!resizeing.compare_exchange_weak(resz, true))
			{
				resz = false;
				std::this_thread::yield();
			}
			/*for (size_t i = 0; i < size; i++)
			{
				S0.push_back(para[i]);
			}*/
			if (counter + size >= S0.size())
				S0.resize(counter * 2);
			std::memcpy(&S0[counter], para, sizeof(T) * size);
			counter += size;
			resizeing = false;
			LOGINFO(" size={0} counter={1}", size, counter);
			//std::memcpy(&S0[ip], para, size * sizeof(T));
		}

		const size_t size()const {
			return counter.load(std::memory_order_relaxed);
		}
		auto& sz() {
			return counter;
		}
		void clear() {
			counter = 0;
		}
	};

	/*template<class I>
	struct ItemLocker
	{
		phmap::parallel_flat_hash_set<I> _set;
		bool Lock(const I& i) {
			if (_set.contains(i)) {
				return false;
			}
			return true;
		}
	};*/
	/*
	  What this code does:
		 Rasterizes a single Field Of View octant on a grid, similar to the way
		 FOV / shadowcasting is implemented in some roguelikes.
		 Clips to bitmap
		 Steps on pixel centers
		 Optional attenuation
		 Optional circle clip
		 Optional lit blocking tiles

	  To rasterize the entire FOV, call this in a loop with octant in range 0-7
	  Inspired by http://blogs.msdn.com/b/ericlippert/archive/2011/12/12/shadowcasting-in-c-part-one.aspx


	   static inline int Mini(int a, int b) {
		   return a < b ? a : b;
	   }

	   static inline int Maxi(int a, int b) {
		   return a > b ? a : b;
	   }

	   static inline int Clampi(int v, int min, int max) {
		   return Maxi(min, Mini(v, max));
	   }

	   typedef union c2_s {
		   struct {
			   int x, y;
		   };
		   int a[2];
	   } c2_t;

	   static const c2_t c2zero = { .a = { 0, 0 } };
	   static const c2_t c2one = { .a = { 1, 1 } };

	   static inline c2_t c2xy(int x, int y) {
		   c2_t c = { { x, y } };
		   return c;
	   }

	   static inline c2_t c2Neg(c2_t c) {
		   return c2xy(-c.x, -c.y);
	   }

	   static inline c2_t c2Add(c2_t a, c2_t b) {
		   return c2xy(a.x + b.x, a.y + b.y);
	   }

	   static inline c2_t c2Sub(c2_t a, c2_t b) {
		   return c2xy(a.x - b.x, a.y - b.y);
	   }

	   static inline int c2Dot(c2_t a, c2_t b) {
		   return a.x * b.x + a.y * b.y;
	   }

	   static inline int c2CrossC(c2_t a, c2_t b) {
		   return a.x * b.y - a.y * b.x;
	   }

	   static inline c2_t c2Clamp(c2_t c, c2_t min, c2_t max) {
		   return c2xy(Clampi(c.x, min.x, max.x), Clampi(c.y, min.y, max.y));
	   }

	   static inline c2_t c2Scale(c2_t a, int s) {
		   return c2xy(a.x * s, a.y * s);
	   }

	   void RasterizeFOVOctant(int originX, int originY,
		   int radius,
		   int bitmapWidth, int bitmapHeight,
		   int octant,
		   int skipAttenuation,
		   int skipClampToRadius,
		   int darkWalls,
		   const unsigned char* inBitmap,
		   unsigned char* outBitmap) {
   #define READ_PIXEL(c) inBitmap[(c).x+(c).y*bitmapWidth]
   #define WRITE_PIXEL(c,color) outBitmap[(c).x+(c).y*bitmapWidth]=(color)
   #define MAX_RAYS 64
   #define ADD_RAY(c) {nextRays->rays[Mini(nextRays->numRays,MAX_RAYS-1)] = (c);nextRays->numRays++;}
   #define IS_ON_MAP(c) ((c).x >= 0 && (c).x < bitmapWidth && (c).y >= 0 && (c).y < bitmapHeight)
		   typedef struct {
			   int numRays;
			   c2_t rays[MAX_RAYS];
		   } raysList_t;
		   // keep these coupled like this
		   static const const c2_t bases[] = {
			   { { 1, 0  } }, { { 0, 1  } },
			   { { 1, 0  } }, { { 0, -1 } },
			   { { -1, 0 } }, { { 0, -1 } },
			   { { -1, 0 } }, { { 0, 1  } },
			   { { 0, 1  } }, { { -1, 0 } },
			   { { 0, 1  } }, { { 1, 0  } },
			   { { 0, -1 } }, { { 1, 0  } },
			   { { 0, -1 } }, { { -1, 0 } },
		   };
		   c2_t e0 = bases[(octant * 2 + 0) & 15];
		   c2_t e1 = bases[(octant * 2 + 1) & 15];
		   raysList_t rayLists[2] = { {
			   .numRays = 2,
			   .rays = {
				   c2xy(1, 0),
				   c2xy(1, 1),
			   },
		   } };
		   c2_t bitmapSize = c2xy(bitmapWidth, bitmapHeight);
		   c2_t bitmapMax = c2Sub(bitmapSize, c2one);
		   c2_t origin = c2Clamp(c2xy(originX, originY), c2zero, bitmapMax);
		   if (READ_PIXEL(origin)) {
			   WRITE_PIXEL(origin, 255);
			   return;
		   }
		   c2_t dmin = c2Neg(origin);
		   c2_t dmax = c2Sub(bitmapMax, origin);
		   int dmin0 = c2Dot(dmin, e0);
		   int dmax0 = c2Dot(dmax, e0);
		   int limit0 = Mini(radius, dmin0 > 0 ? dmin0 : dmax0);
		   int dmin1 = c2Dot(dmin, e1);
		   int dmax1 = c2Dot(dmax, e1);
		   int limit1 = Mini(radius, dmin1 > 0 ? dmin1 : dmax1);
		   c2_t ci = origin;
		   for (int i = 0; i <= limit0; i++) {
			   int i2 = i * 2;
			   raysList_t* currRays = &rayLists[(i + 0) & 1];
			   raysList_t* nextRays = &rayLists[(i + 1) & 1];
			   nextRays->numRays = 0;
			   for (int r = 0; r < currRays->numRays - 1; r += 2) {
				   c2_t r0 = currRays->rays[r + 0];
				   c2_t r1 = currRays->rays[r + 1];
				   int inyr0 = (i2 - 1) * r0.y / r0.x;
				   int outyr0 = (i2 + 1) * r0.y / r0.x;
				   int inyr1 = (i2 - 1) * r1.y / r1.x;
				   int outyr1 = (i2 + 1) * r1.y / r1.x;

				   // every pixel with a center INSIDE the frustum is lit

				   int starty = outyr0 + 1;
				   if (c2CrossC(r0, c2xy(i2, outyr0)) < 0) {
					   starty++;
				   }
				   starty /= 2;
				   c2_t start = c2Add(ci, c2Scale(e1, starty));
				   int endy = inyr1 + 1;
				   if (c2CrossC(r1, c2xy(i2, inyr1 + 1)) > 0) {
					   endy--;
				   }
				   endy /= 2;
				   //c2_t end = c2Add( ci, c2Scale( e1, endy ) );
				   {
					   int y;
					   c2_t p;
					   int miny = starty;
					   int maxy = Mini(endy, limit1);
					   for (y = miny, p = start; y <= maxy; y++, p = c2Add(p, e1)) {
						   WRITE_PIXEL(p, 255);
					   }
				   }

				   // push rays for the next column

				   // correct the bounds first

				   c2_t bounds0;
				   c2_t bounds1;
				   c2_t firstin = c2Add(ci, c2Scale(e1, (inyr0 + 1) / 2));
				   c2_t firstout = c2Add(ci, c2Scale(e1, (outyr0 + 1) / 2));
				   if ((IS_ON_MAP(firstin) && !READ_PIXEL(firstin))
					   && (IS_ON_MAP(firstout) && !READ_PIXEL(firstout))) {
					   bounds0 = r0;
				   }
				   else {
					   int top = (outyr0 + 1) / 2;
					   int bottom = Mini((inyr1 + 1) / 2, limit1);
					   int y;
					   c2_t p = c2Add(ci, c2Scale(e1, top));
					   for (y = top * 2; y <= bottom * 2; y += 2, p = c2Add(p, e1)) {
						   if (!READ_PIXEL(p)) {
							   break;
						   }
						   // pixels that force ray corrections are lit too
						   WRITE_PIXEL(p, 255);
					   }
					   bounds0 = c2xy(i2 - 1, y - 1);
					   inyr0 = (i2 - 1) * bounds0.y / bounds0.x;
					   outyr0 = (i2 + 1) * bounds0.y / bounds0.x;
				   }
				   c2_t lastin = c2Add(ci, c2Scale(e1, (inyr1 + 1) / 2));
				   c2_t lastout = c2Add(ci, c2Scale(e1, (outyr1 + 1) / 2));
				   if ((IS_ON_MAP(lastin) && !READ_PIXEL(lastin))
					   && (IS_ON_MAP(lastout) && !READ_PIXEL(lastout))) {
					   bounds1 = r1;
				   }
				   else {
					   int top = (outyr0 + 1) / 2;
					   int bottom = Mini((inyr1 + 1) / 2, limit1);
					   int y;
					   c2_t p = c2Add(ci, c2Scale(e1, bottom));
					   for (y = bottom * 2; y >= top * 2; y -= 2, p = c2Sub(p, e1)) {
						   if (!READ_PIXEL(p)) {
							   break;
						   }
						   // pixels that force ray corrections are lit too
						   WRITE_PIXEL(p, 255);
					   }
					   bounds1 = c2xy(i2 + 1, y + 1);
					   inyr1 = (i2 - 1) * bounds1.y / bounds1.x;
					   outyr1 = (i2 + 1) * bounds1.y / bounds1.x;
				   }

				   // closed frustum - quit
				   if (c2CrossC(bounds0, bounds1) <= 0) {
					   continue;
				   }

				   // push actual rays
				   {
					   ADD_RAY(bounds0);
					   int top = (outyr0 + 1) / 2;
					   int bottom = Mini((inyr1 + 1) / 2, limit1);
					   c2_t p = c2Add(ci, c2Scale(e1, top));
					   int prevPixel = READ_PIXEL(p);
					   for (int y = top * 2; y <= bottom * 2; y += 2, p = c2Add(p, e1)) {
						   int pixel = READ_PIXEL(p);
						   if (prevPixel != pixel) {
							   c2_t ray;
							   if (pixel) {
								   ray = c2xy(i2 + 1, y - 1);
							   }
							   else {
								   ray = c2xy(i2 - 1, y - 1);
							   }
							   ADD_RAY(ray);
						   }
						   prevPixel = pixel;
					   }
					   ADD_RAY(bounds1);
				   }
			   }
			   ci = c2Add(ci, e0);
		   }

		   if (!skipAttenuation) {
			   c2_t ci = origin;
			   int rsq = radius * radius;
			   for (int i = 0; i <= limit0; i++) {
				   c2_t p = ci;
				   for (int j = 0; j <= limit1; j++) {
					   c2_t d = c2Sub(p, origin);
					   int dsq = c2Dot(d, d);
					   int mod = 255 - Mini(dsq * 255 / rsq, 255);
					   int lit = !!outBitmap[p.x + p.y * bitmapWidth];
					   WRITE_PIXEL(p, mod * lit);
					   p = c2Add(p, e1);
				   }
				   ci = c2Add(ci, e0);
			   }
		   }
		   else if (!skipClampToRadius) {
			   c2_t ci = origin;
			   int rsq = radius * radius;
			   for (int i = 0; i <= limit0; i++) {
				   c2_t p = ci;
				   for (int j = 0; j <= limit1; j++) {
					   c2_t d = c2Sub(p, origin);
					   if (c2Dot(d, d) > rsq) {
						   WRITE_PIXEL(p, 0);
					   }
					   p = c2Add(p, e1);
				   }
				   ci = c2Add(ci, e0);
			   }
		   }

		   if (darkWalls) {
			   c2_t ci = origin;
			   for (int i = 0; i <= limit0; i++) {
				   c2_t p = ci;
				   for (int j = 0; j <= limit1; j++) {
					   if (READ_PIXEL(p)) {
						   WRITE_PIXEL(p, 0);
					   }
					   p = c2Add(p, e1);
				   }
				   ci = c2Add(ci, e0);
			   }
		   }
	   }
	   */
}
namespace GraphicUtil {

	struct quad
	{
		using float3 = Eigen::Vector3i;
		quad() {}
		std::array<float3, 4> Vertexs = {
			 float3(-1, -1, 0),
			 float3(-1, +1, 0),
			 float3(+1, +1, 0),
			 float3(+1, -1, 0) };
	};
}
namespace PUtility {
	template<typename T>
	class WrapFuncObj;
	template<typename T, typename... Args>
	class WrapFuncObj<T(Args...)> {
		T(*f)(Args...);

	public:
		WrapFuncObj(T(*t)(Args...)) {
			f = t;
		}
		WrapFuncObj() {}
		void init(T(*t)(Args...)) {
			f = t;
		}
		T operator()(Args&&... args) {
			if (f != nullptr) {
				return (*f)(std::forward<Args>(args)...);
			}
			else {
				return T();
			}
		}
	};
}