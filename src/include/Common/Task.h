#pragma once
#include"taskflow/taskflow.hpp"
#include"ecs.hpp"

namespace pj2::TaskEx
{

	using namespace tf;

	// Function: parallel_for
	template <typename C, typename T>
	Task parallelforleg(Taskflow& tf, C&& c0,
		std::vector<T*>& l2,
		size_t chunk_size0 = 1)
	{

		//using category = typename std::iterator_traits<I>::iterator_category;

		Task task = tf.emplace(
			[&l2, chunk_size = std::forward<size_t>(chunk_size0), c = std::forward<C>(c0)](Subflow& sf) mutable {

			std::atomic<T*> curritem = l2[0];
			//size_t N = distance(beg, end, chunk_size);
			std::atomic<size_t> next(0), next2(0), end = curritem.load(std::memory_order_relaxed)->size();;
			std::atomic_bool steping = false;
			size_t W = std::thread::hardware_concurrency();
			W = W > 0 ? W : 8;
			for (size_t w = 0; w < W; w++) {

				sf.emplace([&next, &steping, &end, &next2, &l2, &curritem, chunk_size, W, &c]() mutable {
					size_t next2l = 0;
					// end = curritem.load(std::memory_order_relaxed)->size();
					size_t s0 = next.load(std::memory_order_relaxed);
					if (!cds::threading::Manager::isThreadAttached())
						cds::threading::Manager::attachThread();
					while (1) {
						s0 = next.fetch_add(chunk_size, std::memory_order_relaxed);
						next2l = next2.load(std::memory_order_relaxed);
						size_t sz = (*curritem).size();
						if (s0 >= end) {
							bool zero = false;
							if (steping.compare_exchange_strong(zero, true))
							{
								next2++;
								if (next2 < l2.size()) {
									curritem = l2[next2];
									end = (*curritem).size();
									next = 0;
									steping = false;
									continue;
								}
								else {
									steping = false; return;
								}
							}
							while (steping)
							{

							}
							/*while (next >= end)
							{
								if (!steping.load(std::memory_order_relaxed) && next >= end)
									return;
							}*/
							continue;
						}
						T& curr = *curritem.load(std::memory_order_relaxed);

						//T& itm = *curritem;
						size_t e0 = std::min(s0 + chunk_size, end.load(std::memory_order_relaxed));
						for (size_t x = s0; x < e0; x++) {
							c(x, curr);
						}
					}
					}).name("pfg_"s + std::to_string(w));
			}
			sf.join();
		});
		return task;
	}

	template <typename B, typename E, typename S, typename C, typename H>
	Task for_each_index_guided(tf::Taskflow& tfl,
		B&& beg, E&& end, S&& inc, C&& c, H&& chunk_size
	) {

		using I = stateful_index_t<B, E, S>;
		using namespace std::string_literals;

		Task task = tfl.emplace(
			[b = std::forward<B>(beg),
			e = std::forward<E>(end),
			a = std::forward<S>(inc),
			c = std::forward<C>(c),
			h = std::forward<H>(chunk_size)] (Subflow& sf) mutable {

			// fetch the iterator values
			I beg = b;
			I end = e;
			I inc = a;

			size_t chunk_size = (h == 0) ? 1 : h;
			size_t W = sf._executor.num_workers();
			size_t N = distance(beg, end, inc);

			// only myself - no need to spawn another graph
			if (W <= 1 || N <= chunk_size) {
				for (size_t x = 0; x < N; x++, beg += inc) {
					c(beg);
				}
				return;
			}

			if (N < W) {
				W = N;
			}

			std::atomic<size_t> next(0);

			for (size_t w = 0; w < W; w++) {

				//sf.emplace([&next, beg, inc, N, chunk_size, W, &c] () mutable {
				sf.silent_async([&next, beg, inc, N, chunk_size, W, &c]() mutable {

					size_t p1 = 2 * W * (chunk_size + 1);
					double p2 = 0.5 / W;
					size_t s0 = next.load(std::memory_order_relaxed);

					while (s0 < N) {

						size_t r = N - s0;

						// find-grained
						if (r < p1) {
							while (1) {
								s0 = next.fetch_add(chunk_size, std::memory_order_relaxed);
								if (s0 >= N) {
									return;
								}
								size_t e0 = (chunk_size <= (N - s0)) ? s0 + chunk_size : N;
								auto s = static_cast<I>(s0) * inc + beg;
								for (size_t x = s0; x < e0; x++, s += inc) {
									c(s);
								}
							}
							break;
						}
						// coarse-grained
						else {
							size_t q = static_cast<size_t>(p2 * r);
							if (q < chunk_size) {
								q = chunk_size;
							}
							size_t e0 = (q <= r) ? s0 + q : N;
							if (next.compare_exchange_strong(s0, e0, std::memory_order_acquire,
								std::memory_order_relaxed)) {
								auto s = static_cast<I>(s0) * inc + beg;
								for (size_t x = s0; x < e0; x++, s += inc) {
									c(s);
								}
								s0 = next.load(std::memory_order_relaxed);
							}
						}
					}
					//}).name("pfg_"s + std::to_string(w));
					});
			}

			sf.join();
		});

		return task;
	}
	template <typename Retriver, typename C, typename T>
	Task parallelfor(Taskflow& tf, C&& c0, Retriver&& RT,
		std::vector<T>& l2,
		size_t chunk_size0 = 1)
	{
		Task task = tf.emplace(
			[&l2, chunk_size = std::forward<size_t>(chunk_size0),
			c = std::forward<C>(c0), rt = std::forward<Retriver>(RT)
			](Subflow& sf) mutable {
				size_t b = 0,splmt= chunk_size * 4;
				for (size_t i = 0; i < l2.size()&&b<splmt; i++)
				{
					b+=rt(l2[i]).size();
				}
				if(b< splmt){
					for (auto& rf : l2)
					{
						auto&& curr = rt(rf);
						for (size_t i = 0; i < curr.size(); i++)
						{
							c(i, curr);
						}
					}
					return; 
				}
			//std::atomic<T*> currcmps = &ff[0];
			//size_t N = distance(beg, end, chunk_size);
			std::atomic<size_t> next(0), next2(0), end(0);// = curritem.load(std::memory_order_relaxed)->size();;
			std::atomic_bool steping = false;
			size_t W = std::thread::hardware_concurrency();
			W = W > 0 ? W : 8;
			for (size_t w = 0; w < W; w++) {

				sf.emplace([&next, &steping, &end, &next2, &l2, chunk_size, W, &c, &rt]() mutable {
					//constexpr auto getele = [&l2, &rt](auto&& i) {return rt(l2[i]); };
					//T* curritem = NULL;
					// end = curritem.load(std::memory_order_relaxed)->size();
					 //= next.load(std::memory_order_relaxed);
					if (!cds::threading::Manager::isThreadAttached())
						cds::threading::Manager::attachThread();
					size_t endf = 0, next3l = 0;
					while (1) {
						size_t s0 = next.fetch_add(chunk_size, std::memory_order_relaxed);
						//endl = std::min(endl, s0+chunk_size-endf);
						while (s0 >= endf)
						{
							if (next3l >= l2.size())
								return;
							endf += rt(l2[next3l++]).size();
						}
						size_t next2l = next3l - 1, endl = rt(l2[next2l]).size();
						//auto&& curritem = rt(l2[next2l]);
						s0 = endl - (endf - s0);
						size_t e0 = std::min(endl, (s0 + chunk_size));
					iter:
						{
							auto&& curr = rt(l2[next2l]);
							for (size_t x = s0; x < e0; x++) {
								c(x, curr);
							}
						}
						size_t rem = (s0 + chunk_size - e0);
						while (rem != 0 && ++next2l < l2.size()) {
							auto&& curr = rt(l2[next2l]);
							endl = curr.size();
							e0 = std::min(endl, rem);
							for (size_t x = 0; x < e0; x++) {
								c(x, curr);
							}
							rem = (rem - e0);
						}
					}
					}).name("pfg_"s + std::to_string(w));
			}
			sf.join();
		});
		return task;
	}

	template<class C, class AY, class... _Types>
	inline auto parallelloop(tf::Taskflow& tf, AY&& Objects, std::vector<math::ui64>& gids, const math::ui64 chunksize, C&& f, _Types&&... _Args) {
		if constexpr (sizeof...(_Types) == 0) {
			return TaskEx::parallelfor(tf, std::forward<C>(f), [&](auto&& gid) {return &Objects[gid]; },
				gids, chunksize);
		}
		return TaskEx::parallelfor(tf, std::forward<C>(f), [&](auto&& gid) {return &Objects[gid]; },
			gids, chunksize, std::forward<_Types>(_Args)...);
	}
	template <typename C, typename T>
	Task parallelfor(Taskflow& tf, C&& c0,
		std::vector<T*>& l2,
		size_t chunk_size0 = 1)
	{

		//using category = typename std::iterator_traits<I>::iterator_category;
		Task task = tf.emplace(
			[&l2, chunk_size = std::forward<size_t>(chunk_size0), c = std::forward<C>(c0)](Subflow& sf) mutable {

			//std::atomic<T*> currcmps = &ff[0];
			//size_t N = distance(beg, end, chunk_size);
			std::atomic<size_t> next(0), next2(0), end(0);// = curritem.load(std::memory_order_relaxed)->size();;
			std::atomic_bool steping = false;
			size_t W = std::thread::hardware_concurrency();
			W = W > 0 ? W : 8;
			for (size_t w = 0; w < W; w++) {

				sf.emplace([&next, &steping, &end, &next2, &l2, chunk_size, W, &c]() mutable {
					T* curritem = NULL;
					// end = curritem.load(std::memory_order_relaxed)->size();
					 //= next.load(std::memory_order_relaxed);
					if (!cds::threading::Manager::isThreadAttached())
						cds::threading::Manager::attachThread();
					size_t endf = 0, next3l = 0;
					while (1) {
						size_t s0 = next.fetch_add(chunk_size, std::memory_order_relaxed);
						//endl = std::min(endl, s0+chunk_size-endf);
						while (s0 >= endf)
						{
							if (next3l >= l2.size())
								return;
							endf += l2[next3l++]->size();
						}
						size_t next2l = next3l - 1, endl = l2[next2l]->size();
						curritem = (l2[next2l]);
						s0 = endl - (endf - s0);
						size_t e0 = std::min(endl, (s0 + chunk_size));
						{
							T& curr = *curritem;
							for (size_t x = s0; x < e0; x++) {
								c(x, curr);
							}
						}
						size_t rem = (s0 + chunk_size - e0);
						while (rem != 0 && ++next2l < l2.size()) {
							//size_t e0 = chunk_size - e0;
							T& curr = *l2[next2l];
							endl = curr.size();
							e0 = std::min(endl, rem);
							for (size_t x = 0; x < e0; x++) {
								c(x, curr);
							}
							rem = (rem - e0);
						}
					}
					}).name("pfg_"s + std::to_string(w));
			}
			sf.join();
		});
		return task;
	}

	template <typename B, typename E, typename S, typename C, typename H>
	Task parallelfor(Taskflow& tf,
		B&& beg, E&& end, C&& c, H&& chunk_size
	) {

		using I = stateful_index_t<B, E, S>;
		using namespace std::string_literals;

		Task task = tf.emplace(
			[
				b = std::forward<B>(beg),
				e = std::forward<E>(end),
				c = std::forward<C>(c),
				h = std::forward<H>(chunk_size)] (Subflow& sf) mutable {

			I beg = b;
			I end = e;
			size_t chunk_size = (h == 0) ? 1 : h;
			size_t W = std::thread::hardware_concurrency();
			std::atomic<size_t> next(0);

			for (size_t w = 0; w < W; w++) {

				sf.emplace([&next, beg, end, chunk_size, &c]() mutable {

					while (1) {

						size_t s0 = next.fetch_add(chunk_size, std::memory_order_relaxed);

						if (s0 >= end) {
							break;
						}

						size_t e0 = std::min(s0 + chunk_size, end);
						for (size_t x = s0; x < e0; x++) {
							c(x);
						}
					}
					}).name("pfd_"s + std::to_string(w));
			}

			sf.join();
		});


		return task;
	}

	template <typename I, typename C>
	inline Task dparallelfor(Taskflow& tf, I beg, I end, C&& c, size_t chunk) {
		return tf.emplace([=](tf::Subflow& sub) {
			TaskEx::parallelfor(sub, beg, end, c, chunk);
			});
	}
	template <typename I, typename C, typename TF>
	std::pair<Task, Task> parallelfor(TF& tf, I beg, I end, C&& c, size_t chunk)
	{

		//using category = typename std::iterator_traits<I>::iterator_category;

		auto source = tf.placeholder();
		auto target = tf.placeholder();
		// default partition equals to the worker count
		if (chunk == 0)
		{
			chunk = 1;
		}

		while (beg != end)
		{
			auto o = static_cast<I>(chunk);
			auto e = std::min(beg + o, end);
			auto task = tf.emplace([=]() mutable {
				for (auto i = beg; i < e; i++)
				{
					c(i);
				}
				}
			);

			task.name(fmt::format("task{0}", e));
			source.precede(task);
			task.precede(target);
			beg = e;
		}

		if (source.num_successors() == 0)
		{
			source.precede(target);
		}

		return std::make_pair(source, target);
	}
};
namespace pj2::task {

    using namespace tf;
    template <typename C, typename T, class... _Types>
    Task parallelforarch(Taskflow& tf, C&& c0,
        std::vector<T>& l2,
        std::vector<ECS::dfarchs>& ach,
        size_t chunk_size0 = 1)
    {
        
        //using category = typename std::iterator_traits<I>::iterator_category;
        Task task = tf.emplace(
            [&l2, chunk_size = std::forward<size_t>(chunk_size0), &ach, c = std::forward<C>(c0)](Subflow& sf) mutable {

            //std::atomic<T*> currcmps = &ff[0];
            //size_t N = distance(beg, end, chunk_size);
            std::atomic<size_t> next(0), next2(0), end(0);// = curritem.load(std::memory_order_relaxed)->size();;
            std::atomic_bool steping = false;
            size_t W = std::thread::hardware_concurrency();
            W = W > 0 ? W : 8;
            for (size_t w = 0; w < W; w++) {

                sf.emplace([&next, &steping, &end, &next2, &l2, chunk_size, W, &c,
                    &ach]() mutable {
                        T* curritem = NULL;
                        // end = curritem.load(std::memory_order_relaxed)->size();
                         //= next.load(std::memory_order_relaxed);
                        if (!cds::threading::Manager::isThreadAttached())
                            cds::threading::Manager::attachThread();
                        size_t endf = 0, next3l = 0;
                        while (1) {
                            size_t s0 = next.fetch_add(chunk_size, std::memory_order_relaxed);
                            //endl = std::min(endl, s0+chunk_size-endf);
                            while (s0 >= endf)
                            {
                                if (next3l >= l2.size())
                                    return;
                                endf += l2[next3l++]->size();
                            }
                            size_t next2l = next3l - 1, endl = l2[next2l]->size();
                            curritem = (l2[next2l]);
                            s0 = endl - (endf - s0);
                            size_t e0 = std::min(endl, (s0 + chunk_size));
                        iter:
                            {
                                T& curr = *curritem;
                                for (size_t x = s0; x < e0; x++) {
                                    c(x, curr, ach);
                                }
                            }
                            size_t rem = (s0 + chunk_size - e0);
                            while (rem != 0 && ++next2l < l2.size()) {
                                //ECS::CompAccessor cmp2(cmps0.get(next2l));
                                T& curr = *l2[next2l];
                                endl = curr.size();
                                e0 = std::min(endl, rem);
                                for (size_t x = 0; x < e0; x++) {
                                    c(x, curr, ach);
                                }
                                rem = (rem - e0);
                            }
                        }
                    }).name("pfg_"s + std::to_string(w));
            }
            sf.join();
        });
        return task;
    }
}