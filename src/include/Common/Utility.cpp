#include "Utility.h"
#include"Common/aqdefs.h"
#include"physics/physics.hpp"

auto constexpr A = std::memory_order_acquire;
auto constexpr R = std::memory_order_release;
auto constexpr X = std::memory_order_relaxed;
auto constexpr C = std::memory_order_seq_cst;
class SpinLock {
	std::atomic_flag locked = ATOMIC_FLAG_INIT;
public:
	void lock() {
		while (locked.test_and_set(std::memory_order_acquire)) { ; }
	}
	void unlock() {
		locked.clear(std::memory_order_release);
	}
};
namespace Utility {
	struct tas_lock {
		std::atomic<bool> lock_ = { false };

		void lock() { while (lock_.exchange(true, std::memory_order_acquire)); }

		void unlock() { lock_.store(false, std::memory_order_release); }
	};
	inline void lock(std::atomic<bool>& lock_)
	{
		while (lock_.exchange(true, std::memory_order_acquire))atomic_queue::spin_loop_pause();
	}
	inline bool trylock(std::atomic<bool>& lock_)
	{
		//bool des = false;
		return !(lock_.exchange(true, std::memory_order_acquire));
	}

	void unlock(std::atomic<bool>& lock_) { lock_.store(false, std::memory_order_release); }
	using namespace pj2;
	/// <summary>
	///  assume no insert happening same time
	/// </summary>
	/// <param name="v"></param>
	/// <returns></returns>
	template<>
	template<>
	bool spa<Physics::CollideID>::erase<true, Physics::CollideID>(const Physics::CollideID& v) {
		auto i = 0;
		while (!trylock(removeing))
		{
			return false;
		}
		auto& i2 = count._Storage._Value;
		for (size_t i = 0; i < i2; i++)
		{

			if (ele[i] == v) {
				ele[i] = ele[--i2];
				unlock(removeing);
				return true;
			}
		}
		unlock(removeing);
		return 0;
		//int i =~(0x1 << (i2));
		while (1)
		{
			if (ele[i] == v) {
				if (trylock(removeing)) {
					i2 = --count;
					ele[i] = ele[i2];
					unlock(removeing);
					return true;
				}
				else
				{
					//research
					i = 0;
					continue;
				}
			}
			if (i++ >= i2) {
				i = 0;
			}
		}
	}
	template<>
	template<>
	bool spa<Physics::CollideID>::erase<false, Physics::CollideID>(const Physics::CollideID& v) {
		auto i = 0;
		while (!trylock(removeing))
		{
			i = 10;
			for ( i = 0; i >0; i--)
			{

			}
		}
		auto& i2 = count._Storage._Value;
		for (size_t i = 0; i < i2; i++)
		{

			if (ele[i] == v) {
					ele[i] = ele[--i2];
					unlock(removeing);
					return true;
				}
		}
		unlock(removeing);
		return 0;
	}
	/// <summary>
	/// assume no erase happening same time
	/// </summary>
	/// <param name="v"></param>
	/// <returns></returns>
	template<>
	bool spa<Physics::CollideID>::insert(const Physics::CollideID& v) {
		auto c = count++;
		ele[c] = v;
		return c < _Size;
	}
	bool FixAy::erase(const T& v) {
		auto& vl = counter._Storage._Value;
		for (size_t i = 0; i < vl; i++)
		{
			if (datas[i] == v) {
				datas[i] = datas[--vl];
				return true;
			}
		}
		return false;
	}
	bool FixAy::add(const T& v) {
		auto& vl = counter._Storage._Value;
		auto c = vl++;
		datas[c] = v;
		return c < _Size;
		auto i2 = v % _Size;
		int i = 0x1 << (i2);
		while (1)
		{
			if ((i & map.fetch_or(i)) == 0) {
				datas[i2] = v;
				break;
			}
			i2 = ++i2 % _Size;
			i = 0x1 << i2;
		}
	}
}