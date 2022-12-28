#pragma once
#include"comm.h"

namespace pj2 {
	struct fermion
	{

	};
	template<int numprop = 8>
	struct fmele0:fermion//, n_BasObj::obj<Cmp_unit>
	{
		using t_self = fmele0<numprop>;
		//using GameProps::ENU::t_Dmg;
		static constexpr int eft = 3;
		//static constexpr std::array<int, sizeof...(lo)> EffLoc = { (int)lo... };
		using t_data = Fix::fix<eft>;
		//std::conditional_t < EffLoc.size() < 4,Fix::fix4,Fix::fix<EffLoc.size()>> datas;
		t_data datas;
		std::array<char, 4> peft;//0 for count ,follow by effet id;
		fmele0(){}
		//static constexpr t_Dmg _tDmg = dmg;
		//df() {}
	};
	using fm0 = fmele0<>;
	using fmtarget = std::array<GameProps::EntityId, 2>;
	using ecsfm= ECS::ECSclass<fmtarget,fm0>;
	//template<class ...Ts>
	struct fermion0:n_BasObj::poolobj<ecsfm>
	{

	};

}