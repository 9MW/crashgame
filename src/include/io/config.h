#pragma once
#include<array>
namespace pj2{
struct cfg
{
	std::array<int64_t,512> datas;
	static cfg& main() {
		static cfg cfgs;
		return cfgs;
	}
	int64_t& cfgget(void * const p);
	template<class ...Ts>
	cfg& cfgexe(Ts&...args);
	template<int I,class T, class ...Ts>
	T cfgexe(Ts&&...args);
};
}