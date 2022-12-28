#pragma once

#include"structs.h"
#include <ImGuiImplDiligent.hpp>
#include <ImGuiImplWin32.hpp>
#include <ShaderMacroHelper.hpp>
#include"Common/span.hpp"
#include"Render/geomatrys.h"
#include <FirstPersonCamera.hpp>
#include"Render/RenderUtil.h"
namespace pj2::n_Render {
	using RenderUtil::CreatePipelineStates;
	using mtransfrom = math::mtransform;
	using Ichar = char;
	struct LineRender
	{
		struct bflinevtxdf
		{
			using float3 = Diligent::float3;
			using float2 = Diligent::float2;
			using float4 = Diligent::float4;
			float4 pos;
			float4 color;
			float& operator[](int i) {
				return pos[i];
			}
			bflinevtxdf() = default;
			bflinevtxdf(float3&& v3) {
				pos = v3;
			}
			operator float3& () {
				return reinterpret_cast<float3&>(pos);
			}

		};
		void init(RenderContext& rt);
		void LineStrip(void* para);
		void NewLine(void* para);
		void Render();

		static LineRender*& main();
		PSB<1> _psb;
		RefCntAutoPtr<IBuffer> _pLineVertexBuffer, _cbufPara;
		static inline std::vector<bflinevtxdf> _Linebuf;
		RefCntAutoPtr<IDeviceContext> m_pImmediateContext;
	};
}