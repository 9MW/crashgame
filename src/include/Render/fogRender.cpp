#include "fogRender.h"
#include"io/loader.h"
#pragma optimize( "", off )
using namespace Diligent;
RefCntAutoPtr<IBuffer>                m_LitR;
RefCntAutoPtr<IBuffer>                m_Debugbuf;
IBuffer* m_Debugbufv;
#define vdbfnum 3000
using vdt = sStruct::CastArg;
int maxlit = 256;
enum texadss
{
	GroupMap,blenderfog
};
RefCntAutoPtr<IBufferView>           VBFSRVs[10];
RefCntAutoPtr<IBufferView>           VBFUAVs[10];
RefCntAutoPtr<IBuffer>                m_GidBuf;

ShaderResourceVariableDesc Varspixel[] =
{
	//{SHADER_TYPE_PIXEL, "lightMapfow", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE},
	{ SHADER_TYPE_PIXEL, "lightMapfow", SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC },
	{ SHADER_TYPE_PIXEL, "resistanceMapR", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE },
	{ SHADER_TYPE_PIXEL, "lightMaprt", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE },
	{ SHADER_TYPE_PIXEL, "fowresult", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE },
	{ SHADER_TYPE_VERTEX, "Cbuf", SHADER_RESOURCE_VARIABLE_TYPE_STATIC }
};
SamplerDesc SamLinearClampDesc
{
	FILTER_TYPE_LINEAR, FILTER_TYPE_LINEAR, FILTER_TYPE_LINEAR,
	TEXTURE_ADDRESS_CLAMP, TEXTURE_ADDRESS_CLAMP, TEXTURE_ADDRESS_CLAMP
};
SamplerDesc SampointClampDesc
{
	FILTER_TYPE_POINT, FILTER_TYPE_POINT, FILTER_TYPE_POINT,
	TEXTURE_ADDRESS_CLAMP, TEXTURE_ADDRESS_CLAMP, TEXTURE_ADDRESS_CLAMP
};
ImmutableSamplerDesc ImtblSamplers[] =
{
	{ SHADER_TYPE_PIXEL, "fowresult", SampointClampDesc },
	{ SHADER_TYPE_PIXEL, "lightMapfow", SamLinearClampDesc },
	{ SHADER_TYPE_PIXEL, "lightMaprt", SamLinearClampDesc }
};
ShaderResourceVariableDesc Vars[] =
{
	//{SHADER_TYPE_COMPUTE, "queueFB", SHADER_RESOURCE_VARIABLE_TYPE_STATIC},
	{ SHADER_TYPE_COMPUTE, "Constants", SHADER_RESOURCE_VARIABLE_TYPE_STATIC },
	{ SHADER_TYPE_COMPUTE, "ConstantsL", SHADER_RESOURCE_VARIABLE_TYPE_STATIC },
	{ SHADER_TYPE_COMPUTE, "lightMap", SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC },
	{ SHADER_TYPE_COMPUTE, "clearlightMap", SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC },
	{ SHADER_TYPE_VERTEX, "Cbuf", SHADER_RESOURCE_VARIABLE_TYPE_STATIC },
	{ SHADER_TYPE_COMPUTE, "instDatas", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE },
	{ SHADER_TYPE_COMPUTE, "cbSelect", SHADER_RESOURCE_VARIABLE_TYPE_STATIC },
	{ SHADER_TYPE_COMPUTE, "sele_filter", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE },
	{ SHADER_TYPE_COMPUTE, "_tinfo", SHADER_RESOURCE_VARIABLE_TYPE_STATIC },
	{ SHADER_TYPE_COMPUTE, "cbCameraAttribs", SHADER_RESOURCE_VARIABLE_TYPE_STATIC },
	{ SHADER_TYPE_COMPUTE, "instable", SHADER_RESOURCE_VARIABLE_TYPE_STATIC },
	{ SHADER_TYPE_COMPUTE, "sele_ov", SHADER_RESOURCE_VARIABLE_TYPE_STATIC },
	{ SHADER_TYPE_COMPUTE, "in_sele_out", SHADER_RESOURCE_VARIABLE_TYPE_STATIC },
	{ SHADER_TYPE_COMPUTE, "GroupIdMap", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE },
	{ SHADER_TYPE_COMPUTE, "Nebbuf", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE },
	{ SHADER_TYPE_COMPUTE, "Hash2Dtab", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE },
	{ SHADER_TYPE_COMPUTE, "Hash2DtabSB", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE },
};
void CCPSOUS(const char* EntryPoint, const char* FilePath, const char* Name,
	RefCntAutoPtr<IRenderDevice> m_pDevice,
	RefCntAutoPtr<IShaderSourceInputStreamFactory> pShaderSourceFactory, 
	RefCntAutoPtr<IPipelineState>& p_pso,
	Utility::span<Diligent::ShaderResourceVariableDesc> Vars,
	const Utility::span<ImmutableSamplerDesc> StaticSamplers,
	int m_ThreadGroupSize=64) {
	using namespace Diligent;
	Diligent::ComputePipelineStateCreateInfo PSOCreateInfo;
	ShaderCreateInfo ShaderCI;
	// Tell the system that the shader source code is in HLSL.
	// For OpenGL, the engine will convert this into GLSL under the hood.
	ShaderCI.SourceLanguage = SHADER_SOURCE_LANGUAGE_HLSL;
	//ShaderCI.ShaderCompiler = SHADER_COMPILER_DXC;
	Diligent::ShaderMacroHelper Macros;
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

	PSODesc.ResourceLayout.ImmutableSamplers = StaticSamplers.data();
	PSODesc.ResourceLayout.NumImmutableSamplers = StaticSamplers.size();

	PSODesc.Name = Name;// Utility::append(Name, "_PSO");
	PSOCreateInfo.pCS = pCS;
	m_pDevice->CreateComputePipelineState(PSOCreateInfo, &p_pso);
}

namespace pj2 {
	RenderUtil::PSB<2> Uscpsb;
}
template<class T>
using array4 = std::array<T, 4>;
using arytx = std::array<Diligent::ITextureView*, 4>;
template<>
int pj2::n_Render::fogRender::Cmd
<0>
(arytx& tview, array4<IBufferView*>& bfs, IBuffer*&cb) {
	tview[1] = m_ShadowMapSRV[0];
	tview[0] = TEXSRVs[texadss::GroupMap];
	bfs[1] = NULL;// VBFSRVs[texadss::GroupMap]; //tex atomic works ok no need to emulate
	cb = m_brightcb;
	bfs[0] = m_brightObjs->GetDefaultView(BUFFER_VIEW_SHADER_RESOURCE);

	return 0;
}
template<>
int pj2::n_Render::fogRender::Cmd
<1>
() {
	auto& rc = _rt;
	auto& m_pDevice = rc.m_pDevice;
	auto& m_pImmediateContext = rc.m_pImmediateContext;
	auto& pShaderSourceFactory = rc.pShaderSourceFactory;
	auto& layoutsdesc = geomatrys::Laydescs::defDesc;
	RefCntAutoPtr<IPipelineState> pnpp ;
	ImtblSamplers[1].ShaderStages = ImtblSamplers[2].ShaderStages;
	RenderUtil::CreatePipelineStates
	(&pnpp, m_pDevice, RenderTargetFormat, DepthBufferFormat(), pShaderSourceFactory,
		"fov.hlsl", "fowv", "fowf", Varspixel, layoutsdesc, ImtblSamplers, Diligent::PRIMITIVE_TOPOLOGY::PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
		false, false);

	ShaderMacroHelper sm;
	RefCntAutoPtr<IPipelineState> uspso,gfx,gfy;

	ImtblSamplers[1].ShaderStages = SHADER_TYPE_COMPUTE;
	CCPSOUS("usc", "usc.hlsl", "usc", m_pDevice,
		pShaderSourceFactory, uspso, Vars, ImtblSamplers);

	CCPSOUS("gfx", "usc.hlsl", "gfx", m_pDevice,
		pShaderSourceFactory, gfx, Vars, ImtblSamplers);
	CCPSOUS("gfy", "usc.hlsl", "gfy", m_pDevice,
		pShaderSourceFactory, gfy, Vars, ImtblSamplers);
	if (!pnpp||!uspso) {
		return 0;
	}
	Uscpsb.PSO= uspso;
	m_RfowPSO=pnpp;
	m_RfowPSO->GetStaticVariableByName(SHADER_TYPE_VERTEX, "Cbuf")->Set(m_CamConstants);
	if(auto cbp=m_RfowPSO->GetStaticVariableByName(SHADER_TYPE_PIXEL, "Cbuf"))
		cbp->Set(m_CamConstants);
	if (auto cbp = Uscpsb.PSO->GetStaticVariableByName(SHADER_TYPE_COMPUTE, "Cbuf"))
		cbp->Set(m_CamConstants);
	for (size_t i = 0; i < 2; i++)
	{
		Uscpsb.SRBs[i] = NULL;
	}
	Uscpsb.CreateShaderResourceBinding();
	for (size_t i = 0; i < m_ShadowMapSRV.size(); i++)
	{
		m_fowSRB[i] = NULL;
		auto& ussrbi = Uscpsb.SRBs[i];
		m_RfowPSO->CreateShaderResourceBinding(&m_fowSRB[i], true);
		if (auto prsis = m_fowSRB[i]->GetVariableByName(SHADER_TYPE_PIXEL, "resistanceMapR"))
			prsis->Set(m_resistanceSRV);
		if (auto prsis = ussrbi->GetVariableByName(SHADER_TYPE_COMPUTE, "lightMapfow")){
			prsis->Set(m_ShadowMapSRV[i]);
			ussrbi->GetVariableByName(SHADER_TYPE_COMPUTE, "lightMapfow");
		}
		if (auto prsis = ussrbi->GetVariableByName(SHADER_TYPE_COMPUTE, "usMap"))
			prsis->Set(TEXUAVs[texadss::blenderfog]);
		m_fowSRB[i]->GetVariableByName(SHADER_TYPE_PIXEL, "lightMapfow")->Set(TEXSRVs[texadss::blenderfog]);// (m_ShadowMapSRV[i]);
	}
	return 1;
}
void pj2::n_Render::fogRender::init(RenderContext& rc, ITextureView* tview,
	Utility::span<sStruct::VFoutArgs> VfArgspan, size_t Maxinstancnum)
{
	RefCntAutoPtr<IBuffer>                m_Shadowqueue;
	RefCntAutoPtr<IBuffer>                m_RWSign;
	RefCntAutoPtr<IBufferView> pRWSignUAV;
	RefCntAutoPtr<IBufferView> pRWBrightUAV;
	maxlit = MaxSOLnum;
	std::vector<math::vec4<uint>>     TexData(shadowmapsz * shadowmapsz, math::vec4<uint>(0, 0, 0, 0));
	
	this->_rt = rc;
	auto& m_pDevice = rc.m_pDevice;
	auto& m_pImmediateContext = rc.m_pImmediateContext;
	auto& pSwapChain = rc.m_pSwapChain;
	this->m_pSwapChain = pSwapChain;
	this->m_pImmediateContext = m_pImmediateContext;
	auto& pShaderSourceFactory = rc.pShaderSourceFactory;
	auto shaderpath = loader::Get(shader);
	parasele.inum = 0;
	auto& cfg_ = GloProp::Get();
	const auto maxtypenum = cfg_.MaxTypenum;
	constexpr int Hash2DCAP = 12288 * 2;
	o_Cull.resize(cfg_.maxcullnum);
	Visible_f::init(_rt, 0,cfg_.maxcullnum,cfg_.MaxTypenum);//init base first
	RefCntAutoPtr<IBuffer>                ConstantsL;
	{

		BufferData VBData;

		plane.init(m_pDevice, Diligent::float3(shadowmapsz, shadowmapsz, shadowmapsz));
		plane.TransitionResource(m_pImmediateContext);
		BufferDesc BuffDesc;

		BuffDesc.Name = "m_brightObjs CB";
		BuffDesc.Size = 128;// sizeof(sStruct::GlobalConstants);// 24byte round to 32 byte
		BuffDesc.Usage = USAGE_DYNAMIC;
		BuffDesc.BindFlags = BIND_UNIFORM_BUFFER;
		BuffDesc.CPUAccessFlags = CPU_ACCESS_WRITE;
		m_pDevice->CreateBuffer(BuffDesc, nullptr, &m_brightcb);

		m_CamConstants = rc.pCameraAttribs;

		BuffDesc.Name = "m_brightObjs";
		BuffDesc.ElementByteStride = sizeof(sStruct::objrep);
		BuffDesc.Size = static_cast<Diligent::Uint64>(MaxSOLnum) * BuffDesc.ElementByteStride;
		BuffDesc.Usage = USAGE_DEFAULT;
		BuffDesc.Mode = BUFFER_MODE_STRUCTURED;
		BuffDesc.BindFlags = BIND_UNORDERED_ACCESS | BIND_SHADER_RESOURCE;;
		BuffDesc.CPUAccessFlags = CPU_ACCESS_NONE;
		m_pDevice->CreateBuffer(BuffDesc, nullptr, &m_brightObjs);

		BuffDesc.Usage = USAGE_STAGING;
		BuffDesc.Mode = BUFFER_MODE_STRUCTURED;
		BuffDesc.BindFlags = BIND_NONE;
		BuffDesc.CPUAccessFlags = CPU_ACCESS_READ;
		m_pDevice->CreateBuffer(BuffDesc, nullptr, &m_LitR);
		BuffDesc.ElementByteStride = sizeof(vdt);
		BuffDesc.Size = 3000 * BuffDesc.ElementByteStride;
		m_pDevice->CreateBuffer(BuffDesc, nullptr, &m_Debugbuf);

		BuffDesc.Name = "Cast Light IndirectAttribs";
		BuffDesc.ElementByteStride = sizeof(int) * 4;
		BuffDesc.Size = 1 * BuffDesc.ElementByteStride;
		BuffDesc.Usage = USAGE_DEFAULT;
		BuffDesc.Mode = BUFFER_MODE_STRUCTURED;
		BuffDesc.BindFlags = BIND_UNORDERED_ACCESS | BIND_INDIRECT_DRAW_ARGS;;
		BuffDesc.CPUAccessFlags = CPU_ACCESS_NONE;
		m_pDevice->CreateBuffer(BuffDesc, nullptr, &_DisptchLit);
		CLitAttribs.pAttribsBuffer = _DisptchLit;
		CLitAttribs.AttribsBufferStateTransitionMode = RESOURCE_STATE_TRANSITION_MODE_TRANSITION;

		BuffDesc.Name = "m_moveProp DF";
		//BuffDesc.Size = deltaChunks * BuffDesc.ElementByteStride;
		BuffDesc.Usage = USAGE_DEFAULT;
		BuffDesc.BindFlags = BIND_UNORDERED_ACCESS;
		BuffDesc.CPUAccessFlags = CPU_ACCESS_NONE;
		m_pDevice->CreateBuffer(BuffDesc, nullptr, &m_deltaObjsL);

		BuffDesc.Name = "m_moveProp";
		//BuffDesc.ElementByteStride = sizeof(sStruct::objrep);
		BuffDesc.Size = deltaChunks;//around 1000 element
		BuffDesc.Usage = USAGE_DYNAMIC;
		BuffDesc.BindFlags = BIND_SHADER_RESOURCE;
		BuffDesc.CPUAccessFlags = CPU_ACCESS_WRITE;
		m_pDevice->CreateBuffer(BuffDesc, nullptr, &m_deltaObjs);

		BuffDesc.Name = "ConstantsL CB";
		BuffDesc.ElementByteStride = 256;
		BuffDesc.Size = 256;
		BuffDesc.Usage = USAGE_DEFAULT;
		BuffDesc.BindFlags = BIND_UNORDERED_ACCESS | BIND_UNIFORM_BUFFER;
		BuffDesc.CPUAccessFlags = CPU_ACCESS_NONE;
		m_pDevice->CreateBuffer(BuffDesc, nullptr, &ConstantsL);
		//BuffDesc;
		BuffDesc.Name = "Shadowqueue";
		BuffDesc.Size = QueueSize * sizeof(sStruct::CastArg);
		BuffDesc.ElementByteStride = sizeof(sStruct::CastArg);
		BuffDesc.Mode = BUFFER_MODE_STRUCTURED;
		BuffDesc.Usage = USAGE_DEFAULT;
		BuffDesc.BindFlags = BIND_UNORDERED_ACCESS | BIND_SHADER_RESOURCE;
		BuffDesc.CPUAccessFlags = CPU_ACCESS_NONE;
		m_pDevice->CreateBuffer(BuffDesc, nullptr, &m_Shadowqueue);
		{
			VfArgREle = cfg_.MaxTypenum;
			BuffDesc.Name = "VfArg";
			BuffDesc.ElementByteStride = sizeof(sStruct::VFoutArgs);
			BuffDesc.Size = BuffDesc.ElementByteStride * VfArgREle;
			//BuffDesc.BindFlags = ;
			//BuffDesc.Mode = BUFFER_MODE_STRUCTURED;
			BuffDesc.Usage = USAGE_DEFAULT;
			BuffDesc.BindFlags = BIND_UNORDERED_ACCESS | BIND_INDIRECT_DRAW_ARGS;
			BuffDesc.CPUAccessFlags = CPU_ACCESS_NONE;
			VBData.pData = VfArgspan.data();
			VBData.DataSize = VfArgspan.size_bytes();
			m_pDevice->CreateBuffer(BuffDesc, &VBData, &m_DrawArglocal);

			std::vector<StateTransitionDesc> Barriers;
#if 1
			BuffDesc.Name = "VfArgR";
			BuffDesc.ElementByteStride = sizeof(sStruct::VFoutArgs);;
			//BuffDesc.Size = BuffDesc.ElementByteStride * VfArgREle;
			//BuffDesc.BindFlags = ;
			//BuffDesc.Mode = BUFFER_MODE_STRUCTURED;
			BuffDesc.Usage = USAGE_STAGING;
			BuffDesc.BindFlags = BIND_NONE;
			BuffDesc.CPUAccessFlags = CPU_ACCESS_READ;
			m_pDevice->CreateBuffer(BuffDesc, NULL, &m_VFArgR);
			Barriers.emplace_back(m_VFArgR, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_COPY_DEST, STATE_TRANSITION_FLAG_UPDATE_STATE);
#endif
		}

		/*const auto cbv = [&](const int Stride, RefCntAutoPtr<IBuffer>& tbuf) {
		BufferViewDesc ViewDesc;
		for (size_t i1 = 0, elesum=0; i1 < bufinfo.size(); i1++)
		{
		const auto& maxele = bufinfo[i1].count;
		BufferViewDesc ViewDesc;
		ViewDesc.ViewType = BUFFER_VIEW_SHADER_RESOURCE;
		ViewDesc.ByteOffset = elesum * Stride;
		ViewDesc.ByteWidth = maxele * Stride;
		elesum += maxele;
		IBufferView* bv=NULL;
		tbuf->CreateView(ViewDesc, &bv);
		_bfviews.emplace_back(bv);
		}
		};
		cbv(sizeof(sStruct::VfProp), m_vfProps);
		cbv(sizeof(sStruct::mtransfrom), m_vfTrans);*/

		BuffDesc.Name = "so_bufferinfo";
		BuffDesc.ElementByteStride = sizeof(sStruct::bufferinfo);
		BuffDesc.Size = maxtypenum * BuffDesc.ElementByteStride;
		BuffDesc.Usage = USAGE_DEFAULT;
		BuffDesc.Mode = BUFFER_MODE_STRUCTURED;
		BuffDesc.BindFlags = BIND_UNORDERED_ACCESS | BIND_SHADER_RESOURCE;;
		BuffDesc.CPUAccessFlags = CPU_ACCESS_NONE;
		//m_pDevice->CreateBuffer(BuffDesc, nullptr, &m_soBFinfo);

		/*BuffDesc.Name = "co_bufferinfo";
		BuffDesc.ElementByteStride = sizeof(sStruct::bufferinfo);
		BuffDesc.Size = maxtypenum * BuffDesc.ElementByteStride;
		BuffDesc.Usage = USAGE_DEFAULT;
		BuffDesc.Mode = BUFFER_MODE_STRUCTURED;
		BuffDesc.BindFlags = BIND_UNORDERED_ACCESS | BIND_SHADER_RESOURCE;;
		BuffDesc.CPUAccessFlags = CPU_ACCESS_NONE;
		m_pDevice->CreateBuffer(BuffDesc, nullptr, &_rdBFinfo);*/

		BuffDesc.Name = "RWSign";
		BuffDesc.Size = (QueueSize / 32) * 4 + 256;
		BuffDesc.Usage = USAGE_DEFAULT;
		BuffDesc.Mode = BUFFER_MODE_FORMATTED;
		BuffDesc.ElementByteStride = 4;
		BuffDesc.BindFlags = BIND_UNORDERED_ACCESS;
		BuffDesc.CPUAccessFlags = CPU_ACCESS_NONE;
		std::vector<UINT32> signs(BuffDesc.Size * 4, 0);
		VBData.pData = signs.data();
		VBData.DataSize = sizeof(UINT32) * static_cast<Uint32>(signs.size());
		m_pDevice->CreateBuffer(BuffDesc, &VBData, &m_RWSign);
		BufferViewDesc ViewDesc;
		ViewDesc.ViewType = BUFFER_VIEW_UNORDERED_ACCESS;
		ViewDesc.Format.ValueType = VT_UINT32;
		ViewDesc.Format.NumComponents = 1;
		m_RWSign->CreateView(ViewDesc, &pRWSignUAV);

		/*BuffDesc.Size = shadowmapsz * shadowmapsz;
		BuffDesc.Name = "GroupIdMapbuffer";
		BuffDesc.ElementByteStride = 16;
		BuffDesc.Mode = BUFFER_MODE_STRUCTURED;
		BuffDesc.BindFlags = BIND_UNORDERED_ACCESS| BIND_SHADER_RESOURCE;
		BuffDesc.CPUAccessFlags = CPU_ACCESS_NONE;
		m_pDevice->CreateBuffer(BuffDesc, NULL, &m_GidBuf);
		VBFSRVs[texadss::GroupMap] = m_GidBuf->GetDefaultView(BUFFER_VIEW_SHADER_RESOURCE);
		VBFUAVs[texadss::GroupMap] = m_GidBuf->GetDefaultView(BUFFER_VIEW_UNORDERED_ACCESS);*/


		RefCntAutoPtr<ITexture> Tex2d;
		TextureDesc TexDesc;
		TexDesc.Type = RESOURCE_DIM_TEX_2D;
		TexDesc.Usage = Diligent::USAGE::USAGE_DEFAULT;
		//TexDesc.Usage = Diligent::USAGE::USAGE_DYNAMIC;
		TexDesc.BindFlags = BIND_SHADER_RESOURCE | BIND_UNORDERED_ACCESS;
		TexDesc.Width = shadowmapsz;
		TexDesc.Height = shadowmapsz;
		TexDesc.Format = TEX_FORMAT_RGBA8_UNORM;
		TexDesc.MipLevels = 1;
		//TextureDesc TexDesc(RESOURCE_DIM_TEX_2D, shadowmapsz, shadowmapsz, 1, TEX_FORMAT_RGBA32_UINT, USAGE_DEFAULT, BIND_SHADER_RESOURCE);
		TexDesc.Name = "shadow texture1";
		m_pDevice->CreateTexture(TexDesc, NULL, &Tex2d);
		m_ShadowMapSRV[0] = Tex2d->GetDefaultView(TEXTURE_VIEW_SHADER_RESOURCE);
		m_ShadowMapUAV[0] = Tex2d->GetDefaultView(TEXTURE_VIEW_UNORDERED_ACCESS);
		TexDesc.Name = "shadow texture2";
		Tex2d = NULL;
		m_pDevice->CreateTexture(TexDesc, NULL, &Tex2d);
		m_ShadowMapSRV[1] = Tex2d->GetDefaultView(TEXTURE_VIEW_SHADER_RESOURCE);
		m_ShadowMapUAV[1] = Tex2d->GetDefaultView(TEXTURE_VIEW_UNORDERED_ACCESS);


		TexDesc.Name = "shadow upscale texture2";
		Tex2d = NULL;
		TexDesc.Width = shadowmapsz*4;
		TexDesc.Height = TexDesc.Width;
		TexDesc.Format = TEX_FORMAT_R8_UNORM;
		TexDesc.BindFlags = BIND_SHADER_RESOURCE | BIND_UNORDERED_ACCESS;
		m_pDevice->CreateTexture(TexDesc, NULL, &Tex2d);
		TEXSRVs[texadss::blenderfog] = Tex2d->GetDefaultView(TEXTURE_VIEW_SHADER_RESOURCE);
		TEXUAVs[texadss::blenderfog] = Tex2d->GetDefaultView(TEXTURE_VIEW_UNORDERED_ACCESS);

		TexDesc.Name = "GroupId texture";
		TexDesc.Format = TEX_FORMAT_R32_UINT;
		TexDesc.Width = shadowmapsz;
		TexDesc.Height = shadowmapsz;
		Tex2d = NULL;
		std::memset(TexData.data(), 1, Utility::VecByteSize(TexData));
		TextureSubResData       Level0Data{ TexData.data(), TexDesc.Width * 16 };
		TextureData             InitData{ &Level0Data, 1 };
		m_pDevice->CreateTexture(TexDesc, &InitData, &Tex2d);
		TEXSRVs[texadss::GroupMap] = Tex2d->GetDefaultView(TEXTURE_VIEW_SHADER_RESOURCE);
		TEXUAVs[texadss::GroupMap] = Tex2d->GetDefaultView(TEXTURE_VIEW_UNORDERED_ACCESS);


		TexDesc.Name = "resistance texture";
		Tex2d = NULL;
		m_pDevice->CreateTexture(TexDesc, &InitData, &Tex2d);
		m_resistanceSRV = Tex2d->GetDefaultView(TEXTURE_VIEW_SHADER_RESOURCE);
		m_resistanceUAV = Tex2d->GetDefaultView(TEXTURE_VIEW_UNORDERED_ACCESS);

		TextureDesc RTColorDesc;
		RTColorDesc.Name = "Offscreen render target";
		RTColorDesc.Type = RESOURCE_DIM_TEX_2D;
		RTColorDesc.Width = m_pSwapChain->GetDesc().Width;
		RTColorDesc.Height = m_pSwapChain->GetDesc().Height;
		RTColorDesc.MipLevels = 1;
		RTColorDesc.Format = RenderTargetFormat;//m_pSwapChain->GetDesc().ColorBufferFormat;

		// The render target can be bound as a shader resource and as a render target
		RTColorDesc.BindFlags = BIND_SHADER_RESOURCE | BIND_RENDER_TARGET;

		RTColorDesc.ClearValue.Format = RTColorDesc.Format;
		RTColorDesc.ClearValue.Color[0] = 0.350f;
		RTColorDesc.ClearValue.Color[1] = 0.350f;
		RTColorDesc.ClearValue.Color[2] = 0.350f;
		RTColorDesc.ClearValue.Color[3] = 1.f;
		m_RendeRT.Release();
		m_pDevice->CreateTexture(RTColorDesc, nullptr, &pRTColor);
		m_pDevice->CreateTexture(RTColorDesc, nullptr, &pRTColorfow);
		std::vector<StateTransitionDesc> decc_;
		
		{
			using ts = sStruct::SPHKV;
			auto&& buf = _Hash2Dtab;
			BuffDesc.Mode = BUFFER_MODE_STRUCTURED;
			RenderUtil::md_BufferDesc("_hash2dbuffer", BuffDesc, Hash2DCAP, sizeof(ts), USAGE_DEFAULT, BIND_SHADER_RESOURCE | BIND_UNORDERED_ACCESS, CPU_ACCESS_NONE);
			m_pDevice->CreateBuffer(BuffDesc, NULL, &_Hash2Dtab);
			decc_.emplace_back(buf, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_CONSTANT_BUFFER, STATE_TRANSITION_FLAG_UPDATE_STATE);
		}
		{
			using ts = sStruct::Selects;
			auto&& buf = _Selecbuf;
			BuffDesc.Mode = BUFFER_MODE_STRUCTURED;
			RenderUtil::md_BufferDesc("cbSelect", BuffDesc, 1, sizeof(ts), USAGE_DEFAULT, BIND_UNIFORM_BUFFER, CPU_ACCESS_NONE);
			rc.m_pDevice->CreateBuffer(BuffDesc, nullptr, &buf);
			decc_.emplace_back(_Selecbuf, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_CONSTANT_BUFFER, STATE_TRANSITION_FLAG_UPDATE_STATE);
		}
		//rc.m_pImmediateContext->TransitionResourceStates(decc_.size(), decc_.data());
		/*
		decc_.reserve(32);
		{
			using ts = sStruct::tinfo;
			auto&& buf = _culltinfo;
			RenderUtil::BuildBDesc<ts>(BuffDesc, maxtinfo, "_culltinfo",
				Diligent::BUFFER_MODE_STRUCTURED,
				Diligent::USAGE_DEFAULT, Diligent::BIND_SHADER_RESOURCE, Diligent::CPU_ACCESS_NONE);
			buf.reserve(rc, BuffDesc);
			decc_.emplace_back(buf, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_SHADER_RESOURCE, STATE_TRANSITION_FLAG_UPDATE_STATE);
		}
		{
			using ts = sStruct::bufferinfo;
			auto&& buf = _instable;
			RenderUtil::BuildBDesc<ts>(BuffDesc, maxtinfo, "instable",
				Diligent::BUFFER_MODE_STRUCTURED,
				Diligent::USAGE_DEFAULT, Diligent::BIND_SHADER_RESOURCE, Diligent::CPU_ACCESS_NONE);
			buf.reserve(rc, BuffDesc);
			decc_.emplace_back(buf, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_GENERIC_READ, STATE_TRANSITION_FLAG_UPDATE_STATE);
		}
		using t_sinout = sStruct::seleout;
		int maxcullresult = cfg_.maxcullnum;
		{
			using ts = t_sinout;
			auto&& buf = Visibleout;
			std::vector<ts> initdata(maxcullresult);
			const auto& dsize = Utility::VecByteSize(initdata);
			std::memset(initdata.data(), 0, dsize);
			RenderUtil::BuildBDesc<ts>(BuffDesc, maxcullresult, "Visibleout",
				Diligent::BUFFER_MODE_STRUCTURED,
				Diligent::USAGE_DEFAULT, BIND_UNORDERED_ACCESS | BIND_SHADER_RESOURCE, Diligent::CPU_ACCESS_NONE);
			buf.reserve(rc, BuffDesc);
			m_pImmediateContext->UpdateBuffer(buf, 0, dsize, initdata.data(), RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
			decc_.emplace_back(buf, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_UNORDERED_ACCESS, STATE_TRANSITION_FLAG_UPDATE_STATE);
		}
		{
			using ts = t_sinout;
			auto&& buf = seleout1;
			std::vector<ts> initdata(maxcullresult);
			const auto& dsize = Utility::VecByteSize(initdata);
			std::memset(initdata.data(), 0, dsize);
			RenderUtil::BuildBDesc<ts>(BuffDesc, maxcullresult, "seleout1",
				Diligent::BUFFER_MODE_STRUCTURED,
				Diligent::USAGE_DEFAULT, BIND_UNORDERED_ACCESS | BIND_SHADER_RESOURCE, Diligent::CPU_ACCESS_NONE);
			buf.reserve(rc, BuffDesc);
			m_pImmediateContext->UpdateBuffer(buf, 0, dsize, initdata.data(), RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
			decc_.emplace_back(buf, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_UNORDERED_ACCESS, STATE_TRANSITION_FLAG_UPDATE_STATE);
		}
		RefCntAutoPtr<IRenderDevice>& m_pDevice = rc.m_pDevice;
		FenceDesc fenceDesc;
		fenceDesc.Name = "select buf cpy fence";
		m_pDevice->CreateFence(fenceDesc, &_cpyfen);
		rc.m_pImmediateContext->TransitionResourceStates(decc_.size(), decc_.data());*/
	}

		{
		ShaderMacroHelper sm;
		sm.AddShaderMacro("FiltAudio", 1);
		sm.AddShaderMacro("TSELECT", 1);
		RenderUtil::CCPSO("sCull", "cull.hlsl", "sCull", m_pDevice,
			pShaderSourceFactory, Vfilterpsb.PSO, Vars, sm);
		//m_RfowPSO->Release();
		auto& layoutsdesc = geomatrys::Laydescs::defDesc;
		RenderUtil::CreatePipelineStates
		(&m_pRTPSO, m_pDevice, m_pSwapChain->GetDesc().ColorBufferFormat, DepthBufferFormat(), pShaderSourceFactory,
			"fov.hlsl", "fowv1", "fowf1", Varspixel, layoutsdesc, ImtblSamplers, Diligent::PRIMITIVE_TOPOLOGY::PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP,
			false, false);
		/*CreateRenderTargetPSO(&m_pRTPSO, m_pDevice, m_pSwapChain, pShaderSourceFactory,
			m_pSwapChain->GetDesc().ColorBufferFormat, DepthBufferFormat(),"fov.hlsl", "fowv1", "fowf1");*/
			//CreateRenderTargetPSO(&m_RfowPSO, m_pDevice, m_pSwapChain, pShaderSourceFactory, "fov.hlsl", "fowv", "fowf");
			/*const auto RenderTargetFormat = m_pSwapChain->GetDesc().ColorBufferFormat;
			const auto DepthBufferFormat = m_pSwapChain->GetDesc().DepthBufferFormat;*/
			//auto& DepthBufferFormat = DepthBufferFormat();
		Cmd<1>();
		/*GIns::CreatePipelineStates(&m_RfowPSO, m_pDevice, m_pSwapChain,
		pShaderSourceFactory, "fov.hlsl", "fowv", "fowf", Varspixel, "fowv", "fowf", GIns::defDesc, ImtblSamplers);*/
		//PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP,false,false);

		sm.AddShaderMacro("Hash2DCAP", Hash2DCAP);
		DrawFunc::CreateComputePSO("reset", "ShadowCast.hlsl", "reset", m_pDevice, pShaderSourceFactory, m_pResetPSO, Vars, -1, sm);
		DrawFunc::CreateComputePSO("move", "ShadowCast.hlsl", "move", m_pDevice, pShaderSourceFactory, m_pMovePSO, Vars);
		DrawFunc::CreateComputePSO("WriteGMap", "ShadowCast.hlsl", "WriteGMap", m_pDevice, pShaderSourceFactory, WriteGMap_PSB.PSO, Vars, -1, sm);
		int casttgs = 8;
		const char* tgsname = "THREAD_GROUP_SIZE";
		sm.AddShaderMacro(tgsname, casttgs);
		sm.Finalize();
		DrawFunc::CreateComputePSO("fov", "CastMain.hlsl", "fov", m_pDevice, pShaderSourceFactory, m_CSLitPSO, Vars, -1, sm);
		sm.Clear();
		//Vars[0] = { SHADER_TYPE_COMPUTE, "clearlightMap", SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC };

		}

	ShaderResourceVariableDesc VarsCull[] =
	{
		//{SHADER_TYPE_COMPUTE, "queueFB", SHADER_RESOURCE_VARIABLE_TYPE_STATIC},
		{ SHADER_TYPE_COMPUTE, "_tinfo", SHADER_RESOURCE_VARIABLE_TYPE_STATIC },
		{ SHADER_TYPE_COMPUTE, "cbCameraAttribs", SHADER_RESOURCE_VARIABLE_TYPE_STATIC },
		{ SHADER_TYPE_COMPUTE, "instable", SHADER_RESOURCE_VARIABLE_TYPE_STATIC },
		{ SHADER_TYPE_COMPUTE, "in_sele_out", SHADER_RESOURCE_VARIABLE_TYPE_STATIC },
		{ SHADER_TYPE_COMPUTE, "sele_ov", SHADER_RESOURCE_VARIABLE_TYPE_STATIC },
		{ SHADER_TYPE_COMPUTE, "cbSelect", SHADER_RESOURCE_VARIABLE_TYPE_STATIC },
	};

	IBuffer* vbs[] = {
		_culltinfo,
		rc.pCameraAttribs,
		_instable,
		seleout,
		_SeleVout,
		_cbSelect,
	};

	Diligent::BUFFER_VIEW_TYPE bvs[] = {
		BUFFER_VIEW_SHADER_RESOURCE,
		BUFFER_VIEW_UNDEFINED,
		BUFFER_VIEW_SHADER_RESOURCE,
		BUFFER_VIEW_UNORDERED_ACCESS,
		BUFFER_VIEW_UNORDERED_ACCESS,
		BUFFER_VIEW_UNDEFINED,
	};
	RenderUtil::setVbuffer(NULL, Vfilterpsb.PSO, VarsCull, bvs, vbs);

	m_CSLitPSO->GetStaticVariableByName(SHADER_TYPE_COMPUTE, "Constants")->Set(m_brightcb);
	m_pResetPSO->GetStaticVariableByName(SHADER_TYPE_COMPUTE, "Constants")->Set(m_brightcb);

	Vfilterpsb.PSO->GetStaticVariableByName(SHADER_TYPE_COMPUTE, "Constants")->Set(m_brightcb);
	WriteGMap_PSB.PSO->GetStaticVariableByName(SHADER_TYPE_COMPUTE, "Constants")->Set(m_brightcb);
	/*Vfilterpsb.PSO->GetStaticVariableByName(SHADER_TYPE_COMPUTE, "so_bufferinfo")->Set(m_soBFinfo);
	Vfilterpsb.PSO->GetStaticVariableByName(SHADER_TYPE_COMPUTE, "co_bufferinfo")->Set(_rdBFinfo);*/
	//m_ClightPSO->GetStaticVariableByName(SHADER_TYPE_COMPUTE, "queueFB")->Set(m_brightcbl);
	m_pMovePSO->GetStaticVariableByName(SHADER_TYPE_COMPUTE, "Constants")->Set(m_brightcb);

	m_pRTPSO->GetStaticVariableByName(SHADER_TYPE_VERTEX, "cScreenRect")->Set(_Selecbuf);

	m_pMoveSRB.Release();
	m_plightSRB.Release();
	m_ClearSRV.Release();
	m_CSLitPSO->CreateShaderResourceBinding(&m_plightSRB, true);
	m_pMovePSO->CreateShaderResourceBinding(&m_pMoveSRB, true);
	m_pResetPSO->CreateShaderResourceBinding(&m_pResetSRB, true);
	m_pResetPSO->CreateShaderResourceBinding(&m_pReset1SRB, true);
	//m_pResetArgPSO->CreateShaderResourceBinding(&m_pResetArgSRB, true);
	m_pRTPSO->CreateShaderResourceBinding(&m_pRTSRB, true);
	Vfilterpsb.CreateShaderResourceBinding();
	WriteGMap_PSB.CreateShaderResourceBinding();
	{
		IShaderResourceBinding* svsrb[] = {
			WriteGMap_PSB.GetShaderResourceBinding(),//write hash
			m_plightSRB,//read hash
			m_pResetSRB//clear hash
		};
		Diligent::BUFFER_VIEW_TYPE bvs[] = {
		   BUFFER_VIEW_UNORDERED_ACCESS,
		   BUFFER_VIEW_SHADER_RESOURCE,
		   BUFFER_VIEW_UNORDERED_ACCESS,
		};
		auto vuav = TEXUAVs;
		auto vsrv = TEXSRVs;
		for (size_t i = 0; i < _countof(svsrb); i++)
		{
			if(i!=1)
			svsrb[i]->GetVariableByName(SHADER_TYPE_COMPUTE, "GroupIdMap")
				->Set(bvs[i] == BUFFER_VIEW_UNORDERED_ACCESS ? vuav[texadss::GroupMap] : vsrv	[texadss::GroupMap]);
			auto ptr = svsrb[i]->GetVariableByName(SHADER_TYPE_COMPUTE, i == 1 ? "Hash2DtabSB" : "Hash2Dtab");
			if (ptr == NULL && i == 1)
				continue;
			ptr->Set(_Hash2Dtab->GetDefaultView(bvs[i]));

		}
	}

	WriteGMap_PSB.GetShaderResourceBinding()->GetVariableByName(SHADER_TYPE_COMPUTE, "s_objProps")->Set(_vbIns->GetDefaultView(BUFFER_VIEW_SHADER_RESOURCE));
	WriteGMap_PSB.GetShaderResourceBinding()->GetVariableByName(SHADER_TYPE_COMPUTE, "objPropRW")->Set(m_brightObjs->GetDefaultView(BUFFER_VIEW_UNORDERED_ACCESS));
	WriteGMap_PSB.GetShaderResourceBinding()->GetVariableByName(SHADER_TYPE_COMPUTE, "CastGxyz")->Set(_DisptchLit->GetDefaultView(BUFFER_VIEW_UNORDERED_ACCESS));
	WriteGMap_PSB.GetShaderResourceBinding()->GetVariableByName(SHADER_TYPE_COMPUTE, "s_objtrans")->Set(_vbtrans->GetDefaultView(BUFFER_VIEW_SHADER_RESOURCE));
	for (size_t i = 0; i < m_ShadowMapSRV.size(); i++)
	{
		auto& Vfiltersrb = Vfilterpsb.SRBs[i];
		if (auto audio_bufout = Vfiltersrb->GetVariableByName(SHADER_TYPE_COMPUTE, "audio_insout"))
			audio_bufout->Set(a_vfag->GetDefaultView(BUFFER_VIEW_UNORDERED_ACCESS));
		Vfiltersrb->GetVariableByName(SHADER_TYPE_COMPUTE, "lightMapSRV")->Set(m_ShadowMapSRV[i]);
		Vfiltersrb->GetVariableByName(SHADER_TYPE_COMPUTE, "s_objProps")->Set(_vbIns->GetDefaultView(BUFFER_VIEW_SHADER_RESOURCE));
		Vfiltersrb->GetVariableByName(SHADER_TYPE_COMPUTE, "s_objtrans")->Set(_vbtrans->GetDefaultView(BUFFER_VIEW_SHADER_RESOURCE));
		Vfiltersrb->GetVariableByName(SHADER_TYPE_COMPUTE, "Visibleout")->Set(Visibleout->GetDefaultView(BUFFER_VIEW_UNORDERED_ACCESS));
		Vfiltersrb->GetVariableByName(SHADER_TYPE_COMPUTE, "VfArg")->Set(m_DrawArglocal->GetDefaultView(BUFFER_VIEW_UNORDERED_ACCESS));
	}

	//m_plightSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "Nebbuf")->Set(Nebbuf->GetDefaultView(BUFFER_VIEW_UNORDERED_ACCESS));;
	m_pMoveSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "resistanceMapW")->Set(m_resistanceUAV);
	m_pMoveSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "moveProp")->Set(m_deltaObjs->GetDefaultView(BUFFER_VIEW_SHADER_RESOURCE));
	m_pMoveSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "movePropLocal")->Set(m_deltaObjsL->GetDefaultView(BUFFER_VIEW_UNORDERED_ACCESS));
	m_plightSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "resistanceMap")->Set(m_resistanceSRV);;
	//m_plightSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "queueFB")->Set(queueFB->GetDefaultView(BUFFER_VIEW_UNORDERED_ACCESS));
	m_plightSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "RWSign")->Set(pRWSignUAV);
	m_lightMapUAV = m_plightSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "lightMap");
	m_plightSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "Shadowqueue")->Set(m_Shadowqueue->GetDefaultView(BUFFER_VIEW_UNORDERED_ACCESS));
	m_plightSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "objProp")->Set(m_brightObjs->GetDefaultView(BUFFER_VIEW_UNORDERED_ACCESS));
	//m_plightSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "RWSign")->Set(pRWSignUAV);
	//m_pImmediateContext->buf
	//m_LSRV= m_pMoveSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "lightMap");
	m_ClearSRV = m_pResetSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "clearlightMap");
	m_pResetSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "Hash2Dtab")->Set(_Hash2Dtab->GetDefaultView(BUFFER_VIEW_UNORDERED_ACCESS));
	m_pResetSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "VfArg")->Set(m_DrawArglocal->GetDefaultView(BUFFER_VIEW_UNORDERED_ACCESS));
	m_pReset1SRB->GetVariableByName(SHADER_TYPE_COMPUTE, "VfArg")->Set(m_DrawArglocal->GetDefaultView(BUFFER_VIEW_UNORDERED_ACCESS));
	m_pReset1SRB->GetVariableByName(SHADER_TYPE_COMPUTE, "clearlightMap")->Set(m_resistanceUAV);

	m_pRTSRB->GetVariableByName(SHADER_TYPE_PIXEL, "fowresult")->Set(tview);
	m_TargetMapSRV = m_pRTSRB->GetVariableByName(SHADER_TYPE_PIXEL, "lightMaprt");
	m_TargetMapSRV->Set(pRTColorfow->GetDefaultView(TEXTURE_VIEW_SHADER_RESOURCE));
	//m_RendeRT= pRTColor->GetDefaultView(TEXTURE_VIEW_RENDER_TARGET);
	m_RendeRTf = pRTColorfow->GetDefaultView(TEXTURE_VIEW_RENDER_TARGET);

	auto* pRTV = m_pSwapChain->GetCurrentBackBufferRTV();
	gCons.mapsize = { shadowmapsz ,shadowmapsz };
	gCons.lightcolor = { 2 ,2,2,2 };
	RTDrawAttrs.NumVertices = 4;
	RTDrawAttrs.Flags = DRAW_FLAG_VERIFY_ALL; // Verify the state of vertex and index buffers

	m_Debugbufv = m_Shadowqueue;
}

void pj2::n_Render::fogRender::Cull(RefCntAutoPtr<IDeviceContext>& m_pImmediateContext, nonstd::span<sStruct::objrep>, int insTancenum, int) {
	if (m_CurrentFenceValue > 1) [[likely]]
		GetLastResult();
	constexpr int szsu = sizeof(Zero1Kbyte);
	if(0)
	{
		//Diligent::MapHelper<sStruct::objrep> litout(_rt.m_pImmediateContext, m_LitR, MAP_READ, MAP_FLAG_DO_NOT_WAIT);
		Diligent::MapHelper<vdt> dbgbuf(_rt.m_pImmediateContext, m_Debugbuf, MAP_READ, MAP_FLAG_DO_NOT_WAIT);
		math::int4 vtp = { 0,0,0,0 }, mi = vtp;
		mi.tail<2>().setConstant(MAXINT32);
		static int cnum = 64;
		for (size_t i = 0; i < cnum; i++)
		{
			const vdt& vt = dbgbuf[i];
			mi.head<2>() = vt.pos.head<2>().cwiseMax(mi.head<2>());
			mi.tail<2>() = vt.pos.head<2>().cwiseMin(mi.tail<2>());
			if (vt.se[0] == -100) {
				break;
			}
			vtp += vt.pos;
		}
	}
		//Diligent::MapHelper<sStruct::AroundTg> nebout(_rt.m_pImmediateContext, stagNebbuf, MAP_READ, MAP_FLAG_DO_NOT_WAIT);
		Diligent::MapHelper<sStruct::Selects> sout(_rt.m_pImmediateContext
			, _cbSelect, MAP_WRITE, MAP_FLAG_DISCARD);
		sout[0].inum = _dyvbc;
		sout[1] = parasele;
	
	constexpr uint dfdsp[] = { 0,1,1,0 };
	if (parasele.inum > 0)
		_rt.m_pImmediateContext->UpdateBuffer(seleout, 0, szsu, &Zero1Kbyte, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
	_rt.m_pImmediateContext->UpdateBuffer(a_vfag, 0, szsu, &Zero1Kbyte, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
	_rt.m_pImmediateContext->UpdateBuffer(m_DrawArglocal, 0, szsu, &Zero1Kbyte, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

	//_rt.m_pImmediateContext->UpdateBuffer(m_brightObjs, 0, szsu, &Zero1Kbyte, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
	_rt.m_pImmediateContext->UpdateBuffer(_DisptchLit, 0, sizeof(dfdsp), &dfdsp, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

	ctex = 0;// Tim% m_ShadowMapSRV.size(), ntex = (Tim += 1) % m_ShadowMapSRV.size();
	auto& cSRV = m_ShadowMapSRV[ctex];
	auto& nextUAV = m_ShadowMapUAV[ntex];
	auto& cUAV = m_ShadowMapUAV[ctex];
	//m_plightSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "objProp")->Set(m_brightObjs->GetDefaultView(BUFFER_VIEW_UNORDERED_ACCESS));
	//m_TargetMapSRV->Set(cSRV);
	//clear next lightmap
	/*m_ClearSRV->Set(nextUAV);
	m_pImmediateContext->SetPipelineState(m_pResetArgPSO);
	m_pImmediateContext->CommitShaderResources(m_pResetArgSRB, RESOURCE_STATE_TRANSITION_MODE_VERIFY);
	DispatAttribs.ThreadGroupCountX = gCons.pedd/m_ThreadGroupSize+1;
	m_pImmediateContext->DispatchCompute(DispatAttribs);*/
	auto max = [](const auto& a, const auto& b) {return a > b ? a : b; };
	m_ClearSRV->Set(cUAV);
	int gcx = math::min(_dyvbc, maxlit);
	gCons.uiNum = _dyvbc;
	m_pImmediateContext->SetPipelineState(m_pResetPSO);
	{
		/*MapHelper<sStruct::objrep> m_MappedData(m_pImmediateContext, m_brightObjs, MAP_TYPE::MAP_WRITE, MAP_FLAGS::MAP_FLAG_DISCARD);
		std::memcpy(m_MappedData, Lighteobjs.data(), Lighteobjs.size_bytes());
		for (size_t i = 0; i < Lighteobjs.size(); i++)
		{
			m_MappedData[i].pos.head<2>() += (math::int2::Constant(shadowmapsz / 2));
		}*/

		Diligent::MapHelper<sStruct::GlobalConstants> gconst(m_pImmediateContext, m_brightcb, MAP_TYPE::MAP_WRITE, MAP_FLAGS::MAP_FLAG_DISCARD);
		*gconst = gCons;
	}
	m_pImmediateContext->CommitShaderResources(m_pResetSRB, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
	//clear 4x4 block each thread
	DispatAttribs.ThreadGroupCountX = (shadowmapsz / 32);
	DispatAttribs.ThreadGroupCountY = shadowmapsz / 32;
	m_pImmediateContext->DispatchCompute(DispatAttribs);


	//write gid on map & fill light source buffer
	m_pImmediateContext->SetPipelineState(WriteGMap_PSB.PSO);
	m_pImmediateContext->CommitShaderResources(WriteGMap_PSB.GetShaderResourceBinding(), RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
	DispatAttribs.ThreadGroupCountX = max(1,std::ceil((float)maxlit / threadgsize));
	DispatAttribs.ThreadGroupCountY = 1;
	m_pImmediateContext->DispatchCompute(DispatAttribs);
	//cast light
	m_lightMapUAV->Set(cUAV);
	m_pImmediateContext->SetPipelineState(m_CSLitPSO);
	m_pImmediateContext->CommitShaderResources(m_plightSRB, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
	DispatAttribs.ThreadGroupCountX = max(1, gcx);
	DispatAttribs.ThreadGroupCountY = 1;
	DispatAttribs.ThreadGroupCountZ = 1;
	m_pImmediateContext->DispatchComputeIndirect(CLitAttribs);
	//m_pImmediateContext->DispatchCompute(DispatAttribs);

	//TODO: clear up sample texture
	m_pImmediateContext->SetPipelineState(Uscpsb.PSO);
	m_pImmediateContext->CommitShaderResources(Uscpsb.SRBs[ctex], RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
	DispatAttribs.ThreadGroupCountX = math::ceil(shadowmapsz*4/ 32);
	DispatAttribs.ThreadGroupCountY = math::ceil(shadowmapsz * 4 / 32);
	DispatAttribs.ThreadGroupCountZ = 1;
	m_pImmediateContext->DispatchCompute(DispatAttribs);
	
	//m_pImmediateContext->DispatchComputeIndirect(CLitAttribs);
	if (insTancenum != 0) [[likely]]
		//cull
	{
		auto& pCtx = m_pImmediateContext;
		m_pImmediateContext->SetPipelineState(Vfilterpsb.PSO);
		m_pImmediateContext->CommitShaderResources(Vfilterpsb.SRBs[ctex], RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
		gcx = insTancenum;// (insTancenum * 8) / m_ThreadGroupSize;
		DispatAttribs.ThreadGroupCountX = (_dyvbc / m_ThreadGroupSize) + 1;
		DispatAttribs.ThreadGroupCountY = 1;
		DispatAttribs.ThreadGroupCountZ = 1;
		m_pImmediateContext->DispatchCompute(DispatAttribs);

		pCtx->CopyBuffer(m_DrawArglocal, 0, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION, m_VFArgR, 0, sizeof(sStruct::VFoutArgs) * VfArgREle, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
#if 0
		pCtx->CopyBuffer(m_brightObjs, 0, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION, m_LitR, 0, sizeof(sStruct::objrep) * 128, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
#endif	
		pCtx->CopyBuffer(m_Debugbufv, 0, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION, m_Debugbuf, 0, sizeof(vdt) * vdbfnum, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
		//pCtx->CopyBuffer(Nebbuf, 0, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION, stagNebbuf, 0, sizeof(sStruct::AroundTg) * insTancenum, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

		const int cpynum = 1024;// std::min(256 * tsize, 2048);
		if (parasele.inum == rmk || parasele.inum == remk) {
			_rt.m_pImmediateContext->CopyBuffer(this->seleout, 0,
				Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION,
				stagsele,
				0, cpynum * sizeof(t_sinout), Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
			//_rt.m_pImmediateContext->EnqueueSignal(_cpyfen, m_CurrentFenceValue-1);
			Fencesele = m_CurrentFenceValue;
			parasele.inum = 0;
		}
		/*_rt.m_pImmediateContext->EnqueueSignal(_cpyfen, m_CurrentFenceValue);
		const auto& fencev = _cpyfen->GetCompletedValue();*/
		m_CurrentFenceValue++;
	}
}

void pj2::n_Render::fogRender::SetDrawArg(const ui64& src, Utility::span<VFoutArgs> datas) {
	auto& pCtx = m_pImmediateContext;
	pCtx->UpdateBuffer(m_DrawArglocal, src * sizeof(VFoutArgs), datas.size_bytes(), datas.data(),
		Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
}

void pj2::n_Render::fogRender::Render(RefCntAutoPtr<IDeviceContext>& m_pImmediateContext) {
	//const math::ui64 ctex = Tim % m_ShadowMapSRV.size(), ntex = (Tim) % m_ShadowMapSRV.size();
	static int mixrt = 1;
	if (mixrt)
	{
		auto& pCtx = m_pImmediateContext;
		{
			auto& pRTV = m_RendeRTf;//  m_pSwapChain->GetCurrentBackBufferRTV();
			m_pImmediateContext->SetRenderTargets(1, &pRTV, m_pSwapChain->GetDepthBufferDSV(), RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
			m_pImmediateContext->ClearRenderTarget(pRTV, ClearColor, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
		}
		IBuffer* pVBs[] = { plane.pVertexBuffer };
		math::ui64   Offsets[_countof(pVBs)] = {};
		//pCtx->TransitionResourceStates(_countof(Barriers), Barriers);
		pCtx->SetVertexBuffers(0, _countof(pVBs), pVBs, Offsets, RESOURCE_STATE_TRANSITION_MODE_VERIFY, SET_VERTEX_BUFFERS_FLAG_NONE);
		pCtx->SetIndexBuffer(plane.m_IndexBuffer, 0, RESOURCE_STATE_TRANSITION_MODE_VERIFY);
		pCtx->SetPipelineState(m_RfowPSO);
		m_pImmediateContext->CommitShaderResources(m_fowSRB[ctex], RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
		DrawIndexedAttribs drawAttrs(plane.IndexCount, VT_UINT32, DRAW_FLAG_VERIFY_ALL);
		pCtx->DrawIndexed(drawAttrs);
	}
	{
		auto pRTV = m_pSwapChain->GetCurrentBackBufferRTV();
		m_pImmediateContext->SetRenderTargets(1, &pRTV, m_pSwapChain->GetDepthBufferDSV(), RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
		m_pImmediateContext->ClearRenderTarget(pRTV, ClearColor, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
		m_pImmediateContext->ClearDepthStencil(m_pSwapChain->GetDepthBufferDSV(), CLEAR_DEPTH_FLAG | CLEAR_STENCIL_FLAG, 1.f, 0, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
		// //Set the render target pipeline state
		m_pImmediateContext->SetPipelineState(m_pRTPSO);
		if (RTDrawAttrs.NumVertices == 8)
		{
			_rt.m_pImmediateContext->UpdateBuffer(_Selecbuf, 0, sizeof(sStruct::Selects), &_rect, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

			StateTransitionDesc Barriers[] = {
				{ _Selecbuf, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_GENERIC_READ,STATE_TRANSITION_FLAG_UPDATE_STATE } };
			_rt.m_pImmediateContext->TransitionResourceStates(1, Barriers);
		}
		//// Commit the render target shader's resources
		m_pImmediateContext->CommitShaderResources(m_pRTSRB, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
		// Draw the render target's vertices
		m_pImmediateContext->Draw(RTDrawAttrs);
		RTDrawAttrs.NumVertices = 4;
	}
}

void pj2::n_Render::fogRender::GenerateBlock(Utility::span<sStruct::objrep> bks, math::int4 size) {
	//(num);
	auto offset = size.head<2>() / -2 + size.tail(2);
	for (size_t i = 0; i < bks.size(); i++)
	{

		auto& bk = bks[i];
		bk.pos[0] = std::rand() % size[0];
		bk.pos[1] = std::rand() % size[1];
		bk.pos[2] = 30;
		bk.pos.head<2>() += offset;
	}
	AddBlock(bks);
}
