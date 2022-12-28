#include "comm.h"
namespace  pj2::n_BasObj {

	template<>
	int pool::poolresize<ui64, ui64>(ui64 p, ui64 pd) {
		_pooldel.resize(pd);
		_extractpool.resize(p);
		return 1;
	}

	template<>
	int pool::poolresize<ui64>(ui64 p) {
		_pooldel.resize(p);
		_extractpool.resize(p);
		return 1;
	}

}