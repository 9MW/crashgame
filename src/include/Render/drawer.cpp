#include "drawer.h"
#include"structs.h"
#include"DiligentTools/TextureLoader/interface/TextureUtilities.h"
namespace pj2::n_Render {
	using Ichar = char;
	math::Efloat2 Gridsize__ = { 100,100 };
	std::vector<debug::bflinevtxdf> gddd(2 * (Gridsize__[0] + Gridsize__[1]));
	void CreatePS(IPipelineState** const pPSO,
		RefCntAutoPtr<IRenderDevice> m_pDevice,
		Diligent::TEXTURE_FORMAT ColorBufferFormat,
		Diligent::TEXTURE_FORMAT DepthBufferFormat,
		RefCntAutoPtr<IShaderSourceInputStreamFactory> pShaderSourceFactory,
		const std::string FilePath, const Ichar* ven, const Ichar* pen, const Ichar* Geo,
		const Utility::span<ShaderResourceVariableDesc> sd,
		const Utility::span<LayoutElement> inputLayoutDesc,
		const Utility::span<ImmutableSamplerDesc> StaticSamplers,
		PRIMITIVE_TOPOLOGY VertexTop, bool AntialiasedLineEnable, bool DepthEnable,
		const Diligent::CULL_MODE cullmode = CULL_MODE_NONE,
		ShaderMacroHelper Macros = {}) {
		assert(false);
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
		RefCntAutoPtr<IShader> pGS;
		{
			ShaderCI.Desc.ShaderType = SHADER_TYPE_GEOMETRY;
			ShaderCI.EntryPoint = "main";
			ShaderCI.Desc.Name = "Line GS";
			ShaderCI.FilePath = "Debug.hlsl";
			m_pDevice->CreateShader(ShaderCI, &pGS);
		}
		ShaderCI.Desc.Name = stbuffer.data();;
		ShaderCI.EntryPoint = pen;
		ShaderCI.FilePath = FilePath.c_str();
		ShaderCI.Desc.ShaderType = SHADER_TYPE_PIXEL;
		RefCntAutoPtr<IShader> pPS;
		m_pDevice->CreateShader(ShaderCI, &pPS);
		//for (Uint32 vb = 0; vb < m_Mesh._subMesh.size(); ++vb)
		{
			GraphicsPipelineStateCreateInfo PSOCreateInfo;
			Diligent::GraphicsPipelineDesc& GraphicsPipeline = PSOCreateInfo.GraphicsPipeline;
			PipelineStateDesc& PSODesc = PSOCreateInfo.PSODesc;
			auto& RT0 = GraphicsPipeline.BlendDesc.RenderTargets[0];
			RT0.BlendEnable = 0;
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
			PSOCreateInfo.pGS = pGS;
			GraphicsPipeline.RTVFormats[0] = ColorBufferFormat;
			GraphicsPipeline.NumRenderTargets = 1;
			GraphicsPipeline.PrimitiveTopology = VertexTop;
			GraphicsPipeline.DSVFormat = DepthBufferFormat;
			//PSODesc.GraphicsPipeline.DepthStencilDesc.DepthFunc = COMPARISON_FUNC_LESS_EQUAL; // Cull back faces
			GraphicsPipeline.RasterizerDesc.CullMode = cullmode;// CULL_MODE_BACK;
			GraphicsPipeline.RasterizerDesc.AntialiasedLineEnable = AntialiasedLineEnable;
																// Enable depth testing
			GraphicsPipeline.DepthStencilDesc.DepthEnable = 0;
			m_pDevice->CreateGraphicsPipelineState(PSOCreateInfo, pPSO);
		}
	}
	std::vector<math::v4f> circlevtx, circlevtxstrip;
	static debug* p_dbg;
	using namespace pj2::GIns;
	debug::debug(RefCntAutoPtr<IRenderDevice> m_pDevice, RefCntAutoPtr<IDeviceContext> m_pImmediateContext, const TEXTURE_FORMAT RenderTargetFormat, const TEXTURE_FORMAT DepthBufferFormat, IBuffer* pCameraAttribs, RefCntAutoPtr<IShaderSourceInputStreamFactory> pShaderSourceFactory, ITexture* Maptex, const int instancenum) :m_pDevice(m_pDevice), m_pImmediateContext(m_pImmediateContext),
		m_pShaderSourceFactory(pShaderSourceFactory) {
		p_dbg = this;
		RenderContext Rct;
		Rct.DepthBufferFormat = DepthBufferFormat;
		Rct.RenderTargetFormat = RenderTargetFormat;
		Rct.m_pDevice = m_pDevice;
		Rct.m_pImmediateContext = m_pImmediateContext;
		Rct.pShaderSourceFactory = pShaderSourceFactory;
		Rct.pCameraAttribs = pCameraAttribs;
		Gridsize__ = this->Gridsize;
		this->cellsize = { 4.0f,4.0,0.0,1.0 };
		ShaderResourceVariableDesc Vars[] =
		{
			{ SHADER_TYPE_PIXEL, "g_ColorMap", SHADER_RESOURCE_VARIABLE_TYPE_STATIC },
			{ SHADER_TYPE_PIXEL, "cell", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE },
			{ SHADER_TYPE_PIXEL, "map", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE },
			{ SHADER_TYPE_VERTEX, "animreco", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE },
			{ SHADER_TYPE_VERTEX, "instDatas", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE },
			{ SHADER_TYPE_VERTEX, "objectTransfromBuffer", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE }
		};
		static LayoutElement LineDesc[] =
		{
		 {0, 0, 4, VT_FLOAT32},   //float3 Pos     : ATTRIB0;
		 {1, 0, 4, VT_FLOAT32},   //float3 Normal  : ATTRIB1;
		};
		GIns::CreatePipelineStates
		(&pRenderMeshPSO, m_pDevice, RenderTargetFormat, DepthBufferFormat, pShaderSourceFactory, "sMesh.hlsl", "vert", "frag", Vars);
		
		auto& pPSO = pRenderMeshPSO;
		qd.reset(new geomatrys::ngeo_quads(m_pDevice, Diligent::float3(1000, 1000, 1000)));
		//map.resize(sqsize * sqsize);
		CreateBuffer<math::Efloat4>(m_pDevice, m_cellbuf);
		{
			BufferDesc BuffDesc;
			BuffDesc.Name = "m_mapbuf buffer";
			BuffDesc.Usage = USAGE_DYNAMIC;
			BuffDesc.BindFlags = BIND_SHADER_RESOURCE;
			BuffDesc.Mode = BUFFER_MODE_STRUCTURED;
			BuffDesc.CPUAccessFlags = CPU_ACCESS_WRITE;
			BuffDesc.ElementByteStride = sizeof(math::Efloat4);
			BuffDesc.Size = BuffDesc.ElementByteStride * debugoffset * debugoffset;
			m_pDevice->CreateBuffer(BuffDesc, nullptr, &m_mapbuf);
		}
		if(!Maptex)
		{
			float4 black[8 * 8];
			memset(black, 0, sizeof(black));
			for (auto& co : black)
			{
				co.a = 1;
			}
			Render::CreateintTex(m_pDevice, m_pImmediateContext, Maptex, black);
		}
		pPSO->GetStaticVariableByName(SHADER_TYPE_VERTEX, "cbCameraAttribs")->Set(pCameraAttribs);
		pPSO->GetStaticVariableByName(SHADER_TYPE_PIXEL, "cell1")->Set(m_cellbuf);
		pPSO->GetStaticVariableByName(SHADER_TYPE_PIXEL, "g_ColorMap")->Set(Maptex? Maptex->GetDefaultView(TEXTURE_VIEW_SHADER_RESOURCE):
			Render::m_pBlackTexSRV);
		pPSO->CreateShaderResourceBinding(&pSRB, true);
		trs.emplace_back();
		trs[0].Scale.head<2>().setConstant(100);
		BufferDesc BuffDesc;
		BuffDesc.Name = "transform buffer";
		BuffDesc.Usage = USAGE_DYNAMIC;
		BuffDesc.BindFlags = BIND_SHADER_RESOURCE;
		BuffDesc.Mode = BUFFER_MODE_STRUCTURED;
		BuffDesc.CPUAccessFlags = CPU_ACCESS_WRITE;
		BuffDesc.ElementByteStride = sizeof(mtransfrom);
		BuffDesc.Size = sizeof(mtransfrom) * instancenum;
		m_pDevice->CreateBuffer(BuffDesc, nullptr, &m_pTransAttribsBuffer);
		auto teins = pSRB->GetVariableByName(SHADER_TYPE_VERTEX, "objectTransfromBuffer");
		//pSRB->GetVariableByIndex();
		auto mapf = pSRB->GetVariableByName(SHADER_TYPE_PIXEL, "map");
		if (0)
			pSRB->GetVariableByName(SHADER_TYPE_PIXEL, "g_ColorMap")->Set(Maptex->GetDefaultView(TEXTURE_VIEW_SHADER_RESOURCE));
		teins->Set(m_pTransAttribsBuffer->GetDefaultView(BUFFER_VIEW_SHADER_RESOURCE));
		mapf->Set(m_mapbuf->GetDefaultView(BUFFER_VIEW_SHADER_RESOURCE));
		{
			_Linebuf.reserve(4096);
			BufferDesc VertBuffDesc;
			VertBuffDesc.Name = "line vertex buffer";
			VertBuffDesc.Usage = USAGE_DYNAMIC;
			VertBuffDesc.BindFlags = BIND_VERTEX_BUFFER;
			VertBuffDesc.CPUAccessFlags = CPU_ACCESS_WRITE;
			VertBuffDesc.Size = _Linebuf.capacity() * sizeof(bflinevtxdf);
			//m_pDevice->CreateBuffer(VertBuffDesc, nullptr, &_pLineVertexBuffer);
		}
		StateTransitionDesc Barriers[] =
		{
			//{m_pTransAttribsBuffer,  RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_SHADER_RESOURCE, STATE_TRANSITION_FLAG_UPDATE_STATE},
			{ m_mapbuf,  RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_SHADER_RESOURCE, STATE_TRANSITION_FLAG_UPDATE_STATE },
			{ m_cellbuf,  RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_SHADER_RESOURCE, STATE_TRANSITION_FLAG_UPDATE_STATE },
			{ _pLineVertexBuffer, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_VERTEX_BUFFER, STATE_TRANSITION_FLAG_UPDATE_STATE }
		};

		m_pImmediateContext->TransitionResourceStates(1, Barriers);
		{
			math::Efloat4 line(-20, -20, 20, 20);
			_Linebuf.resize(2);
			_Linebuf[0].pos[0] = line[0];
			_Linebuf[0].pos[1] = line[1];
			_Linebuf[1].pos[0] = line[2];
			_Linebuf[1].pos[1] = line[3];
		}
		typecolors.resize(10);
		int num_segments = 16, r = 1;
		circlevtx.resize(num_segments);
		for (int ii = 0; ii < num_segments; ii++) {
			float theta = 2.0f * 3.1415926f * float(ii) / float(num_segments);//get the current angle 
			float x = r * cosf(theta);//calculate the x component 
			float y = r * sinf(theta);//calculate the y component 
			auto& vt = circlevtx[ii];
			vt = { x,y,0,0 };
		}

		circlevtxstrip.clear();
		circlevtxstrip.resize(circlevtx.size() + 3);
		auto& v0 = circlevtxstrip[0];
		v0 = v0 - circlevtx.back();
		memcpy(&circlevtxstrip[0] + 1, &circlevtx[0], Utility::VecByteSize(circlevtx));
		auto& vn = circlevtxstrip.rbegin()[1];
		vn = circlevtx[0];
		circlevtxstrip.back() = v0;

		{
			gddd.resize(2 * (Gridsize[0] + Gridsize[1]));
			FillGrid(Gridsize, gddd);
		}
		_lRd.init(Rct);
	}

	inline void debug::FillGrid(math::Efloat2 grid, span<bflinevtxdf> disds) {
		int io = 0;
		auto dis = disds.data();
		math::int2 off;
		bflinevtxdf girdx;
		girdx.color = { 1,0,0,0.5 };
		math::Efloat4 gridx(-grid[0] / 2, 0, 0, 1);
		std::memcpy(&girdx.pos, &girdx, 16);
		math::Efloat3 gridy(0, grid[1] / 2, 0);
		math::Efloat3 grid3(0, 0, 0);
		off[0] = grid[0] / 2;
		off[1] = grid[1] / 2;
		for (size_t i = 0; i < grid[0]; i++)
		{
			grid3 = gridy;
			grid3[0] = i - (float)off[0];
			std::memcpy(dis++->pos.Data(), grid3.data(), sizeof(bflinevtxdf::pos));
			grid3[1] *= -1.0;
			std::memcpy(dis++->pos.Data(), grid3.data(), sizeof(bflinevtxdf::pos));
		}
		for (size_t i = 0; i < grid[1]; i++)
		{
			girdx.pos[1] = i - (float)off[1];
			std::memcpy(dis++, &girdx, sizeof(bflinevtxdf));
			girdx.pos[0] *= -1.0;
			std::memcpy(dis++, &girdx, sizeof(bflinevtxdf));
		}
	}

	void debug::DrawLine(span<bflinevtxdf> lines, bool persistent) {
		_lRd.Render();
		return;
	}

	void debug::Render()
	{
		static auto sign = 0;

		if (sign) {
			return;
		}
		/*static double dt;
		dt += std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now()).time_since_epoch().count();
		auto rad = std::fmod(dt, (1000 * period)) / (1000 * period) * PI * 2;

		Rotation2Df ROT2(rad);*/
		const int instancenum = 1;
		auto& pCtx = m_pImmediateContext;
		Uint64   Offsets[2] = {};
		IBuffer* pVBs[] = { qd->pVertexBuffer };
		m_pImmediateContext->SetVertexBuffers(0, _countof(pVBs), pVBs, Offsets,
			RESOURCE_STATE_TRANSITION_MODE_TRANSITION, SET_VERTEX_BUFFERS_FLAG_RESET);
		pCtx->SetIndexBuffer((qd->m_IndexBuffer), 0, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
		pCtx->SetPipelineState(pRenderMeshPSO);
		MapHelper<mtransfrom>mtransfromData(pCtx, m_pTransAttribsBuffer, MAP_WRITE, MAP_FLAG_DISCARD);
		std::memcpy(mtransfromData, trs.data(), nonstd::span(trs).size_bytes());
		MapHelper<math::Efloat4>mmapData(pCtx, m_mapbuf, MAP_WRITE, MAP_FLAG_DISCARD);
		std::memset(mmapData, 0, m_mapbuf->GetDesc().Size);
		if (0)
		{
		static std::vector<math::int2> res;
			/*for (size_t i = 0; i < res.size(); i++)
			{
			const auto idx = CalCoord.CalIndex(res[i][0], res[i][1]);
			mmapData[idx].setConstant(0);
			}*/
			res.clear();
			auto cq = Eigen::Map<math::Efloat2>(circlepos.Data()).cwiseQuotient(cellsize.head<2>()).array().round();
			Utility::circle([&](const auto a, const auto b) {res.emplace_back(a, b); }, cq[0], cq[1], std::round(circleRadius / cellsize[0]));
			math::Efloat4 line;
			std::memcpy(line.data(), _Linebuf[0].pos.Data(), sizeof(math::Efloat2));
			std::memcpy(line.data() + 2, _Linebuf[1].pos.Data(), sizeof(math::Efloat2));
			if (_Linebuf.size() < 2 + cirSeg * 2)
				_Linebuf.resize(2 + cirSeg * 2);
			auto lineptr = _Linebuf.data() + 2;
			Utility::circle(nonstd::span(lineptr, cirSeg * 2), circlepos, circleRadius);

			int coun = 0;
			Utility::DiscreteLine(line, cellsize.head<2>(), [&](const Eigen::Vector2i vox) {
				if (coun++ > quiryatep)
					return true;
				res.emplace_back(vox);
				return false;
				});
			for (size_t i = 0; i < res.size(); i++)
			{
				const auto idx = CalCoord.CalIndex(res[i][0], res[i][1]);
				mmapData[idx].setConstant(1);
			}
		}
		cellsize[2] = debugoffset;
		pCtx->UpdateBuffer(m_cellbuf, 0, sizeof(cellsize), &cellsize, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
		pCtx->CommitShaderResources(pSRB, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
		DrawIndexedAttribs drawAttrs(qd->IndexCount, VT_UINT32, DRAW_FLAG_VERIFY_ALL, instancenum);
		pCtx->DrawIndexed(drawAttrs);
		//in order render above floor must put line render blow rendering floor
		DrawLine(_Linebuf);
		_Linebuf.clear();
	}

	void debug::DrawUI()
	{
		return;
		ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiCond_FirstUseEver);
		ImGui::Begin("Debug");
		ImGui::DragFloat4("cellsize", cellsize.data());
		ImGui::PushItemWidth(80);
		ImGui::SliderFloat("SeparationWeight", &SeparationWeight, 0, 1);
		ImGui::SameLine();
		ImGui::SliderFloat("cohesion", &cohesion, 0, 1);
		ImGui::SameLine();
		ImGui::SliderFloat("aligment", &w2, -3, 3);
		ImGui::Text(fmt::format("maxspeed={0}", maxspeed).c_str());
		ImGui::SameLine();
		ImGui::Text(fmt::format("minspeed={0}", minspeed).c_str());
		ImGui::SameLine();
		ImGui::Text(fmt::format("anvNeg={0}", anvNeg).c_str());
		ImGui::SameLine();
		ImGui::Text(fmt::format("itor={0}", itor).c_str());
		ImGui::DragInt("func ser", &CreateDesType, 0, 3);
		ImGui::DragInt("_stopneb", &_stopneb, 1, 0, 30);
		ImGui::DragInt("_nebsz", &_nebsz, 1, 0, 30);
		ImGui::PopItemWidth();
		ImGui::PushItemWidth(160);
		ImGui::DragFloat2("Gridsize ", Gridsize.data());
		ImGui::PopItemWidth();
		ImGui::PushItemWidth(80);
		ImGui::SameLine();
		ImGui::DragInt("quiry step ", &quiryatep);
		ImGui::DragInt("circleRadius ", &circleRadius);
		ImGui::PopItemWidth();
		ImGui::Checkbox("DrawLine", &DrawLineb);
		ImGui::ColorEdit4("colorfoma", _Linebuf[0].color.Data());
		ImGui::ColorEdit4("color2", _Linebuf[1].color.Data());
		ImGui::DragFloat3("pos1", _Linebuf[0].pos.Data());
		ImGui::DragFloat3("pos2", _Linebuf[1].pos.Data());
		ImGui::DragFloat3("circle postion", circlepos.Data());
		for (size_t i = 0; i < typecolors.size(); i++)
		{
			ImGui::PushID(i);
			ImGui::ColorEdit4(("color" + std::to_string(i)).c_str(), typecolors[i].Data());
			ImGui::PopID();
		}
		ImGui::End();
	}

	void debug::DrawCircle(void* pd) {
		return;//thread unsafe call
		math::v4f* prfptr = reinterpret_cast<math::v4f*>(pd);
		math::v4f& prf = prfptr[0];
		static std::mutex mx;
		auto& _Linebuf = _lRd._Linebuf;
		auto v = _Linebuf.size();
		
		_Linebuf.resize(v + circlevtxstrip.size());
		for (size_t i = 0; i < circlevtxstrip.size(); i++)
		{
			math::v4f scomp = (circlevtxstrip[i] + prf) * prf[3];
			auto& lv = _Linebuf[i + v];
			std::memcpy(&lv.pos, &scomp, 12);
			std::memcpy(&lv.color, prfptr+1, 16);
		}
	}

}