#pragma once

#include"Common/Task.h"
struct MyObserver : public tf::ObserverInterface {

	struct timer final {
		timer() {}
		void start0() {
			start = std::chrono::system_clock::now();
		}
		void elapsed(std::string&& nm = "") {
			auto now = std::chrono::system_clock::now();
			LOGINFO("cost {0} sec {1}", std::chrono::duration<double>(now - start).count(), nm);
		}
		auto stop() {
			auto now = std::chrono::system_clock::now();
			return  std::chrono::duration<double>(now - start).count();
		}
	private:
		std::chrono::time_point<std::chrono::system_clock> start;
	};
	timer t;
	MyObserver(const std::string& name) {
		std::cout << "constructing observer " << name << '\n';
	}

	void set_up(size_t num_workers) override final {
		std::cout << "setting up observer with " << num_workers << " workers\n";
	}

	void on_entry(tf::WorkerView w, tf::TaskView tv) override final {
		t.start0();
	}

	void on_exit(tf::WorkerView w, tf::TaskView tv) override final {
		INFO("{0} cost {1} ms", tv.name(), t.stop() * 1000);
	}
};