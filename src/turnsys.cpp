#include "turnsys.h"
namespace cage {
	int equal(float a, float b) {
		return !((a - b) * 1000);
	}
	int zero(float a, float b) {
		return !((a - b) * 1000);
	}
	std::queue<std::tuple<agentstate::state, mtransform::v3>> cmdbuffer;
	/**
	 * @brief 
	 * agent main logic include update animation and AI
	 * @param i 
	 * agent id
	*/
	void cage::warsys::update(int i) {
		auto iop = getopponent(i);
		auto& ip = *proppaie[i];
		auto& tg = *proppaie[iop];
		auto& prop0 = transpair[i];
		auto& stani = ip.anims[ip.state];
		int totalframe = stani.size / ip.renderinfo.bonenum;
		int cyclenum = ((ip.state.tick) * stani.speed) / totalframe;
		auto framaddrss = stani.size > 0 ? ((int)(ip.state.tick * stani.speed) % totalframe) : 0;
		float pratio = std::clamp((framaddrss / (float)totalframe), 0.0f, 0.9999f);
		prop0->insd.frameIndex = stani.beg + (int)(pratio * totalframe) * ip.renderinfo.bonenum;
		auto attackgoalpos = [&]() {
			float& v = ip.get<PropId::animActionrange>();
			auto vgoal3 = (transpair[iop]->transform.pos4 - prop0->transform.pos4).head<3>();
			//assume both model radius = 0.7
			constexpr float radius = 0.7;
			return prop0->transform.pos4.head<3>() + vgoal3.normalized() * (vgoal3.norm() - (radius*2 + v));
		};
		auto LmLookAt = [&]() {
			auto forward = prop0->transform.froward();
			mtransform::v4 targetdir = tg.defaulttrans.pos4 - prop0->transform.pos4;
			auto radcos = (math::dot(targetdir.head<2>().normalized(), forward.head<2>()));
			if (1000 * (1 - (radcos)) == 0) {
				return true;
			}
			auto idealrotation = prop0->transform.rotation * math::Quaternionf::FromTwoVectors(forward, targetdir.head<3>());
			auto adis = idealrotation.angularDistance(prop0->transform.rotation);
			prop0->transform.rotation = prop0->transform.rotation.slerp(context::convertime(3), idealrotation);
			auto adis2 = idealrotation.angularDistance(prop0->transform.rotation);
			return false;
		};
		auto Lmwritecommand = [&]() {
			decltype(cmdbuffer) empty;
			std::swap(cmdbuffer, empty);
			auto& ar = ip.get<PropId::attackrange>();
			if ( ar> 0) {
				auto& [action, acpos] = cmdbuffer.emplace();
				action = agentstate::attack;
				acpos = tg.defaulttrans.pos;
			}
			else
			{
				{
					auto& [action, acpos] = cmdbuffer.emplace();
					action = agentstate::move;
					acpos = attackgoalpos();
				}
				{
					auto& [action, acpos] = cmdbuffer.emplace();
					action = agentstate::attack;
					acpos = tg.defaulttrans.pos;
				}
				{
					auto& [action, acpos] = cmdbuffer.emplace();
					action = agentstate::move;
					acpos = ip.defaulttrans.pos;
				}
			}

		};
		auto Lmnextturn = [&]() {
			activer++;
			activer %= totalplayer;
		};
		auto Lmexecmd = [&]() {
			if (cmdbuffer.size() > 0)
			{
				auto& [action, acpos] = cmdbuffer.front();
				ip.goalpos.head<3>() = acpos;
				ip.state.SetState(action);
				cmdbuffer.pop();

			}
			else {
				ip.state.SetState(agentstate::idel);
				Lmnextturn();
			}
		};
		if (ip.state.tick == 1) {
			ip.renderinfo.clip = ip.anims[ip.state];
		}
		if (ip.get<PropId::hp>() <= 0) {
			tg.state.SetState(agentstate::died);
		}
		float& speed = ip.get<PropId::speed>();
		float posfactor = 0;
		bool lk = LmLookAt();
		if (ip.get<PropId::hp>() <= 0) {
			ip.state.SetState(agentstate::died);
			activer = -1;
		}
		switch (ip.state)
		{
		case agentstate::move:

			posfactor = context::convertime(ip.state.tick) / ctx->tmove;
			if (posfactor >= 1) {
				if (cmdbuffer.size() > 0)
					Lmexecmd();
				else if (speed >= 1)
				{
					turnbeg(activer);
					Lmwritecommand();
					Lmexecmd();
				}
				else {
					Lmexecmd();
				}
			}
			break;
		case agentstate::attack:
			posfactor = 1;
			if (cyclenum >= 1)//only play once
			{
				attack(i);
				float& speed = ip.get<PropId::speed>();
				speed -= 1;
				Lmexecmd();
			}
			break;
		case agentstate::died:

			break;
		case agentstate::idel:
			if (i == activer) {
				//if (speed < 1)
				speed += ip.get<PropId::speedaccum>();
				if (speed >= 1) {
					turnbeg(activer);
					Lmwritecommand();
					Lmexecmd();
				}
				else { Lmnextturn(); }
			}
			break;
		case agentstate::reset:
			break;
		default:
			std::abort();
			break;
		}
		ip.state.Tick();
		mtransform::v4 targetvec = ip.goalpos - prop0->transform.pos4;
		if (!equal(posfactor, 1))
			prop0->transform.pos4 += targetvec * posfactor;
	}
	void warsys::applydmg(Dmg& dmg, int i) {
		int iop = getopponent(i);
		auto& tg = *proppaie[iop];
		dmg.normal -= tg.get<PropId::shield>();
		tg.get<PropId::shield>() = std::max(0.0f, -dmg.normal);
		tg.get<PropId::hp>() -= dmg.real;
		tg.get<PropId::hp>() -= dmg.normal;
	}
	/**
	 * @brief 
	 * called while agent turns begain
	 * @param i 
	 * current active agent id
	*/
	inline void warsys::turnbeg(size_t i) {
		{
			auto& ip = *proppaie[i];
			auto& tp = *transpair[i];
			for (size_t i1 = 0; i1 < ip.buffnum; i1++)
			{
				auto& buff = ip.buffs[i1];
				buff.tim--;
				assert(buff.tim >= 0);
				//if time limit reached
				if (buff.tim == 0)
					std::swap(ip.buffs[i1], ip.buffs[--ip.buffnum]);
			}
			integrateffect(i);

			for (size_t i1 = 0; i1 < ip.skillnum; i1++)
			{
				auto& skil = ip.skillequpaed[i1];
				skil.action += skil.actionaccum;
			}
			ip.turn++;
		}
	}
	//input:active player id
	void warsys::attack(size_t i) {

		auto& ip = *proppaie[i];
		{
			int idop = getopponent(i);
			Dmg dmg; float df = 1;
			dmg.normal = ip.get<PropId::damage>();
			dmg.real = ip.get<PropId::realdamage>();
			auto& buff = ip.getbuff<PropId::damage>();
			if (buff.tim >= 1) {
				dmg.normal *= buff.ft;
				dmg.normal += buff.fa;
			}
			if (trigger(ip.get<PropId::Critical>()))
			{
				df = 1.5;
			}
			if (!dmg.Zero()) {
				auto& iop = *proppaie[idop];
					int sk = triggerskill<turnstate::underattack>(idop);
				if (trigger(iop.get<PropId::agile>())) {
					context::Text("miss", i);
					return;
				}
				dmg *= df;
				triggerskill<turnstate::attack>(i);
				string dmginfo;
				if (df > 1) {
					dmginfo=("Critical\n");
				}
				applydmg(dmg, i);
				dmginfo += std::to_string(-(std::max(dmg.normal, 0.0f) + dmg.real));
				context::Text(dmginfo, idop);
			}
		}
	}
	/**
	 * @brief 
	 * @return 
	 * static instance
	*/
	cage::context& cage::context::main() {
		static context m;
		if (!context::maincontext) {
			context::maincontext = &m;
			auto& skills = m.skills;
			//load default skill config
			if (m.skills.size() == 0)
			{
				{
					auto& sk = skills.emplace_back();
					sk.name = "throw sand";
					sk.trigger = turnstate::start;
					sk.weight = 2;
					sk.probability = 18;
					sk.effnum = 1;
					auto& skillbuff = sk.effect[0];
					skillbuff.propid = PropId::damage;
					skillbuff.ft = 0;
					skillbuff.tim = 1;
					skillbuff.probability = 80;
				}
				{
					auto& sk = skills.emplace_back();
					sk.name = "knee attack";
					sk.trigger = turnstate::attack;
					sk.weight = 2;
					sk.probability = 10;
					sk.effnum = 1;
					auto& skillbuff = sk.effect[0];
					skillbuff.propid = PropId::realdamage;
					skillbuff.ft = 1;
					skillbuff.fa = 2;
					skillbuff.tim = 1;
					auto& skillbuff2 = sk.effect[1];
					skillbuff2.propid = PropId::speed;
					skillbuff2.ft = 0;
					skillbuff2.fa = 0;
					skillbuff2.tim = 1;
					skillbuff2.probability = 10;
				}
				{
					auto& sk = skills.emplace_back(skills.back());
					sk.name = "subtitute";
					sk.trigger = turnstate::underattack;
					sk.weight = 2;
					sk.probability = 10;
					sk.effnum = 1;
					auto& skillbuff = sk.effect[0];
					skillbuff.propid = PropId::agile;
					skillbuff.ft = 1;
					skillbuff.fa = 100;
					skillbuff.tim = 1;
				}
				{
					auto& sk = skills.emplace_back();
					sk.name = "boxing";
					sk.trigger = turnstate::start;
					sk.weight = 2;
					sk.probability = 20;
					sk.effnum = 3;
					//jump this turn;
					{
						auto& skillbuff = sk.effect[0];
						skillbuff.mode = 1;
						skillbuff.propid = PropId::speed;
						skillbuff.ft = 0;
						skillbuff.tim = 1;
					}
					{
						//4x damage at next turn
						auto& selfbuff = sk.effect[1];
						selfbuff.mode = 1;
						selfbuff.propid = PropId::damage;
						selfbuff.ft = 4;
						selfbuff.tim = 2;
					}
					{
						//-2 armor for opponent
						auto& skillbuff = sk.effect[2];
						skillbuff.propid = PropId::Armor;
						skillbuff.ft = 1;
						skillbuff.fa = -2;
						skillbuff.tim = 3;
					}
				}
				{
					auto& sk = skills.emplace_back();
					sk.name = "fast";
					sk.trigger = turnstate::start;
					sk.weight = 2;
					sk.probability = 15;
					sk.effnum = 2;
					{
						auto& skillbuff = sk.effect[0];
						skillbuff.propid = PropId::damage;
						skillbuff.ft = 1;
						skillbuff.fa = 1;//tral dmg
						skillbuff.tim = 1;
					}
					{
						auto& selfbuff = sk.effect[1];
						selfbuff.mode = 1;
						selfbuff.propid = PropId::speedaccum;
						selfbuff.ft = 1;
						selfbuff.tim = 2;
					}
				}
				{
					auto& sk = skills.emplace_back();
					sk.name = "hunter skill";
					sk.trigger = turnstate::underattack;
					sk.weight = 2;
					sk.probability = 15;
					sk.effnum = 1;
					auto& skillbuff = sk.effect[0];
					skillbuff.propid = PropId::Armor;
					skillbuff.ft = 1;
					skillbuff.fa = 1;
					skillbuff.tim = 1;
				}
				{
					auto& sk = skills.emplace_back();
					sk.name = "warrior skill";;
					sk.trigger = turnstate::afterattack;
					sk.weight = 2;
					sk.probability = 18;
					sk.effnum = 1;
					auto& skillbuff2 = sk.effect[0];
					skillbuff2.propid = PropId::realdamage;
					skillbuff2.ft = 1;
					skillbuff2.fa = 1;
					skillbuff2.tim = 1;
				}
			}
		}
		return m;
	}

}