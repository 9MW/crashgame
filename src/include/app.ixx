
#pragma once
#include<chrono>
#include<imgui_internal.h>
#include "InputController.hpp"
#include"Common/FirstPersonCamera.hpp"
#include "ecs.hpp"
#include "Common/interface/RefCntAutoPtr.hpp"
#include <Graphics/GraphicsEngineVulkan/interface/EngineFactoryVk.h>

#include "Graphics/GraphicsEngine/interface/RenderDevice.h"
#include "Graphics/GraphicsEngine/interface/DeviceContext.h"
#include "Graphics/GraphicsEngine/interface/SwapChain.h"
#include"shpso.h"
#include"AudioManager.h"
#include"Render/geomatrys.h"
#include"Render/Renders0.h"
#include<imgui.h>
#include"Common/InputUtil.h"
#include"Gameprops0.h"
#include"cfgparser.h"
#include"Macros.h"
#include"editor.h"
export module app;
namespace pj2 {
	using InputKeys = Diligent::InputKeys;
	using MouseState = Diligent::MouseState;
	using InputController = Diligent::InputController;
	EXPO namespace AppProp {
		constexpr double fixfeq = 1 / 50.0;
		constexpr double griphicfeq = 1 / 60.0;
	};
	EXPO struct comandin
	{
		Diligent::MouseState m_LastMouseState;
	};
	EXPO struct sPI
	{
		InputController* Controller;
	};
	using namespace Diligent;
	EXPO struct Vbuffer
	{
		RefCntAutoPtr<IBuffer>_tinfo;
		RefCntAutoPtr<IBuffer>_Vfprop;
		//void init(RenderContext& rc,
		//	Utility::span<sStruct::tinfo> tf_ = {},
		//	Utility::span<sStruct::VfProp> vfp_ = {}) {
		//	Diligent::BufferDesc BuffDesc;
		//	{
		//		using ts = sStruct::tinfo;
		//		auto&& buf = _tinfo;
		//		//auto&& dspan=tf_;
		//		BuffDesc.Mode = BUFFER_MODE_STRUCTURED;
		//		/*VBData.DataSize = fdf.size() * 16;
		//		VBData.pData = fdf.data();*/
		//		RenderUtil::md_BufferDesc("sele_out", BuffDesc, tf_.size(), sizeof(ts), USAGE_DEFAULT, BIND_SHADER_RESOURCE, CPU_ACCESS_NONE);
		//		rc.m_pDevice->CreateBuffer(BuffDesc, nullptr, &buf);
		//		//m_pDevice->CreateBuffer(BuffDesc,nullptr, &audio_bufin);
		//	}
		//}
		void init(RenderContext& rc,
			const int maxtf_ = 1024,
			const int maxvfp_ = 4096 * 4) {
			Diligent::BufferDesc BuffDesc;
			{
				using ts = sStruct::tinfo;
				auto&& buf = _tinfo;
				//auto&& dspan=tf_;
				BuffDesc.Mode = BUFFER_MODE_STRUCTURED;
				/*VBData.DataSize = fdf.size() * 16;
				VBData.pData = fdf.data();*/
				RenderUtil::md_BufferDesc("_tinfo", BuffDesc, maxtf_, sizeof(ts), USAGE_DEFAULT, BIND_SHADER_RESOURCE, CPU_ACCESS_NONE);
				rc.m_pDevice->CreateBuffer(BuffDesc, nullptr, &buf);
				//m_pDevice->CreateBuffer(BuffDesc,nullptr, &audio_bufin);
			}
			{
				using ts = sStruct::VfProp;
				auto&& buf = _Vfprop;
				//auto&& dspan=tf_;
				BuffDesc.Mode = BUFFER_MODE_STRUCTURED;
				/*VBData.DataSize = fdf.size() * 16;
				VBData.pData = fdf.data();*/
				RenderUtil::md_BufferDesc("_Vfprop-vbuffer", BuffDesc, maxvfp_, sizeof(ts), USAGE_DEFAULT, BIND_SHADER_RESOURCE, CPU_ACCESS_NONE);
				rc.m_pDevice->CreateBuffer(BuffDesc, nullptr, &buf);
				//m_pDevice->CreateBuffer(BuffDesc,nullptr, &audio_bufin);
			}
		}
	};
	EXPO struct cmdGroup
	{
		int id = 11;
		std::vector<GameProps::Cmd0> cmds;
		int usize;
		//GameProps::ucmdbuf cmds;
		//std::vector<GameProps::ucmdbuf> cmds;;
	};
	/*struct udesc
	{

	};*/
	using unitde = std::tuple<unitdesc, GameProps::weaponpara>;
	//using IBuffer = Diligent::IBuffer;
	constexpr int activeGroupId = 10;
	EXPO class App0
	{
		int activeGroupId = -1;
		using IBuffer = Diligent::IBuffer;
		math::Matrix4f* guizmo;
		n_Render::SkinRender _skinr;
	public:
		using vt_prop = sStruct::inscmb;
		RefCntAutoPtr<IBuffer>                cSelect;
		RefCntAutoPtr<IBuffer>                cbCameraAttribs;
		RefCntAutoPtr<IBuffer>                V_Props;
		RefCntAutoPtr<IBuffer>                _tinfo;
		RefCntAutoPtr<IBuffer>                sele_out;
		std::vector<std::vector<ECS::Entity>> _cmdGroups;
		std::vector<unitde> units;
		Cfgs _customcfg;
		inline std::vector<ECS::Entity>& ActiveCGP() {
			//if(activeGroupId!=-1)
			return (_cmdGroups.back());
			/*else {

			}*/
		}
		shpso::SelectCS scs;
		int objnum = 0;
		//void (*createobj(std::tuple<mtransfrom, int>));
		using t_cObjarg = GameProps::ubuilder0;// std::tuple<mtransfrom, int, unitdesc, GameProps::weaponpara>;
		PUtility::WrapFuncObj<void(t_cObjarg)>createobj;
		//double delta_time;
		using t_clock = std::chrono::high_resolution_clock;
		bool running = true;
		comandin instate;
		double desired_frametime = AppProp::fixfeq, frame_accumulator = 0;
		inline void SetData(void* d) {
			_etor.p_giz = d;
			guizmo =reinterpret_cast<Eigen::Matrix4f*>(d);// (Eigen::Map<Eigen::Matrix4f>((float*)d));
			//RenderTest::_mt.set(a3f);
			//for (sStruct::instData& dss : RenderTest::Insts) {
			//	//inverse to match visual appearance
			//	dss.color = mt.pos4;
			//	dss.transform.Scale4 = mt.Scale4;
			//}
		}
		void init(RenderContext& rt);
		static inline cfgout _co;
		void loadcfg(cfgout& co=_co) {
			//n_Render::rendermanager& rm = *(co.rm);
			phmap::flat_hash_map<std::string, int> ri;
			for (size_t i = 0; i < _countof(rendertag); i++)
			{
				ri.try_emplace(rendertag[i], i);
			}

			_customcfg._rt = rct;
			_customcfg.init();
			//if(amanager.daudios.size()==0)
			/*amanager.LoadAudios(loader::Get(Paths::audios).string(),
				1024 * 1024 * 20, 1, 44100);*/
			GameProps::autiosetting& asetting = GameProps::autiosetting::Main();
			asetting.perframeprogess = 44100 / logicframerate / 4;
			auto& audios = _customcfg._datas.audios;
			auto& us = _customcfg._datas.unitout;
			auto& models = _customcfg._datas.models;
			std::sort(us.begin(), us.end(), [&](unitdesc& a, unitdesc& b)
				{return ri[a.render] < ri[b.render]; });
			units.clear();
			std::string prerender = "";
			//Model:model modpkg;
			const auto modelpath = loader::Get(Paths::model);
			//for (size_t i = 0, start = 0; i < us.size(); i++)
			//{
			//	auto& desc = us[i];
			//	if (prerender != desc.render) {
			//		if (mods.size() > 0) {
			//			mods.back().Build(rct.m_pDevice);
			//		}
			//	}
			//	{
			//		auto& mp = mods.emplace_back();

			//		mods.back().LoadModel(rct.m_pDevice, rct.m_pImmediateContext,
			//			models[desc.Name].string(), NULL);
			//	}
			//	//if (desc.st == resource::unit) 
			//	{
			//		auto&& [uout, weap] = units.emplace_back();
			//		uout = desc;
			//		//Utility::memcpy(&uout.peop, &desc.peop, sizeof(desc.peop));
			//		if (uout.weapons != "") {
			//			auto& wep = _customcfg._datas.mp_weapons[uout.weapons];
			//			const auto& audioinfo = amanager._audiomap[wep.shotaudio];
			//			weap.audiolength = audioinfo.size;
			//			weap.audiosrc = audioinfo.offset;
			//			weap.freq = wep.wprop.freq;
			//			//uout.peop = wep.peop;
			//		}
			//	}
			//}
			//mods.back().Build(rct.m_pDevice);
			//co= _customcfg.
		}
		//input big instance buffer with offset table to info  type corresponding position size
		//filter to result vbuffer store result with previous offset
		void render() {
			//RenderTest::Ren(rct);
			constexpr auto convertid = [](const sStruct::seleout& resu, ECS::Entity& re) {
				//constexpr ii16 i2(resu.itid);
				re.tid = resu.itid >> 16;
				re.id = resu.itid ^ (re.tid << 16);
			};
			//Ren(rct, cbCameraAttribs,)
			if (cSelection.TaskRunning()) {

			}
			if (_screenrect.org[3] > 0)
				cSelection.GetNewResult([&](Diligent::MapHelper<sStruct::seleout>& sout) {
				objnum = sout[0].itid;// wd.id;
				const size_t e = objnum + 1;
				activeGroupId = objnum > 0 ? _cmdGroups.size() - 1 : -1;
				ActiveCGP().resize(objnum);
				for (size_t i = 1; i < e; i++)
				{
					convertid(sout[i], ActiveCGP()[i - 1]);
				}
				//cSelection.RenderSelect(objnum);
					});
			_etor.Render();
		}
		using t_map = sStruct::seleout;
		void drawwire(Utility::span<t_map> dri) {

			for (size_t i = 0; i < dri.size(); i++){
				auto&  ed = dri[i];
				auto* ttid = reinterpret_cast<int16*> (&ed.itid);// >> 16, id = ed.itid & (1 << 16);

			}
		}
		//inline void FixUpdate() {}
		void Update() {
			if(false)
			{

				for (size_t i = 0,ip=1; i < ip; i++)
				{
				    /*IndirectArgs& inddrawl = inddraw[i];
				    inddrawl.NumInstances = objnum;*/
				    if (cSelection.ResultReady()) {
				   	 Diligent::MapHelper<t_map> sout(rct.m_pImmediateContext
				   		 , cSelection.stagsele, MAP_READ, MAP_FLAG_DO_NOT_WAIT);
				   	 objnum = sout[0].itid;
					 auto ps = (sStruct::seleout*)sout;
					 std::sort( ps+1, ps+objnum + 1, [](t_map& a, t_map& b) {return a.itid > b.itid; });
				    }
				    auto& m_pImmediateContext = rct.m_pImmediateContext;
				    //MapHelper<mtransfrom>mtransfromData(m_pImmediateContext, instRender2->m_pTransAttribsBuffer, MAP_WRITE, MAP_FLAG_DISCARD);
				    //std::memcpy(mtransfromData, cont.filter._pos[0].data(), sizeof(mtransfrom)*Currinstancenum);
				    //MapHelper<sStruct::instData>minstData(m_pImmediateContext, instRender2->m_pinspropertysBuffer, MAP_WRITE, MAP_FLAG_DISCARD);
				}
			}
			if (objnum > 0)
			amanager.Update(objnum);
			_etor.update();
			/*ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver);
			ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiCond_FirstUseEver);
			ImGui::Begin("App0::Update");
			ImGui::InputInt4("app0 mod", _controlmode.data());
			if (ImGui::Button("load cfg")) {
				this->loadcfg();
			}
			ImGui::End();*/

			//constexpr auto fixed_deltatime = AppProp::fixfeq;
			////while (running)
			//{
			//	const auto now = t_clock::now();;
			//	//LOGINFO("cost {0} micro {1}", std::chrono::duration<double>(now - start).count() * 1000000, nm);
			//	const auto delta_time = std::chrono::duration<double, std::milli>(now - prev_frame_time).count();
			//	auto consumedDeltaTime = delta_time;
			//	//current_frame_time = std::chrono::duration<double>(now);
			//	prev_frame_time = now;
			//	int maxiter = 2;
			//	while (frame_accumulator >= desired_frametime&&--maxiter>0) {
			//		game.fixed_update(fixed_deltatime);
			//		if (consumedDeltaTime > desired_frametime) { //cap variable update's dt to not be larger than fixed update, and interleave it (so game state can always get animation frames it needs)
			//			game.variable_update(fixed_deltatime);
			//			consumedDeltaTime -= desired_frametime;
			//		}
			//		frame_accumulator -= desired_frametime;
			//	}

			//	game.variable_update((double)consumedDeltaTime);
			//	game.render((double)frame_accumulator / desired_frametime);
			//	display(); //swap buffers
			//}
		}
		inline auto rayzero(const MouseState& mouseState, const Camera& m_Camera) {
			math::Efloat3 v1;
			math::Efloat3 v22;
			m_Camera.getRay({ mouseState.PosX,mouseState.PosY }, v1, v22);
			return(v1 + v22 * (std::abs((v1[2]) / v22[2])));
		}
		editor _etor;
	private:
		Audio::AudioManager amanager;
		/*std::unique_ptr<insRander2> instRender2;*/
		std::vector<IndirectArgs> inddraw;
		math::vec4<int> _controlmode;
		sStruct::Selects _screenrect;
		RenderContext rct;
		computelib::SelectCS cSelection;
		
		/*inline bool keydown(InputController& Controller, const InputKeys k) {
			return Controller.IsKeyDown(k);
		}*/
		std::chrono::time_point<t_clock>  prev_frame_time, current_frame_time;;
	};



	//input big instance buffer with offset table to info  type corresponding position size
	//filter to result vbuffer store result with previous offset

	inline void App0::init(RenderContext& rt) {
		rct = rt;
		{
			using namespace n_Render;
			int _bxmax = 10000;
			auto& m_pDevice = rt.m_pDevice;
			auto& unitCube = geomatrys::GetColl().uCube1;
		}
		_screenrect.org.setZero();
		_cmdGroups.resize(11);
		_etor.p_rt = &rct;
		_etor.Init();
		int tnum = 1;
		Vbuffer vbf;
		_controlmode = { 1,0,1,0 };
		{
			const int maxtf_ = 1024;
			const int maxvfp_ = 4096 * 4;
			Diligent::BufferDesc BuffDesc;
			{
				using ts = sStruct::tinfo;
				auto&& buf = _tinfo;
				//auto&& dspan=tf_;
				BuffDesc.Mode = BUFFER_MODE_STRUCTURED;
				/*VBData.DataSize = fdf.size() * 16;
				VBData.pData = fdf.data();*/
				RenderUtil::md_BufferDesc("_tinfo", BuffDesc, maxtf_, sizeof(ts), USAGE_DEFAULT, BIND_SHADER_RESOURCE, CPU_ACCESS_NONE);
				rct.m_pDevice->CreateBuffer(BuffDesc, nullptr, &buf);
				//m_pDevice->CreateBuffer(BuffDesc,nullptr, &audio_bufin);
			}
			{
				auto&& buf = V_Props;
				//auto&& dspan=tf_;
				BuffDesc.Mode = BUFFER_MODE_STRUCTURED;
				/*VBData.DataSize = fdf.size() * 16;
				VBData.pData = fdf.data();*/
				RenderUtil::md_BufferDesc("prop-vbuffer", BuffDesc, maxvfp_, sizeof(vt_prop), USAGE_DYNAMIC, BIND_SHADER_RESOURCE, CPU_ACCESS_WRITE);
				rct.m_pDevice->CreateBuffer(BuffDesc, nullptr, &buf);
				//m_pDevice->CreateBuffer(BuffDesc,nullptr, &audio_bufin);
			}
			vbf._tinfo = _tinfo;
			vbf._Vfprop = V_Props;
		}
		//constexpr int tsi = sizeof(sStruct::tinfo);
	std:vector<sStruct::tinfo> tinfos_;
		auto& ft = tinfos_.emplace_back();
		Utility::span<sStruct::tinfo> stf(tinfos_);
		ft.halfext.setOnes();
		ft.halfext *= 0.5;
		rct.m_pImmediateContext->UpdateBuffer(vbf._tinfo, 0,
			stf.size_bytes(), stf.data(),
			Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
		cSelection.Vf_Props = vbf._Vfprop;
		cSelection.tinfo = vbf._tinfo;
		cSelection.init(rct);
		amanager.cb_camera = rt.pCameraAttribs;
		//amanager.init(rct, 12288);
		loadcfg();
		//RenderTest::Init(rct);
		//inddraw.emplace_back(-1, 0, 0, 0, 0);
	}

};