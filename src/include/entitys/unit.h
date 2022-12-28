#pragma once
#include<AI/Config.h>
#include"Gameprops0.h"
#include"Render/ShaderStructs.h"
#include"common/ecs.hpp"
#include "Common/interface/RefCntAutoPtr.hpp"
#include "Graphics/GraphicsEngine/interface/Buffer.h"
#include"comm.h"
#include"io/AudioManager.h"
#include"GObjs0.h"
namespace pj2{

	namespace n_BasObj {
		using namespace GameProps::ENU;
		using namespace math::Fix;
		/*struct primaryweapon {
			static	int& feq() {
				static int feq;
				return feq;
			};
		};*/
		//template<class Eclass>
		//struct Ubase
		//{
		//	
		//	//static int update(Manager::UpdatePara const* par);
		//};
		//template<state ...st>
		//struct statecate
		//{
		//	static constexpr std::array<state, sizeof...(st)> states = { st... };
		//	template<state t>
		//	static constexpr bool contain() {
		//		for (size_t i = 0; i < states.size(); i++)
		//		{
		//			if (states[i] == t)
		//				return true;
		//		}
		//		return false;
		//	}
		//};
		//struct ustate
		//{
		//	PropID prp= PropID::nan;
		//	Fix::fix_s d, dur;
		//};
		//struct u0states
		//{
		//	std::array<ustate, 32> _states;
		//	inline auto&& operator[](const math::ui64& i) {
		//		return _states[i];
		//	}
		//};
		using statecate1 = statecate<state::move, state::pick, state::put>;
		struct ust0
		{

			union {
				//GameProps::states2 pcstate;
				state _st;
				array<int16, 2> pcstate;
			};
			int  _statetick;
			using a_st = GameProps::ENU::state;
			inline void SetState(const GameProps::ENU::state st) {
				pcstate[0] = pcstate[1];
				auto&& CState = pcstate[1];
				CState = (int16)st;
				/*for (size_t i = 0; i < pcstate.size()-1; i++)
				{
					pcstate[i] = pcstate[i + 1];
				}
				pcstate.back() = st;*/
			}

			inline void SwiState() {
				_st = (state)(pcstate[1]);
				_statetick = 0;
			}
			inline operator const a_st()const {
				return (a_st)_st;
			}
		};
		struct unitcp0
		{
			static constexpr int stateslots = 2;
			using t_states = array<ust0, stateslots>;
			//GameProps::Cmd0 currentCmd;
			//Manager::ObjState _state;
			t_states _states;
			//array<ust0, stateslots>_states;
			/*union {
				struct {
					ust0 PState, CState;
				}
			}*/
			//state _aistate;
			//set 0 when switch state
			//animation switching minus tick determine if state is fully switched
			//switch time could be find based on previous and target state
			//state switch need wait after minimal allow current state tick
			int _statetick, CommandId, cmdqueueid, audioo, audiol;
			/*inline void SetState(const state st) {
				_state.SetState(st);
				_statetick = 0;

			}*/
			inline void tick() {
				_statetick++;
				for (size_t i = 0; i < stateslots; i++)
				{
					_states[i]._statetick++;
				}
			}
			template<state st>
			inline void SetState() {
				if constexpr (statecate1::contain<st>()) {
					_states[0].SetState(st);
					_statetick = 0;
				}
				else {
					_states[1].SetState(st);
					_statetick = 0;
				}
			}
			inline void SetState(const std::array<GameProps::ENU::state, stateslots>& cmmd) {
				for (size_t i = 0; i < stateslots; i++)
				{
					_states[i].SetState(cmmd[i]);
					_states[i]._statetick = 0;
				}
			}

		};
		using audioloc = std::array<int, 2>;
		struct Ubase:Renderble
		{
			struct equprop
			{
				mtransform eqpoffset;
			};
			static std::vector<equprop>& eqptan(const int& i) {
				static std::vector<equprop> eqps;
				return eqps;
			}
		};
		using Graphic::stateanim;
		struct Ubaseanim:Ubase
		{
			int bonenum = 0;
			std::array<stateanim, vilstate*8> animinfo;
			std::array<std::string, vilstate> aninames;
			int update(const size_t& W) { return -1; }
		};
		struct muzzle4 {
			std::array<math::v4f, 4> muzzels;
			inline math::v4f& muze0() { return muzzels[0]; }
		};
		
		struct unit0prop {
			int treload, reload;
		};
		template<class C> class Ub0 :Ubase
		{
			using t_prop = C;
		};
		struct VisEle
		{
			Physics::LAVNe lv;
			operator Physics::LAVNe& () { return lv; }
		};
		using unitScanner = std::array< VisEle, 8>;
		using Cmp_unit = ECS::ECSclass<n_BasObj::ecspoobserve,unitcp0,
			Ub0<GameProps::ArDm<Armor::nan, t_Dmg::nan>>, n_objProp::dRender, unitScanner,
			GameProps::deltaProp, GameProps::BasicProp, GameProps::Cmd0,n_Objs::stateTab>;

		/*using Cmp_unit = ECS::ECSclass<pj2::AI::postp, unitcp0,
			Ub0<GameProps::ArDm<Armor::nan, t_Dmg::nan>>, mtransfrom, n_objProp::dRender,
			GameProps::deltaProp, GameProps::BasicProp, GameProps::Cmd0>;*/

		/*template<state s, class T>
		inline int u0state::update<state::autoreload,T>(Manager::UpdatePara const* par, T&& data)
		{
			return 0;
		}*/

		/*using Cmp_unit = ECS::ECSclass<pj2::AI::postp, unitcp0,
			Ub0<GameProps::ArDm<Armor::nan, t_Dmg::nan>>, sStruct::instData, GameProps::deltaProp, GameProps::BasicProp>;*/
	}
}