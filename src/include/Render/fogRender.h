#pragma once
#include <FirstPersonCamera.hpp>
#include"Render/ShaderStructs.h"
#include"Common/span.hpp"
#include"RenderUtil.h"
#include"AssetLoader/RenderStr0.h"
#include"parallel_hashmap/phmap.h"
#include"Gameprops0.h"
#include <Render/RenderCommon.h>
#include"Render/geomatrys.h"
#include <Graphics/GraphicsTools/interface/MapHelper.hpp>
#include"shpso.h"
namespace pj2::n_Render {
	constexpr auto sele0Array = []() {
		std::array<int, 256> result{};
		for (auto&& ei : result)
		{
			ei = 0;
		}
		return result;
	};
	using shpso::Visible_f;
	constexpr auto Zero1Kbyte = sele0Array();
	class fogRender :public Visible_f
	{
		static constexpr int deltaChunks = (1024 * 64);
		static constexpr int deltaObjs = deltaChunks / sizeof(sStruct::objrep);
		static constexpr int m_ThreadGroupSize = 64;
		static constexpr int QueueSize = 1024 * 128;
		template<class T>
		using span = nonstd::span<T>;
		RefCntAutoPtr<IBuffer>                m_CamConstants;
		RefCntAutoPtr<IBuffer>                m_deltaObjs;
		RefCntAutoPtr<IBuffer>                m_brightObjs;
		RefCntAutoPtr<IBuffer>                m_deltaObjsL;
		RefCntAutoPtr<IBuffer>                m_brightcb;
		RefCntAutoPtr<IBuffer>                queueFB;
		RefCntAutoPtr<IBuffer>                m_Moveprop;
		geomatrys::ngeo_quads plane;

		RefCntAutoPtr<ITextureView>           TEXSRVs[10];
		RefCntAutoPtr<ITextureView>           TEXUAVs[10];
	public:

		template<int I,typename ...Ts>
		inline int Cmd(Ts&...arg);
		RefCntAutoPtr<ITexture> pRTColor, pRTColorfow;
		//FowbatRender(
		//	RefCntAutoPtr<IRenderDevice> m_pDevice,
		//	RefCntAutoPtr<IDeviceContext> m_pImmediateContext,
		//	RefCntAutoPtr<ISwapChain> pSwapChain,
		//	//IBuffer* pCameraAttribs,
		//	RefCntAutoPtr<IBuffer>                m_VertexBuffer,
		//	RefCntAutoPtr<IBuffer>                m_IndexBuffer,
		//	RefCntAutoPtr<IEngineFactory>              pEngineFactory) :m_pSwapChain(pSwapChain) {
		//	auto shaderpath = loader::Get(shader);
		//	init(m_pDevice, m_pImmediateContext, pSwapChain, pEngineFactory);
		//}
		void ClearBlock(int num = 1000) {
			auto& pso = m_pResetPSO;
			{
				Diligent::MapHelper<sStruct::GlobalConstants> gconst(m_pImmediateContext, m_brightcb, MAP_TYPE::MAP_WRITE, MAP_FLAGS::MAP_FLAG_DISCARD);
				*gconst = gCons;
			}
			m_pImmediateContext->SetPipelineState(pso);
			m_pImmediateContext->CommitShaderResources(m_pReset1SRB, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
			//[numthreads(8, 8, 1)] 4x4 pixel perthread
			DispatAttribs.ThreadGroupCountX = shadowmapsz / 32;
			DispatAttribs.ThreadGroupCountY = shadowmapsz / 32;
			m_pImmediateContext->DispatchCompute(DispatAttribs);
		}
		void CreateRenderTargetPSO(
			IPipelineState** m_pRTPSO,
			RefCntAutoPtr<IRenderDevice> m_pDevice,
			RefCntAutoPtr<ISwapChain> pSwapChain,
			RefCntAutoPtr<IShaderSourceInputStreamFactory> pShaderSourceFactory,
			Diligent::TEXTURE_FORMAT ColorBufferFormat,
			Diligent::TEXTURE_FORMAT DepthBufferFormat,
			const std::string FilePath,
			const char* ven,
			const char* pen)
		{
			GraphicsPipelineStateCreateInfo RTPSOCreateInfo;
			auto& GraphicsPipeline = RTPSOCreateInfo.GraphicsPipeline;
			// Pipeline state name is used by the engine to report issues
			// It is always a good idea to give objects descriptive names
			// clang-format off
			RTPSOCreateInfo.PSODesc.Name = "Render Target PSO";
			// This is a graphics pipeline
			RTPSOCreateInfo.PSODesc.PipelineType = PIPELINE_TYPE_GRAPHICS;
			// This tutorial will render to a single render target
			RTPSOCreateInfo.GraphicsPipeline.NumRenderTargets = 1;
			// Set render target format which is the format of the swap chain's color buffer
			RTPSOCreateInfo.GraphicsPipeline.RTVFormats[0] = m_pSwapChain->GetDesc().ColorBufferFormat;
			// Set depth buffer format which is the format of the swap chain's back buffer
			RTPSOCreateInfo.GraphicsPipeline.DSVFormat = m_pSwapChain->GetDesc().DepthBufferFormat;
			// Primitive topology defines what kind of primitives will be rendered by this pipeline state
			RTPSOCreateInfo.GraphicsPipeline.PrimitiveTopology = PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
			// Cull back faces
			RTPSOCreateInfo.GraphicsPipeline.RasterizerDesc.CullMode = CULL_MODE_BACK;
			// Enable depth testing
			RTPSOCreateInfo.GraphicsPipeline.DepthStencilDesc.DepthEnable = False;
			GraphicsPipeline.RTVFormats[0] = ColorBufferFormat;
			GraphicsPipeline.DSVFormat = DepthBufferFormat;
			ShaderCreateInfo ShaderCI;
			// Tell the system that the shader source code is in HLSL.
			// For OpenGL, the engine will convert this into GLSL under the hood.
			ShaderCI.SourceLanguage = SHADER_SOURCE_LANGUAGE_HLSL;

			// OpenGL backend requires emulated combined HLSL texture samplers (g_Texture + g_Texture_sampler combination)
			ShaderCI.UseCombinedTextureSamplers = true;

			ShaderCI.pShaderSourceStreamFactory = pShaderSourceFactory;

			// Create a vertex shader
			RefCntAutoPtr<IShader> pRTVS;
			{
				ShaderCI.Desc.ShaderType = SHADER_TYPE_VERTEX;
				ShaderCI.Desc.Name = FilePath.c_str();
				ShaderCI.EntryPoint = ven;
				ShaderCI.FilePath = FilePath.c_str();
				m_pDevice->CreateShader(ShaderCI, &pRTVS);
			}


#if PLATFORM_ANDROID
			// Vulkan on mobile platforms may require handling surface pre-transforms
			const bool TransformUVCoords = m_pDevice->GetDeviceCaps().IsVulkanDevice();
#else
			constexpr bool TransformUVCoords = false;
#endif

			ShaderMacroHelper Macros;
			Macros.AddShaderMacro("TRANSFORM_UV", TransformUVCoords);
			ShaderCI.Macros = Macros;

			// Create a pixel shader
			RefCntAutoPtr<IShader> pRTPS;
			{
				ShaderCI.Desc.ShaderType = SHADER_TYPE_PIXEL;
				ShaderCI.Desc.Name = FilePath.c_str();
				ShaderCI.EntryPoint = pen;
				ShaderCI.FilePath = FilePath.c_str();
				m_pDevice->CreateShader(ShaderCI, &pRTPS);

				//// Create dynamic uniform buffer that will store our transformation matrix
				//// Dynamic buffers can be frequently updated by the CPU
				//BufferDesc CBDesc;
				//CBDesc.Name = "RTPS constants CB";
				//CBDesc.Size = sizeof(float4) + sizeof(float2x2) * 2;
				//CBDesc.Usage = USAGE_DYNAMIC;
				//CBDesc.BindFlags = BIND_UNIFORM_BUFFER;
				//CBDesc.CPUAccessFlags = CPU_ACCESS_WRITE;
				//m_pDevice->CreateBuffer(CBDesc, nullptr, &m_RTPSConstants);
			}

			RTPSOCreateInfo.pVS = pRTVS;
			RTPSOCreateInfo.pPS = pRTPS;

			// Define variable type that will be used by default
			RTPSOCreateInfo.PSODesc.ResourceLayout.DefaultVariableType = SHADER_RESOURCE_VARIABLE_TYPE_STATIC;

			// clang-format off
			// Shader variables should typically be mutable, which means they are expected
			// to change on a per-instance basis
			ShaderResourceVariableDesc Vars[] =
			{
				{ SHADER_TYPE_PIXEL, "fowresult", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE },
				{ SHADER_TYPE_PIXEL, "lightMapfow", SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC }
			};
			// clang-format on
			RTPSOCreateInfo.PSODesc.ResourceLayout.Variables = Vars;
			RTPSOCreateInfo.PSODesc.ResourceLayout.NumVariables = _countof(Vars);

			// clang-format off
			// Define immutable sampler for g_Texture. Immutable samplers should be used whenever possible
			ImmutableSamplerDesc ImtblSamplers[] =
			{
				{ SHADER_TYPE_PIXEL, "fowresult", Sam_LinearClamp }
			};
			// clang-format on
			RTPSOCreateInfo.PSODesc.ResourceLayout.ImmutableSamplers = ImtblSamplers;
			RTPSOCreateInfo.PSODesc.ResourceLayout.NumImmutableSamplers = _countof(ImtblSamplers);

			m_pDevice->CreateGraphicsPipelineState(RTPSOCreateInfo, m_pRTPSO);

			// Since we did not explcitly specify the type for Constants, default type
			// (SHADER_RESOURCE_VARIABLE_TYPE_STATIC) will be used. Static variables never change and are bound directly
			// to the pipeline state object.
		}
		using vbuffer = n_Render::dyvbf;

		//std::vector<IDeviceObject*> _bfviews;
		int VfArgREle = 512, _bufCount;
		//span<sStruct::VFoutArgs> dspan = {};
		void UpdateVFoutArgs(span<sStruct::VFoutArgs> VfArgspan,
			RefCntAutoPtr<IDeviceContext> m_pImmediateContext,
			Uint32 offset = 0) {
			std::abort();//m_DrawArglocal unused
			m_pImmediateContext->UpdateBuffer(m_DrawArglocal, offset, VfArgspan.size_bytes(), VfArgspan.data(), Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
		}

		void appendtable(Utility::span<sStruct::bufferinfo> binf, Utility::span<sStruct::tinfo> tinf) {
			_instable.merge(binf, *p_rc);
			_culltinfo.merge(tinf, *p_rc);
		}
		static constexpr TEXTURE_FORMAT RenderTargetFormat = TEX_FORMAT_RGBA8_UNORM;
		static inline TEXTURE_FORMAT DepthFormat = TEX_FORMAT_D32_FLOAT;
		static TEXTURE_FORMAT& DepthBufferFormat() {
			return DepthFormat;
		}
		static const void DepthBufferFormat(const TEXTURE_FORMAT& fm ) {
			DepthFormat = fm;
		}
		void addlitbuf(RenderAttr& para) {

		}
		void init(
			RenderContext& rc,
			//Utility::span<RenderAttr> buffers,
			ITextureView* tview,
			Utility::span<sStruct::VFoutArgs> VfArgspan = {},
			size_t Maxinstancnum = 16384);

		template <typename C>
		constexpr void Updateinsdata(C&& fc) {
			//MapHelper<T> m_MappedData(m_pImmediateContext, m_Vresi, MAP_TYPE::MAP_WRITE, MAP_FLAGS::MAP_FLAG_DISCARD);
			fc(_vbIns);
		}
		using t_sinout = sStruct::seleout;
		using VFoutArgs = sStruct::VFoutArgs;
		std::vector<VFoutArgs>o_Cull;
		//math::ui64 m_CurrentFenceValue = 1;
		sStruct::GlobalConstants gCons;
		//set maxtinfo on vram _tinfo table
		math::ui64 ctex = 0, ntex = 0, maxtinfo = 1024;
		INT64 Fencesele = -1;
		void GetLastResult() {
			copy2loacl();
			auto& pCtx = m_pImmediateContext;
			auto&& rct = this->_rt;
			Diligent::MapHelper<VFoutArgs> cullout(rct.m_pImmediateContext, m_VFArgR, MAP_READ, MAP_FLAG_DO_NOT_WAIT);
			auto& dest = o_Cull;
			std::memcpy(dest.data(), cullout, Utility::VecByteSize(dest));
		}

		void Cull(RefCntAutoPtr<IDeviceContext>& m_pImmediateContext,
			nonstd::span<sStruct::objrep> Lighteobjs, //std::vector<sStruct::VFoutArgs> drawresult,
			int insTancenum,
			int MaxArgnum = 100);
		void SetDrawArg(const ui64& src, Utility::span<VFoutArgs> datas);
		void Render2Fog(){
		}
		//clean lightMap feed lightMap -> calculate lightMap -> pixel sample-> blend with input tex
		void Render() { Render(m_pImmediateContext); }
		void Render(RefCntAutoPtr<IDeviceContext>& m_pImmediateContext);

		void AddOneBlock(sStruct::objrep o) {
			o.pos.head<2>().array() += shadowmapsz / 2;
			/*void* p_deltaObjs = NULL;
			auto flag = p_deltaObjs == NULL ? MAP_FLAGS::MAP_FLAG_DISCARD : MAP_FLAGS::MAP_FLAG_NO_OVERWRITE;
			m_pImmediateContext->MapBuffer(m_deltaObjs, MAP_TYPE::MAP_WRITE, flag, p_deltaObjs);*/
			Diligent::MapHelper<sStruct::GlobalConstants> gconst(m_pImmediateContext, m_brightcb, MAP_TYPE::MAP_WRITE, MAP_FLAGS::MAP_FLAG_DISCARD);
			gconst->uiNum = 1;
			gconst->mapsize = { shadowmapsz ,shadowmapsz };
			MapHelper<Uint8> m_MappedData;
			m_MappedData.Map(m_pImmediateContext, m_deltaObjs, MAP_TYPE::MAP_WRITE, MAP_FLAGS::MAP_FLAG_DISCARD);
			std::memcpy(m_MappedData, &o, sizeof(sStruct::objrep));
			m_pImmediateContext->SetPipelineState(m_pMovePSO);
			m_pImmediateContext->CommitShaderResources(m_pMoveSRB, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
			DispatAttribs.ThreadGroupCountX = 1;
			m_pImmediateContext->DispatchCompute(DispatAttribs);
		}
		void AddBlock(nonstd::span<sStruct::objrep> o) {
			/*void* p_deltaObjs = NULL;
			auto flag = p_deltaObjs == NULL ? MAP_FLAGS::MAP_FLAG_DISCARD : MAP_FLAGS::MAP_FLAG_NO_OVERWRITE;
			m_pImmediateContext->MapBuffer(m_deltaObjs, MAP_TYPE::MAP_WRITE, flag, p_deltaObjs);*/
			{
				Diligent::MapHelper<sStruct::GlobalConstants> gconst(m_pImmediateContext, m_brightcb, MAP_TYPE::MAP_WRITE, MAP_FLAGS::MAP_FLAG_DISCARD);
				gconst->uiNum = o.size();
				gconst->mapsize = { shadowmapsz ,shadowmapsz };
				MapHelper<sStruct::objrep> m_MappedData(m_pImmediateContext, m_deltaObjs, MAP_TYPE::MAP_WRITE, MAP_FLAGS::MAP_FLAG_DISCARD);
				std::memcpy(m_MappedData, o.data(), o.size_bytes());
			}
			m_pImmediateContext->SetPipelineState(m_pMovePSO);
			m_pImmediateContext->CommitShaderResources(m_pMoveSRB, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
			DispatAttribs.ThreadGroupCountX = (o.size() + m_ThreadGroupSize - 1) / m_ThreadGroupSize;
			m_pImmediateContext->DispatchCompute(DispatAttribs);
		}
		void GenerateBlock(Utility::span<sStruct::objrep> bks, math::int4 size = { 100,100,0,0 });
		static constexpr int rmk = 0xFFFFFF;
		static constexpr int remk = 0xFFFFFF + 1;
		sStruct::Selects parasele;
		void rectSele(const sStruct::Selects& csele, const ui64 sz = 1024)
		{
			parasele = csele;
			parasele.inum = remk;
		}
		void Raysele(const sStruct::Selects& csele, const ui64 sz = 1024)
		{
			parasele = csele;
			parasele.inum = rmk;
		}
		inline void RenderScreenRect(const sStruct::Selects& rect) {
			_rect = rect;
			RTDrawAttrs.NumVertices = 8;
		}
		static constexpr float ClearColor[] = { 0.2, 0.3, 0.4, 1 };
		int shadowmapsz = 1024, Texsize = 512;
		int64_t Tim = 0,MaxSOLnum=256;
		sStruct::Selects _rect;
		RefCntAutoPtr<IBuffer>                m_VFArgR;
		RefCntAutoPtr<IBuffer>                m_DrawArglocal;
		RefCntAutoPtr<IBuffer>                _Hash2Dtab;
		RefCntAutoPtr<IBuffer>                _DisptchLit;
		RefCntAutoPtr<IBuffer>                _Selecbuf;
		//RefCntAutoPtr<IBuffer>                _vbtrans;

		DispatchComputeIndirectAttribs CLitAttribs;
		vbuffer Nebbuf;
		vbuffer stagNebbuf;
		RenderContext* p_rc;
		RefCntAutoPtr<IDeviceContext> m_pImmediateContext;
		std::array<RefCntAutoPtr<ITextureView>, 2>           m_ShadowMapSRV;
		std::array<RefCntAutoPtr<ITextureView>, 2>           m_ShadowMapUAV;
		/*void* p_deltaObjs = NULL;
		void* p_brightObjs = NULL;*/
		RefCntAutoPtr<ITextureView>           m_resistanceSRV;
		RefCntAutoPtr<ITextureView>           m_resistanceUAV;
		RefCntAutoPtr<ITextureView>           m_resistanceClearUAV;
		RefCntAutoPtr<ITextureView>           m_RendeRT;
		RefCntAutoPtr<ITextureView>           m_RendeRTf;
		RefCntAutoPtr<ITextureView>           m_ShadowSRV;

		RefCntAutoPtr<IShaderResourceVariable>  m_lightMapUAV;
		RefCntAutoPtr<IShaderResourceVariable>  m_lightMapUAV1;
		RefCntAutoPtr<IShaderResourceVariable>  m_ShadowMap1SRV;
		RefCntAutoPtr<IShaderResourceVariable>  m_ClearSRV;
		RefCntAutoPtr<IShaderResourceVariable>  m_TargetMapSRV;
		RefCntAutoPtr<IShaderResourceVariable>  m_lightMapfowSRV;
		RefCntAutoPtr<IPipelineState>         m_pResetPSO;
		RefCntAutoPtr<IShaderResourceBinding> m_pResetSRB;
		RefCntAutoPtr<IPipelineState>         m_pResetArgPSO;
		RefCntAutoPtr<IShaderResourceBinding> m_pResetArgSRB;
		RefCntAutoPtr<IShaderResourceBinding> m_pReset1SRB;
		RefCntAutoPtr<IShaderResourceBinding> m_pReset2SRB;
		RefCntAutoPtr<IPipelineState>         m_pMovePSO;
		RefCntAutoPtr<IShaderResourceBinding> m_pMoveSRB;
		RefCntAutoPtr<IPipelineState> m_CSLitPSO;
		RefCntAutoPtr<IShaderResourceBinding> m_plightSRB;
		RefCntAutoPtr<IPipelineState> m_RfowPSO;
		RefCntAutoPtr<IShaderResourceBinding> m_fowSRB[2];
		RefCntAutoPtr<IShaderResourceBinding> m_fowSRB0;
		RefCntAutoPtr<IPipelineState> m_pRTPSO;
		RefCntAutoPtr<IShaderResourceBinding> m_pRTSRB;
		RefCntAutoPtr<ISwapChain> m_pSwapChain;
		RenderUtil::PSB<2> Vfilterpsb;
		RenderUtil::PSB<2> WriteGMap_PSB;
	private:
		DispatchComputeAttribs DispatAttribs;
		DrawAttribs RTDrawAttrs;

	};
}

