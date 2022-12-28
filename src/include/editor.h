#pragma once

#include<imgui.h>
#include"Gameprops0.h"
#include"Common/ecs.hpp"
#include"Render/RenderUtil.h"
#include"AllType.h"
#include"io/loader.h"
#include"shpso.h"
#include"io/commandin.h"
#include"UI/uicoll0.h"
namespace pj2{
	using math::mtransform;
	class editor
	{
	public:
		static inline string MaindockspaceID = "dockspaceID";
		void* p_cont,*p_giz;
		ResManager* rem;
		inputmanager* imagr;
		RenderContext* p_rt;
		void* RTEXSRV;
		uicallback callbacks;
		shpso::Visible_f* p_vcull;
		//n_Render::fogRender* fogrd;
		computelib::SelectCS* p_picker=NULL;
		void Init();

		void updateUI() {

		}
		//void Drawunit(n_units::plclose::unpackTup& tup);
		void update();
		void Render();
		void RayAt(const math::Efloat3& v1,
			const math::Efloat3& v22);
		void listof(ResManager& resm, int& item_current);
		template<int I,class ...Ts>
		void func(Ts&...args);
	};
}
