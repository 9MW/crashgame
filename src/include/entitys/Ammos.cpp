#include "Ammos.h"

//not thread safe
//template<int tid0>
namespace  pj2 {
	namespace   n_weapon {
		using Collid = Physics::VirtCollid;
		//not threadsafe function
		int bullet0::build(const n_Component::BuildPara para) {
			assert(0);//abandon
			using namespace GameProps;
			auto& i = para.id;//pool::Get();
			auto tup = this->comps[i];
			Cmd0& cmd = getComp<Cmd0>(tup);
			auto& tgpos = cmd.tpos;
			tgpos = para.tpos;
			auto& trans = getComp<mtransfrom>(tup);
			auto& clid = getComp<Collid>(tup);
			sStruct::instData1& rprop = getComp< sStruct::instData1>(tup);
			rprop.tid = tid0;
			rprop.id = i;
			cmd.cState<0>().SetStateb(state::move);
			auto& am = getComp<gammo>(tup);
			trans.pos4 = para.pos;
			auto dv = tgpos - trans.pos4;
			auto vl3 = (dv).head<3>().norm();
			clid.vel4.head<3>() = (dv).head<3>()/vl3 * vel;
			clid.AngularVelocity.setZero();
			am.begframe=GloProp::Get().FrameId;
			am.activeCounter = 300;
			am.frimpact = vl3 / vel * 50;
			return i;
		}

		int bullet0::build(const n_Component::BuildPara para, cmps::typkg& archs)
		{
			using namespace GameProps;
			auto& i = para.id;//pool::Get();
			auto tup = this->comps[i];
			Cmd0& cmd = getComp<Cmd0>(tup);
			auto& tgpos = cmd.tpos;
			tgpos = para.tpos;
			auto& trans = getComp<mtransfrom>(tup);
			auto& clid = getComp<Collid>(tup);
			//clid.Active = 1;
			auto&& [prefcid,amod] = get<Collid, gammo>(archs);
			clid = prefcid;
			sStruct::instData1& rprop = getComp< sStruct::instData1>(tup);
			rprop.tid = tid0;
			rprop.id = i;
			cmd.cState<0>().SetStateb(state::move);
			auto& am = getComp<gammo>(tup);
			am = amod;
			am.LastImpact = para.hostid;
			trans.pos4 = para.pos;
			auto dv = tgpos - trans.pos4;
			auto vl3 = (dv).head<3>().norm();
			clid.vel4.head<3>() = (dv).head<3>() / vl3 * prefcid.MaxVelocity;
			am.frimpact = vl3 / prefcid.MaxVelocity * (int)(1 / GameProps::Step1s);
			am.begframe = GloProp::Get().FrameId;
			am.activeCounter = 300;//max life time
			return i;
		}

	}

}