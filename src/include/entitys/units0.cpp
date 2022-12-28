#include "units0.h"
#include"AllType.h"
#include"GProps.h"
namespace pj2::n_BasObj {
	using namespace math::Fix;
	using namespace n_weapon;
	using  GameProps::ENU::state;
	//template<class sclass, class Eclass>
	//int Ubase0<sclass, Eclass>::update(Manager::UpdatePara const* par)
	//{
	//	using C = sclass;
	//	using T = Eclass;
	//	Manager::UpdatePara const& para = *par;
	//	auto& achs = para.dfac;
	//	auto& ct = para.ct;
	//	auto&& tuptop = Manager::Cmps<Eclass>(achs, para.id);
	//	auto&& [dp, bp] = achs.template
	//		getCompFrom<T, GameProps::deltaProp, GameProps::BasicProp>(para.id);
	//	auto& sprop = tuptop.get<n_objProp::dRender>();
	//	unitcp0& prop0 = tuptop.get<unitcp0>();

	//	GameProps::BasicProp ij;
	//	/*t_auff& af = t_self::audiooffset()[(int)prop0._state];
	//	const auto offreal = af.off0 - prop0._statetick;
	//	sprop.audiosrc = offreal < 0 ? -1 : af.off1 + offreal;*/
	//	constexpr auto tid = n_Registry::Idof<C>;
	//	auto&& insid = para.id;
	//	prop0.tick();
	//	auto&& trs = ct.getTrans(tid, insid);
	//	auto&& currentCmd = tuptop.get<GameProps::Cmd0>();
	//	const auto distsq = math::Fix::FixDot3(currentCmd.tpos, trs.pos4);
	//	const auto perframeprogess = GameProps::autiosetting::Main().perframeprogess;
	//	//using gun =typename n_Registry::Registry::firestderi<n_BasObj::primaryweapon>;// n_weapon::gun0;
	//	using gun = typename ECS::derif<Eclass, n_BasObj::primaryweapon>;// n_weapon::gun0;

	//	prop0.audioo = prop0.audioo < prop0.audiol ? prop0.audioo + perframeprogess : math::ui32max;
	//	for (size_t i = 0; i < unitcp0::stateslots; i++) {
	//		auto& st = prop0._states[i];
	//		//const state st = prop0._states[i];
	//		switch (st)
	//		{
	//		case state::attack:
	//			auto&& freq = bp.get<PropID::feq>();
	//			const auto&& reg = bp.get<PropID::attackrangesq>();

	//			//sprop.audiosrc< = gun.audiosrc;
	//			if (reg > distsq) {
	//				if (prop0._statetick != 0 && (prop0._statetick % freq) == 0) {
	//					auto&& guni = tuptop.get<gun>();
	//					auto ret = guni.fire(trs.pos4, para, AmmPara0{});
	//					if (ret.result == wstate::reload)
	//						prop0.SetState(state::autoreload);
	//					else {
	//						prop0.audioo = guni.audiosrc;
	//						prop0.audiol = guni.audiolength + prop0.audioo;
	//					}
	//					//sprop.audiosrc = gun::prop().audio_src;
	//				}
	//			}
	//			else
	//			{
	//				prop0.SetState<state::move>();
	//			}
	//			break;
	//		case state::autoreload:
	//			auto&& gun = tuptop.get<n_weapon::gun0>();
	//			//gun.reload().result==wstate::finsh?
	//			break;
	//		case state::move:
	//			auto&& gun = tuptop.get<n_weapon::gun0>();
	//			//gun.reload().result==wstate::finsh?
	//			break;
	//		default:
	//			const state s1 = (state)(st.pcstate[0]);
	//			const state s2 = (state)(st.pcstate[1]);
	//			st.SwiState();
	//			//sprop.audiosrc = -1;
	//			/*switch (s2)
	//			{
	//			case state::attack:
	//				auto&& gun = tuptop.get<gun>();

	//			}*/
	//			//state transition
	//			break;
	//		}
	//	}
	//	sprop.audiosrc = prop0.audioo;
	//	prop0.tick();
	//	return 0;
	//}

	/*template<class sclass, class Eclass>
	int Ubase0<sclass, Eclass>::update2(Manager::UpdatePara const* par)
	{
		Manager::UpdatePara const& para = *par;
		auto& achs = para.dfac;
		auto& ct = para.ct;
		auto&& tuptop = Manager::Cmps<Eclass>(achs, para.id);
		u0states&& prop0 = tuptop.get<u0states>();
		GameProps::BasicProp& bprop = tuptop.get< GameProps::BasicProp>();
		for (size_t i = 0; prop0[i].prp != PropID::nan; i++)
		{
			auto& cst = prop0[i];
			bprop.get(cst.prp) += cst.d;
		}
		return 0;
	}

	template<class sclass, class Eclass>
	int Ubase0<sclass, Eclass>::Build(Manager::UpdatePara const* par)
	{
		Manager::UpdatePara const& para = *par;
		auto& achs = para.dfac;
		auto&& tuptop = Manager::Cmps<Eclass>(achs, para.id);
		auto&& prop0 = tuptop.get<unitcp0>();
		return 0;
	}*/

}

namespace pj2::n_units {
	using namespace n_objProp;
	/*template<class Tp>
	static int Build(Manager::t_Arch& achs, const int& id, Tp& arg);*/

	//int pl0::Build(Manager::t_Arch& achs, const int& id, ubuilder0& arg)
	/*void pl0::build(Manager::t_Arch& ach, const int& id, const mtransfrom& pos) {
		using gun = typename ECS::derif<cmps, n_BasObj::primaryweapon>;
		auto&& [trans, aminfo,wep, ucp] = Cmps<cmps>(ach, id).getref<mtransfrom,
			n_objProp::dRender,gun, unitcp0>();
		//wep.audiosrc=
		trans = pos;
		aminfo = {};
	}*/
	//template<class Tp>
	//int pl0::Build(Manager::t_Arch& achs, const int& id, Tp& arg) {
	//	const mtransfrom& trans_ = Utility::get<mtransfrom>(arg);
	//	auto&& tuptop = Manager::Cmps<cmps>(achs, id);
	//	unitdesc& ud = Utility::get<unitdesc>(arg);
	//	auto& wd = Utility::get<GameProps::weaponpara>(arg);
	//	auto&& gun = tuptop.get<n_weapon::gun0>();
	//	gun.audiolength = wd.audiolength;
	//	gun.audiosrc = wd.audiosrc;

	//	auto&& [dp, bp] = achs.template
	//		getCompFrom<cmps, GameProps::deltaProp, GameProps::BasicProp>(para.id);
	//	//gun.audiosrc=
	//	Bs::Build(achs, id, trans_);
	//	return 0;
	//}
	//extern template int pl0::Build<App0::t_cObjarg>(Manager::t_Arch& achs, const int& id, App0::t_cObjarg& arg);
	//template<>     
	//int pl0::Build<App0::t_cObjarg>(Manager::t_Arch& achs, const int& id, App0::t_cObjarg& arg)
	//int pl0::update(Manager::UpdatePara const* par)
	//{
	//	using sclass = pl0;
	//	using Eclass = cmps;
	//	//string sds = "getCompFrom<cmps, GameProps::deltaProp, GameProps::BasicProp>(para.id)";
	//	constexpr int ssz = sizeof(string);
	//	using C = sclass;
	//	using T = Eclass;
	//	Manager::UpdatePara const& para = *par;
	//	auto& achs = para.dfac;
	//	auto& ct = para.ct;
	//	auto&& tuptop = Manager::Cmps<cmps>(achs, para.id);
	//	auto&& [dp, bp] = achs.template
	//		getCompFrom<cmps, GameProps::deltaProp, GameProps::BasicProp>(para.id);
	//	auto& sprop = tuptop.get<n_objProp::dRender>();
	//	unitcp0& prop0 = tuptop.get<unitcp0>();

	//	GameProps::BasicProp ij;
	//	/*t_auff& af = t_self::audiooffset()[(int)prop0._state];
	//	const auto offreal = af.off0 - prop0._statetick;
	//	sprop.audiosrc = offreal < 0 ? -1 : af.off1 + offreal;*/
	//	const auto tid = para.tid;// n_Registry::Idof<C>;
	//	auto&& insid = para.id;
	//	prop0.tick();
	//	auto&& trs = tuptop.get<mtransfrom>();// ct.getTrans(tid, insid);
	//	auto&& currentCmd = tuptop.get<GameProps::Cmd0>();
	//	const auto distsq = math::Fix::FixDot3(currentCmd.tpos, trs.pos4);
	//	const auto perframeprogess = GameProps::autiosetting::Main().perframeprogess;
	//	//using gun =typename n_Registry::Registry::firestderi<n_BasObj::primaryweapon>;// n_weapon::gun0;
	//	using gun = typename ECS::derif<Eclass, n_BasObj::primaryweapon>;// n_weapon::gun0;

	//	constexpr auto tcbufid = n_Registry::Idof<Objs::t_unitcbf>;
	//	auto& acmds = ct.getArch(tcbufid);
	//	prop0.audioo = prop0.audioo < prop0.audiol ? prop0.audioo + perframeprogess : math::ui32max;
	//	for (size_t i = 0; i < unitcp0::stateslots; i++) {
	//		auto& st = prop0._states[i];
	//		//auto& st = tuptop.get<GameProps::Cmd0>().action[i];
	//		//const state st = prop0._states[i];
	//		switch (st)
	//		{
	//		case state::attack:
	//			//const auto&& reg = bp.get<PropID::attackrangesq>();

	//			//sprop.audiosrc< = gun.audiosrc;
	//			//if (reg > distsq)
	//		{
	//			auto&& freq = bp.get<PropID::feq>();
	//			if (prop0._statetick != 0 && (prop0._statetick % freq) == 0) {
	//				auto&& guni = tuptop.get<gun>();
	//				auto ret = guni.fire(trs.pos4, para, AmmPara0{});
	//				if (ret.result == wstate::reload) {
	//					prop0.SetState< state::autoreload>();
	//					sprop.audiolength = 0;
	//				}
	//				else {
	//					prop0.audioo = guni.audiosrc;
	//					prop0.audiol = guni.audiolength + prop0.audioo;
	//					sprop.audiolength = prop0.audiol;
	//				}
	//				//sprop.audiosrc = gun::prop().audio_src;
	//			}
	//		}
	//		/*else
	//		{
	//			prop0.SetState<state::move>();
	//		}*/
	//		break;
	//		case state::attackmove:

	//			break;
	//		case state::autoreload:
	//			//auto&& gun = tuptop.get<n_weapon::gun0>();
	//			//gun.reload().result==wstate::finsh?
	//			break;
	//		case state::move:
	//			//auto&& gun = tuptop.get<n_weapon::gun0>();
	//			//gun.reload().result==wstate::finsh?
	//			break;
	//		case state::died:
	//			//const auto& prop0.CommandId
	//			using t_cmdbuf = Objs::t_unitcbf;
	//			if (prop0.CommandId > -1) {
	//				Objs::cmdbuf& tuptop = acmds.get1Comp<t_cmdbuf::cmps, Objs::cmdbuf>(prop0.CommandId);
	//				int rem = tuptop.members--;
	//				assert(rem >= 0);
	//				if (rem == 0) {
	//					ct.Delete(tcbufid, prop0.CommandId);
	//				}
	//				prop0.CommandId = -1;
	//			}
	//			break;
	//		case state::nan:
	//			if (prop0.CommandId > -1) {
	//				Objs::cmdbuf& tuptop = acmds.get1Comp<t_cmdbuf::cmps, Objs::cmdbuf>(prop0.CommandId);
	//			}
	//			break;
	//		case state::idle:
	//			if (prop0.CommandId > -1) {
	//				Objs::cmdbuf& tuptop = acmds.get1Comp<t_cmdbuf::cmps, Objs::cmdbuf>(prop0.CommandId);
	//				const auto& ccmd = tuptop.getcmd(prop0.cmdqueueid);
	//				prop0.SetState(ccmd.action);
	//				currentCmd = ccmd;
	//			}
	//			break;
	//		default:
	//			const state s1 = (state)(st.pcstate[0]);
	//			const state s2 = (state)(st.pcstate[1]);
	//			st.SwiState();
	//			sprop.audiolength = 0;
	//			//sprop.audiosrc = 0;
	//			/*switch (s2)
	//			{
	//			case state::attack:
	//				auto&& gun = tuptop.get<gun>();

	//			}*/
	//			//state transition
	//			break;
	//		}
	//	}
	//	sprop.audiosrc = prop0.audioo;
	//	prop0.tick();
	//	return 0;
	//}

	//int pl0::Build(prefunit t) {
	//	const auto d = comps.add();
	//	dRender& dr = getcmp<sStruct::instData1>(d);
	//	dr.tid = this->tid;
	//	dr.id = d;
	//	Copy<mtransform>(d, t.datup);
	//	return d;
	//}

	//int pl0::Build() {
	//	uint d;
	//	if (!_pooldels.try_pop(d)) {
	//		d = comps.add();
	//	}
	//	dRender& dr = getcmp<sStruct::instData1>(d);
	//	dr.tid = this->tid;
	//	dr.id = d;
	//	return d;
	//}


int pl0::update(const size_t& W) {
		auto&& [trs, prop0, cmd, bp] = this->comps.get<mtransform, sStruct::instData1,
			GameProps::Cmd0, GameProps::BasicProp>(W);
		const auto perframeprogess = GameProps::autiosetting::Main().perframeprogess;
		//auto&& prop0 = cmd;
		const auto& fmid = GloProp::Get().FrameId;
		prop0.audiosrc = prop0.audiosrc < prop0.audiolength ? prop0.audiosrc + perframeprogess : -1;
		prop0.frameIndex = prop0.frameIndex == -1 ? -1 : prop0.base + this->bonenum;
		double pratio = 0;
		return 1;
	}

	//int pl0::Render(Manager::RenderPara& rp)
		//{
		//	//RenderContext bb;
		//	n_Render::renderbatch& rb = *rp.rb;
		//	auto& ach = rp.ct.getArch(rp.tid);
		//	auto& insdata = ach.getComps<cmps, n_objProp::dRender>();
		//	rb.WriteVbf(0, insdata, rp.rct);
		//	auto& pCtx = rp.rct.m_pImmediateContext;
		//	pCtx->CommitShaderResources(rb.m_pSRB, RESOURCE_STATE_TRANSITION_MODE::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
		//	pCtx->DrawIndexed(rb.DRG);
		//	return 1;
		//}
};
namespace pj2::n_weapon {

	//fireret gun0::fire(mtransfrom::v4 const pos, Manager::UpdatePara const& para, const AmmPara0 ampara) {
	//	const fireret rst(--bullet >= 0 ? wstate::fire : wstate::reload);
	//	//para.ct.Create()
	//	//rst.result=--bullet>=0?wstate::fire:wstate::reload;
	//	//this->treload = this->periodreload();
	//	return rst;
	//}
}