#include "EvulatAction.h"
namespace  pj2::AI {

	inline INT64 Cannotattack(const Physics::LAVNe *&& lk, const GameProps::Cmd0& cmd, const int& ammo, GameProps::BasicProp& bp){
		auto& atsq = bp.get<GameProps::PropID::attackrangesq>();
		return
			lk ? (ammo * ammo > 0 ? (math::Fix::toFix((lk->distSq()) - atsq)) : -100) : 50;
	}
	template<>
	INT64 Exm<GameProps::ENU::state::attack>
		(const Physics::LAVNe& lk, const GameProps::Cmd0& cmd, const int& ammo, 
			const float& dissq,const GameProps::BasicProp& bp)
	{
		auto& atsq = const_cast<GameProps::BasicProp&>(bp).get<GameProps::PropID::attackrangesq>();

		INT64 scol = ammo * ammo > 0 && cmd.eid.id >= 0 &&
			((atsq - (math::Fix::toFix((dissq)))) >= 0) ||
			(&lk&&(atsq - (math::Fix::toFix(lk.distSq())) >= 0))
			? 75 : -125;

		return scol;
	}
	//lk : closet enemy
	template<>
	INT64 Exm<GameProps::ENU::state::attackwhenfind>
		(const Physics::LAVNe& lk, const GameProps::Cmd0& cmd, const int& ammo, const  GameProps::BasicProp& bp) {
		auto& atsq = const_cast<GameProps::BasicProp&>(bp).get<GameProps::PropID::attackrangesq>();

		return
			Cannotattack(&lk, cmd, ammo, const_cast<GameProps::BasicProp&>(bp));
	}
	//, Physics::LAVNe*, GameProps::Cmd0,int, GameProps::BasicProp
	template<>
	INT64 Exm<GameProps::ENU::state::idle>
		(const Physics::LAVNe& lk, const GameProps::Cmd0& cmd, const int& ammo, const  GameProps::BasicProp& bp)  {
		auto& atsq = const_cast<GameProps::BasicProp&>(bp).get<GameProps::PropID::attackrangesq>();

		return
			&lk ? (ammo * ammo > 0 ? (INT64)(math::Fix::toFix((lk.distSq()) - atsq)) : (INT64)-100) : (INT64)50;
	}
	template<>
	INT64 Exm<GameProps::ENU::state::autoreload>
		(const Physics::LAVNe& lk, const GameProps::Cmd0& cmd, const int& ammo, const  GameProps::BasicProp& bp)  {
		auto& atsq = const_cast<GameProps::BasicProp&>(bp).get<GameProps::PropID::attackrangesq>();
		return
			(ammo <= 0 ? 75 : -125);// : 75;
	}

	template<>
	INT64 Exm<GameProps::ENU::state::nan>
		(const Physics::LAVNe& lk, const GameProps::Cmd0& cmd, const int& ammo, const float& dissq,const  GameProps::BasicProp& bp) {

		return -125;
	}
}
INT64 pj2::AI::vtvy() { return 0; }
