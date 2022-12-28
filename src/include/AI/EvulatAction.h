#pragma once
#include"physics/physics.hpp"
#include"Gameprops0.h"
namespace pj2::AI {
	
	/*template<GameProps::ENU::state GST, class ...Ts>
	INT64 Exm(const Ts&...arg);*/
	template<GameProps::ENU::state GST, class ...Ts>
	INT64 Exm(const Ts&...arg);
	template<GameProps::ENU::state GST, GameProps::ENU::state TS, class ...Ts>
	INT64 Exm(Ts&...arg);
	INT64 vtvy();

}