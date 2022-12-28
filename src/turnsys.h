#include"ecs.hpp"
#include"entitys/unit.h"
#include"cagentity.h"
#include"Render/ShaderStructs.h"
namespace cage {
	using std::string;
	struct context
	{
		static float convertime(int i) {
			return i * 0.016;//assume 60 frame per second
		}
		int turn;
		float tmove=2;
		std::vector<std::tuple<string, array<int, 4>>> textppool;
		static inline context* maincontext;
		std::vector<bskill> skills;
		static context& main() {
			static context m;
			if (!context::maincontext) {
				context::maincontext = &m;
				auto& skills = m.skills;
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
						sk.name = "solider skill";
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
						sk.name = "hunter skill";;
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
		static void Text(string msg, int i) {
			auto& textppool = maincontext->textppool;
			auto& [text, id] = textppool.emplace_back();
			text = msg;
			id[0] = i;
			id[1] = maincontext->turn;
		}
	};

	using unitd = ECS::ECSclass<playerprop, sStruct::inscmb>;
	bool trigger(float p) {
		return p > (std::rand() % 100);
	}
	struct actioninfo {
		int from, to;
		string msg;
		turnstate trigger;
	};
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
		void update(int i) {
			auto iop = getopponent(i);
			auto& ip = *proppaie[i];
			auto& tg = *proppaie[iop];
			auto& prop0 = transpair[i];
			auto& stani = ip.renderinfo.clip;
			int cyclenum = (ip.state.tick * stani.speed) / stani.size;
			int precyclenum = ((ip.state.tick - 1) * stani.speed) / stani.size;
			auto framaddrss = stani.size > 0 ? ((int)(ip.state.tick * stani.speed) % stani.size) : 0;
			float pratio = std::min(stani.size > 0 ? (framaddrss / (float)stani.size) : 0, 0.9999f);
			prop0->insd.frameIndex = (int)(stani.size * pratio) * ip.renderinfo.bonenum;
			auto attackgoalpos = [&]() {
				float& v = ip.get<PropId::animActionrange>();
				auto vgoal3 = (transpair[iop]->transform.pos4 - prop0->transform.pos4).head<3>();
				//assume both model radius = 0.5
				return prop0->transform.pos4.head<3>() + vgoal3.normalized() * (vgoal3.norm() - (1 + v));
			};
			auto LookAt = [&]() {
				auto forward = prop0->transform.froward();
				mtransform::v4 targetdir = ip.goalpos - prop0->transform.pos4;
				//constexpr int kmk =( 1.012 - 1.003)*1000;
				auto radcos = (math::dot(targetdir.head<2>().normalized(), forward.head<2>()));
				if (1000 * (1 - (radcos)) == 0) {
					return true;
				}
				auto idealrotation = math::Quaternionf::FromTwoVectors(forward, targetdir.head<3>()) * prop0->transform.rotation;
				prop0->transform.rotation = prop0->transform.rotation.slerp(context::convertime(1), idealrotation);
				return false;
			};
			if (ip.state.tick == 0) {
				ip.renderinfo.clip = ip.anims[ip.state];
			}
			float& speed = ip.get<PropId::speed>();
			float posfactor = 0;
			LookAt();
			switch (ip.state)
			{
			case agentstate::move:
				posfactor = context::convertime(ip.state.tick) / ctx->tmove;

				if (posfactor >= 1) {
					{
						ip.goalpos.head<3>() = attackgoalpos();
						//return
						if ((int)((ip.goalpos - ip.defaulttrans.pos4).norm() * 1000) == 0) {
							if (speed >= 1) {
								speed -= 1;
								turnbeg(activer);
								ip.state.SetState(agentstate::move);
							}
							else
							{
								ip.state.SetState(agentstate::idel);
								activer++;
							}
						}
						else
						{
							ip.state.SetState(agentstate::attack);
						}
					}
				}
				break;
			case agentstate::attack:
				posfactor = 1;
				if (cyclenum >= 1 && precyclenum == 0)//only play once
				{
					attack(getopponent(i));
					if (tg.get< PropId::hp>() <= 0) {
						tg.state.SetState(agentstate::died);
					}
					ip.goalpos = ip.defaulttrans.pos4;
					float& speed = ip.get<PropId::speed>();
					speed -= 1;
				}
				//return
				if (precyclenum >= 1 && LookAt()) {
					ip.state.SetState(agentstate::move);
				}
				break;
			case agentstate::died:
				break;
			case agentstate::idel:
				if (ip.state.tick == 0 && i == activer) {
					if (speed < 1)
						speed += ip.get<PropId::speedaccum>();
					if (speed >= 1) {
						turnbeg(activer);
						ip.goalpos = transpair[iop]->transform.pos4;
						ip.state.SetState(agentstate::move);
					}
					else
					{
						activer++;
						activer %= totalplayer;
					}
				}
				break;
			case agentstate::reset:
				break;
			default:
				std::abort();
				break;
			}
			ip.state.Tick();
		}
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
				auto effect = skil.effect;
				auto& effnum = skil.effnum;
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
					auto& skil = ip.skillequpaed[i];
					auto& msg = msgs.emplace_back();
					auto& skdesc = ctx->skills[skil.id];
					msg.msg = skdesc.name;
					msg.from = i;
					msg.trigger = skdesc.trigger;
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
		void attack(size_t i) {

			auto& ip = *proppaie[i];
			auto& tp = *transpair[i];
			{
				float agile = ip.get<PropId::agile>();
				Dmg dmg;
				dmg.normal = ip.get<PropId::damage>();
				dmg.real = ip.get<PropId::realdamage>();
				auto& buff = ip.getbuff<PropId::damage>();
				if (buff.tim >= 1) {
					dmg.normal *= buff.ft;
					dmg.normal += buff.fa;
				}
				if (trigger(ip.get<PropId::Critical>()))
				{
					dmg *= 1.5;
				}
				if (!dmg.Zero()) {
					auto& iop = *proppaie[getopponent(i)]; \
						int sk = triggerskill<turnstate::underattack>(getopponent(i));
					if (trigger(iop.get<PropId::agile>())) {
						context::Text("miss", i);
						return;
					}
					triggerskill<turnstate::attack>(i);
					applydmg(dmg, i);
				}
			}
		}
	};
}