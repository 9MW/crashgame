#pragma once
#include<atomic>
#include <mutex>
namespace pj2 {
	//template<class T>
	struct aint
	{
		using al = std::atomic_llong;
		al t;
		aint(const aint& a) { std::memcpy(this,&a,sizeof(t)); }
		auto add1() {
			return t++;
		}
		auto mns1() {
			return t--;
		}
		aint() = default;
		auto operator ++() { return t++; }
		auto operator =(const aint& a) { std::memcpy(this, &a, sizeof(t)); }
		auto& operator ()() { return t._Storage._Value; }
	};

	struct amtx
	{
		std::mutex mk;
		amtx(const amtx& m) {}
		amtx() = default;
		void lock() {
			mk.lock();
		}
		void unlock() {
			mk.unlock();
		}
	};
}