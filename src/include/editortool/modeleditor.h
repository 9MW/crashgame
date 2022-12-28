#pragma once
#include"animationeditor.h"
#include"Graphic/modelcom.h"

namespace pj2::n_editor {
	using Graphic::stateanim;
	struct modeldesc
	{
		std::string name;
		std::vector<stateanim> AnimClips;
		math::v4f bbox, offset;
	};
	struct modeldelarg
	{
		int modid;
		std::string name;
	};
	struct minitpara
	{
		RenderContext* rct;
		void* puser;
		void (*onchange)(modeldelarg&,void*);
	};
	/// <summary>
	/// edito bound box clip animation
	/// </summary>
	class modeleditor
	{
	public:
		void* rtargetid;
		modeleditor()
		{
		}
		void init(minitpara& mp);
		void Draw();
		~modeleditor();

	private:

	};

}