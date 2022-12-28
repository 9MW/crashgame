#include "LineRender.h"
#include"Common/UtilityGeo.h"
namespace pj2::n_Render{

	void LPS(IPipelineState** const pPSO,
		RefCntAutoPtr<IRenderDevice> m_pDevice,
		Diligent::TEXTURE_FORMAT ColorBufferFormat,
		Diligent::TEXTURE_FORMAT DepthBufferFormat,
		RefCntAutoPtr<IShaderSourceInputStreamFactory> pShaderSourceFactory,
		const std::string FilePath, const Ichar* ven, const Ichar* pen,
		const Utility::span<ShaderResourceVariableDesc> sd,
		PRIMITIVE_TOPOLOGY VertexTop, bool AntialiasedLineEnable) {
		using namespace Diligent;
		const Utility::span<ImmutableSamplerDesc> StaticSamplers = {};
		bool DepthEnable = true;
		const Ichar* vnm = "vert_rline";
		const Ichar* pnm = "frag_rline";
		const Diligent::CULL_MODE cullmode = CULL_MODE_NONE;
		ShaderMacroHelper Macros;
		ShaderCreateInfo                               ShaderCI;
		ShaderCI.pShaderSourceStreamFactory = pShaderSourceFactory;
		ShaderCI.SourceLanguage = SHADER_SOURCE_LANGUAGE_HLSL;
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

			InputLayout.NumElements = 0;
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

	using al = std::atomic_llong;
	al t,tvnum;
	void LineRender::init(RenderContext& rt) {
		main() = this;
		{
			t = 0;
			_Linebuf.resize(18*1024);//~ 1024 circle | 18 point per circle ~4k line
			BufferDesc VertBuffDesc;
			VertBuffDesc.Name = "line vertex buffer";
			VertBuffDesc.Usage = USAGE_DYNAMIC;
			VertBuffDesc.BindFlags = BIND_SHADER_RESOURCE;
			VertBuffDesc.CPUAccessFlags = CPU_ACCESS_WRITE;
			VertBuffDesc.Mode = BUFFER_MODE_STRUCTURED;
			VertBuffDesc.Size = _Linebuf.capacity() * sizeof(bflinevtxdf);
			VertBuffDesc.ElementByteStride = sizeof(bflinevtxdf);
			rt.m_pDevice->CreateBuffer(VertBuffDesc, nullptr, &_pLineVertexBuffer);
		}
		ShaderResourceVariableDesc Vars[] =
		{
			{ SHADER_TYPE_PIXEL, "g_ColorMap", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE },
			{ SHADER_TYPE_PIXEL, "cell", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE },
			{ SHADER_TYPE_PIXEL, "map", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE },
			{ SHADER_TYPE_VERTEX, "animreco", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE },
			{ SHADER_TYPE_VERTEX, "LineVertex", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE },
			{ SHADER_TYPE_VERTEX, "objectTransfromBuffer", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE }
		};;
		auto& pRenderLinePSO = _psb.PSO;
		LPS
		(&pRenderLinePSO, rt.m_pDevice, rt.RenderTargetFormat, rt.DepthBufferFormat,
			rt.pShaderSourceFactory, "NoSkin.hlsl", "vert1", "frag", Vars,
			 PRIMITIVE_TOPOLOGY_LINE_LIST, true);

		pRenderLinePSO->GetStaticVariableByName(SHADER_TYPE_VERTEX, "cbCameraAttribs")->Set(rt.pCameraAttribs);
		if (auto psvb = pRenderLinePSO->GetStaticVariableByName(SHADER_TYPE_VERTEX, "GlobalConstants"))
			psvb->Set(_cbufPara);
		_psb.CreateShaderResourceBinding();
		_psb.GetShaderResourceBinding(0)->GetVariableByName(SHADER_TYPE_VERTEX,"LineVertex")->
			Set(_pLineVertexBuffer->GetDefaultView(BUFFER_VIEW_SHADER_RESOURCE));
		m_pImmediateContext = rt.m_pImmediateContext;
		return;
	}
	inline void LineRender::LineStrip(void* para) {}
	void LineRender::NewLine(void* para) {
		auto vb = t.fetch_add(4);
		tvnum += 3;
		float4* f3 = (float4*)para;
		auto& lb= _Linebuf[vb++];
		lb.pos=f3[0];
		lb.pos[3] = 0xa0;
		_Linebuf[vb++] = { f3[0] };
		_Linebuf[vb++] = { f3[1] };
		_Linebuf[vb++] = { f3[1] };
		_Linebuf[vb-1].pos[3] = 0xa0;
		//_Linebuf[vb++] = { f3[0]};
		for (size_t i = 0; i < 4; i++)
		{
			_Linebuf[--vb].color = f3[2];
		}
	}
	/// <summary>
/// Create an array with the corners points of the line strip.
///  The first and the last point define the start and end tangents of the line strip.
///  So you need to add 1 point before the line and one point after the line.
///  Of course it would be easy, to identify the first and last element of the array by comparing the index to 0 and the length of the array, but we don't want to do any extra checks in the shader.
//  If a line loop has to be draw, then the last point has to be add to the array headand the first point to its tail.
/// </summary>
 void LineRender::Render() {
	 auto& ti = t._Storage._Value;
	 auto& tl = tvnum._Storage._Value;
	 if (ti == 0) {
		 return;
		 float4 l02[] = { {0,1,0,0},{0,4,0,0},{1,1,1,1} };
		 NewLine(l02);
		 l02[0] = { 1,2,0,0 };
		 l02[1] = { 2,2,0,0 };
		 NewLine(l02);
	 }
	//refer to	https://stackoverflow.com/questions/60440682/drawing-a-line-in-modern-opengl
	{
		auto& line = _Linebuf;
		MapHelper<bflinevtxdf>LineDate(m_pImmediateContext, _pLineVertexBuffer, MAP_WRITE, MAP_FLAG_DISCARD);
		{
			//std::memcpy(LineDate + lines.size(), gddd.data(), nonstd::span(gddd).size_bytes());
			std::memcpy(LineDate , _Linebuf.data(),ti*sizeof(bflinevtxdf));
		}
	}
	DrawAttribs dA;
	dA.Flags = DRAW_FLAG_VERIFY_ALL;
	dA.NumVertices = (tl) * 6;// +gddd.size();
	m_pImmediateContext->SetPipelineState(_psb.PSO);
	Uint64   Offsets[1] = {};
	IBuffer* pVBs[] = { _pLineVertexBuffer };

	m_pImmediateContext->CommitShaderResources(_psb.GetShaderResourceBinding(0), RESOURCE_STATE_TRANSITION_MODE_VERIFY);

	m_pImmediateContext->Draw(dA);
	ti = 0;
	tl = 0;
}
 LineRender*& LineRender::main()
 {
	 static
		 LineRender* lb = NULL;
	 return lb;
 }


}