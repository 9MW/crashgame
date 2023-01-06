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
#include"turnsys.h"
#include"UI0.h"
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
		using IBuffer = Diligent::IBuffer;
		math::Matrix4f* guizmo;
		RenderContext rct;
	public:
		t_renders Renders;
		using vt_prop = sStruct::inscmb;
		pIbuffer                cbCameraAttribs;
		int objnum = 0;
		//double delta_time;
		using t_clock = std::chrono::high_resolution_clock;
		bool running = true;
		void SetData(void* d);
		void init(pj2::Imp::RenderContext& rt);
		void loadcfg();
		void render();
		void Update();
	};

};