#pragma once
namespace pj2::n_Render {
	template<class...Ts>
	void execmd(Ts... args);
	template<class T,class...Ts>
	void execmd(Ts... args);
	template<int I, class...Ts>
	void execmd(Ts&... args);
	template<class...Ts>
	void execmd(Ts&... args);
	void execmd(void*);
}