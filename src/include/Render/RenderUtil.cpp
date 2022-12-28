#include "RenderUtil.h"
//namespace RenderUtil{
//    using namespace Diligent;
//    void md_BufferDesc(const char* name, BufferDesc& BuffDesc, ui32 uiSize, ui32 ElementByteStride, USAGE Usage, BIND_FLAGS BindFlags, CPU_ACCESS_FLAGS CPUAccessFlags)
//  
//}
namespace pj2 {
	namespace DrawFunc {
		void CreateComputePSO(
			const char* EntryPoint,
			const char* FilePath,
			const char* Name,
			RefCntAutoPtr<IRenderDevice> m_pDevice,
			RefCntAutoPtr<IShaderSourceInputStreamFactory> pShaderSourceFactory,
			RefCntAutoPtr<IPipelineState>& p_pso,
			nonstd::span<ShaderResourceVariableDesc>Vars,
			int m_ThreadGroupSize , ShaderMacroHelper Macros) {

			ShaderCreateInfo ShaderCI;
			// Tell the system that the shader source code is in HLSL.
			// For OpenGL, the engine will convert this into GLSL under the hood.
			ShaderCI.SourceLanguage = SHADER_SOURCE_LANGUAGE_HLSL;
			ShaderCI.ShaderCompiler = SHADER_COMPILER_DXC;
			// OpenGL backend requires emulated combined HLSL texture samplers (g_Texture + g_Texture_sampler combination)
			//ShaderCI.UseCombinedTextureSamplers = true;
			ShaderCI.pShaderSourceStreamFactory = pShaderSourceFactory;
			if (m_ThreadGroupSize > 0) {
				Macros.AddShaderMacro("THREAD_GROUP_SIZE", m_ThreadGroupSize);
				Macros.Finalize();
			}

			RefCntAutoPtr<IShader> pCS;
			{
				ShaderCI.Desc.ShaderType = SHADER_TYPE_COMPUTE;
				ShaderCI.EntryPoint = EntryPoint;
				ShaderCI.Desc.Name = Name;
				ShaderCI.FilePath = FilePath;
				ShaderCI.Macros = Macros;
				m_pDevice->CreateShader(ShaderCI, &pCS);
			}
			ComputePipelineStateCreateInfo PSOCreateInfo;
			//PipelineStateCreateInfo PSOCreateInfo;
			PipelineStateDesc& PSODesc = PSOCreateInfo.PSODesc;

			// This is a compute pipeline
			PSODesc.PipelineType = PIPELINE_TYPE_COMPUTE;
			PSODesc.ResourceLayout.DefaultVariableType = SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE;

			// clang-format on
			PSODesc.ResourceLayout.Variables = Vars.data();
			PSODesc.ResourceLayout.NumVariables = Vars.size();

			PSODesc.Name = Name;// Utility::append(Name, "_PSO");
			PSOCreateInfo.pCS = pCS;
			if (p_pso) {
				p_pso.Release();
			}
			m_pDevice->CreateComputePipelineState(PSOCreateInfo, &p_pso);

		}
		void CreateComputePSO(const char* EntryPoint, const char* FilePath, const char* Name, RefCntAutoPtr<IRenderDevice> m_pDevice, RefCntAutoPtr<IShaderSourceInputStreamFactory> pShaderSourceFactory, RefCntAutoPtr<IPipelineState>& p_pso, int m_ThreadGroupSize)

		{

			ShaderCreateInfo ShaderCI;
			// Tell the system that the shader source code is in HLSL.
			// For OpenGL, the engine will convert this into GLSL under the hood.
			ShaderCI.SourceLanguage = SHADER_SOURCE_LANGUAGE_HLSL;

			ShaderCI.ShaderCompiler = SHADER_COMPILER_DXC;
			// OpenGL backend requires emulated combined HLSL texture samplers (g_Texture + g_Texture_sampler combination)
			ShaderCI.UseCombinedTextureSamplers = true;
			ShaderCI.pShaderSourceStreamFactory = pShaderSourceFactory;
			ShaderMacroHelper Macros;
			if (m_ThreadGroupSize > 0) {
				Macros.AddShaderMacro("THREAD_GROUP_SIZE", m_ThreadGroupSize);
				Macros.Finalize();
			}

			RefCntAutoPtr<IShader> pCS;
			{
				ShaderCI.Desc.ShaderType = SHADER_TYPE_COMPUTE;
				ShaderCI.EntryPoint = EntryPoint;
				ShaderCI.Desc.Name = Name;
				ShaderCI.FilePath = FilePath;
				ShaderCI.Macros = Macros;
				m_pDevice->CreateShader(ShaderCI, &pCS);
			}
			ComputePipelineStateCreateInfo PSOCreateInfo;
			//PipelineStateCreateInfo PSOCreateInfo;
			PipelineStateDesc& PSODesc = PSOCreateInfo.PSODesc;

			// This is a compute pipeline
			PSODesc.PipelineType = PIPELINE_TYPE_COMPUTE;
			PSODesc.ResourceLayout.DefaultVariableType = SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE;


			PSODesc.Name = Utility::append(Name, "_PSO");
			PSOCreateInfo.pCS = pCS;
			m_pDevice->CreateComputePipelineState(PSOCreateInfo, &p_pso);

		}
		void CreateSetBUf(RefCntAutoPtr<IRenderDevice>& m_pDevice, RefCntAutoPtr<IDeviceContext>& _pImmediateContext, const BufferDesc BuffDesc, IBuffer** ppBuffer, RefCntAutoPtr<IShaderResourceBinding>& psrb, Diligent::SHADER_TYPE ST, const BufferData* pBuffData) {

			m_pDevice->CreateBuffer(BuffDesc, nullptr, ppBuffer);
			BUFFER_VIEW_TYPE bv;
			StateTransitionDesc dec;
			dec.pResource = *ppBuffer;
			switch (BuffDesc.BindFlags)
			{
			case BIND_SHADER_RESOURCE:
				bv = BUFFER_VIEW_SHADER_RESOURCE;
				dec.OldState = RESOURCE_STATE_UNKNOWN;
				dec.NewState = RESOURCE_STATE_SHADER_RESOURCE;
				break;
			case BIND_UNORDERED_ACCESS:
				bv = BUFFER_VIEW_UNORDERED_ACCESS;
				dec.OldState = RESOURCE_STATE_UNKNOWN;
				dec.NewState = RESOURCE_STATE_UNORDERED_ACCESS;
				break;
			default:
				if (BuffDesc.Usage == USAGE_STAGING) {
					dec.OldState = RESOURCE_STATE_UNKNOWN;
					dec.NewState = RESOURCE_STATE_COPY_SOURCE;
					_pImmediateContext->TransitionResourceStates(1, &dec);
				}
				return;
				break;
			}
			_pImmediateContext->TransitionResourceStates(1, &dec);
			//psrb->GetVariableByName(ST, BuffDesc.Name)->Set((*ppBuffer)->GetDefaultView(bv));
			auto pC = psrb->GetVariableByName(ST, BuffDesc.Name);
			pC->Set((*ppBuffer)->GetDefaultView(bv));
			/*if (pC) {
				pC->Set((*ppBuffer)->GetDefaultView(bv));
			}*/
		}

		void SetBUf(RefCntAutoPtr<IRenderDevice>& m_pDevice, RefCntAutoPtr<IDeviceContext>& _pImmediateContext, const BufferDesc BuffDesc, IBuffer** ppBuffer, RefCntAutoPtr<IShaderResourceBinding>& psrb, Diligent::SHADER_TYPE ST, const BufferData* pBuffData) {
			//m_pDevice->CreateBuffer(BuffDesc, nullptr, ppBuffer);
			BUFFER_VIEW_TYPE bv;
			StateTransitionDesc dec;
			dec.pResource = *ppBuffer;
			switch (BuffDesc.BindFlags)
			{
			case BIND_SHADER_RESOURCE:
				bv = BUFFER_VIEW_SHADER_RESOURCE;
				dec.OldState = RESOURCE_STATE_UNKNOWN;
				dec.NewState = RESOURCE_STATE_SHADER_RESOURCE;
				break;
			case BIND_UNORDERED_ACCESS:
				bv = BUFFER_VIEW_UNORDERED_ACCESS;
				dec.OldState = RESOURCE_STATE_UNKNOWN;
				dec.NewState = RESOURCE_STATE_UNORDERED_ACCESS;
				break;
			default:
				if (BuffDesc.Usage == USAGE_STAGING) {
					dec.OldState = RESOURCE_STATE_UNKNOWN;
					dec.NewState = RESOURCE_STATE_COPY_SOURCE;
					_pImmediateContext->TransitionResourceStates(1, &dec);
				}
				return;
				break;
			}
			_pImmediateContext->TransitionResourceStates(1, &dec);
			auto pC = psrb->GetVariableByName(ST, BuffDesc.Name);
			//if (pC) 
			{
				pC->Set((*ppBuffer)->GetDefaultView(bv));
			}
		}
		void CreateSetBUf(RefCntAutoPtr<IRenderDevice>& m_pDevice, RefCntAutoPtr<IDeviceContext>& _pImmediateContext,
			const BufferDesc BuffDesc, IBuffer** ppBuffer, RefCntAutoPtr<IShaderResourceBinding>& psrb,
			Diligent::SHADER_TYPE ST, const BufferData* pBuffData, Diligent::BIND_FLAGS bf) {
			m_pDevice->CreateBuffer(BuffDesc, pBuffData, ppBuffer);
			BUFFER_VIEW_TYPE bv = BUFFER_VIEW_SHADER_RESOURCE;
			StateTransitionDesc dec;
			dec.pResource = *ppBuffer;
			bf = bf == BIND_NONE ? BuffDesc.BindFlags : bf;
			{
				switch (bf)
				{
				case BIND_SHADER_RESOURCE:
					dec.OldState = RESOURCE_STATE_UNKNOWN;
					dec.NewState = RESOURCE_STATE_SHADER_RESOURCE;
					break;
				case BIND_UNIFORM_BUFFER:
					dec.OldState = RESOURCE_STATE_UNKNOWN;
					dec.NewState = RESOURCE_STATE_CONSTANT_BUFFER;
					break;
				case BIND_UNORDERED_ACCESS:
					bv = BUFFER_VIEW_UNORDERED_ACCESS;
					dec.OldState = RESOURCE_STATE_UNKNOWN;
					dec.NewState = RESOURCE_STATE_UNORDERED_ACCESS;
					break;
				default:
					if (BuffDesc.Usage == USAGE_STAGING) {
						dec.OldState = RESOURCE_STATE_UNKNOWN;
						dec.NewState = RESOURCE_STATE_COPY_SOURCE;
						_pImmediateContext->TransitionResourceStates(1, &dec);
						break;
					}
					std::abort();
					break;
				}
			}
			dec.Flags = Diligent::STATE_TRANSITION_FLAG_UPDATE_STATE;
			_pImmediateContext->TransitionResourceStates(1, &dec);
			auto pC = psrb->GetVariableByName(ST, BuffDesc.Name);
			{
				pC->Set((*ppBuffer)->GetDefaultView(bv));
			}
		}
	}
	void RenderUtilImp::CreatePipelineStates(IPipelineState** const pPSO,
		RefCntAutoPtr<IRenderDevice> m_pDevice,
		Diligent::TEXTURE_FORMAT ColorBufferFormat,
		Diligent::TEXTURE_FORMAT DepthBufferFormat,
		RefCntAutoPtr<IShaderSourceInputStreamFactory> pShaderSourceFactory,
		const std::string FilePath, const Ichar* ven, const Ichar* pen,
		const Utility::span<ShaderResourceVariableDesc> sd,
		const Utility::span<LayoutElement> inputLayoutDesc,
		const Utility::span<ImmutableSamplerDesc> StaticSamplers,
		PRIMITIVE_TOPOLOGY VertexTop, bool AntialiasedLineEnable, bool DepthEnable, const Ichar* vnm, const Ichar* pnm,
		const Diligent::CULL_MODE cullmode,
		ShaderMacroHelper Macros) {

		ShaderCreateInfo                               ShaderCI;
		ShaderCI.pShaderSourceStreamFactory = pShaderSourceFactory;
		ShaderCI.SourceLanguage = SHADER_SOURCE_LANGUAGE_HLSL;
		//ShaderCI.ShaderCompiler = SHADER_COMPILER_DXC;
		ShaderCI.CompileFlags = SHADER_COMPILE_FLAG_ENABLE_UNBOUNDED_ARRAYS;
		ShaderCI.UseCombinedTextureSamplers = true;


		// clang-format on
		ShaderCI.Macros = Macros;
		std::array<char, 4096> stbuffer;
		//fixed_iterator<char*> fi(stbuffer.data(), sizeof(stbuffer));
		//stbuffer.reserve(4096);
		std::memset(stbuffer.data(), 0, sizeof(stbuffer));
		fmt::format_to_n(stbuffer.data(), stbuffer.size(), "{0} {1}", FilePath, ven);
		//Utility::append(stbuffer, FilePath.c_str(), '_'+ven);
		//fmt::format_to(std::back_inserter(stbuffer), "FilePath is {0} name={1}", FilePath,vnm==NULL? ven :vnm);
		ShaderCI.Desc.ShaderType = SHADER_TYPE_VERTEX;
		//stbuffer.push_back('/0');
		ShaderCI.Desc.Name = stbuffer.data();
		ShaderCI.EntryPoint = ven;
		ShaderCI.FilePath = FilePath.c_str();
		RefCntAutoPtr<IShader> pVS;
		m_pDevice->CreateShader(ShaderCI, &pVS);
		std::memset(stbuffer.data(), 0, sizeof(stbuffer));
		fmt::format_to_n(stbuffer.data(), stbuffer.size(), "{0} {1}", FilePath, pen);
		/*Utility::append(stbuffer, FilePath.c_str(), "_");
		Utility::appendTo(stbuffer, pen);*/
		//stbuffer.push_back('/0');
		ShaderCI.Desc.Name = stbuffer.data();;
		ShaderCI.EntryPoint = pen;
		ShaderCI.FilePath = FilePath.c_str();
		ShaderCI.Desc.ShaderType = SHADER_TYPE_PIXEL;
		RefCntAutoPtr<IShader> pPS;
		m_pDevice->CreateShader(ShaderCI, &pPS);
		BlendStateDesc BlendState[5];
		BlendState[1].RenderTargets[0].BlendEnable = true;
		BlendState[1].RenderTargets[0].SrcBlend = BLEND_FACTOR_SRC_ALPHA;
		BlendState[1].RenderTargets[0].DestBlend = BLEND_FACTOR_INV_SRC_ALPHA;

		BlendState[2].RenderTargets[0].BlendEnable = true;
		BlendState[2].RenderTargets[0].SrcBlend = BLEND_FACTOR_INV_SRC_ALPHA;
		BlendState[2].RenderTargets[0].DestBlend = BLEND_FACTOR_SRC_ALPHA;

		BlendState[3].RenderTargets[0].BlendEnable = true;
		BlendState[3].RenderTargets[0].SrcBlend = BLEND_FACTOR_SRC_COLOR;
		BlendState[3].RenderTargets[0].DestBlend = BLEND_FACTOR_INV_SRC_COLOR;

		BlendState[4].RenderTargets[0].BlendEnable = true;
		BlendState[4].RenderTargets[0].SrcBlend = BLEND_FACTOR_INV_SRC_COLOR;
		BlendState[4].RenderTargets[0].DestBlend = BLEND_FACTOR_SRC_COLOR;
		//for (Uint32 vb = 0; vb < m_Mesh._subMesh.size(); ++vb)
		{
			GraphicsPipelineStateCreateInfo PSOCreateInfo;
			Diligent::GraphicsPipelineDesc& GraphicsPipeline = PSOCreateInfo.GraphicsPipeline;
			PipelineStateDesc& PSODesc = PSOCreateInfo.PSODesc;
			auto& RT0 = GraphicsPipeline.BlendDesc.RenderTargets[0];
			RT0.BlendEnable = true;
			RT0.SrcBlend = BLEND_FACTOR_SRC_ALPHA;
			RT0.DestBlend = BLEND_FACTOR_INV_SRC_ALPHA;
			//RT0.SrcBlendAlpha = BLEND_FACTOR_SRC_ALPHA;

			//std::vector<LayoutElement> Elements;
			auto& InputLayout = GraphicsPipeline.InputLayout;
			InputLayout.LayoutElements = inputLayoutDesc.data();
			InputLayout.NumElements = inputLayoutDesc.size();
			// clang-format on
			PSODesc.ResourceLayout.ImmutableSamplers = StaticSamplers.data();
			PSODesc.ResourceLayout.NumImmutableSamplers = StaticSamplers.size();


			// clang-format on
			PSODesc.ResourceLayout.Variables = sd.data();
			PSODesc.ResourceLayout.NumVariables = sd.size();

			PSODesc.Name = FilePath.c_str();
			PSOCreateInfo.pVS = pVS;
			PSOCreateInfo.pPS = pPS;
			GraphicsPipeline.RTVFormats[0] = ColorBufferFormat;
			GraphicsPipeline.NumRenderTargets = 1;
			GraphicsPipeline.PrimitiveTopology = VertexTop;
			GraphicsPipeline.DSVFormat = DepthBufferFormat;
			//PSODesc.GraphicsPipeline.DepthStencilDesc.DepthFunc = COMPARISON_FUNC_LESS_EQUAL; // Cull back faces
			GraphicsPipeline.RasterizerDesc.CullMode = cullmode;// CULL_MODE_BACK;
																//PSODesc.GraphicsPipeline.RasterizerDesc.AntialiasedLineEnable = AntialiasedLineEnable;
																// Enable depth testing
			GraphicsPipeline.DepthStencilDesc.DepthEnable = DepthEnable;
			m_pDevice->CreateGraphicsPipelineState(PSOCreateInfo, pPSO);
		}
	}

	void RenderUtilImp::CreateSetStaticBUf(RefCntAutoPtr<IRenderDevice>& m_pDevice, RefCntAutoPtr<IDeviceContext>& _pImmediateContext, const BufferDesc BuffDesc, IBuffer** ppBuffer, RefCntAutoPtr<IPipelineState>& pso, Diligent::SHADER_TYPE ST, const BufferData* pBuffData, Diligent::BIND_FLAGS bf) {

		m_pDevice->CreateBuffer(BuffDesc, pBuffData, ppBuffer);
		BUFFER_VIEW_TYPE bv;
		StateTransitionDesc dec;
		dec.pResource = *ppBuffer;
		bf = bf == BIND_NONE ? BuffDesc.BindFlags : bf;
		{
			switch (bf)
			{
			case BIND_SHADER_RESOURCE:
				bv = BUFFER_VIEW_SHADER_RESOURCE;
				dec.OldState = RESOURCE_STATE_UNKNOWN;
				dec.NewState = RESOURCE_STATE_SHADER_RESOURCE;
				break;
			case BIND_UNIFORM_BUFFER:
				bv = BUFFER_VIEW_SHADER_RESOURCE;
				dec.OldState = RESOURCE_STATE_UNKNOWN;
				dec.NewState = RESOURCE_STATE_SHADER_RESOURCE;
				break;
			case BIND_UNORDERED_ACCESS:
				bv = BUFFER_VIEW_UNORDERED_ACCESS;
				dec.OldState = RESOURCE_STATE_UNKNOWN;
				dec.NewState = RESOURCE_STATE_UNORDERED_ACCESS;
				break;
			default:
				if (BuffDesc.Usage == USAGE_STAGING) {
					dec.OldState = RESOURCE_STATE_UNKNOWN;
					dec.NewState = RESOURCE_STATE_COPY_SOURCE;
					_pImmediateContext->TransitionResourceStates(1, &dec);
					break;
				}
				std::abort();
				return;
				break;
			}
		}
		_pImmediateContext->TransitionResourceStates(1, &dec);
		auto pC = pso->GetStaticVariableByName(ST, BuffDesc.Name);
		{
			pC->Set((*ppBuffer)->GetDefaultView(bv));
		}
	}

	namespace RenderUtil {
		void CreateCompPSOStr(const char* EntryPoint, const char* FilePath, const char* Name, RefCntAutoPtr<IRenderDevice> m_pDevice, RefCntAutoPtr<IShaderSourceInputStreamFactory> pShaderSourceFactory, RefCntAutoPtr<IPipelineState>& p_pso, Utility::span<Diligent::ShaderResourceVariableDesc> Vars, int m_ThreadGroupSize)

		{
			using namespace Diligent;
			ShaderCreateInfo ShaderCI;
			// Tell the system that the shader source code is in HLSL.
			// For OpenGL, the engine will convert this into GLSL under the hood.
			ShaderCI.SourceLanguage = SHADER_SOURCE_LANGUAGE_HLSL;

			ShaderCI.ShaderCompiler = SHADER_COMPILER_DXC;
			// OpenGL backend requires emulated combined HLSL texture samplers (g_Texture + g_Texture_sampler combination)
			//ShaderCI.UseCombinedTextureSamplers = true;
			ShaderCI.pShaderSourceStreamFactory = pShaderSourceFactory;
			ShaderMacroHelper Macros;
			if (m_ThreadGroupSize > 0) {
				Macros.AddShaderMacro("THREAD_GROUP_SIZE", m_ThreadGroupSize);
				Macros.Finalize();
			}

			RefCntAutoPtr<IShader> pCS;
			{
				ShaderCI.Desc.ShaderType = SHADER_TYPE_COMPUTE;
				ShaderCI.EntryPoint = EntryPoint;
				ShaderCI.Desc.Name = Name;
				ShaderCI.Source = FilePath;
				ShaderCI.Macros = Macros;
				m_pDevice->CreateShader(ShaderCI, &pCS);
			}
			ComputePipelineStateCreateInfo PSOCreateInfo;
			//PipelineStateCreateInfo PSOCreateInfo;
			PSOCreateInfo.pCS = pCS;
			if (p_pso) {
				p_pso.Release();
			}
			m_pDevice->CreateComputePipelineState(PSOCreateInfo, &p_pso);

		}
		void CCPSO(const char* EntryPoint, const char* FilePath, const char* Name, RefCntAutoPtr<IRenderDevice> m_pDevice, RefCntAutoPtr<IShaderSourceInputStreamFactory> pShaderSourceFactory, RefCntAutoPtr<IPipelineState>& p_pso, Utility::span<Diligent::ShaderResourceVariableDesc> Vars, Diligent::ShaderMacroHelper Macros, int m_ThreadGroupSize) {
			using namespace Diligent;
			Diligent::ComputePipelineStateCreateInfo PSOCreateInfo;
			ShaderCreateInfo ShaderCI;
			// Tell the system that the shader source code is in HLSL.
			// For OpenGL, the engine will convert this into GLSL under the hood.
			ShaderCI.SourceLanguage = SHADER_SOURCE_LANGUAGE_HLSL;
			//ShaderCI.ShaderCompiler = SHADER_COMPILER_DXC;

			// OpenGL backend requires emulated combined HLSL texture samplers (g_Texture + g_Texture_sampler combination)
			ShaderCI.UseCombinedTextureSamplers = true;
			ShaderCI.pShaderSourceStreamFactory = pShaderSourceFactory;
			if (m_ThreadGroupSize > 0) {
				Macros.AddShaderMacro("THREAD_GROUP_SIZE", m_ThreadGroupSize);
				Macros.Finalize();
			}

			RefCntAutoPtr<IShader> pCS;
			{
				ShaderCI.Desc.ShaderType = SHADER_TYPE_COMPUTE;
				ShaderCI.EntryPoint = EntryPoint;
				ShaderCI.Desc.Name = Name;
				ShaderCI.FilePath = FilePath;
				ShaderCI.Macros = Macros;
				m_pDevice->CreateShader(ShaderCI, &pCS);
			}
			//PipelineStateCreateInfo PSOCreateInfo;
			PipelineStateDesc& PSODesc = PSOCreateInfo.PSODesc;

			// This is a compute pipeline
			PSODesc.PipelineType = PIPELINE_TYPE_COMPUTE;
			PSODesc.ResourceLayout.DefaultVariableType = SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE;

			// clang-format on
			PSODesc.ResourceLayout.Variables = Vars.data();
			PSODesc.ResourceLayout.NumVariables = Vars.size();

			PSODesc.Name = Name;// Utility::append(Name, "_PSO");
			PSOCreateInfo.pCS = pCS;
			m_pDevice->CreateComputePipelineState(PSOCreateInfo, &p_pso);
		}
	}
}
