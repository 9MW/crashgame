#pragma once
#include"Render/Renders0.h"

namespace pj2 {
	namespace Editor {
		struct AnimContext {
			std::vector<std::string>modelNames;
			sStruct::instData1* rep;
			int size;
			int modid = -1;
		};
		struct Anim
		{
			RenderContext _rt;
			n_Render::SkinRender* _Skr;
			void init(void*& pimage);
			int Update(AnimContext& ac);
			int Render();
		};
	}
}