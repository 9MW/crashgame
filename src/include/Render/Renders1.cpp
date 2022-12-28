#include "Renders1.h"
#include"Render/Renders1.h"

namespace pj2 {
	namespace n_Render {

		BarRender::BarRender()
		{
		}

		BarRender::~BarRender()
		{
		}
		void BarRender::Render(RenderContext& rc, void* para) {
			StateTransitionDesc barr = { _instbuf, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_GENERIC_READ, STATE_TRANSITION_FLAG_UPDATE_STATE };
			rc.m_pImmediateContext->TransitionResourceStates(1,&barr);

			auto& itp = (int*&)para;
			auto& pCtx = rc.m_pImmediateContext;
			pCtx->SetPipelineState(_PSB.PSO);
			pCtx->CommitShaderResources(_PSB.SRBs[0], RESOURCE_STATE_TRANSITION_MODE_VERIFY);
			attribs.NumInstances = itp[0];
			pCtx->Draw(attribs);
		}
		void BarRender::init(RenderContext& rc, void* p2) {
			int _PowSTRIDE = 16, maxtexs = 10;
			auto& _pPSO = _PSB.PSO;
			auto& m_pDevice = rc.m_pDevice;
			auto& pCameraAttribs = rc.pCameraAttribs;
			new(&attribs)DrawIndexedAttribs(0, VT_UINT32, DRAW_FLAG_VERIFY_ALL);
			RefCntAutoPtr<IShaderSourceInputStreamFactory> pShaderSourceFactory = rc.pShaderSourceFactory;
			ShaderResourceVariableDesc Vars[] =
			{
				{ SHADER_TYPE_PIXEL, "g_ColorMap", SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC },
				{ SHADER_TYPE_VERTEX,"cbCameraAttribs", SHADER_RESOURCE_VARIABLE_TYPE_STATIC },
				{ SHADER_TYPE_VERTEX,"cbPrim", SHADER_RESOURCE_VARIABLE_TYPE_STATIC },
				{ SHADER_TYPE_VERTEX,"cmes", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE },
				{ SHADER_TYPE_PIXEL, "cell", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE },
				{ SHADER_TYPE_PIXEL, "map", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE },
				{ SHADER_TYPE_VERTEX, "animreco", SHADER_RESOURCE_VARIABLE_TYPE_STATIC },
				{ SHADER_TYPE_VERTEX, "instDatas", SHADER_RESOURCE_VARIABLE_TYPE_STATIC },
				{ SHADER_TYPE_VERTEX, "objectTransfromBuffer", SHADER_RESOURCE_VARIABLE_TYPE_STATIC },
				{ SHADER_TYPE_VERTEX, "cbOl", SHADER_RESOURCE_VARIABLE_TYPE_STATIC }
			};

			LayoutElement inputDesc[] =
			{
				{ 0, 0, 3, VT_FLOAT32 },   //float3 Pos     : ATTRIB0;
				{ 1, 0, 3, VT_FLOAT32 },   //float3 Normal  : ATTRIB1;
				{ 2, 0, 2, VT_FLOAT32 },   //float2 UV0     : ATTRIB2;
				{ 3, 0, 2, VT_FLOAT32 },   //float2 UV1     : ATTRIB3;
			};
			ShaderMacroHelper mcr;
			constexpr int isst = 1 << 17;
			constexpr int is8st = 1 << (32 - 17);
			constexpr int isste = (isst + 545) >> 17;
			constexpr int is80s = (isst + 545) % (1 << 17);
			Utility::span<LayoutElement> inputLayoutDesc;
			mcr.AddShaderMacro("SCMB", 1);
			mcr.AddShaderMacro("Bar", 1);
			mcr.AddShaderMacro("VTXCOLOR", 1);
			mcr.AddShaderMacro("NUM_TEXTURES", maxtexs);
			mcr.AddShaderMacro("BOSTRIDE", _PowSTRIDE);
			Utility::span<LayoutElement> lr = inputDesc;
			RenderUtil::CreatePipelineStates
			(&_pPSO, m_pDevice, rc.RenderTargetFormat, rc.DepthBufferFormat,
				pShaderSourceFactory,
				"skined.hlsl", "vert", "frag", Vars, mcr, {}, {},
				Diligent::PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP, 0, 0, "barv", "barpixel", Diligent::CULL_MODE_NONE);
			Diligent::PipelineResourceSignatureDesc sd;
			auto vtb = _pPSO->GetStaticVariableByName(Diligent::SHADER_TYPE_VERTEX, "instDatas");

			ShaderResourceVariableDesc Varsl[] =
			{
				{ SHADER_TYPE_VERTEX,"cbPrim", SHADER_RESOURCE_VARIABLE_TYPE_STATIC },
				{ SHADER_TYPE_VERTEX, "animreco", SHADER_RESOURCE_VARIABLE_TYPE_STATIC }
			};
			//using namespace Diligent;
			Diligent::BUFFER_VIEW_TYPE bvs[] = {
				BUFFER_VIEW_UNDEFINED,
				BUFFER_VIEW_SHADER_RESOURCE
			};
			vtb->Set(_instbuf->GetDefaultView(Diligent::BUFFER_VIEW_SHADER_RESOURCE));

			_pPSO->GetStaticVariableByName(SHADER_TYPE_VERTEX, "cbCameraAttribs")->Set(pCameraAttribs);
			_PSB.CreateShaderResourceBinding();

			/*attribs.BaseVertex = 0;
			attribs.NumIndices = 6;*/
			attribs.Flags |= DRAW_FLAG_DYNAMIC_RESOURCE_BUFFERS_INTACT;
			attribs.NumVertices = 4;
		}

		void SeleQuadRender::Render(RenderContext& rc, void* para) {
			auto& itp = (int*&)para;
			auto& pCtx = rc.m_pImmediateContext;
			pCtx->SetPipelineState(_PSB.PSO);
			pCtx->CommitShaderResources(_PSB.SRBs[0], RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
			attribs.NumInstances = itp[0];
			pCtx->Draw(attribs);
		}
		void SeleQuadRender::init(RenderContext& rc, void* p2) {
			int _PowSTRIDE = 16, maxtexs = 10;
			auto& _pPSO = _PSB.PSO;
			auto& m_pDevice = rc.m_pDevice;
			auto& pCameraAttribs = rc.pCameraAttribs;
			new(&attribs)DrawIndexedAttribs(0, VT_UINT32, DRAW_FLAG_VERIFY_ALL);
			RefCntAutoPtr<IShaderSourceInputStreamFactory> pShaderSourceFactory = rc.pShaderSourceFactory;
			ShaderResourceVariableDesc Vars[] =
			{
				{ SHADER_TYPE_PIXEL, "g_ColorMap", SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC },
				{ SHADER_TYPE_VERTEX,"cbPrim", SHADER_RESOURCE_VARIABLE_TYPE_STATIC },
				{ SHADER_TYPE_VERTEX,"sele_filter", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE },
				{ SHADER_TYPE_PIXEL, "cell", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE },
				{ SHADER_TYPE_PIXEL, "map", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE },
				{ SHADER_TYPE_VERTEX, "animreco", SHADER_RESOURCE_VARIABLE_TYPE_STATIC },
				{ SHADER_TYPE_VERTEX, "instDatas", SHADER_RESOURCE_VARIABLE_TYPE_STATIC },
				{ SHADER_TYPE_VERTEX, "objectTransfromBuffer", SHADER_RESOURCE_VARIABLE_TYPE_STATIC },
				{ SHADER_TYPE_VERTEX, "cbOl", SHADER_RESOURCE_VARIABLE_TYPE_STATIC }
			};

			LayoutElement inputDesc[] =
			{
				{ 0, 0, 3, VT_FLOAT32 },   //float3 Pos     : ATTRIB0;
				{ 1, 0, 3, VT_FLOAT32 },   //float3 Normal  : ATTRIB1;
				{ 2, 0, 2, VT_FLOAT32 },   //float2 UV0     : ATTRIB2;
				{ 3, 0, 2, VT_FLOAT32 },   //float2 UV1     : ATTRIB3;
			};
			ShaderMacroHelper mcr;
			constexpr int isst = 1 << 17;
			constexpr int is8st = 1 << (32 - 17);
			constexpr int isste = (isst + 545) >> 17;
			constexpr int is80s = (isst + 545) % (1 << 17);
			Utility::span<LayoutElement> inputLayoutDesc;
			mcr.AddShaderMacro("SCMB", 1);
			mcr.AddShaderMacro("QuadCircle", 1);
			mcr.AddShaderMacro("Bar", 1);
			mcr.AddShaderMacro("VTXCOLOR", 1);
			mcr.AddShaderMacro("NUM_TEXTURES", maxtexs);
			mcr.AddShaderMacro("BOSTRIDE", _PowSTRIDE);
			Utility::span<LayoutElement> lr = inputDesc;
			RenderUtil::CreatePipelineStates
			(&_pPSO, m_pDevice, rc.RenderTargetFormat, rc.DepthBufferFormat,
				pShaderSourceFactory,
				"UITool.hlsl", "vert", "fsele", Vars, mcr, {}, {},
				Diligent::PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP, 0, 0, "barv", "barpixel", Diligent::CULL_MODE_NONE);
			Diligent::PipelineResourceSignatureDesc sd;
			_pPSO->GetStaticVariableByName(SHADER_TYPE_VERTEX, "cbCameraAttribs")->Set(pCameraAttribs);
			_PSB.CreateShaderResourceBinding();

			auto vtb = _PSB.GetShaderResourceBinding()->GetVariableByName(Diligent::SHADER_TYPE_VERTEX, "sele_filter");
			vtb->Set(_instbuf->GetDefaultView(Diligent::BUFFER_VIEW_SHADER_RESOURCE));
			/*attribs.BaseVertex = 0;
			attribs.NumIndices = 6;*/
			attribs.Flags |= DRAW_FLAG_DYNAMIC_RESOURCE_BUFFERS_INTACT;
			attribs.NumVertices = 4;
		}
	}


}