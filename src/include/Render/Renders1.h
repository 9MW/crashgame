#pragma once

#include"Render/RenderUtil.h"
#include"ShaderStructs.h"
#include <Render/RenderCommon.h>
namespace pj2::n_Render {

	class BarRender
	{
		//DrawIndexedAttribs attribs;
		DrawAttribs attribs;
	public:
		dyvbf                 _cmb;
		dyvbf                 _instbuf;
		PSB<> _PSB;
		BarRender();
		~BarRender();
		void init(RenderContext& rc, void* p2);
		void Render(RenderContext& rc, void* para);
	private:

	};
	class SeleQuadRender
	{
		//DrawIndexedAttribs attribs;
		DrawAttribs attribs;
	public:
		dyvbf                 _instbuf;
		PSB<> _PSB;
		void init(RenderContext& rc, void* p2);
		void Render(RenderContext& rc, void* para);
	private:

	};
}