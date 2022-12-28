module;
#pragma once
#include "common/Math1.h"
#include"ShaderStructs.h"
//#include"physics/physics.hpp"
#include<PCH.h>
#include<imgui.h>
#include "Common/interface/RefCntAutoPtr.hpp"
//#include"Render/structs.h"
#include <Graphics/GraphicsEngineVulkan/interface/EngineFactoryVk.h>

#include "Graphics/GraphicsEngine/interface/RenderDevice.h"
#include "Graphics/GraphicsEngine/interface/DeviceContext.h"
#include "Graphics/GraphicsEngine/interface/SwapChain.h"
#include <ImGuiImplDiligent.hpp>
//#include <ImGuiImplWin32.hpp>
#include <ShaderMacroHelper.hpp>
#include"Mesh.h"
#include"io/loader.h"
#include"Common/span.hpp"
#include <FirstPersonCamera.hpp>
#include"Render/RenderUtil.h"
#include"Render/geomatrys.h"
#include<Render/structs.h>
//import DiliGFloader;
export module drawerm;

//import<Render/geomatrys.h>;
//export import<>;
//export  pj2::GIns::CreatePipelineStates();
namespace pj2 {
	struct Instancedata
	{
		math::uint transformid, animid;
	};
	struct cdata {
		math::uint base, frameSize;
	};
	//export
	template<uint srb = 1>
	struct PSB
	{
		RefCntAutoPtr<IPipelineState> PSO;
		//RefCntAutoPtr<IShaderResourceBinding> SRB;
		std::array<RefCntAutoPtr<IShaderResourceBinding>, srb> SRBs;
		void CreateShaderResourceBinding() {
			for (size_t i = 0; i < srb; i++)
			{
				auto& cptr = SRBs[i];
				PSO->CreateShaderResourceBinding(&cptr, true);
			}
		}

		constexpr auto& GetShaderResourceBinding(uint i) {
			return SRBs[i];
		}
	};
	template<int i = 1>
	using POBs = PSB<i>;// std::tuple<RefCntAutoPtr<IPipelineState>, std::array<RefCntAutoPtr<IShaderResourceBinding>, i>>;
	namespace n_Render {
		export struct InstanceRender :Render {
			const float Samplerate = 0.02;
			std::vector<GLTF::Node*> submeshnodes;
			InstanceRender(
				RefCntAutoPtr<IRenderDevice> m_pDevice,
				RefCntAutoPtr<IDeviceContext> m_pImmediateContext,
				TEXTURE_FORMAT RenderTargetFormat,
				TEXTURE_FORMAT DepthBufferFormat,
				IBuffer* pCameraAttribs,
				GLTF::Model& GLTFModel,
				RefCntAutoPtr<IEngineFactory>              pEngineFactory,
				const int instancenum = 1
			) :m_pDevice(m_pDevice),
				m_pImmediateContext(m_pImmediateContext),
				m_pEngineFactory(pEngineFactory) {
				for (const auto& node : GLTFModel.Nodes)
				{
					if (node->_Mesh)
						submeshnodes.emplace_back(node.get());
					readallnode(node.get(), submeshnodes);
				}
				m_Model.reset(&GLTFModel);


				for (const auto& sn : submeshnodes)
				{
					for (const auto& mt : sn->_Mesh->Primitives)
					{
						mtmp[(mt.material)] = NULL;
					}

				}

				std::vector<math::Matrix4f> bonerec;
				slice = makeAnimrec(*m_Model, bonerec, _AniReco, submeshnodes);
				createbuffer(bonerec, instancenum);
				//m_pImmediateContext->MapBuffer()
				for (auto& mat : GLTFModel.Materials)
				{
					const auto pps = CreateMaterialSRB(mat, pCameraAttribs, nullptr, RenderTargetFormat, DepthBufferFormat);
				}
			}
			InstanceRender(
				RefCntAutoPtr<IRenderDevice> m_pDevice,
				RefCntAutoPtr<IDeviceContext> m_pImmediateContext,
				TEXTURE_FORMAT RenderTargetFormat,
				TEXTURE_FORMAT DepthBufferFormat,
				IBuffer* pCameraAttribs,
				const std::string GLTFModel,
				RefCntAutoPtr<IEngineFactory>              pEngineFactory,
				const int instancenum = 1
			) :InstanceRender(m_pDevice, m_pImmediateContext, RenderTargetFormat, DepthBufferFormat, pCameraAttribs,
				*new GLTF::Model(m_pDevice, m_pImmediateContext, GLTFModel),
				pEngineFactory, instancenum) {
			}
			IShaderResourceBinding* CreateMaterialSRB(
				GLTF::Material& Material,
				IBuffer* pCameraAttribs,
				IBuffer* pLightAttribs,
				TEXTURE_FORMAT RenderTargetFormat,
				TEXTURE_FORMAT DepthBufferFormat
			) {

				ShaderResourceVariableDesc Vars[] =
				{
					{SHADER_TYPE_PIXEL, "g_ColorMap", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE},
					{SHADER_TYPE_PIXEL, "cell", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE},
					{SHADER_TYPE_PIXEL, "map", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE},
					{SHADER_TYPE_VERTEX, "animreco", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE},
					{SHADER_TYPE_VERTEX, "instDatas", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE},
					{SHADER_TYPE_VERTEX, "objectTransfromBuffer", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE}
				};
				LayoutElement SkinningmDesc[] =
				{
				 {0, 0, 3, VT_FLOAT32},   //float3 Pos     : ATTRIB0;
				 {1, 0, 3, VT_FLOAT32},   //float3 Normal  : ATTRIB1;
				 {2, 0, 2, VT_FLOAT32},   //float2 UV0     : ATTRIB2;
				 {3, 0, 2, VT_FLOAT32},   //float2 UV1     : ATTRIB3;
				 {4, 0, 4, VT_FLOAT32},   //float3 Pos     : ATTRIB0;
				 {5, 0, 4, VT_FLOAT32},   //float3 Normal  : ATTRIB1;
				};
				RefCntAutoPtr<IShaderSourceInputStreamFactory> pShaderSourceFactory;
				auto shaderpath = loader::Get(shader);
				m_pEngineFactory->CreateDefaultShaderSourceStreamFactory(shaderpath.string().c_str(), &pShaderSourceFactory);
				auto& pPSO = m_RenderMeshPSO.emplace_back();
				GIns::CreatePipelineStates
				(&pPSO, m_pDevice, RenderTargetFormat, DepthBufferFormat, pShaderSourceFactory,
					"skined.hlsl", "vert", "frag", Vars, SkinningmDesc);
				//m_RenderMeshPSO[0] =

				pPSO->GetStaticVariableByName(SHADER_TYPE_VERTEX, "cbCameraAttribs")->Set(pCameraAttribs);
				pPSO->GetStaticVariableByName(SHADER_TYPE_VERTEX, "cbAnimAttribs")->Set(m_InsCbuf);
				RefCntAutoPtr<IShaderResourceBinding> pSRB;
				pPSO->CreateShaderResourceBinding(&pSRB, true);

				ITexture* pBaseColorTex = nullptr;
				ITexture* pPhysDescTex = nullptr;
				if (Material.workflow == GLTF::Material::PbrWorkflow::MetallicRoughness)
				{
					pBaseColorTex = Material.pBaseColorTexture;
					pPhysDescTex = Material.pMetallicRoughnessTexture;
				}
				else if (Material.workflow == GLTF::Material::PbrWorkflow::SpecularGlossiness)
				{
					pBaseColorTex = Material.extension.pDiffuseTexture;
					pPhysDescTex = Material.extension.pSpecularGlossinessTexture;
				}
				if (!pBaseColorTex)
					pBaseColorTex = Material.pBaseColorTexture;
				auto shv = pSRB->GetVariableByName(SHADER_TYPE_PIXEL, "g_ColorMap");
				auto mtxs = pSRB->GetVariableByName(SHADER_TYPE_VERTEX, "animreco");
				auto inpro = pSRB->GetVariableByName(SHADER_TYPE_VERTEX, "instDatas");
				auto teins = pSRB->GetVariableByName(SHADER_TYPE_VERTEX, "objectTransfromBuffer");
				if (shv)
					shv->Set(pBaseColorTex == NULL ? m_pWhiteTexSRV : pBaseColorTex->GetDefaultView(TEXTURE_VIEW_SHADER_RESOURCE));
				if (teins)
					teins->Set(m_pTransAttribsBuffer->GetDefaultView(BUFFER_VIEW_SHADER_RESOURCE));
				if (mtxs)
					mtxs->Set(m_paniinsBuffer->GetDefaultView(BUFFER_VIEW_SHADER_RESOURCE));

				if (inpro)
					inpro->Set(m_pinspropertysBuffer->GetDefaultView(BUFFER_VIEW_SHADER_RESOURCE));
				//		m_pinspropertysBuffer->CreateView();
				m_SRBs.emplace_back(std::move(pSRB));
				return pSRB;
			}
			void readallnode(const GLTF::Node* nd, std::vector<GLTF::Node*>& st) {

				/*if (nd->_Mesh)
					st.emplace_back(nd);*/
				for (const auto& node : nd->Children)
				{
					if (node->_Mesh)
						st.emplace_back(node.get());
					readallnode(node.get(), st);
				}
			}

			int makeAnimrec(GLTF::Model& GLTFModel, std::vector<math::Matrix4f>& bonerec,
				std::vector<AnimRec>& _AniReco, std::vector<GLTF::Node*>& nodes) {
				float dt = 0;
				sStruct::instData ii;
				auto slice = 0;
				freamcount = 0;
				for (size_t i1 = 0; i1 < nodes.size(); i1++) {
					slice += nodes[i1]->_Mesh->Transforms.jointcount;
				}
				bonerec.reserve(slice * 600);

				AnimRec rec;
				rec.size = 0;
				rec.base = 0;
				rec.base = 0;
				rec.name = "default";
				for (size_t i1 = 0; i1 < nodes.size(); i1++)
				{
					const auto nd1 = nodes[i1];
					rec.bonenum = nd1->_Mesh->Transforms.jointcount;
					const auto st = bonerec.size();
					bonerec.resize(st + rec.bonenum);
					for (size_t i = st; i < rec.bonenum; i++)
					{
						bonerec[i] = math::Matrix4f::Identity();
					}
					//std::memcpy(&bonerec[st], nd1->_Mesh->Transforms.jointMatrix, i2.bonenum * sizeof(math::Matrix4f));
					rec.size += rec.bonenum;
				}
				freamcount++;
				_AniReco.emplace_back(rec);
				for (size_t i = 0; i < GLTFModel.Animations.size(); i++)
				{
					auto& ani = GLTFModel.Animations[i];
					dt = ani.Start;

					auto& i2 = _AniReco.emplace_back();
					i2.base = rec.base + rec.size;
					i2.name = ani.Name;
					while (dt < ani.End)
					{
						GLTFModel.UpdateAnimation(i, dt);
						for (size_t i1 = 0; i1 < nodes.size(); i1++)
						{
							const auto nd1 = nodes[i1];
							i2.bonenum = nd1->_Mesh->Transforms.jointcount;
							if (i2.bonenum == 0) {
								continue;
							}
							const auto st = bonerec.size();
							bonerec.resize(st + i2.bonenum);
							std::memcpy(&bonerec[st], nd1->_Mesh->Transforms.jointMatrix, i2.bonenum * sizeof(math::Matrix4f));
							i2.size += i2.bonenum;
						}

						dt += Samplerate;
						freamcount++;
					}
					rec = i2;
				}
				return slice;
			}
			RefCntAutoPtr<IBuffer>                     m_InsCbuf;
			uint freamcount;
			RefCntAutoPtr<IBuffer>                m_pTransAttribsBuffer;
			RefCntAutoPtr<IBuffer>                m_pinspropertysBuffer;

			std::unique_ptr<GLTF::Model>          m_Model;
			std::vector<Uint32>                                m_PSOIndex;
			std::vector<RefCntAutoPtr<IPipelineState>>         m_RenderMeshPSO;
			std::vector<RefCntAutoPtr<IPipelineState>>         m_RenderMeshShadowPSO;
			std::vector<RefCntAutoPtr<IShaderResourceBinding>> m_SRBs;
			std::vector<RefCntAutoPtr<IShaderResourceBinding>> m_ShadowSRBs;

			void createbuffer(nonstd::span<math::Matrix4f> anims, const int instancenum = 1) {

				std::vector<StateTransitionDesc> Barriers;

				BufferDesc BuffDesc;
				BuffDesc.Name = "transform buffer";
				BuffDesc.Usage = USAGE_DYNAMIC;
				BuffDesc.BindFlags = BIND_SHADER_RESOURCE;
				BuffDesc.Mode = BUFFER_MODE_STRUCTURED;
				BuffDesc.CPUAccessFlags = CPU_ACCESS_WRITE;
				BuffDesc.ElementByteStride = sizeof(mtransfrom);
				BuffDesc.Size = sizeof(mtransfrom) * instancenum;
				m_pDevice->CreateBuffer(BuffDesc, nullptr, &m_pTransAttribsBuffer);

				Barriers.emplace_back(m_pTransAttribsBuffer, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_SHADER_RESOURCE, STATE_TRANSITION_FLAG_UPDATE_STATE);
				{
					BufferDesc BuffDesc;
					BuffDesc.Name = "animation matrix buffer";
					BuffDesc.Usage = Diligent::USAGE::USAGE_IMMUTABLE;
					BuffDesc.BindFlags = BIND_SHADER_RESOURCE;
					BuffDesc.Mode = BUFFER_MODE_STRUCTURED;
					//BuffDesc.CPUAccessFlags = CPU_ACCESS_WRITE;
					BuffDesc.ElementByteStride = sizeof(anims[0]);
					BuffDesc.Size = anims.size_bytes();
					BufferData Data(anims.data(), BuffDesc.Size);
					m_pDevice->CreateBuffer(BuffDesc, &Data, &m_paniinsBuffer);
					Barriers.emplace_back(m_paniinsBuffer, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_SHADER_RESOURCE, STATE_TRANSITION_FLAG_UPDATE_STATE);
				}
				using tt = sStruct::instData;
				BuffDesc.Name = "perinstance buffer";
				BuffDesc.Usage = USAGE_DYNAMIC;
				BuffDesc.BindFlags = BIND_SHADER_RESOURCE;
				BuffDesc.Mode = BUFFER_MODE_STRUCTURED;
				BuffDesc.CPUAccessFlags = CPU_ACCESS_WRITE;
				BuffDesc.ElementByteStride = sizeof(tt);
				BuffDesc.Size = sizeof(tt) * instancenum;
				m_pDevice->CreateBuffer(BuffDesc, nullptr, &m_pinspropertysBuffer);
				Barriers.emplace_back(m_pinspropertysBuffer, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_SHADER_RESOURCE, STATE_TRANSITION_FLAG_UPDATE_STATE);

				CreateUniformBuffer(m_pDevice, sizeof(cdata), "GLTF node transforms CB", &m_InsCbuf);
				Barriers.emplace_back(m_InsCbuf, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_CONSTANT_BUFFER, STATE_TRANSITION_FLAG_UPDATE_STATE);

				auto& pCtx = m_pImmediateContext;
				IBuffer* pVBs[] = { m_Model->pVertexBuffer[0],m_Model->pVertexBuffer[1] };
				Barriers.emplace_back(pVBs[0], RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_VERTEX_BUFFER,STATE_TRANSITION_FLAG_UPDATE_STATE);
				Barriers.emplace_back(pVBs[1], RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_VERTEX_BUFFER,STATE_TRANSITION_FLAG_UPDATE_STATE);
				m_pImmediateContext->TransitionResourceStates(static_cast<Uint32>(Barriers.size()), Barriers.data());
			}
			void Render(math::uint instancenum = 1) {
				GLTF::Model& GLTFModel = *m_Model;
				auto& pCtx = m_pImmediateContext;

				IBuffer* pVBs[] = { GLTFModel.pVertexBuffer[0], GLTFModel.pVertexBuffer[1] };
				Uint64   Offsets[_countof(pVBs)] = {};
				pCtx->SetVertexBuffers(0, _countof(pVBs), pVBs, Offsets, RESOURCE_STATE_TRANSITION_MODE_VERIFY, SET_VERTEX_BUFFERS_FLAG_NONE);
				if (GLTFModel.pIndexBuffer)
				{
					pCtx->SetIndexBuffer(GLTFModel.pIndexBuffer, 0, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
				}
				uint c = 0;

				StateTransitionDesc Barriers[] =
				{
					{m_pTransAttribsBuffer,  RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_SHADER_RESOURCE, STATE_TRANSITION_FLAG_UPDATE_STATE},
					{m_pinspropertysBuffer, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_SHADER_RESOURCE, STATE_TRANSITION_FLAG_UPDATE_STATE}
				};
				// clang-format on
				pCtx->TransitionResourceStates(_countof(Barriers), Barriers);

				/*for (size_t i = 0; i < submeshnodes.size(); i++)
				{

					pCtx->SetPipelineState(m_RenderMeshPSO[0]);
					const auto mesh = submeshnodes[i]->_Mesh->Transforms;
					{
						MapHelper<cdata> InsCbuf(m_pImmediateContext, m_InsCbuf, MAP_WRITE, MAP_FLAG_DISCARD);
						InsCbuf[0].base = c;
						InsCbuf[0].frameSize = slice;
					}
					c += mesh.jointcount;
					for (const auto& primitive : submeshnodes[i]->_Mesh->Primitives) {
						pCtx->CommitShaderResources(m_SRBs[0], RESOURCE_STATE_TRANSITION_MODE_VERIFY);
						if (primitive->hasIndices) {
							DrawIndexedAttribs drawAttrs(primitive->IndexCount, VT_UINT32, DRAW_FLAG_VERIFY_ALL, instancenum);
							drawAttrs.FirstIndexLocation = primitive->FirstIndex;
							pCtx->DrawIndexed(drawAttrs);
						}
						else {

							DrawAttribs drawAttrs(primitive->VertexCount, DRAW_FLAG_VERIFY_ALL);
							pCtx->Draw(drawAttrs);
						}
					}
				}*/
			}
			std::vector<AnimRec> _AniReco;
		protected:
			RefCntAutoPtr<IRenderDevice> m_pDevice;
			RefCntAutoPtr<IDeviceContext> m_pImmediateContext;
			RefCntAutoPtr<IEngineFactory>              m_pEngineFactory;
			math::uint freamindex, animid, slice;
			RefCntAutoPtr<IBuffer>                m_paniinsBuffer;
			std::unordered_map<ui64, IShaderResourceBinding*> mtmp;

		};
		export struct insRander2 :Render
		{
		protected:
			math::uint IndexCount;
			RefCntAutoPtr<IRenderDevice> m_pDevice;
			RefCntAutoPtr<IDeviceContext> m_pImmediateContext;
			RefCntAutoPtr<IBuffer>                m_VertexBuffer;
			RefCntAutoPtr<IBuffer>                m_IndexBuffer;
			RefCntAutoPtr<IPipelineState> _pPSO;
			RefCntAutoPtr<IShaderResourceBinding> m_pSRB;
			POBs<> _pobs[10];
		public:

			RefCntAutoPtr<IBuffer>                m_pTransAttribsBuffer;
			RefCntAutoPtr<IBuffer>                m_pinspropertysBuffer;
			RefCntAutoPtr<IBuffer>                M_pIndirectArgsBuff;
			insRander2(
				RefCntAutoPtr<IRenderDevice>& m_pDevice,
				RefCntAutoPtr<IDeviceContext>& m_pImmediateContext,
				TEXTURE_FORMAT RenderTargetFormat,
				TEXTURE_FORMAT DepthBufferFormat,
				IBuffer* pCameraAttribs,
				RefCntAutoPtr<IBuffer>                m_VertexBuffer,
				RefCntAutoPtr<IBuffer>                m_IndexBuffer,
				RefCntAutoPtr<IShaderSourceInputStreamFactory>& pShaderSourceFactory,
				const uint IndexCount,
				const int instancenum = 1
			) :m_pDevice(m_pDevice), m_pImmediateContext(m_pImmediateContext),
				m_VertexBuffer(m_VertexBuffer), m_IndexBuffer(m_IndexBuffer),
				IndexCount(IndexCount) {

				auto shaderpath = loader::Get(shader);

				ShaderResourceVariableDesc Vars[] =
				{
					{SHADER_TYPE_PIXEL, "g_ColorMap", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE},
					{SHADER_TYPE_PIXEL, "cell", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE},
					{SHADER_TYPE_PIXEL, "map", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE},
					{SHADER_TYPE_VERTEX, "animreco", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE},
					{SHADER_TYPE_VERTEX, "instDatas", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE},
					{SHADER_TYPE_VERTEX, "objectTransfromBuffer", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE}
				};

				LayoutElement inputDesc[] =
				{
				 {0, 0, 3, VT_FLOAT32},   //float3 Pos     : ATTRIB0;
				 {1, 0, 3, VT_FLOAT32},   //float3 Normal  : ATTRIB1;
				 {2, 0, 2, VT_FLOAT32},   //float2 UV0     : ATTRIB2;
				 {3, 0, 2, VT_FLOAT32},   //float2 UV1     : ATTRIB3;
				};
				GIns::CreatePipelineStates
				(&_pPSO, m_pDevice, RenderTargetFormat, DepthBufferFormat, pShaderSourceFactory,
					"NoSkin.hlsl", "vert", "frag", Vars, inputDesc);

				Init(m_pDevice, m_pImmediateContext);
				{
					std::vector<StateTransitionDesc> Barriers;

					BufferDesc BuffDesc;
					BuffDesc.Name = "transform buffer insRander2";
					BuffDesc.Usage = USAGE_DYNAMIC;
					BuffDesc.BindFlags = BIND_SHADER_RESOURCE;
					BuffDesc.Mode = BUFFER_MODE_STRUCTURED;
					BuffDesc.CPUAccessFlags = CPU_ACCESS_WRITE;
					BuffDesc.ElementByteStride = sizeof(mtransfrom);
					BuffDesc.Size = sizeof(mtransfrom) * instancenum;
					m_pDevice->CreateBuffer(BuffDesc, nullptr, &m_pTransAttribsBuffer);
					Barriers.emplace_back(m_pTransAttribsBuffer, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_SHADER_RESOURCE, STATE_TRANSITION_FLAG_UPDATE_STATE);
					using tt = sStruct::instData1;
					BuffDesc.Name = "perinstance buffer";
					BuffDesc.Usage = USAGE_DYNAMIC;
					BuffDesc.BindFlags = BIND_SHADER_RESOURCE;
					BuffDesc.Mode = BUFFER_MODE_STRUCTURED;
					BuffDesc.CPUAccessFlags = CPU_ACCESS_WRITE;
					BuffDesc.ElementByteStride = sizeof(tt);
					BuffDesc.Size = sizeof(tt) * instancenum;
					m_pDevice->CreateBuffer(BuffDesc, nullptr, &m_pinspropertysBuffer);
					Barriers.emplace_back(m_pinspropertysBuffer, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_SHADER_RESOURCE, STATE_TRANSITION_FLAG_UPDATE_STATE);

					auto& pCtx = m_pImmediateContext;
					IBuffer* pVBs[] = { m_VertexBuffer };
					Barriers.emplace_back(pVBs[0], RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_VERTEX_BUFFER,STATE_TRANSITION_FLAG_UPDATE_STATE);
					Barriers.emplace_back(m_IndexBuffer, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_INDEX_BUFFER, STATE_TRANSITION_FLAG_UPDATE_STATE);
					m_pImmediateContext->TransitionResourceStates(static_cast<Uint32>(Barriers.size()), Barriers.data());
				}
				auto& pPSO = _pPSO;

				pPSO->GetStaticVariableByName(SHADER_TYPE_VERTEX, "cbCameraAttribs")->Set(pCameraAttribs);
				pPSO->CreateShaderResourceBinding(&m_pSRB, true);

				ITexture* pPhysDescTex = nullptr;
				auto shv = m_pSRB->GetVariableByName(SHADER_TYPE_PIXEL, "g_ColorMap");
				auto inpro = m_pSRB->GetVariableByName(SHADER_TYPE_VERTEX, "instDatas");
				auto teins = m_pSRB->GetVariableByName(SHADER_TYPE_VERTEX, "objectTransfromBuffer");
				if (shv)
					shv->Set(m_pWhiteTexSRV);
				teins->Set(m_pTransAttribsBuffer->GetDefaultView(BUFFER_VIEW_SHADER_RESOURCE));
				inpro->Set(m_pinspropertysBuffer->GetDefaultView(BUFFER_VIEW_SHADER_RESOURCE));

				BufferDesc BuffDesc;
				BuffDesc.Name = "IndirectDrawArgsBuffer";
				BuffDesc.BindFlags = BIND_INDIRECT_DRAW_ARGS;
				BuffDesc.ElementByteStride = sizeof(pj2::IndirectArgs);
				BuffDesc.Size = BuffDesc.ElementByteStride * 128;
				/*BufferData InitialData;
				InitialData.pData = DrawArgs.data();
				InitialData.DataSize = BuffDesc.Size;*/

				m_pDevice->CreateBuffer(BuffDesc, NULL, &M_pIndirectArgsBuff);
				/*if (m_pinsBuffer)
					initGarg(RenderTargetFormat, DepthBufferFormat,pCameraAttribs, pEngineFactory, IndexCount, m_pinsBuffer);*/
			}

			insRander2(RenderContext& rt,
				const RefCntAutoPtr<IBuffer>&                m_VertexBuffer_,
				const RefCntAutoPtr<IBuffer>&                m_IndexBuffer_,
				const uint IndexCount,
				const int instancenum = 1
			) :m_pDevice(rt.m_pDevice), m_pImmediateContext(rt.m_pImmediateContext),
				m_VertexBuffer(m_VertexBuffer_), m_IndexBuffer(m_IndexBuffer_),
				IndexCount(IndexCount) {
				const auto& RenderTargetFormat = rt.RenderTargetFormat;
				const auto& DepthBufferFormat = rt.DepthBufferFormat;
				auto& pShaderSourceFactory = rt.pShaderSourceFactory;
				auto& pCameraAttribs = rt.pCameraAttribs;
				auto shaderpath = loader::Get(shader);
				ShaderResourceVariableDesc Vars[] =
				{
					{SHADER_TYPE_PIXEL, "g_ColorMap", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE},
					{SHADER_TYPE_PIXEL, "cell", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE},
					{SHADER_TYPE_PIXEL, "map", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE},
					{SHADER_TYPE_VERTEX, "animreco", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE},
					{SHADER_TYPE_VERTEX, "instDatas", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE},
					{SHADER_TYPE_VERTEX, "objectTransfromBuffer", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE}
				};

				LayoutElement inputDesc[] =
				{
				 {0, 0, 3, VT_FLOAT32},   //float3 Pos     : ATTRIB0;
				 {1, 0, 3, VT_FLOAT32},   //float3 Normal  : ATTRIB1;
				 {2, 0, 2, VT_FLOAT32},   //float2 UV0     : ATTRIB2;
				 {3, 0, 2, VT_FLOAT32},   //float2 UV1     : ATTRIB3;
				};
				GIns::CreatePipelineStates
				(&_pPSO, m_pDevice, RenderTargetFormat, DepthBufferFormat, pShaderSourceFactory,
					"NoSkin.hlsl", "vert", "frag", Vars, inputDesc);

				Init(m_pDevice, m_pImmediateContext);
				{
					std::vector<StateTransitionDesc> Barriers;

					BufferDesc BuffDesc;
					BuffDesc.Name = "transform buffer insRander2";
					BuffDesc.Usage = USAGE_DYNAMIC;
					BuffDesc.BindFlags = BIND_SHADER_RESOURCE;
					BuffDesc.Mode = BUFFER_MODE_STRUCTURED;
					BuffDesc.CPUAccessFlags = CPU_ACCESS_WRITE;
					BuffDesc.ElementByteStride = sizeof(mtransfrom);
					BuffDesc.Size = sizeof(mtransfrom) * instancenum;
					m_pDevice->CreateBuffer(BuffDesc, nullptr, &m_pTransAttribsBuffer);
					Barriers.emplace_back(m_pTransAttribsBuffer, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_SHADER_RESOURCE, STATE_TRANSITION_FLAG_UPDATE_STATE);
					using tt = sStruct::instData1;
					BuffDesc.Name = "perinstance buffer";
					BuffDesc.Usage = USAGE_DYNAMIC;
					BuffDesc.BindFlags = BIND_SHADER_RESOURCE;
					BuffDesc.Mode = BUFFER_MODE_STRUCTURED;
					BuffDesc.CPUAccessFlags = CPU_ACCESS_WRITE;
					BuffDesc.ElementByteStride = sizeof(tt);
					BuffDesc.Size = sizeof(tt) * instancenum;
					m_pDevice->CreateBuffer(BuffDesc, nullptr, &m_pinspropertysBuffer);
					Barriers.emplace_back(m_pinspropertysBuffer, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_SHADER_RESOURCE, STATE_TRANSITION_FLAG_UPDATE_STATE);

					auto& pCtx = m_pImmediateContext;
					IBuffer* pVBs[] = { m_VertexBuffer };
					Barriers.emplace_back(pVBs[0], RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_VERTEX_BUFFER, STATE_TRANSITION_FLAG_UPDATE_STATE);
					Barriers.emplace_back(m_IndexBuffer, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_INDEX_BUFFER, STATE_TRANSITION_FLAG_UPDATE_STATE);
					m_pImmediateContext->TransitionResourceStates(static_cast<Uint32>(Barriers.size()), Barriers.data());
				}
				auto& pPSO = _pPSO;

				pPSO->GetStaticVariableByName(SHADER_TYPE_VERTEX, "cbCameraAttribs")->Set(pCameraAttribs);
				pPSO->CreateShaderResourceBinding(&m_pSRB, true);

				ITexture* pPhysDescTex = nullptr;
				auto shv = m_pSRB->GetVariableByName(SHADER_TYPE_PIXEL, "g_ColorMap");
				auto inpro = m_pSRB->GetVariableByName(SHADER_TYPE_VERTEX, "instDatas");
				auto teins = m_pSRB->GetVariableByName(SHADER_TYPE_VERTEX, "objectTransfromBuffer");
				if (shv)
					shv->Set(m_pWhiteTexSRV);
				teins->Set(m_pTransAttribsBuffer->GetDefaultView(BUFFER_VIEW_SHADER_RESOURCE));
				inpro->Set(m_pinspropertysBuffer->GetDefaultView(BUFFER_VIEW_SHADER_RESOURCE));

				BufferDesc BuffDesc;
				BuffDesc.Name = "IndirectDrawArgsBuffer";
				BuffDesc.BindFlags = BIND_INDIRECT_DRAW_ARGS;
				BuffDesc.ElementByteStride = sizeof(pj2::IndirectArgs);
				BuffDesc.Size = BuffDesc.ElementByteStride * 128;

				m_pDevice->CreateBuffer(BuffDesc, NULL, &M_pIndirectArgsBuff);
			}

			insRander2(RenderContext& rt,
				const RefCntAutoPtr<IBuffer>& m_VertexBuffer_,
				const RefCntAutoPtr<IBuffer>& m_IndexBuffer_,
				const RefCntAutoPtr<IBuffer>& trBuffer_,
				const RefCntAutoPtr<IBuffer>& insBuffer_,
				const uint IndexCount,
				const int instancenum = 1
			) :m_pDevice(rt.m_pDevice), m_pImmediateContext(rt.m_pImmediateContext),
				m_VertexBuffer(m_VertexBuffer_), m_IndexBuffer(m_IndexBuffer_),
				IndexCount(IndexCount) {
				const auto& RenderTargetFormat = rt.RenderTargetFormat;
				const auto& DepthBufferFormat = rt.DepthBufferFormat;
				auto& pShaderSourceFactory = rt.pShaderSourceFactory;
				auto& pCameraAttribs = rt.pCameraAttribs;
				auto shaderpath = loader::Get(shader);
				ShaderResourceVariableDesc Vars[] =
				{
					{SHADER_TYPE_PIXEL, "g_ColorMap", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE},
					{SHADER_TYPE_PIXEL, "cell", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE},
					{SHADER_TYPE_PIXEL, "map", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE},
					{SHADER_TYPE_VERTEX, "animreco", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE},
					{SHADER_TYPE_VERTEX, "instDatas", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE},
					{SHADER_TYPE_VERTEX, "objectTransfromBuffer", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE}
				};

				LayoutElement inputDesc[] =
				{
				 {0, 0, 3, VT_FLOAT32},   //float3 Pos     : ATTRIB0;
				 {1, 0, 3, VT_FLOAT32},   //float3 Normal  : ATTRIB1;
				 {2, 0, 2, VT_FLOAT32},   //float2 UV0     : ATTRIB2;
				 {3, 0, 2, VT_FLOAT32},   //float2 UV1     : ATTRIB3;
				};
				GIns::CreatePipelineStates
				(&_pPSO, m_pDevice, RenderTargetFormat, DepthBufferFormat, pShaderSourceFactory,
					"NoSkin.hlsl", "vert", "frag", Vars, inputDesc);

				Init(m_pDevice, m_pImmediateContext);
				{
					std::vector<StateTransitionDesc> Barriers;
					m_pTransAttribsBuffer = trBuffer_;
					m_pinspropertysBuffer = insBuffer_;
					Barriers.emplace_back(m_pTransAttribsBuffer, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_SHADER_RESOURCE, STATE_TRANSITION_FLAG_UPDATE_STATE);
					Barriers.emplace_back(m_pinspropertysBuffer, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_SHADER_RESOURCE, STATE_TRANSITION_FLAG_UPDATE_STATE);

					auto& pCtx = m_pImmediateContext;
					IBuffer* pVBs[] = { m_VertexBuffer };
					Barriers.emplace_back(pVBs[0], RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_VERTEX_BUFFER, STATE_TRANSITION_FLAG_UPDATE_STATE);
					Barriers.emplace_back(m_IndexBuffer, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_INDEX_BUFFER, STATE_TRANSITION_FLAG_UPDATE_STATE);
					m_pImmediateContext->TransitionResourceStates(static_cast<Uint32>(Barriers.size()), Barriers.data());
				}
				auto& pPSO = _pPSO;

				pPSO->GetStaticVariableByName(SHADER_TYPE_VERTEX, "cbCameraAttribs")->Set(pCameraAttribs);
				pPSO->CreateShaderResourceBinding(&m_pSRB, true);

				ITexture* pPhysDescTex = nullptr;
				auto shv = m_pSRB->GetVariableByName(SHADER_TYPE_PIXEL, "g_ColorMap");
				auto inpro = m_pSRB->GetVariableByName(SHADER_TYPE_VERTEX, "instDatas");
				auto teins = m_pSRB->GetVariableByName(SHADER_TYPE_VERTEX, "objectTransfromBuffer");
				if (shv)
					shv->Set(m_pWhiteTexSRV);
				teins->Set(m_pTransAttribsBuffer->GetDefaultView(BUFFER_VIEW_SHADER_RESOURCE));
				inpro->Set(m_pinspropertysBuffer->GetDefaultView(BUFFER_VIEW_SHADER_RESOURCE));

				BufferDesc BuffDesc;
				BuffDesc.Name = "IndirectDrawArgsBuffer";
				BuffDesc.BindFlags = BIND_INDIRECT_DRAW_ARGS;
				BuffDesc.ElementByteStride = sizeof(pj2::IndirectArgs);
				BuffDesc.Size = BuffDesc.ElementByteStride * 128;

				m_pDevice->CreateBuffer(BuffDesc, NULL, &M_pIndirectArgsBuff);
			}
			inline RefCntAutoPtr<IBuffer> CreateIndirectDrawArgsBuffer(nonstd::span<uint> DrawArgs)
			{
				BufferDesc BuffDesc;
				BuffDesc.Name = "IndirectDrawArgsBuffer";
				BuffDesc.BindFlags = BIND_INDIRECT_DRAW_ARGS;
				BuffDesc.Size = DrawArgs.size_bytes();

				BufferData InitialData;
				InitialData.pData = DrawArgs.data();
				InitialData.DataSize = BuffDesc.Size;

				RefCntAutoPtr<IBuffer> pBuffer;
				m_pDevice->CreateBuffer(BuffDesc, &InitialData, &pBuffer);
				return pBuffer;
			}

			void Render(math::uint instancenum = 1, int instanceoffset = 0) {
				if (instancenum == 0)
					return;
				auto& pCtx = m_pImmediateContext;

				IBuffer* pVBs[] = { m_VertexBuffer };
				Uint64   Offsets[_countof(pVBs)] = {};
				StateTransitionDesc Barriers[] =
				{
					{m_pTransAttribsBuffer,  RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_SHADER_RESOURCE, STATE_TRANSITION_FLAG_UPDATE_STATE},
					{m_pinspropertysBuffer, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_SHADER_RESOURCE, STATE_TRANSITION_FLAG_UPDATE_STATE},
					/*{m_VertexBuffer, RESOURCE_STATE_TRANSITION_MODE_VERIFY, RESOURCE_STATE_SHADER_RESOURCE, STATE_TRANSITION_FLAG_UPDATE_STATE},
					{m_pinspropertysBuffer, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_SHADER_RESOURCE, STATE_TRANSITION_FLAG_UPDATE_STATE}*/
				};
				pCtx->SetPipelineState(_pPSO);
				// clang-format on
				pCtx->TransitionResourceStates(_countof(Barriers), Barriers);
				pCtx->SetVertexBuffers(0, _countof(pVBs), pVBs, Offsets, RESOURCE_STATE_TRANSITION_MODE_VERIFY, SET_VERTEX_BUFFERS_FLAG_NONE);
				pCtx->SetIndexBuffer(m_IndexBuffer, 0, RESOURCE_STATE_TRANSITION_MODE_VERIFY);

				pCtx->CommitShaderResources(m_pSRB, RESOURCE_STATE_TRANSITION_MODE_VERIFY);
				DrawIndexedAttribs drawAttrs(IndexCount, VT_UINT32, DRAW_FLAG_VERIFY_ALL, instancenum, 0, 0, instanceoffset);
				drawAttrs.FirstIndexLocation = 0;
				pCtx->DrawIndexed(drawAttrs);

			}

			void IndirectDraw(std::vector<IndirectArgs>& DrawArgs) {

				if (DrawArgs.size() == 0)
					return;
				auto& pCtx = m_pImmediateContext;

				if (DrawArgs[0].NumIndices == -1)
					for (size_t i = 0; i < DrawArgs.size(); i++)
					{
						DrawArgs[i].NumIndices = IndexCount;
					}
				pCtx->UpdateBuffer(M_pIndirectArgsBuff, 0, sizeof(IndirectArgs) * DrawArgs.size(), DrawArgs.data(), RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
				IBuffer* pVBs[] = { m_VertexBuffer };
				Uint64   Offsets[_countof(pVBs)] = {};
				StateTransitionDesc Barriers[] =
				{
					{m_pTransAttribsBuffer,  RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_SHADER_RESOURCE, STATE_TRANSITION_FLAG_UPDATE_STATE},
					{m_pinspropertysBuffer, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_SHADER_RESOURCE, STATE_TRANSITION_FLAG_UPDATE_STATE},
					/*{m_VertexBuffer, RESOURCE_STATE_TRANSITION_MODE_VERIFY, RESOURCE_STATE_SHADER_RESOURCE, STATE_TRANSITION_FLAG_UPDATE_STATE}
					{m_pinspropertysBuffer, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_SHADER_RESOURCE, STATE_TRANSITION_FLAG_UPDATE_STATE}*/
				};
				pCtx->SetPipelineState(_pPSO);
				// clang-format on
				pCtx->TransitionResourceStates(_countof(Barriers), Barriers);
				pCtx->SetVertexBuffers(0, _countof(pVBs), pVBs, Offsets, RESOURCE_STATE_TRANSITION_MODE_VERIFY, SET_VERTEX_BUFFERS_FLAG_NONE);
				pCtx->SetIndexBuffer(m_IndexBuffer, 0, RESOURCE_STATE_TRANSITION_MODE_VERIFY);

				pCtx->CommitShaderResources(m_pSRB, RESOURCE_STATE_TRANSITION_MODE_VERIFY);
				DrawIndexedIndirectAttribs drawAttrs{ VT_UINT32,M_pIndirectArgsBuff, DRAW_FLAG_VERIFY_ALL, RESOURCE_STATE_TRANSITION_MODE_TRANSITION};
				drawAttrs.DrawArgsStride = sizeof(IndirectArgs);
				drawAttrs.DrawCount = DrawArgs.size();
				pCtx->DrawIndexedIndirect(drawAttrs);

			}
			void initGarg(
				TEXTURE_FORMAT RenderTargetFormat,
				TEXTURE_FORMAT DepthBufferFormat,
				IBuffer* pCameraAttribs,
				RefCntAutoPtr<IShaderSourceInputStreamFactory> pShaderSourceFactory,
				const uint IndexCount,
				RefCntAutoPtr<IBuffer>                m_InsBuffer,
				const int instancenum = 1)
			{

				auto shaderpath = loader::Get(shader);

				ShaderResourceVariableDesc Vars[] =
				{
					{SHADER_TYPE_PIXEL, "g_ColorMap", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE},
					{SHADER_TYPE_PIXEL, "cell", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE},
					{SHADER_TYPE_PIXEL, "map", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE},
					{SHADER_TYPE_VERTEX, "animreco", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE},
					{SHADER_TYPE_VERTEX, "instDatas", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE},
					{SHADER_TYPE_VERTEX, "objectTransfromBuffer", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE}
				};

				LayoutElement inputDesc[] =
				{
				 {0, 0, 3, VT_FLOAT32},   //float3 Pos     : ATTRIB0;
				 {1, 0, 3, VT_FLOAT32},   //float3 Normal  : ATTRIB1;
				 {2, 0, 2, VT_FLOAT32},   //float2 UV0     : ATTRIB2;
				 {3, 0, 2, VT_FLOAT32},   //float2 UV1     : ATTRIB3;
				};
				GIns::CreatePipelineStates
				(&(_pobs[0].PSO), m_pDevice, RenderTargetFormat, DepthBufferFormat, pShaderSourceFactory, "NoSkin.hlsl", "vert1", "frag", Vars, inputDesc);
				Init(m_pDevice, m_pImmediateContext);
				{
					std::vector<StateTransitionDesc> Barriers;

					auto& pCtx = m_pImmediateContext;
					IBuffer* pVBs[] = { m_VertexBuffer };
					Barriers.emplace_back(pVBs[0], RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_VERTEX_BUFFER,STATE_TRANSITION_FLAG_UPDATE_STATE);
					Barriers.emplace_back(m_IndexBuffer, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_INDEX_BUFFER, STATE_TRANSITION_FLAG_UPDATE_STATE);
					m_pImmediateContext->TransitionResourceStates(static_cast<Uint32>(Barriers.size()), Barriers.data());
				}
				_pobs[0].PSO->GetStaticVariableByName(SHADER_TYPE_VERTEX, "cbCameraAttribs")->Set(pCameraAttribs);
				_pobs[0].CreateShaderResourceBinding();
				_pobs[0].GetShaderResourceBinding(0)->GetVariableByName(SHADER_TYPE_VERTEX, "instDatas")->Set(m_InsBuffer->GetDefaultView(BUFFER_VIEW_SHADER_RESOURCE));

			}
			template<class ArgStruct = sStruct::VFoutArgs>
			void IndirectDraw(const nonstd::span<ArgStruct> DrawArgs, RefCntAutoPtr<IBuffer>& pIndirectArgsBuff) {
				static std::vector<size_t> ArgPos;
				ArgPos.clear();
				for (size_t i = 0; i < DrawArgs.size(); i++)
				{
					if (DrawArgs[i].NumInstances == 0)
						continue;
					ArgPos.emplace_back(i * sizeof(ArgStruct));
				}
				if (ArgPos.empty())
					return;
				auto& pCtx = m_pImmediateContext;

				IBuffer* pVBs[] = { m_VertexBuffer };
				Uint64   Offsets[_countof(pVBs)] = {};
				//StateTransitionDesc Barriers[] =
				//{
				//	{m_pTransAttribsBuffer,  RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_SHADER_RESOURCE, STATE_TRANSITION_FLAG_UPDATE_STATE},
				//	{m_pinspropertysBuffer, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_SHADER_RESOURCE, STATE_TRANSITION_FLAG_UPDATE_STATE},
				//	/*{m_VertexBuffer, RESOURCE_STATE_TRANSITION_MODE_VERIFY, RESOURCE_STATE_SHADER_RESOURCE, STATE_TRANSITION_FLAG_UPDATE_STATE}
				//	{m_pinspropertysBuffer, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_SHADER_RESOURCE, STATE_TRANSITION_FLAG_UPDATE_STATE}*/
				//};

				pCtx->SetPipelineState(_pobs->PSO);
				// clang-format on
				//pCtx->TransitionResourceStates(_countof(Barriers), Barriers);
				pCtx->SetVertexBuffers(0, _countof(pVBs), pVBs, Offsets, RESOURCE_STATE_TRANSITION_MODE_VERIFY, SET_VERTEX_BUFFERS_FLAG_NONE);
				pCtx->SetIndexBuffer(m_IndexBuffer, 0, RESOURCE_STATE_TRANSITION_MODE_VERIFY);

				pCtx->CommitShaderResources(_pobs->SRBs[0], RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
				DrawIndexedIndirectAttribs drawAttrs{ VT_UINT32, DRAW_FLAG_VERIFY_ALL, RESOURCE_STATE_TRANSITION_MODE_TRANSITION };
				for (size_t i = 0; i < ArgPos.size(); i++)
				{

					drawAttrs.DrawArgsOffset = ArgPos[i];
					pCtx->DrawIndexedIndirect(drawAttrs, pIndirectArgsBuff);
				}

			}
		};
	}
	using n_Render::InstanceRender;
	using n_Render::insRander2;
	//export
	export class FowRender
	{
		static constexpr int deltaChunks = (1024 * 32);
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
		void init2(
			RefCntAutoPtr<IRenderDevice>& m_pDevice) {

		}
	public:
		FowRender() {}
		//FowRender(
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
			const std::string FilePath,
			const char* ven,
			const char* pen)
		{
			GraphicsPipelineStateCreateInfo RTPSOCreateInfo;

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
			// clang-format on

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
		RefCntAutoPtr<IBuffer>                m_VFArg;
		RefCntAutoPtr<IBuffer>                m_VFArgR;
		RefCntAutoPtr<IBuffer>                m_VFArglocal;
		RefCntAutoPtr<IBuffer>                m_Vresi;
		RefCntAutoPtr<IBuffer>                m_vfProps;
		int VfArgREle = 512;
		//span<sStruct::VFoutArgs> dspan = {};
		void UpdateVFoutArgs(span<sStruct::VFoutArgs> VfArgspan,
			RefCntAutoPtr<IDeviceContext> m_pImmediateContext,
			Uint32 offset = 0) {
			m_pImmediateContext->UpdateBuffer(m_VFArg, offset, VfArgspan.size_bytes(), VfArgspan.data(), Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
		}
		void init(
			RefCntAutoPtr<IRenderDevice> m_pDevice,
			RefCntAutoPtr<IDeviceContext> m_pImmediateContext,
			RefCntAutoPtr<ISwapChain> pSwapChain,
			//IBuffer* pCameraAttribs,
			//RefCntAutoPtr<IBuffer>                m_VertexBuffer,
			//RefCntAutoPtr<IBuffer>                m_IndexBuffer,
			RefCntAutoPtr<IEngineFactory>              pEngineFactory,
			ITextureView* tview,
			span<sStruct::VFoutArgs> VfArgspan = {},
			size_t Maxinstancnum = 16384)
		{
			return;
			RefCntAutoPtr<IBuffer>                m_Shadowqueue;
			RefCntAutoPtr<IBuffer>                m_RWSign;
			RefCntAutoPtr<IBufferView> pRWSignUAV;
			RefCntAutoPtr<IBufferView> pRWBrightUAV;
			this->m_pSwapChain = pSwapChain;
			this->m_pImmediateContext = m_pImmediateContext;
			RefCntAutoPtr<IShaderSourceInputStreamFactory> pShaderSourceFactory;
			RefCntAutoPtr<ITexture> pRTColor, pRTColorfow;
			auto shaderpath = loader::Get(shader);
			RefCntAutoPtr<IBuffer>                ConstantsL;
			pEngineFactory->CreateDefaultShaderSourceStreamFactory(shaderpath.string().c_str(), &pShaderSourceFactory);
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

				/*BuffDesc.Name = "RTPS constants CB";
				BuffDesc.Size = sizeof(sStruct::CameraAttribs);
				BuffDesc.Usage = USAGE_DYNAMIC;
				BuffDesc.BindFlags = BIND_UNIFORM_BUFFER;
				BuffDesc.CPUAccessFlags = CPU_ACCESS_WRITE;
				m_pDevice->CreateBuffer(BuffDesc, nullptr, &m_CamConstants);*/

				CreateUniformBuffer(m_pDevice, 256 * 2, "Camera attribs buffer", &m_CamConstants);

				BuffDesc.Name = "m_brightObjs";
				BuffDesc.ElementByteStride = sizeof(sStruct::objrep);
				BuffDesc.Size = deltaObjs * BuffDesc.ElementByteStride;
				BuffDesc.Usage = USAGE_DYNAMIC;
				BuffDesc.Mode = BUFFER_MODE_STRUCTURED;
				BuffDesc.BindFlags = BIND_UNORDERED_ACCESS;// | BIND_SHADER_RESOURCE;;
				BuffDesc.CPUAccessFlags = CPU_ACCESS_WRITE;
				m_pDevice->CreateBuffer(BuffDesc, nullptr, &m_brightObjs);

				BuffDesc.Name = "m_moveProp DF";
				//BuffDesc.Size = deltaChunks * BuffDesc.ElementByteStride;
				BuffDesc.Usage = USAGE_DEFAULT;
				BuffDesc.BindFlags = BIND_UNORDERED_ACCESS;
				BuffDesc.CPUAccessFlags = CPU_ACCESS_NONE;
				m_pDevice->CreateBuffer(BuffDesc, nullptr, &m_deltaObjsL);

				BuffDesc.Name = "m_moveProp";
				//BuffDesc.Size = deltaChunks;
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

				BuffDesc.Name = "VfArg";
				BuffDesc.ElementByteStride = sizeof(sStruct::VFoutArgs);
				BuffDesc.Size = BuffDesc.ElementByteStride * 4096;
				//BuffDesc.BindFlags = ;
				//BuffDesc.Mode = BUFFER_MODE_STRUCTURED;
				BuffDesc.Usage = USAGE_DEFAULT;
				BuffDesc.BindFlags = BIND_UNORDERED_ACCESS | BIND_INDIRECT_DRAW_ARGS;
				BuffDesc.CPUAccessFlags = CPU_ACCESS_NONE;
				VBData.pData = VfArgspan.data();
				VBData.DataSize = VfArgspan.size_bytes();
				m_pDevice->CreateBuffer(BuffDesc, &VBData, &m_VFArg);


				BuffDesc.Name = "VfArgR";
				BuffDesc.ElementByteStride = sizeof(sStruct::VFoutArgs);;
				BuffDesc.Size = BuffDesc.ElementByteStride * VfArgREle;
				//BuffDesc.BindFlags = ;
				//BuffDesc.Mode = BUFFER_MODE_STRUCTURED;
				BuffDesc.Usage = USAGE_STAGING;
				BuffDesc.BindFlags = BIND_NONE;
				BuffDesc.CPUAccessFlags = CPU_ACCESS_READ;
				m_pDevice->CreateBuffer(BuffDesc, NULL, &m_VFArgR);
				std::vector<StateTransitionDesc> Barriers;
				Barriers.emplace_back(m_VFArgR, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_COPY_DEST, STATE_TRANSITION_FLAG_UPDATE_STATE);

				BuffDesc.Name = "VfOIns";
				BuffDesc.ElementByteStride = sizeof(sStruct::VfProp::insd);
				BuffDesc.Size = Maxinstancnum * BuffDesc.ElementByteStride;
				BuffDesc.Usage = USAGE_DEFAULT;
				BuffDesc.BindFlags = BIND_UNORDERED_ACCESS | BIND_SHADER_RESOURCE;
				BuffDesc.CPUAccessFlags = CPU_ACCESS_NONE;
				m_pDevice->CreateBuffer(BuffDesc, nullptr, &m_Vresi);
				Barriers.emplace_back(m_Vresi, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_SHADER_RESOURCE, STATE_TRANSITION_FLAG_UPDATE_STATE);
				Barriers.emplace_back(m_Vresi, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_UNORDERED_ACCESS, STATE_TRANSITION_FLAG_UPDATE_STATE);
				m_pImmediateContext->TransitionResourceStates(Barriers.size(), Barriers.data());

				BuffDesc.Name = "Vf_Props";
				BuffDesc.ElementByteStride = sizeof(sStruct::VfProp);
				BuffDesc.Size = Maxinstancnum * BuffDesc.ElementByteStride;
				BuffDesc.Usage = USAGE_DYNAMIC;
				BuffDesc.Mode = BUFFER_MODE_STRUCTURED;
				BuffDesc.BindFlags = BIND_UNORDERED_ACCESS | BIND_SHADER_RESOURCE;;
				BuffDesc.CPUAccessFlags = CPU_ACCESS_WRITE;
				m_pDevice->CreateBuffer(BuffDesc, nullptr, &m_vfProps);

				BuffDesc;
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


				RefCntAutoPtr<ITexture> Tex2d;
				TextureDesc TexDesc;
				TexDesc.Type = RESOURCE_DIM_TEX_2D;
				TexDesc.Usage = Diligent::USAGE::USAGE_DEFAULT;
				//TexDesc.Usage = Diligent::USAGE::USAGE_DYNAMIC;
				TexDesc.BindFlags = BIND_SHADER_RESOURCE | BIND_UNORDERED_ACCESS;
				TexDesc.Width = shadowmapsz;
				TexDesc.Height = shadowmapsz;
				TexDesc.Format = TEX_FORMAT_RGBA32_SINT;
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

				TexDesc.Name = "resistance texture";
				Tex2d = NULL;
				//TexDesc.BindFlags = BIND_SHADER_RESOURCE;
				std::vector<math::vec4<int>>     Data(TexDesc.Width * TexDesc.Height, math::vec4<int>(1, 1, 1, 1));
				TextureSubResData       Level0Data{ Data.data(), TexDesc.Width * 16 };
				TextureData             InitData{ &Level0Data, 1 };
				m_pDevice->CreateTexture(TexDesc, NULL, &Tex2d);
				m_resistanceSRV = Tex2d->GetDefaultView(TEXTURE_VIEW_SHADER_RESOURCE);
				m_resistanceUAV = Tex2d->GetDefaultView(TEXTURE_VIEW_UNORDERED_ACCESS);

				TextureDesc RTColorDesc;
				RTColorDesc.Name = "Offscreen render target";
				RTColorDesc.Type = RESOURCE_DIM_TEX_2D;
				RTColorDesc.Width = m_pSwapChain->GetDesc().Width;
				RTColorDesc.Height = m_pSwapChain->GetDesc().Height;
				RTColorDesc.MipLevels = 1;
				RTColorDesc.Format = FowRender::RenderTargetFormat;//m_pSwapChain->GetDesc().ColorBufferFormat;

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
				/*TextureViewDesc.AccessFlags = UAV_ACCESS_FLAG_WRITE;
				Tex2d->CreateView(TextureViewDesc,&m_resistanceClearUAV);*/
				/*BufferDesc CBDesc;
				CBDesc.Name = "m_moveObjs";
				CBDesc.Size = deltaChunks;
				CBDesc.Usage = USAGE_DYNAMIC;
				CBDesc.BindFlags = BIND_UNIFORM_BUFFER;
				CBDesc.CPUAccessFlags = CPU_ACCESS_WRITE;
				m_pDevice->CreateBuffer(CBDesc, nullptr, &m_Moveprop);*/
			}
			/*RefCntAutoPtr<ITexture> pRTColor;
			m_pDevice->CreateTexture(RTColorDesc, nullptr, &pRTColor);
			m_RendeRT = pRTColor->GetDefaultView(TEXTURE_VIEW_RENDER_TARGET);
			m_ShadowSRV = pRTColor->GetDefaultView(TEXTURE_VIEW_SHADER_RESOURCE);*/

			ShaderResourceVariableDesc Vars[] =
			{
				//{SHADER_TYPE_COMPUTE, "queueFB", SHADER_RESOURCE_VARIABLE_TYPE_STATIC},
				{SHADER_TYPE_COMPUTE, "Constants", SHADER_RESOURCE_VARIABLE_TYPE_STATIC},
				{SHADER_TYPE_COMPUTE, "ConstantsL", SHADER_RESOURCE_VARIABLE_TYPE_STATIC},
				{SHADER_TYPE_COMPUTE, "lightMap", SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC},
				{ SHADER_TYPE_COMPUTE, "clearlightMap", SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC },
				{SHADER_TYPE_VERTEX, "Cbuf", SHADER_RESOURCE_VARIABLE_TYPE_STATIC}
			};
			ShaderResourceVariableDesc Varspixel[] =
			{
				//{SHADER_TYPE_PIXEL, "lightMapfow", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE},
				{SHADER_TYPE_PIXEL, "lightMapfow", SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC},
				{SHADER_TYPE_PIXEL, "resistanceMapR", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE},
				{SHADER_TYPE_VERTEX, "Cbuf", SHADER_RESOURCE_VARIABLE_TYPE_STATIC}
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
				{SHADER_TYPE_PIXEL, "fowresult", SampointClampDesc},
				{SHADER_TYPE_PIXEL, "lightMapfow", SamLinearClampDesc}
			};
			//m_RfowPSO->Release();
			CreateRenderTargetPSO(&m_pRTPSO, m_pDevice, m_pSwapChain, pShaderSourceFactory, "fov.hlsl", "fowv1", "fowf1");
			//CreateRenderTargetPSO(&m_RfowPSO, m_pDevice, m_pSwapChain, pShaderSourceFactory, "fov.hlsl", "fowv", "fowf");
			/*const auto RenderTargetFormat = m_pSwapChain->GetDesc().ColorBufferFormat;
			const auto DepthBufferFormat = m_pSwapChain->GetDesc().DepthBufferFormat;*/
			GIns::CreatePipelineStates
			(&m_RfowPSO, m_pDevice, RenderTargetFormat, DepthBufferFormat, pShaderSourceFactory,
				"fov.hlsl", "fowv", "fowf", Varspixel, GIns::defDesc, ImtblSamplers, Diligent::PRIMITIVE_TOPOLOGY::PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
				false, false);
			/*GIns::CreatePipelineStates(&m_RfowPSO, m_pDevice, m_pSwapChain,
				pShaderSourceFactory, "fov.hlsl", "fowv", "fowf", Varspixel, "fowv", "fowf", GIns::defDesc, ImtblSamplers);*/
				//PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP,false,false);

			DrawFunc::CreateComputePSO("move", "ShadowCast.hlsl", "move", m_pDevice, pShaderSourceFactory, m_pMovePSO, Vars);
			DrawFunc::CreateComputePSO("Vfilter", "ShadowCast.hlsl", "Vfilter", m_pDevice, pShaderSourceFactory, Vfilterpsb.PSO, Vars);
			DrawFunc::CreateComputePSO("fov", "CastMain.hlsl", "fov", m_pDevice, pShaderSourceFactory, m_ClightPSO, Vars);
			//Vars[0] = { SHADER_TYPE_COMPUTE, "clearlightMap", SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC };
			DrawFunc::CreateComputePSO("reset", "ShadowCast.hlsl", "reset", m_pDevice, pShaderSourceFactory, m_pResetPSO, Vars);
			//DrawFunc::CreateComputePSO("resetArg", "ShadowCast.hlsl", "resetArg", m_pDevice, pShaderSourceFactory, m_pResetArgPSO, Vars);

			m_ClightPSO->GetStaticVariableByName(SHADER_TYPE_COMPUTE, "Constants")->Set(m_brightcb);
			m_pResetPSO->GetStaticVariableByName(SHADER_TYPE_COMPUTE, "Constants")->Set(m_brightcb);
			Vfilterpsb.PSO->GetStaticVariableByName(SHADER_TYPE_COMPUTE, "Constants")->Set(m_brightcb);
			//m_ClightPSO->GetStaticVariableByName(SHADER_TYPE_COMPUTE, "queueFB")->Set(m_brightcbl);
			//m_ClightPSO->GetStaticVariableByName(SHADER_TYPE_COMPUTE, "RWSign")->Set(m_RWSign);
			m_pMovePSO->GetStaticVariableByName(SHADER_TYPE_COMPUTE, "Constants")->Set(m_brightcb);
			m_RfowPSO->GetStaticVariableByName(SHADER_TYPE_VERTEX, "Cbuf")->Set(m_CamConstants);

			m_pMoveSRB.Release();
			m_plightSRB.Release();
			m_ClearSRV.Release();
			for (size_t i = 0; i < m_ShadowMapSRV.size(); i++)
			{
				m_RfowPSO->CreateShaderResourceBinding(&m_fowSRB[i], true);
			}
			m_ClightPSO->CreateShaderResourceBinding(&m_plightSRB, true);
			m_pMovePSO->CreateShaderResourceBinding(&m_pMoveSRB, true);
			m_pResetPSO->CreateShaderResourceBinding(&m_pResetSRB, true);
			m_pResetPSO->CreateShaderResourceBinding(&m_pReset1SRB, true);
			//m_pResetArgPSO->CreateShaderResourceBinding(&m_pResetArgSRB, true);
			m_pRTPSO->CreateShaderResourceBinding(&m_pRTSRB, true);
			Vfilterpsb.CreateShaderResourceBinding();

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
			m_pResetSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "VfArg")->Set(m_VFArg->GetDefaultView(BUFFER_VIEW_UNORDERED_ACCESS));
			m_pReset1SRB->GetVariableByName(SHADER_TYPE_COMPUTE, "VfArg")->Set(m_VFArg->GetDefaultView(BUFFER_VIEW_UNORDERED_ACCESS));
			auto GroupMap = m_plightSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "GroupMap");
			//m_TargetSRV = m_fowSRB->GetVariableByName(SHADER_TYPE_PIXEL, "fowresult");
			m_pRTSRB->GetVariableByName(SHADER_TYPE_PIXEL, "fowresult")->Set(tview);;
			m_TargetMapSRV = m_pRTSRB->GetVariableByName(SHADER_TYPE_PIXEL, "lightMapfow");
			m_TargetMapSRV->Set(pRTColorfow->GetDefaultView(TEXTURE_VIEW_SHADER_RESOURCE));
			//m_RendeRT= pRTColor->GetDefaultView(TEXTURE_VIEW_RENDER_TARGET);
			m_RendeRTf = pRTColorfow->GetDefaultView(TEXTURE_VIEW_RENDER_TARGET);
			//m_pRTSRB->GetVariableByName(SHADER_TYPE_PIXEL, "fowresult")->Set(pRWSignUAV);

			for (size_t i = 0; i < m_ShadowMapSRV.size(); i++)
			{
				m_fowSRB[i]->GetVariableByName(SHADER_TYPE_PIXEL, "resistanceMapR")->Set(m_resistanceSRV);
				m_fowSRB[i]->GetVariableByName(SHADER_TYPE_PIXEL, "lightMapfow")->Set(m_ShadowMapSRV[i]);
				Vfilterpsb.SRBs[i]->GetVariableByName(SHADER_TYPE_COMPUTE, "lightMapSRV")->Set(m_ShadowMapSRV[i]);
				Vfilterpsb.SRBs[i]->GetVariableByName(SHADER_TYPE_COMPUTE, "Vf_Props")->Set(m_vfProps->GetDefaultView(BUFFER_VIEW_SHADER_RESOURCE));
				Vfilterpsb.SRBs[i]->GetVariableByName(SHADER_TYPE_COMPUTE, "VfOIns")->Set(m_Vresi->GetDefaultView(BUFFER_VIEW_UNORDERED_ACCESS));
				Vfilterpsb.SRBs[i]->GetVariableByName(SHADER_TYPE_COMPUTE, "VfArg")->Set(m_VFArg->GetDefaultView(BUFFER_VIEW_UNORDERED_ACCESS));
			}

			m_pReset1SRB->GetVariableByName(SHADER_TYPE_COMPUTE, "clearlightMap")->Set(m_resistanceUAV);
			/*m_lightMapUAV->Set(m_ShadowMapUAV[0]);
			m_lightMapUAV1->Set(m_ShadowMapUAV[1]);*/

			auto* pRTV = m_pSwapChain->GetCurrentBackBufferRTV();
			gCons.mapsize = { shadowmapsz ,shadowmapsz };
			gCons.lightcolor = { 2 ,2,2,2 };
		}

		template <typename C>
		constexpr void Updateinsdata(C&& fc) {
			//MapHelper<T> m_MappedData(m_pImmediateContext, m_Vresi, MAP_TYPE::MAP_WRITE, MAP_FLAGS::MAP_FLAG_DISCARD);
			fc(m_vfProps);
		}

		sStruct::GlobalConstants gCons;
		UINT64 ctex = 0, ntex = 0;
		template<typename C>
		void Cull(RefCntAutoPtr<IDeviceContext>& m_pImmediateContext,
			nonstd::span<sStruct::objrep> Lighteobjs, //std::vector<sStruct::VFoutArgs> drawresult,
			C&& PrepareFunc,
			int insTancenum,
			int MaxArgnum = 100) {
			gCons.inum = MaxArgnum;
			gCons.pedd = MaxArgnum;
			ctex = Tim % m_ShadowMapSRV.size(), ntex = (Tim += 1) % m_ShadowMapSRV.size();
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

			m_ClearSRV->Set(m_ShadowMapUAV[ntex]);
			m_pImmediateContext->SetPipelineState(m_pResetPSO);
			{
				MapHelper<sStruct::objrep> m_MappedData(m_pImmediateContext, m_brightObjs, MAP_TYPE::MAP_WRITE, MAP_FLAGS::MAP_FLAG_DISCARD);
				std::memcpy(m_MappedData, Lighteobjs.data(), Lighteobjs.size_bytes());
				for (size_t i = 0; i < Lighteobjs.size(); i++)
				{
					m_MappedData[i].pos.head<2>() += (math::int2::Constant(shadowmapsz / 2));
				}
				Diligent::MapHelper<sStruct::GlobalConstants> gconst(m_pImmediateContext, m_brightcb, MAP_TYPE::MAP_WRITE, MAP_FLAGS::MAP_FLAG_DISCARD);
				*gconst = gCons;
				gconst->uiNum = Lighteobjs.size();
			}
			m_pImmediateContext->CommitShaderResources(m_pResetSRB, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
			//clear 4x4 block each thread
			DispatAttribs.ThreadGroupCountX = (shadowmapsz / 32);
			DispatAttribs.ThreadGroupCountY = shadowmapsz / 32;
			m_pImmediateContext->DispatchCompute(DispatAttribs);


			m_lightMapUAV->Set(cUAV);
			m_pImmediateContext->SetPipelineState(m_ClightPSO);
			m_pImmediateContext->CommitShaderResources(m_plightSRB, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
			int gcx = (Lighteobjs.size() * 8) / m_ThreadGroupSize;
			DispatAttribs.ThreadGroupCountX = gcx + 1;
			DispatAttribs.ThreadGroupCountY = 1;
			DispatAttribs.ThreadGroupCountZ = 1;
			m_pImmediateContext->DispatchCompute(DispatAttribs);
			if (insTancenum != 0) [[likely]]
				//cull
			{
				auto & pCtx = m_pImmediateContext;
				m_pImmediateContext->SetPipelineState(Vfilterpsb.PSO);
				m_pImmediateContext->CommitShaderResources(Vfilterpsb.SRBs[ctex], RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
				gcx = (insTancenum * 8) / m_ThreadGroupSize;
				DispatAttribs.ThreadGroupCountX = gcx + 1;
				DispatAttribs.ThreadGroupCountY = 1;
				DispatAttribs.ThreadGroupCountZ = 1;
				m_pImmediateContext->DispatchCompute(DispatAttribs);

				pCtx->CopyBuffer(m_VFArg, 0, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION, m_VFArgR, 0, sizeof(sStruct::VFoutArgs) * MaxArgnum, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
				void* pMappedData = nullptr;
				pCtx->WaitForIdle();
				pCtx->MapBuffer(m_VFArgR, MAP_READ, MAP_FLAG_DO_NOT_WAIT, pMappedData);
				PrepareFunc(nonstd::span(reinterpret_cast<sStruct::VFoutArgs*>(pMappedData),MaxArgnum),m_VFArg);
				pCtx->UnmapBuffer(m_VFArgR, MAP_READ);
			}
		}
		//clean lightMap feed lightMap -> calculate lightMap -> pixel sample-> blend with input tex
		void Render(const Camera cam, RefCntAutoPtr<IDeviceContext>& m_pImmediateContext,
			nonstd::span<sStruct::objrep> Lighteobjs) {
			//const UINT64 ctex = Tim % m_ShadowMapSRV.size(), ntex = (Tim) % m_ShadowMapSRV.size();
				{
					auto& pCtx = m_pImmediateContext;
					{
						auto& pRTV = m_RendeRTf;//  m_pSwapChain->GetCurrentBackBufferRTV();

						m_pImmediateContext->SetRenderTargets(1, &pRTV, m_pSwapChain->GetDepthBufferDSV(), RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
						m_pImmediateContext->ClearRenderTarget(pRTV, ClearColor, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
						m_pImmediateContext->ClearDepthStencil(m_pSwapChain->GetDepthBufferDSV(), CLEAR_DEPTH_FLAG, 1.f, 0, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
					}
					IBuffer* pVBs[] = { plane.pVertexBuffer };
					Uint64   Offsets[_countof(pVBs)] = {};
					{
						struct CameraAttribs
						{
							using float4 = math::Vector4f;
							using float2 = math::Vector2f;
							using float4x4 = math::Matrix4f;
							float4 f4Position;     // Camera world position
							float4 f4ViewportSize; // (width, height, 1/width, 1/height)

							float2 f2ViewportOrigin; // (min x, min y)
							float fNearPlaneZ;
							float fFarPlaneZ; // fNearPlaneZ < fFarPlaneZ
							float4x4 mViewT;
							float4x4 mProjT;
							float4x4 mViewProjT;
							float4x4 mViewInvT;
							float4x4 mProjInvT;
							float4x4 mViewProjInvT;
							float4 f4ExtraData[5]; // Any appliation-specific data
							// Sizeof(CameraAttribs) == 256*2
						};
						//m_lightMapfowSRV->Set(rendertex);
						MapHelper<CameraAttribs> CamAttribs(m_pImmediateContext, m_CamConstants, MAP_WRITE, MAP_FLAG_DISCARD);
						(CamAttribs->mProjT) = (cam.GetProjMatrix().transpose());
						CamAttribs->mViewProjT = cam.CameraViewProj;
						CamAttribs->mViewProjInvT = cam.CameraViewProj.inverse();
						CamAttribs->f4Position = cam.GetWorldMatrix().col(3);// Diligent::float4(Diligent::float3::MakeVector(cam.GetWorldMatrix()[3]), 1);
						CamAttribs->f4Position[3] = 1;
						//grid size
						CamAttribs->f4ExtraData[0] = { 4,4,0,1 };
						//offset to positive
						CamAttribs->f4ExtraData[1] = { shadowmapsz / 2.0f,shadowmapsz / 2.0f,0.0f,1.0f };
						//= ex;
					}
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
					m_pImmediateContext->ClearDepthStencil(m_pSwapChain->GetDepthBufferDSV(), CLEAR_DEPTH_FLAG, 1.f, 0, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
					// //Set the render target pipeline state
					m_pImmediateContext->SetPipelineState(m_pRTPSO);
					//// Commit the render target shader's resources
					m_pImmediateContext->CommitShaderResources(m_pRTSRB, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
					// Draw the render target's vertices
					DrawAttribs RTDrawAttrs;
					RTDrawAttrs.NumVertices = 4;
					RTDrawAttrs.Flags = DRAW_FLAG_VERIFY_ALL; // Verify the state of vertex and index buffers
					m_pImmediateContext->Draw(RTDrawAttrs);
				}
		}

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
				MapHelper<Uint8> m_MappedData;
				m_MappedData.Map(m_pImmediateContext, m_deltaObjs, MAP_TYPE::MAP_WRITE, MAP_FLAGS::MAP_FLAG_DISCARD);
				std::memcpy(m_MappedData, o.data(), o.size_bytes());
			}
			m_pImmediateContext->SetPipelineState(m_pMovePSO);
			m_pImmediateContext->CommitShaderResources(m_pMoveSRB, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
			DispatAttribs.ThreadGroupCountX = (o.size() + m_ThreadGroupSize - 1) / m_ThreadGroupSize;
			m_pImmediateContext->DispatchCompute(DispatAttribs);
		}
		void GenerateBlock(Utility::span<sStruct::objrep> bks, math::int4 size = { 100,100,0,0 }) {
			//(num);
			auto offset = size.head<2>() / -2 + size.tail(2);// +math::int2::Constant(shadowmapsz / 2);
			for (size_t i = 0; i < bks.size(); i++)
			{

				auto& bk = bks[i];
				bk.pos[0] = std::rand() % size[0];
				bk.pos[1] = std::rand() % size[1];
				bk.pos[2] = 3;
				bk.pos.head<2>() += offset;
			}
			AddBlock(bks);
		}

		~FowRender();
		static constexpr float ClearColor[] = { 0.35, 0.35, 0.35, 1 };
		int shadowmapsz = 1024, Texsize = 512;
		INT64 Tim = 0;
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

		static inline TEXTURE_FORMAT RenderTargetFormat = TEX_FORMAT_RGBA8_UNORM;
		static inline TEXTURE_FORMAT DepthBufferFormat = TEX_FORMAT_D32_FLOAT;
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
		RefCntAutoPtr<IPipelineState> m_ClightPSO;
		RefCntAutoPtr<IShaderResourceBinding> m_plightSRB;
		RefCntAutoPtr<IPipelineState> m_RfowPSO;
		RefCntAutoPtr<IShaderResourceBinding> m_fowSRB[2];
		RefCntAutoPtr<IShaderResourceBinding> m_fowSRB0;
		RefCntAutoPtr<IPipelineState> m_pRTPSO;
		RefCntAutoPtr<IShaderResourceBinding> m_pRTSRB;
		RefCntAutoPtr<ISwapChain> m_pSwapChain;
		PSB<2> Vfilterpsb;
	private:
		DispatchComputeAttribs DispatAttribs;

	};
	/*struct fowResourse {
		RefCntAutoPtr<IShaderResourceBinding> m_fowSRB;
		RefCntAutoPtr<IShaderResourceBinding> m_plightSRB;
		fowResourse(
			RefCntAutoPtr<IPipelineState>& m_RfowPSO,
			RefCntAutoPtr<IPipelineState>& m_ClightPSO,
			RefCntAutoPtr<ITextureView>           m_resistanceSRV,
			RefCntAutoPtr<ITextureView>           m_ShadowMapSRV,
			RefCntAutoPtr<ITextureView>          m_ShadowMapUAV
		) {
			m_fowSRB.Release();
			m_ClightPSO.Release();
			m_RfowPSO->CreateShaderResourceBinding(&m_fowSRB, true);
			m_ClightPSO->CreateShaderResourceBinding(&m_plightSRB, true);
			m_fowSRB->GetVariableByName(SHADER_TYPE_PIXEL, "resistanceMapR")->Set(m_resistanceSRV);
			m_fowSRB->GetVariableByName(SHADER_TYPE_PIXEL, "lightMapfow")->Set(m_ShadowMapSRV[i]);
			auto resistanceMap = m_plightSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "resistanceMap");
			m_lightMapUAV = m_plightSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "lightMap");
			m_ClightPSO->GetVariableByName(SHADER_TYPE_COMPUTE, "Shadowqueue")->Set(m_Shadowqueue->GetDefaultView(BUFFER_VIEW_UNORDERED_ACCESS));
			m_plightSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "objProp")->Set(m_brightObjs->GetDefaultView(BUFFER_VIEW_UNORDERED_ACCESS));
			m_plightSRB->GetVariableByName(SHADER_TYPE_COMPUTE, "RWSign")->Set(pRWSignUAV);
		}
	};*/
	FowRender::~FowRender()
	{
	}
}