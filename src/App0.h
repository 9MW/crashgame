#pragma once
#include"io/AudioManager.h"
#include<chrono>
#include "InputController.hpp"
#include"Common/FirstPersonCamera.hpp"
#include "ecs.hpp"
#include "Common/interface/RefCntAutoPtr.hpp"
#include <Graphics/GraphicsEngineVulkan/interface/EngineFactoryVk.h>
#include<Render/LineRender.h>
#include "Graphics/GraphicsEngine/interface/RenderDevice.h"
#include "Graphics/GraphicsEngine/interface/DeviceContext.h"
#include "Graphics/GraphicsEngine/interface/SwapChain.h"
#include"shpso.h"
#include"Render/Renders0.h"
#include<imgui.h>
#include"Gameprops0.h"
namespace cage {
	using InputKeys = Diligent::InputKeys;
	using MouseState = Diligent::MouseState;
	using InputController = Diligent::InputController;
	using  Diligent::IBuffer;
	using pIbuffer =  Diligent::RefCntAutoPtr<IBuffer>;
	using string=std::string;
	namespace n_Render = pj2::n_Render;
	using RenderContext = pj2::Imp::RenderContext;
	class App0
	{
		using t_renders = Utility::MyTuple< n_Render::SkinRender, n_Render::FixRender, n_Render::OlskRender,
			n_Render::LineRender>;
		int activeGroupId = -1;
		using IBuffer = Diligent::IBuffer;
		math::Matrix4f* guizmo;
		n_Render::SkinRender _skinr;
		math::vec4<int> _controlmode;
		RenderContext rct;
	public:
		t_renders Renders;
		using vt_prop = sStruct::inscmb;
		pIbuffer                cSelect;
		pIbuffer                cbCameraAttribs;
		pIbuffer                V_Props;
		pIbuffer                _tinfo;
		pIbuffer                sele_out;
		std::vector<std::vector<ECS::Entity>> _cmdGroups;
		inline std::vector<ECS::Entity>& ActiveCGP() {
			//if(activeGroupId!=-1)
			return (_cmdGroups.back());
		}
		int objnum = 0;
		//double delta_time;
		using t_clock = std::chrono::high_resolution_clock;
		bool running = true;
		void SetData(void* d);
		void init(pj2::Imp::RenderContext& rt);
		void loadcfg();
		//input big instance buffer with offset table to info  type corresponding position size
		//filter to result vbuffer store result with previous offset
		void render();
		using t_map = sStruct::seleout;
		void drawwire(Utility::span<t_map> dri) {

			for (size_t i = 0; i < dri.size(); i++) {
				auto& ed = dri[i];
				auto* ttid = reinterpret_cast<int16*> (&ed.itid);// >> 16, id = ed.itid & (1 << 16);

			}
		}
		//inline void FixUpdate() {}
		void Update();
	};

};