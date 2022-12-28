#pragma once
#pragma once
#include <string>
#include<Math1.h>
namespace cage {
	using pj2::mtransform;
	enum turnstate
	{
		underattack, attack, start, afterattack
	};
	struct agentstate
	{
		enum state {
			move, attack, idel, reset, died
		};
		state st;
		int tick;
		void Tick() {
			tick++;
		}
		void SetState(state sst) {
			st = sst;
			tick = 0;
		}

		operator state& () { return st; }
	};
	enum class PropId :int {
		hp, Armor, marmor,speedaccum,damage, mdamage,agile, Critical,animActionrange,  energy, shield, visiableRange, speed,  attackrangesq,  realdamage,   count, nan
	};

	template<class T, int I>
	using array = std::array<T, I>;
	template<class T>
	using array16 = std::array<T, 16>;
	struct AnimationClip
	{
		int32 beg, size, fevt;
		float speed = 1, length = 0, pp, p;
	};
	struct RenderData {
		int modelid,bonenum;
		AnimationClip clip;
	};
	struct buff {
		std::string name;

		float ft = 1, fa = 0, tim = 0, probability = 100;
		bool mode = 0;//decide target; 0 for enemy 
		bool permanent = 0;
		PropId propid = PropId::nan;
	};
	struct Dmg
	{
		float real = 0, normal = 0;

		Dmg& operator*=(float f) {
			real *= f;
			normal *= f;
		}
		bool Zero() { return (real + normal) == 0; }
	};
	struct skilldesc {
		int id, action, actionaccum = 1, effnum;
		float probability, weight;
		turnstate trigger;
		array<buff, 4> effect;
	};
	struct playerprop
	{
		string name;
		std::vector<AnimationClip> anims;
		array16<float> props, currentprop;
		array16<buff> buffs;
		array<skilldesc, 8> skillequpaed;
		mtransform::v4 goalpos;
		mtransform defaulttrans;
		agentstate state;
		RenderData renderinfo;
		
		int skillnum = 1, buffnum;
		template<PropId P>
		float& get() {
			return props[(int)P];
		}
		float& get(PropId P) {
			return props[(int)P];
		}
		template<PropId P>
		buff& getbuff() {
			return buffs[(int)P];
		}
	};
	struct bskill
	{
		std::string name;
		float probability, weight;
		int actionrem, actionaccum, effnum;
		array<buff, 4> effect;
		turnstate trigger;

		template<int I>
		void run(playerprop& self, playerprop& tg) {
			for (size_t i = 0; i < effnum; i++)
			{
				auto& ef = effect[i];
				auto& t = ef.mode == 0 ? tg : self;
				if (ef.permanent) {
					t.props[(int)ef.propid] += ef.fa;
					continue;
				}
				t.buffs[(int)ef.propid] = ef;
			}
		}
		// "boxing";
		template<>
		void run<1>(playerprop& self, playerprop& tg) {
			self.props[(int)PropId::speed] = 0;
		}
	};
}