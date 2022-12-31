#pragma once
#pragma once
#include <string>
#include<Common/Math1.h>
namespace cage {
	using mtransform=math::mtransform;
	enum turnstate
	{
		underattack, attack, start, afterattack,count
	};
	struct agentstate
	{
		enum state {
			move, attack, idel, reset, died,count
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
		hp, Armor, marmor,speedaccum,damage, mdamage,agile, Critical,animActionrange,  energy, shield, visiableRange, speed,  attackrange,  realdamage,   count, nan
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

		inline Dmg& operator*=(float f) {
			real *= f;
			normal *= f;
			return *this;
		}
		inline bool Zero() { return (real + normal) == 0; }
	};
	struct skilldesc {
		int id=-1, action, actionaccum = 1, effnum;
		float probability, weight;
		turnstate trigger;
		array<buff, 4> effect;
	};
	struct playerprop
	{
		std::string name;
		std::vector<AnimationClip> anims;
		array16<float> props, currentprop;
		array16<buff> buffs;
		array<skilldesc, 8> skillequpaed;
		mtransform::v4 goalpos;
		mtransform defaulttrans;
		agentstate state;
		RenderData renderinfo;
		//array<array<UCHAR, 4>, turnstate::count> ActiveSkills;
		int skillnum = 1, buffnum, maxskill = 4, maxskillweight = 5, turn = 0;
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
	using std::string;
	struct GameState
	{
		int running = 0;
	};
	struct context
	{
		static float convertime(int i) {
			return i * 0.016;//assume 60 frame per second
		}
		int turn,playerid=-1,minequpskill=2;
		float tmove=2;
		std::vector<std::tuple<string, array<int, 4>>> textppool;
		static inline context* maincontext;
		std::vector<bskill> skills; 
		GameState gstate;
		static context& main();
		static void Text(string msg, int i) {
			auto& textppool = maincontext->textppool;
			auto& [text, id] = textppool.emplace_back();
			std::memset(&id,0,sizeof(id));
			text = msg;
			id[0] = i;
		}
	};
	struct UIEvent
	{
		using pevfunc = void(*)(void*);
		pevfunc pf=NULL;
		UIEvent& operator=(pevfunc p){
			pf = p;
			return *this;
		}
		void operator()(void*para) {
			pf(para);
		}
	};
	template<class T>
	void reset(T& t) {
		t = T();
	}
}