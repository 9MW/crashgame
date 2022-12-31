#include"ecs.hpp"
#include"entitys/unit.h"
#include"cagentity.h"
#include"Render/ShaderStructs.h"
namespace cage {
	using unitd = ECS::ECSclass<playerprop, sStruct::inscmb>;
	struct actioninfo {
		int from, to;
		string msg;
		turnstate trigger;
	};
	inline bool trigger(float p) {
		return p > (std::rand() % 100);
	}
	struct warsys
	{
		playerprop* proppaie[2];
		sStruct::inscmb* transpair[2];
		context* ctx;
		int activer=0, totalplayer = 2;
		std::vector<actioninfo>  msgs;
		void update(int ida, int idb) {
			bool turnbeg = false;
			auto iloop = _countof(proppaie);
			auto loop = [&](auto&& f) {
				for (size_t i = 0; i < _countof(proppaie); i++)
				{
					auto& ip = *proppaie[i];
				}
			};
			for (size_t i = 0; i < iloop; i++)
			{
				turnbeg |= proppaie[i]->state == agentstate::idel;
			}
			if (turnbeg) {
				this->turnbeg(activer);
			}
			//action
			for (size_t i = 0; i < totalplayer; i++)
			{
				auto& ip = *proppaie[i];
				auto& tp = *transpair[i];
				if (ip.state.tick == 0) {
					float& speed = ip.get<PropId::speed>();
					while (speed >= 1) {

						speed -= 1;
					}
				}

			}
		}
		void update(int i);
		void applybuff(PropId prop, playerprop& pdata) {
			auto& ip = pdata;
			auto ib = (int)prop;
			auto& buf = ip.buffs[ib];
			if (trigger(buf.probability))
			{
				auto& tprop = ip.props[(int)buf.propid];
				tprop *= buf.ft;
				tprop += buf.fa;
			}
			buf.tim--;
			assert(buf.tim >= 0);
			if (buf.tim == 0) {
				std::swap(ip.buffs[ib], ip.buffs[ip.buffnum--]);
			}
		}
		void applydmg(Dmg& dmg, int i) {
			auto& tg = *proppaie[getopponent(i)];
			dmg.normal -= tg.get<PropId::shield>();
			tg.get<PropId::shield>() = std::max(0.0f, -dmg.normal);
			tg.get<PropId::hp>() -= dmg.real;
			tg.get<PropId::hp>() -= dmg.normal;
			if (tg.get<PropId::hp>() <= 0) {
				tg.state.SetState(agentstate::died);
			}
		}
		void applyffect(playerprop& pdata, buff& ef) {
			ef.tim--;
			if (!trigger(ef.probability))
				return;
			auto& propi = pdata.props[(int)ef.propid];
			propi += ef.fa;
			propi *= ef.ft;
		}
		template<turnstate S>
		int triggerskill(int i) {
			//for (size_t i = 0; i < _countof(proppaie); i++)
			auto& ip = *proppaie[i];
			auto& op = *proppaie[getopponent(i)];
			auto& tp = *transpair[i];
			for (size_t i1 = 0; i1 < ip.skillnum; i1++)
			{
				auto& skil = ip.skillequpaed[i1];
				auto& skdesc = ctx->skills[skil.id];
				if (skil.trigger != S || skil.action < 1 || !trigger(skil.probability)) {
					return -1;
				}
				skil.action--;
				//auto& skdesc = ctx->skills[skil]; 
				auto& effect = skdesc.effect;
				auto& effnum = skdesc.effnum;
				auto& tg = op;
				auto& self = ip;
				for (size_t i2 = 0; i2 < effnum; i2++)
				{
					auto ef = effect[i2];
					auto& t = ef.mode == 0 ? tg : self;
					if (ef.permanent) {
						t.props[(int)ef.propid] += ef.fa;
						continue;
					}
					if constexpr (S == turnstate::underattack)
						applyffect(t, ef);
					if (ef.tim >= 1)
					{
						t.buffs[(int)ef.propid] = ef;
					}
				}
				return i1;
			}
			return -1;
		}
		void integrateffect(int i) {
			auto& ip = *proppaie[i];
			auto& tp = *transpair[i];
			for (size_t ib = 0; ib < ip.buffnum; ib++)
			{
				auto& buf = ip.buffs[ib];
				auto& tprop = ip.props[(int)buf.propid];
				tprop = (int)tprop % (100);//set
				tprop *= buf.ft;
				tprop += buf.fa;
				buf.tim--;
				assert(buf.tim >= 0);
				if (buf.tim == 0) {
					std::swap(ip.buffs[ib], ip.buffs[ip.buffnum--]);
				}
			}
		}
		void turnbeg(size_t i) {
			{
				auto& ip = *proppaie[i];
				auto& tp = *transpair[i];
				for (size_t i1 = 0; i1 < ip.skillnum; i1++)
				{
					auto& skil = ip.skillequpaed[i1];
					skil.action += skil.actionaccum;
				}
				if (triggerskill<turnstate::start>(i) >= 0) {
				}
				integrateffect(i);

				for (size_t i1 = 0; i1 < ip.skillnum; i1++)
				{
					auto& skil = ip.skillequpaed[i1];
					skil.action += skil.actionaccum;
				}
			}
		}
		int getopponent(int i) {
			return (i + 1) % 2;
		}
		void attack(size_t i);
	};
}