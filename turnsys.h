#pragma once
#include"entitys/units0.h"
namespace pj2 {
	template<int I = -1>
	struct skillaction;
	struct damage {
		float normal, real;
	};
	namespace fix = Fix;
	using GameProps::ENU::PropID;
	using GameProps::turnbuff;
	template<>
	struct skillaction<0> :GameProps::skilldesc {
		template<class F>
		bool exe(F&& f) {
			if (P < std::rand() % 100)
			{
				f();
				return true;
			}
			return false;
		}
		void parsefrom(void* js) {}
	};
	template<>
	struct skillaction<1> : skillaction<0> {
		void action(GameProps::BasicProp& selfprop, GameProps::BasicProp& targetprop) {
			exe([&]() {targetprop.get<PropID::speed>() -= Fix::One(); });
		}
	};
	template<>
	struct skillaction<2> : skillaction<0> {
		void action(GameProps::BasicProp& selfprop, GameProps::BasicProp& targetprop) {
			exe([&]() {targetprop.get<PropID::speed>() -= Fix::One(); });
		}
	};
	template<>
	struct skillaction<3> : skillaction<0> {
		void action(GameProps::BasicProp& selfprop, GameProps::BasicProp& targetprop) {
			targetprop.get<PropID::speed>() -= Fix::One();
		}
	};
	//boxing
	template<>
	struct skillaction<4> : skillaction<0> {
		int numbuff;
		std::array<turnbuff::buff, 4> buffs;
		void action(
			GameProps::BasicProp& selfprop,
			GameProps::BasicProp& targetprop,
			GameProps::turnbuff& sfbuf,
			GameProps::turnbuff& tgbuf) {
			selfprop.get<GameProps::PropID::speed>() = 0;
			for (size_t i = 0; i < numbuff; i++)
			{
				auto  bfi = buffs[i];
				auto& bf = tgbuf[bfi.id];
				bfi.value = bfi::mulid == -1 ? bfi.value : selfprop[bfi.mulid] * bfi.value;
				bf = bfi;
			}
		}
		void parsefrom(void* js) {}
	};
	template<>
	struct skillaction<5> : skillaction<0> {
		void action(GameProps::BasicProp& selfprop, GameProps::BasicProp& targetprop) {
			targetprop.get<PropID::speed>() -= Fix::One();
		}
	};
	template<int I0=8>
	struct actionSet
	{
		template <std::size_t... I>
		static constexpr auto BuildSet()
		{
			return std::tuple<skillaction<I>,...>();
		}
		decltype(BuildSet<std::make_index_sequence<I0>>()) Skills;
	};

	struct Context
	{
		actionSet<> skills;
		n_units::turnunit units;
	};
	namespace systems {
		using GameProps::ENU::state;
		struct turnsys
		{
			int turnnum = 0;
			Context* pcontext = nullptr;

			void turnbeg(const size_t& i) {
				using namespace  n_units;
				using namespace  Fix;
				using GameProps::stateupdateid;
				using repo = pj2::n_units::df_repo;
				auto& comps = pcontext->units.comps;
				GameProps::Cmd0& cmd = comps.get<GameProps::Cmd0>(i);
				GameProps::BasicProp& prop = comps.get<GameProps::BasicProp>(i);
				auto& speed = prop.get<PropID::speed>();
				speed += One();
				repo& skils = comps.get<repo>(i);
				for (size_t i = 0; i < skils.num; i++)
				{
					auto& item = skils.get(i);
					item.skdes.actionrem += item.skdes.actionperturn;
				}
			}
			void update(const size_t& i) {
				using namespace  n_units;
				using namespace  Fix;
				using GameProps::stateupdateid;
				using repo = pj2::n_units::df_repo;
				auto& comps = pcontext->units.comps;
				GameProps::Cmd0& cmd = comps.get<GameProps::Cmd0>(i);
				GameProps::BasicProp& prop = comps.get<GameProps::BasicProp>(i);
				GameProps::statet& prop = comps.get<GameProps::BasicProp>(i);
				auto& speed = prop.get<PropID::speed>();
				speed += One();
				repo& skils = comps.get<repo>(i);
				auto exeskill = [&](auto& skill_) {
					if (skill_.TriggerState == turnstate && skill_.actionrem > 1) {
						ECS::visit_at(skill_.id, [&](auto I) {
							GameProps::BasicProp& tprop = comps.get<GameProps::BasicProp>(cmd.eid.id);
							reinterpret_cast<skillaction<I>&>(skill_).action(prop, tprop);
							});
					}
					skill_.actionrem -= skill_.actionperturn;
				};

				if (cmd.cState() == state::idle) {

				}
				else if (speed > One() && cmd.cState() == state::attack) {
					const auto turnstate = GameProps::stateupdateid::turnbegain;
					for (size_t i = 0; i < skils.num; i++)
					{
						auto& item = skils.get(i);
						exeskill(item.skdes);
					}
				}
			}

			void update(const EntityId& i) {

			}
			void startfight(const EntityId& i) {

			}
			void turnbegain(const EntityId& i) {

			}
			void afterattack(const EntityId& self, const EntityId& target) {

			}
			void underattack(const EntityId& self, const EntityId& target) {

			}
		};
	}
}