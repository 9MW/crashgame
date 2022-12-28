#pragma once
#include "common/Math1.h"
#include"ShaderStructs.h"
//#include"physics/physics.hpp"
#include<PCH.h>
#include "Common/interface/RefCntAutoPtr.hpp"
#include <Graphics/GraphicsEngineVulkan/interface/EngineFactoryVk.h>

#include "Graphics/GraphicsEngine/interface/RenderDevice.h"
#include "Graphics/GraphicsEngine/interface/DeviceContext.h"
#include "Graphics/GraphicsEngine/interface/SwapChain.h"
#include"structs.h"
#include <ImGuiImplDiligent.hpp>
#include <ImGuiImplWin32.hpp>
#include <ShaderMacroHelper.hpp>
#include"Mesh.h"
#include"io/loader.h"
#include"Common/span.hpp"
#include"Render/geomatrys.h"
#include <FirstPersonCamera.hpp>
#include"Render/RenderUtil.h"
#include"Render/LineRender.h"
using namespace Diligent;
namespace pj2 {
	struct Instancedata
	{
		math::uint transformid, animid;
	};
	struct cdata {
		math::uint base, frameSize;
	};
	
	// std::tuple<RefCntAutoPtr<IPipelineState>, std::array<RefCntAutoPtr<IShaderResourceBinding>, i>>;

	namespace n_Render {
		using RenderUtil::CreatePipelineStates;
		using mtransfrom = math::mtransform;

		class debug
		{
			static inline LineRender _lRd;
		public:
			using bflinevtxdf=LineRender::bflinevtxdf;
			std::vector<mtransfrom> trs;
			math::Efloat4 cellsize;
			debug(
				RefCntAutoPtr<IRenderDevice> m_pDevice,
				RefCntAutoPtr<IDeviceContext> m_pImmediateContext,
				const TEXTURE_FORMAT RenderTargetFormat,
				const TEXTURE_FORMAT DepthBufferFormat,
				IBuffer* pCameraAttribs,
				RefCntAutoPtr<IShaderSourceInputStreamFactory> pShaderSourceFactory,
				ITexture* Maptex = NULL,
				const int instancenum = 1
			);
			//void DrawLine(span<LineVertex> lines);
			inline void ClearLine() {
				_lineoffset = 0;
			}
			static void cpy(float4& l, const v4f& i) {
				std::memcpy(&l, &i, 16);
			}
			void FillGrid(math::Efloat2 grid, span<bflinevtxdf> disds);
			static void DrawLine(const v4f& a, const v4f& b, v4f color = { 0,1,0,1 }, v4f color1 = { 0,1,0,1 }) {
				{
					auto&& Linebf = _Linebuf;
					auto& p0 = Linebf.emplace_back();
					auto& p1 = Linebf.emplace_back();
					cpy(p0.color, color);
					cpy(p1.color, color1);
					std::memcpy(&p0.pos, &a, 16);
					std::memcpy(&p1.pos, &b, 16);
				}
			};
			inline void Drawgrid(const math::int2 size) {

			}
			void Render();
			void DrawUI();
			static void DrawCircle(void* pd);
			bool DrawLineb = false;
			math::Efloat2 Gridsize = { 100,100 };
			int CreateDesType = 0, itor, _stopneb = 5, _nebsz = 4;
			float period = 1, SeparationWeight = 0.7, w2 = 0, cohesion = 0.3, maxspeed, anvNeg, minspeed;
			static inline std::vector<bflinevtxdf> _Linebuf;
			std::vector<Diligent::float4> typecolors = {
				{0.5,0,0,1},
				{0,1,0,1}
			};
		private:
			void DrawLine(span<bflinevtxdf> lines, bool persistent = false);
			int circleRadius = 10;
			Diligent::float4 circlepos = { 0,0,0,0 };
			int debugoffset = 100;
			int sqs = 10000;
			int _lineoffset = 0;
			GIns::CalMapId CalCoord;
			//std::vector<math::float4>map;
			RefCntAutoPtr<IShaderResourceBinding> pSRB;
			RefCntAutoPtr<IShaderResourceBinding> pLineSRB;
			std::unique_ptr<geomatrys::ngeo_quads> qd;
			RefCntAutoPtr<IBuffer>                m_pTransAttribsBuffer;
			RefCntAutoPtr<IBuffer>                m_cellbuf;
			RefCntAutoPtr<IBuffer>                m_mapbuf;
			RefCntAutoPtr<IBuffer> _pLineVertexBuffer;
			RefCntAutoPtr<IPipelineState> pRenderMeshPSO;
			RefCntAutoPtr<IPipelineState> pRenderLinePSO;
			RefCntAutoPtr<IRenderDevice> m_pDevice;
			RefCntAutoPtr<IDeviceContext> m_pImmediateContext;
			RefCntAutoPtr<IShaderSourceInputStreamFactory> m_pShaderSourceFactory;
			float time;
			int quiryatep = 5;
			int cirSeg = 360;
		};
	}
}