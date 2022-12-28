#pragma once

#include"Render/structs.h"
#include"Render/geomatrys.h"
#include"Log/log.h"
#include <Math1.h>
export module GraphicUtil;
namespace pj2::GIns{
	using RenderUtil::CreatePipelineStates;

	//using namespace Diligent;
	export class debug
	{
		struct LineVertex
		{
			using float3 = Diligent::float3;
			using float2 = Diligent::float2;
			using float4 = Diligent::float4;
			float3 pos;
			float3 normal;
			float4 color;
			float& operator[](int i) {
				return pos[i];
			}
			LineVertex() = default;
			LineVertex(float3&& v3) {
				pos = v3;
			}
			operator float3& () {
				return pos;
			}

		};
	public:
		std::vector<mtransfrom> trs;
		math::Efloat4 cellsize;
		inline debug(
			RefCntAutoPtr<IRenderDevice> m_pDevice,
			RefCntAutoPtr<IDeviceContext> m_pImmediateContext,
			const TEXTURE_FORMAT RenderTargetFormat,
			const TEXTURE_FORMAT DepthBufferFormat,
			IBuffer* pCameraAttribs,
			RefCntAutoPtr<IShaderSourceInputStreamFactory> pShaderSourceFactory,
			ITexture* Maptex = NULL,
			const int instancenum = 1
		) :m_pDevice(m_pDevice), m_pImmediateContext(m_pImmediateContext),
			m_pShaderSourceFactory(pShaderSourceFactory) {
			this->cellsize = { 4.0f,4.0,0.0,1.0 };
			ShaderResourceVariableDesc Vars[] =
			{
				{SHADER_TYPE_PIXEL, "g_ColorMap", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE},
				{SHADER_TYPE_PIXEL, "cell", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE},
				{SHADER_TYPE_PIXEL, "map", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE},
				{SHADER_TYPE_VERTEX, "animreco", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE},
				{SHADER_TYPE_VERTEX, "instDatas", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE},
				{SHADER_TYPE_VERTEX, "objectTransfromBuffer", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE}
			};
			GIns::CreatePipelineStates
			(&pRenderMeshPSO, m_pDevice, RenderTargetFormat, DepthBufferFormat, pShaderSourceFactory, "sMesh.hlsl", "vert", "frag", Vars);
			CreatePipelineStates
			(&pRenderLinePSO, m_pDevice, RenderTargetFormat, DepthBufferFormat,
				pShaderSourceFactory, "Debug.hlsl", "vert", "frag", Vars, defLineDesc,
				defStaticSamplers, PRIMITIVE_TOPOLOGY_LINE_LIST, true);
			CalCoord.sq = debugoffset;
			{
				pRenderLinePSO->GetStaticVariableByName(SHADER_TYPE_VERTEX, "cbCameraAttribs")->Set(pCameraAttribs);
				pRenderLinePSO->CreateShaderResourceBinding(&pLineSRB, true);
			}
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
			pPSO->GetStaticVariableByName(SHADER_TYPE_VERTEX, "cbCameraAttribs")->Set(pCameraAttribs);
			pPSO->GetStaticVariableByName(SHADER_TYPE_PIXEL, "cell1")->Set(m_cellbuf);
			//pPSO->GetStaticVariableByName(SHADER_TYPE_PIXEL, "map")->Set(m_cellbuf);
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
			if (Maptex)
				pSRB->GetVariableByName(SHADER_TYPE_PIXEL, "g_ColorMap")->Set(Maptex->GetDefaultView(TEXTURE_VIEW_SHADER_RESOURCE));
			teins->Set(m_pTransAttribsBuffer->GetDefaultView(BUFFER_VIEW_SHADER_RESOURCE));
			mapf->Set(m_mapbuf->GetDefaultView(BUFFER_VIEW_SHADER_RESOURCE));
			{
				_Linebuf.reserve(500 + cirSeg * 2);
				BufferDesc VertBuffDesc;
				VertBuffDesc.Name = "line vertex buffer";
				VertBuffDesc.Usage = USAGE_DYNAMIC;
				VertBuffDesc.BindFlags = BIND_VERTEX_BUFFER;
				VertBuffDesc.CPUAccessFlags = CPU_ACCESS_WRITE;
				VertBuffDesc.Size = _Linebuf.capacity() * sizeof(LineVertex);
				m_pDevice->CreateBuffer(VertBuffDesc, nullptr, &_pLineVertexBuffer);
			}
			StateTransitionDesc Barriers[] =
			{
				//{m_pTransAttribsBuffer,  RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_SHADER_RESOURCE, STATE_TRANSITION_FLAG_UPDATE_STATE},
				{m_mapbuf,  RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_SHADER_RESOURCE, STATE_TRANSITION_FLAG_UPDATE_STATE},
				{m_cellbuf,  RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_SHADER_RESOURCE, STATE_TRANSITION_FLAG_UPDATE_STATE},
				{_pLineVertexBuffer, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_VERTEX_BUFFER, STATE_TRANSITION_FLAG_UPDATE_STATE}
			};

			m_pImmediateContext->TransitionResourceStates(1, Barriers);
			{
				math::Efloat4 line(-20, -20, 20, 20);
				Linebuf.resize(2);
				Linebuf[0].pos[0] = line[0];
				Linebuf[0].pos[1] = line[1];
				Linebuf[1].pos[0] = line[2];
				Linebuf[1].pos[1] = line[3];
			}
			typecolors.resize(10);
		}
		//void DrawLine(span<LineVertex> lines);
		inline void ClearLine() {
			_lineoffset = 0;
		}
		inline void FillGrid(math::Efloat2 grid, span<LineVertex> disds) {
			int io = 0;
			auto dis = disds.data();
			math::int2 off;
			math::Efloat3 gridx(-grid[0] / 2, 0, 0);
			math::Efloat3 gridy(0, grid[1] / 2, 0);
			math::Efloat3 grid3(0, 0, 0);
			off[0] = grid[0] / 2;
			off[1] = grid[1] / 2;
			for (size_t i = 0; i < grid[0]; i++)
			{
				grid3 = gridy;
				grid3[0] = i - (float)off[0];
				std::memcpy(dis++->pos.Data(), grid3.data(), sizeof(LineVertex::pos));
				grid3[1] *= -1.0;
				std::memcpy(dis++->pos.Data(), grid3.data(), sizeof(LineVertex::pos));
			}
			for (size_t i = 0; i < grid[1]; i++)
			{
				grid3 = gridx;
				grid3[1] = i - (float)off[1];
				std::memcpy(dis++->pos.Data(), grid3.data(), sizeof(LineVertex::pos));
				grid3[0] *= -1.0;
				std::memcpy(dis++->pos.Data(), grid3.data(), sizeof(LineVertex::pos));
			}
		}
		void DrawLine(span<LineVertex> lines, bool persistent = false) {

			m_pImmediateContext->SetPipelineState(pRenderLinePSO);
			Uint64   Offsets[1] = {};
			IBuffer* pVBs[] = { _pLineVertexBuffer };
			m_pImmediateContext->SetVertexBuffers(0, _countof(pVBs), pVBs, Offsets,
				RESOURCE_STATE_TRANSITION_MODE_VERIFY, SET_VERTEX_BUFFERS_FLAG_RESET);
			MapHelper<LineVertex>LineDate(m_pImmediateContext, _pLineVertexBuffer, MAP_WRITE, MAP_FLAG_DISCARD);
			{
				std::memcpy(LineDate + _lineoffset, lines.data(), lines.size_bytes());
			}
			static std::vector<LineVertex> gddd(2 * (Gridsize[0] + Gridsize[1]));
			if (2 * (Gridsize[0] + Gridsize[1]) != gddd.size())
				gddd.resize(2 * (Gridsize[0] + Gridsize[1]));
			FillGrid(Gridsize, gddd);
			std::memcpy(LineDate + lines.size(), gddd.data(), nonstd::span(gddd).size_bytes());

			m_pImmediateContext->CommitShaderResources(pLineSRB, RESOURCE_STATE_TRANSITION_MODE_VERIFY);
			DrawAttribs dA;
			dA.NumVertices = _lineoffset + lines.size() + gddd.size();
			m_pImmediateContext->Draw(dA);
			if (persistent)
				_lineoffset += lines.size();
		}
		inline void Drawgrid(const math::int2 size) {

		}
		void Render()
		{
			static auto sign = 1;
			/*static double dt;
			dt += std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now()).time_since_epoch().count();
			auto rad = std::fmod(dt, (1000 * period)) / (1000 * period) * PI * 2;

			Rotation2Df ROT2(rad);*/
			DrawLine(Linebuf);
			const int instancenum = 1;
			auto& pCtx = m_pImmediateContext;
			Uint64   Offsets[2] = {};
			IBuffer* pVBs[] = { qd->pVertexBuffer };
			m_pImmediateContext->SetVertexBuffers(0, _countof(pVBs), pVBs, Offsets,
				RESOURCE_STATE_TRANSITION_MODE_TRANSITION, SET_VERTEX_BUFFERS_FLAG_RESET);
			pCtx->SetIndexBuffer((qd->m_IndexBuffer), 0, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
			pCtx->SetPipelineState(pRenderMeshPSO);
			MapHelper<mtransfrom>mtransfromData(pCtx, m_pTransAttribsBuffer, MAP_WRITE, MAP_FLAG_DISCARD);
			MapHelper<math::Efloat4>mmapData(pCtx, m_mapbuf, MAP_WRITE, MAP_FLAG_DISCARD);
			std::memcpy(mtransfromData, trs.data(),  nonstd::span(trs).size_bytes());
			//if(period!=1)
			{
				static std::vector<math::int2> res;
				/*for (size_t i = 0; i < res.size(); i++)
				{
					const auto idx = CalCoord.CalIndex(res[i][0], res[i][1]);
					mmapData[idx].setConstant(0);
				}*/
				std::memset(mmapData, 0, m_mapbuf->GetDesc().Size);
				res.clear();
				auto cq = Eigen::Map<math::Efloat2>(circlepos.Data()).cwiseQuotient(cellsize.head<2>()).array().round();
				Utility::circle([&](const auto a, const auto b) {res.emplace_back(a, b); }, cq[0], cq[1], std::round(circleRadius / cellsize[0]));
				math::Efloat4 line;
				std::memcpy(line.data(), Linebuf[0].pos.Data(), sizeof(math::Efloat2));
				std::memcpy(line.data() + 2, Linebuf[1].pos.Data(), sizeof(math::Efloat2));
				if (Linebuf.size() < 2 + cirSeg * 2)
					Linebuf.resize(2 + cirSeg * 2);
				auto lineptr = Linebuf.data() + 2;
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
			sign++;
		}
		void DrawUI()
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
			ImGui::ColorEdit4("colorfoma", Linebuf[0].color.Data());
			ImGui::ColorEdit4("color2", Linebuf[1].color.Data());
			ImGui::DragFloat3("pos1", Linebuf[0].pos.Data());
			ImGui::DragFloat3("pos2", Linebuf[1].pos.Data());
			ImGui::DragFloat3("circle postion", circlepos.Data());
			for (size_t i = 0; i < typecolors.size(); i++)
			{
				ImGui::PushID(i);
				ImGui::ColorEdit4(("color" + std::to_string(i)).c_str(), typecolors[i].Data());
				ImGui::PopID();
			}
			ImGui::End();
		}

		bool DrawLineb = false;
		math::Efloat2 Gridsize = { 100,100 };
		int CreateDesType = 0, itor, _stopneb = 5, _nebsz = 4;
		float period = 1, SeparationWeight = 0.7, w2 = 0, cohesion = 0.3, maxspeed, anvNeg, minspeed;
		std::vector<LineVertex> Linebuf;
		std::vector<Diligent::float4> typecolors = {
			{0.5,0,0,1},
			{0,1,0,1}
		};
	private:
		int circleRadius = 10;
		Diligent::float4 circlepos = { 0,0,0,0 };
		int debugoffset = 100;
		int sqs = 10000;
		int _lineoffset = 0;
		std::vector<LineVertex> _Linebuf;
		CalMapId CalCoord;
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