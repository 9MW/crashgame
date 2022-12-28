#ifdef GetObject
#undef GetObject
#endif
#pragma once
#include <FirstPersonCamera.hpp>
#include"Render/ShaderStructs.h"
#include"io/loader.h"
#include"Common/span.hpp"
#include"Render/ModuleHead.h"
#include"RenderUtil.h"
#include"AssetLoader/RenderStr0.h"
//#include "ThirdParty/GLTFLoader.hpp";
#include"parallel_hashmap/phmap.h"
#include"cfgparser.h"
#include"Gameprops0.h"
#include <Render/RenderCommon.h>
//import RModuleComm;
import GFloader;
export module batchRender;
namespace pj2::n_Render {

	using namespace GLTF;

	void CreatePSO(RenderContext& rc, RefCntAutoPtr<IPipelineState>& _PSO, const ENUMPSO tp) {
		//auto& m_pDevice = rc.m_pDevice;
		//switch (tp)
		//{
		//case ENUMPSO::Cull:
		//	/* code */
		//	break;

		//case ENUMPSO::Skin:
		//	RefCntAutoPtr<IShaderSourceInputStreamFactory>& pShaderSourceFactory = rc.pShaderSourceFactory;
		//	ShaderResourceVariableDesc Vars[] =
		//	{
		//		{SHADER_TYPE_PIXEL, "g_ColorMap", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE},
		//		{SHADER_TYPE_PIXEL, "cell", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE},
		//		{SHADER_TYPE_PIXEL, "map", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE},
		//		{SHADER_TYPE_VERTEX, "animreco", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE},
		//		{SHADER_TYPE_VERTEX, "instDatas", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE},
		//		{SHADER_TYPE_VERTEX, "objectTransfromBuffer", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE}
		//	};

		//	LayoutElement inputDesc[] =
		//	{
		//	 {0, 0, 3, VT_FLOAT32},   //float3 Pos     : ATTRIB0;
		//	 {1, 0, 3, VT_FLOAT32},   //float3 Normal  : ATTRIB1;
		//	 {2, 0, 2, VT_FLOAT32},   //float2 UV0     : ATTRIB2;
		//	 {3, 0, 2, VT_FLOAT32},   //float2 UV1     : ATTRIB3;
		//	};
		//	RenderUtil::CreatePipelineStates
		//	(&_PSO, m_pDevice, rc.RenderTargetFormat, rc.DepthBufferFormat, pShaderSourceFactory,
		//		"NoSkin.hlsl", "vert", "frag", Vars, inputDesc);
		//	break;
		//case ENUMPSO::Station:
		//	/* code */
		//	break;
		//default:
		//	break;
		//}
	}
	using ModelId = int;
	export struct skined0 :Render
	{
		template<class T>
		using span = Utility::span<T>;
		struct rb {
			RefCntAutoPtr<IShaderResourceBinding> m_pSRB;
			RefCntAutoPtr<IBuffer>                _instbuf;
			RefCntAutoPtr<IBuffer>                _transbuf;
			RefCntAutoPtr<IBuffer>                _cbufpri;
		};
	protected:
		math::uint IndexCount, _vbufnum;
		RefCntAutoPtr<IRenderDevice> m_pDevice;
		RefCntAutoPtr<IDeviceContext> m_pImmediateContext;
		RefCntAutoPtr<IBuffer>                m_VertexBuffer[2];
		RefCntAutoPtr<IBuffer>                m_IndexBuffer;
		RefCntAutoPtr<IBuffer>                _Animbuf;

		RefCntAutoPtr<IShaderResourceBinding> m_pSRB;
		RefCntAutoPtr<IBuffer>                _transbuf;//optional
		RefCntAutoPtr<IPipelineState> _pPSO;
		std::vector<rb> _rbs;
		//std::vector<drawunit> _dus;
		using tnd = std::unique_ptr<GLTF::Node>;
		void AnimNodeFilter(GLTF::Node*& nil, std::vector<GLTF::Node*>& rut) {

			if (nil->_Mesh) {
				rut.emplace_back(nil);
			}
			for (auto& ni : nil->Children)
			{
				AnimNodeFilter(ni, rut);
			}
		}
		//int AniJnum(GLTF::Animation& animation, std::vector<GLTF::Node*>& rut) {

		//	for (auto& channel : animation.Channels)
		//	{
		//		//if(channel.node->_Skin->Joints.)
		//		AnimNodeFilter(channel.node, rut);
		//	}
		//	for (auto& anode:rut)
		//	{

		//	}
		//}
		void AnimNodeFilter(tnd& nil, std::vector<GLTF::Node*>& rut) {

			if (nil->_Skin && nil->_Mesh) {
				rut.emplace_back(nil.get());
			}
			for (auto& ni : nil->Children)
			{
				AnimNodeFilter(ni, rut);
			}
		}
		void AnimNodeFilter(Model::model& mod, const n_Render::modelidn& mdi, std::vector<GLTF::Node*>& rut) {

			auto& nd = mod.LinearNodes;

			for (size_t i = 0; i < mdi.lndl; i++)
			{
				Node*& ni = nd[i + mdi.lnds];
				if (ni->_Mesh)rut.emplace_back(ni);
			}
		}
		static inline int _TexPerModel = -1;
	public:
		RefCntAutoPtr<IBuffer>                _instbuf;
		struct Primitive {
			using GP = Diligent::GLTF::Primitive;
			using Uint32 = math::uint;
			Uint32    FirstIndex;
			Uint32    IndexCount;
			Uint32    VertexCount;
			Uint32    TexStart;
			Primitive(GP& gpri) {
				std::memcpy(this, &gpri, sizeof(*this));
			}
		};
		struct vPrimProo {
			RefCntAutoPtr<Diligent::IShaderResourceBinding> srb;
			RefCntAutoPtr<IBuffer>                _instbuf;
			RefCntAutoPtr<IBuffer>                _transbuf;
			RefCntAutoPtr<IBuffer>                _cbufpri;
		};
		struct modelreco
		{
			math::int2 pse;
			uint bonenumb;
			int& operator[](const int i) {
				return pse[i];
			}
		};
		//using modelreco = math::int2;
		using gPrimitive = sStruct::skinedcbe;
		struct modelrep
		{
			int anis, anil, pris, pril;
		};
		//std::vector<mesh>_mesh;
		std::vector<Primitive>_prims;
		std::vector<vPrimProo>_gprims;
		std::vector<modelreco>_mesh1;
		std::vector<rb>_RBs;
		std::vector<AnimRec> _AniReco;
		//std::vector<n_Render::modelidn> _modeloff;
		math::ui64 modelnum = 0;
		auto& GetRB(const math::ui64 i) {
			return _rbs[i];
		}
		void bake(Model::model& mod) {
			const auto Samplerate = GameProps::Samplerate;
			std::vector<math::Matrix4f> bonerec;
			bonerec.reserve(4096);
			using tnd = GLTF::Node*;
			std::vector<tnd> sd;
			sd.reserve(100);
			int base = 0;
			using namespace Diligent::GLTF;
			phmap::parallel_flat_hash_map<GLTF::Skin*, int>animmap;
			size_t i0 = 0;
			auto& pPSO = _pPSO;
			std::vector<IDeviceObject*> _txvs(_texnum);
			phmap::flat_hash_map<ui64, ui64> timap;
			timap.reserve(_txvs.capacity());
			if(!_dynamic){

				for (size_t i = 0; i < mod.Textures.size(); i++)
				{
					RefCntAutoPtr<ITexture>& tx = mod.Textures[i];
					Diligent::ITextureView* vp = tx->GetDefaultView(TEXTURE_VIEW_SHADER_RESOURCE);
					_txvs[i]=(vp);
					timap.emplace(tx->GetUniqueID(), i);
				}
				_txvs.back() = (m_pDefaultNormalMapSRV);
				ITexture* pBaseColorTex = nullptr;
				auto shv = pPSO->GetStaticVariableByName(SHADER_TYPE_PIXEL, "g_ColorMap");
				shv->SetArray(_txvs.data(), 0, _txvs.size());
			}

			auto& modeloffset = mod.modeloffset;
			const auto modelsize = modeloffset.size();
			_prims.reserve(modelsize);
			_mesh1.resize(mod.modeloffset.size());
			int primct = 0;
			auto& pCtx = m_pImmediateContext;
			std::vector<int> mtids;
			mtids.reserve(sd.size() * 8);
			for (; i0 < mod.modeloffset.size(); i0++)
			{
				n_Render::modelidn& mdi = mod.modeloffset[i0];
				int jointnum = 0;
				assert(mdi.skl < 2);
				/*	for (size_t i1 = mdi.skl; i1 < mdi.sks; i1++)
					{
						const auto p_sk = mod.Skins[i1];
						jointnum += mod.Skins[i1]->Joints.size();
						animmap[p_sk] = jointnum;
					}*/
					//sd.clear();
				primct = sd.size();
				AnimNodeFilter(mod, mdi, sd);
				auto& mr = _mesh1[i0];
				mr[0] = primct;
				mr[1] = sd.size() - primct;
				if (sd.size() > primct) {
					mr.bonenumb = 0;
					mr.bonenumb += sd[mr[0]]->_Mesh->Transforms.jointcount;
					for (size_t i = 0; i < mr[1]; i++)
					{
						auto& mesh = sd[i + mr[0]]->_Mesh;
						auto& primitive = mesh->Primitives;
						auto& pri = primitive[0];
						mtids.emplace_back(mdi.mtls + pri.material);
						_prims.emplace_back(pri);
					}
				}
				//mesh& ms = _prims[i0];
				if (mdi.animl > 0 && !_fxmesh) {

					std::unique_ptr<Skin>& skin = mod.Skins[mdi.sks];
					AnimRec rec;
					rec.modelId = i0;
					rec.bonenum = skin->Joints.size();
					int inc = 0;
					for (size_t i = 0; i < mod.Animations.size(); i++)
					{
						Animation& ani = mod.Animations[i];
						inc += math::ceil((ani.End) / Samplerate);
					}
					bonerec.resize(bonerec.size() + inc * rec.bonenum);
					inc = 0;
					for (size_t i = 0; i < mod.Animations.size(); i++)
					{
						float dt = 0;
						Animation& ani = mod.Animations[i];
						dt = ani.Start;
						rec.name = ani.Name;
						rec.base = base;
						rec.size = 0;
						inc = math::ceil((ani.End - ani.Start) / Samplerate);
						while (dt < ani.End)
						{
							mod.UpdateAnimation(i, dt);
							/*for (size_t i1 = 0; i1 < nodes.size(); i1++)
							{
								const auto nd1 = nodes[i1];
								i2.bonenum = nd1->_Mesh->Transforms.jointcount;
								if (i2.bonenum == 0) {
									continue;
								}
							}*/
							const auto st = bonerec.size();
							/*auto& jmx = nodes[0]->_Mesh->Transforms.jointMatrix;
							for (size_t i = 0; i < nodes[0]->_Mesh->Transforms.jointcount; i++)
							{
								bonerec.emplace_back(jmx[i]);
							}*/
							assert(base + inc < st);
							std::memcpy(&(bonerec[base + rec.size]), sd[primct]->_Mesh->Transforms.jointMatrix, rec.bonenum * sizeof(math::Matrix4f));
							/*for (size_t i4 = 0; i4 < i2.bonenum; i4++)
							{
								bonerec[base + i4].block<3,3>(0,0) *= 0.01;
							}*/
							rec.size += rec.bonenum;
							dt += Samplerate;
						}
						_AniReco.emplace_back(rec);
						base += rec.size;
					}
				}
			}
			_Animbuf = NULL;
			BufferDesc BuffDesc;
			using ts = float4x4;
			RenderUtil::BuildBDesc<ts>(BuffDesc, bonerec.size(), "animreco",
				Diligent::BUFFER_MODE_STRUCTURED,
				Diligent::USAGE_IMMUTABLE, Diligent::BIND_SHADER_RESOURCE, Diligent::CPU_ACCESS_NONE);
			Diligent::BufferData bd;
			if (!_fxmesh) {
				new(&bd)Diligent::BufferData(bonerec.data(), Utility::VecByteSize(bonerec), m_pImmediateContext);
				m_pDevice->CreateBuffer(BuffDesc, &bd, &_Animbuf);
				StateTransitionDesc dec(_Animbuf, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_GENERIC_READ, STATE_TRANSITION_FLAG_UPDATE_STATE);
				m_pImmediateContext->TransitionResourceStates(1, &dec);
				_pPSO->GetStaticVariableByName(Diligent::SHADER_TYPE_VERTEX, BuffDesc.Name)->
					Set(_Animbuf->GetDefaultView(Diligent::BUFFER_VIEW_SHADER_RESOURCE));
			}

			std::vector<StateTransitionDesc> decc_;
			decc_.reserve(sd.size());


			m_pSRB = NULL;
			pPSO->CreateShaderResourceBinding(&m_pSRB, true);
			if (_dynamic){
				for (size_t i = 0; i < mod.Textures.size(); i++)
				{
					RefCntAutoPtr<ITexture>& tx = mod.Textures[i];
					Diligent::ITextureView* vp = tx->GetDefaultView(TEXTURE_VIEW_SHADER_RESOURCE);
					_txvs[i]=(vp);
					timap.emplace(tx->GetUniqueID(), i);
				}
				for (size_t i = timap.size(); i < _txvs.size(); i++)
				{
					_txvs[i] = m_pDefaultNormalMapSRV;
				}
				_txvs.back()=(m_pDefaultNormalMapSRV);
				ITexture* pBaseColorTex = nullptr;
				auto shv = m_pSRB->GetVariableByName(SHADER_TYPE_PIXEL, "g_ColorMap");
				shv->SetArray(_txvs.data(), 0, _txvs.size());
				_txvs.back() = _txvs[0];// (m_pDefaultNormalMapSRV);
				shv->SetArray(_txvs.data(), 0, _txvs.size());
			}

			const auto defid = _txvs.size() - 1;
			std:vector<gPrimitive> gCb(sd.size());
			BuffDesc.ElementByteStride = sizeof(gPrimitive);
			BuffDesc.Size = BuffDesc.ElementByteStride * gCb.size();
			for (size_t i = 0; i < sd.size(); i++)
			{
				//auto& rbl = this->_RBs.emplace_back();
				/*auto& pSRB = rbl.m_pSRB;
				pSRB = NULL;
				pPSO->CreateShaderResourceBinding(&pSRB, true);*/
				auto& mes = sd[i]->_Mesh;
				const auto bonenum = mes->Transforms.jointcount;
				const auto mtId = mtids[i];
				//auto& cb0 = rbl._cbufpri;
				using ts = gPrimitive;
				Material& tsrc = mod.Materials[mtId];
				/*RenderUtil::BuildBDesc<ts>(BuffDesc, 1,256, "cbPrim",
					Diligent::BUFFER_MODE_UNDEFINED,
					Diligent::USAGE_IMMUTABLE, Diligent::BIND_UNIFORM_BUFFER, Diligent::CPU_ACCESS_NONE);*/
				gPrimitive& mtdata = gCb[i];
				const auto gettexid = [&](RefCntAutoPtr<ITexture>& const tex) {return tex ? timap[tex->GetUniqueID()] : defid; };
				std::memcpy(&mtdata.NodeMatrix, &mes->Transforms.matrix, sizeof(Eigen::Matrix4f));
				mtdata.texid = gettexid(tsrc.pBaseColorTexture);
				mtdata.normaltexid = gettexid(tsrc.pNormalTexture);
				mtdata.bonenumb = bonenum;

			}
			RefCntAutoPtr<IBuffer>                cb0;
			Diligent::CreateUniformBuffer(m_pDevice, sizeof(gPrimitive) * gCb.size(), "cbPrim", &cb0,
				Diligent::USAGE_IMMUTABLE, Diligent::BIND_UNIFORM_BUFFER, Diligent::CPU_ACCESS_NONE, gCb.data());
			decc_.emplace_back(cb0, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_GENERIC_READ, STATE_TRANSITION_FLAG_UPDATE_STATE);
			m_pSRB->GetVariableByName(Diligent::SHADER_TYPE_VERTEX, "cbPrim")->Set(cb0);
			//m_pSRB->GetVariableByName(Diligent::SHADER_TYPE_VERTEX, "cmes")->Set(cb0->GetDefaultView(Diligent::BUFFER_VIEW_SHADER_RESOURCE)); 
			m_pImmediateContext->TransitionResourceStates(decc_.size(), decc_.data());
		}

		void sbake(Model::model& mod) {
			assert(_fxmesh);
			using tnd = GLTF::Node*;
			std::vector<tnd> sd;
			sd.reserve(100);
			int base = 0;
			using namespace Diligent::GLTF;
			phmap::parallel_flat_hash_map<GLTF::Skin*, int>animmap;
			size_t i0 = 0;
			auto& pPSO = _pPSO;
			auto& modeloffset = mod.modeloffset;
			const auto modelsize = modeloffset.size();
		}
		//POBs<> _pobs[10];
		int _PowSTRIDE, _totalprimitive, _texnum;
		bool _fxmesh,_dynamic;
		/*RefCntAutoPtr<IBuffer>                m_pTransAttribsBuffer;
		RefCntAutoPtr<IBuffer>                m_pinspropertysBuffer;
		RefCntAutoPtr<IBuffer>                m_pinsBuffer;
		RefCntAutoPtr<IBuffer>                M_pIndirectArgsBuff;*/
		//Model::model _mod;
		void init(
			RenderContext& rc,
			Model::model& mod,
			const uint IndexCount,
			const int mxinstancenum = 5000,
			const int _PowSTRIDE = 17,
			bool fxmesh = false, bool dynamic = true
		) {
			_fxmesh = fxmesh;
			_dynamic = dynamic;
			_TexPerModel = GameProps::_TexSkinnum;
			this->_PowSTRIDE = _PowSTRIDE;
			//_mod = mod;
			span< RefCntAutoPtr<IBuffer> >m_VertexBuffer = mod.pVertexBuffer;
			m_pDevice = rc.m_pDevice;
			m_pImmediateContext = (rc.m_pImmediateContext);
			//m_pEngineFactory = rc.pShaderSourceFactory;
			this->IndexCount = (IndexCount);
			_vbufnum = m_VertexBuffer.size();
			auto& pCameraAttribs = rc.pCameraAttribs;
			m_IndexBuffer = (mod.pIndexBuffer);
			//const auto& RenderTargetFormat=rc.
			for (size_t i = 0; i < m_VertexBuffer.size(); i++)
			{
				this->m_VertexBuffer[i] = m_VertexBuffer[i];
			}
			RefCntAutoPtr<IShaderSourceInputStreamFactory> pShaderSourceFactory = rc.pShaderSourceFactory;
			ShaderResourceVariableDesc Vars[] =
			{
				{SHADER_TYPE_PIXEL, "g_ColorMap",dynamic? SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC : SHADER_RESOURCE_VARIABLE_TYPE_STATIC},
				{SHADER_TYPE_VERTEX,"cbPrim", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE},
				{SHADER_TYPE_VERTEX,"cmes", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE},
				{SHADER_TYPE_PIXEL, "cell", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE},
				{SHADER_TYPE_PIXEL, "map", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE},
				{SHADER_TYPE_VERTEX, "animreco", SHADER_RESOURCE_VARIABLE_TYPE_STATIC},
				{SHADER_TYPE_VERTEX, "instDatas", SHADER_RESOURCE_VARIABLE_TYPE_STATIC},
				{SHADER_TYPE_VERTEX, "objectTransfromBuffer", SHADER_RESOURCE_VARIABLE_TYPE_STATIC}
			};

			LayoutElement inputDesc[] =
			{
			 {0, 0, 3, VT_FLOAT32},   //float3 Pos     : ATTRIB0;
			 {1, 0, 3, VT_FLOAT32},   //float3 Normal  : ATTRIB1;
			 {2, 0, 2, VT_FLOAT32},   //float2 UV0     : ATTRIB2;
			 {3, 0, 2, VT_FLOAT32},   //float2 UV1     : ATTRIB3;
			 {4, 1, 4, VT_FLOAT32},   //float3 Pos     : ATTRIB0;
			 {5, 1, 4, VT_FLOAT32},   //float3 Normal  : ATTRIB1;
			};
			ShaderMacroHelper mcr;
			auto& mb = mod.modeloffset.back();
			_texnum = mb.texstart + mb.texsize;
			constexpr int isst = 1 << 17;
			constexpr int is8st = 1 << (32 - 17);
			constexpr int isste = (isst + 545) >> 17;
			constexpr int is80s = (isst + 545) % (1 << 17);
			assert(false&&"corresponding buffet changeed");
			auto& nd = mod.LinearNodes;
			_totalprimitive = 0;
			for (Node* np : nd)
			{
				_totalprimitive += np->_Mesh ? 0 : 1;
			}
			_texnum = _texnum > 0 ? _texnum : 1;
			mcr.AddShaderMacro("NUM_TEXTURES", _texnum);
			mcr.AddShaderMacro("NUM_PRIM", _totalprimitive);
			mcr.AddShaderMacro("BOSTRIDE", _PowSTRIDE);
			if (fxmesh)
				mcr.AddShaderMacro("FIXMESH", 1);
			_pPSO = NULL;
			RenderUtil::CreatePipelineStates
			(&_pPSO, m_pDevice, rc.RenderTargetFormat, rc.DepthBufferFormat,
				pShaderSourceFactory,
				"skined.hlsl", "vert", "frag", Vars, mcr, inputDesc);
			Diligent::PipelineResourceSignatureDesc sd;
			BufferDesc BuffDesc;
			using ts = sStruct::inscmb;
			RenderUtil::BuildBDesc<ts>(BuffDesc, mxinstancenum, "skined insbuf",
				Diligent::BUFFER_MODE_STRUCTURED,
				Diligent::USAGE_DYNAMIC, Diligent::BIND_SHADER_RESOURCE);
			_instbuf = NULL;
			m_pDevice->CreateBuffer(BuffDesc, NULL, &_instbuf);
			_pPSO->GetStaticVariableByName(Diligent::SHADER_TYPE_VERTEX, "instDatas")->
				Set(_instbuf->GetDefaultView(Diligent::BUFFER_VIEW_SHADER_RESOURCE));
			auto ptbuf = _pPSO->GetStaticVariableByName(Diligent::SHADER_TYPE_VERTEX, "objectTransfromBuffer");
			if (ptbuf)
				ptbuf->Set(_transbuf->GetDefaultView(Diligent::BUFFER_VIEW_SHADER_RESOURCE));
			Init(m_pDevice, m_pImmediateContext);
			_pPSO->GetStaticVariableByName(SHADER_TYPE_VERTEX, "cbCameraAttribs")->Set(pCameraAttribs);

			/*std::vector<IDeviceObject*> _txvs;
			_txvs.resize(texnum + 1);
			for (auto& ptv: _txvs)
			{
				ptv = m_pWhiteTexSRV;
			}
			ITexture* pBaseColorTex = nullptr;
			auto shv = _pPSO->GetStaticVariableByName(SHADER_TYPE_PIXEL, "g_ColorMap");
			Diligent::IDeviceObject* da[] = { (Diligent::IDeviceObject*)m_pWhiteTexSRV };
			if (_txvs.size() > 0)
				shv->SetArray(_txvs.data(), 0, _txvs.size());
			else
				shv->SetArray(da, 0, 1);*/

		}
		//using namespace Diligent;
		struct renderbatch {
			RefCntAutoPtr<IBuffer> trs, State, joints;
		};
		std::vector<int> _insFilter;
		void Render(Utility::span<int> inss) {
			assert(inss.size() == _mesh1.size());
			auto& pCtx = m_pImmediateContext;
			IBuffer* pVBs[] = { m_VertexBuffer[0],m_VertexBuffer[1] };
			Uint64   Offsets[_countof(pVBs)] = {};
			//_insFilter.clear();
			pCtx->SetPipelineState(_pPSO);

			StateTransitionDesc dec(_instbuf, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_SHADER_RESOURCE, STATE_TRANSITION_FLAG_UPDATE_STATE);
			m_pImmediateContext->TransitionResourceStates(1, &dec);
			/*for (size_t i = 0,instct=0; i < _mesh1.size(); i++)
			{
				const auto& NumInstances = inss[i];
				if (NumInstances == 0) [[unlikely]]
					continue;
				auto& prmc = _mesh1[i];
				for (size_t i2 = 0; i2 < prmc[1]; i2++) {
					auto& rb = _RBs[i2 + prmc[0]];
					pCtx->CommitShaderResources(_RBs[i2+ prmc[0]].m_pSRB, RESOURCE_STATE_TRANSITION_MODE_VERIFY);
				}
			}*/

			pCtx->CommitShaderResources(m_pSRB, RESOURCE_STATE_TRANSITION_MODE_VERIFY);
			/*auto& rb = _RBs[0];
			pCtx->CommitShaderResources(rb.m_pSRB, RESOURCE_STATE_TRANSITION_MODE_VERIFY);*/
			DrawIndexedAttribs attribs(0, VT_UINT32, DRAW_FLAG_VERIFY_ALL);
			attribs.BaseVertex = 0;
			attribs.Flags |= DRAW_FLAG_DYNAMIC_RESOURCE_BUFFERS_INTACT;
			// clang-format on
			pCtx->SetVertexBuffers(0, _vbufnum, pVBs, nullptr, RESOURCE_STATE_TRANSITION_MODE_VERIFY, SET_VERTEX_BUFFERS_FLAG_NONE);
			pCtx->SetIndexBuffer(m_IndexBuffer, 0, RESOURCE_STATE_TRANSITION_MODE_VERIFY);
			//const uint inststri=
			int k = 0,insc=0;
			for (size_t i = 0; i < _mesh1.size(); i++)
			{
				const auto& NumInstances = inss[i];
				if (NumInstances == 0) [[unlikely]]
					continue;
				auto& ele = attribs;
				ele.NumInstances = NumInstances;
				auto& prmc = _mesh1[i];
				for (size_t i2 = 0; i2 < prmc[1]; i2++, k++) {
					ele.FirstInstanceLocation = k * ((1 << _PowSTRIDE))+ insc;
					auto& prim = _prims[i2 + prmc[0]];
					ele.NumIndices = prim.IndexCount;
					ele.FirstIndexLocation = prim.FirstIndex;
					pCtx->DrawIndexed(ele);
				}
				insc += NumInstances;
				//_insFilter.emplace_back(i);
			}
		}
		//void Render(RenderAttr& Ra) {
		//	/*if (instancenum == 0)
		//		return;*/
		//	auto& pCtx = m_pImmediateContext;
		//	auto& m_VertexBuffer = Ra.m_VertexBuffer;
		//	auto& m_IndexBuffer = Ra.m_IndexBuffer;
		//	auto& eles = Ra.prims;
		//	IBuffer* pVBs[] = { m_VertexBuffer[0],m_VertexBuffer[1] };
		//	Uint64   Offsets[_countof(pVBs)] = {};

		//	pCtx->SetPipelineState(Ra._PSO);
		//	// clang-format on
		//	//pCtx->TransitionResourceStates(_buftrsdesc.size(), _buftrsdesc.data());
		//	pCtx->SetVertexBuffers(0, pVBs[1] == NULL ? 1 : 2, pVBs, Offsets, RESOURCE_STATE_TRANSITION_MODE_VERIFY, SET_VERTEX_BUFFERS_FLAG_NONE);
		//	pCtx->SetIndexBuffer(m_IndexBuffer, 0, RESOURCE_STATE_TRANSITION_MODE_VERIFY);
		//	for (size_t i = 0; i < eles.size(); i++)
		//	{
		//		auto& ele = eles[i];
		//		if (ele.DRG.NumInstances == 0)
		//			continue;
		//		pCtx->CommitShaderResources(m_pSRB, RESOURCE_STATE_TRANSITION_MODE_VERIFY);
		//		pCtx->DrawIndexed(ele.DRG);
		//	}


		//}
		//template<class ArgStruct = sStruct::VFoutArgs>
		//void IndirectDraw(const nonstd::span<ArgStruct> DrawArgs, RefCntAutoPtr<IBuffer>& pIndirectArgsBuff) {
		//	static std::vector<size_t> ArgPos;
		//	ArgPos.clear();
		//	for (size_t i = 0; i < DrawArgs.size(); i++)
		//	{
		//		if (DrawArgs[i].NumInstances == 0)
		//			continue;
		//		ArgPos.emplace_back(i * sizeof(ArgStruct));
		//	}
		//	if (ArgPos.empty())
		//		return;
		//	auto& pCtx = m_pImmediateContext;

		//	IBuffer* pVBs[] = { m_VertexBuffer };
		//	Uint64   Offsets[_countof(pVBs)] = {};
		//	//StateTransitionDesc Barriers[] =
		//	//{
		//	//	{m_pTransAttribsBuffer,  RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_SHADER_RESOURCE, STATE_TRANSITION_FLAG_UPDATE_STATE},
		//	//	{m_pinspropertysBuffer, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_SHADER_RESOURCE, STATE_TRANSITION_FLAG_UPDATE_STATE},
		//	//	/*{m_VertexBuffer, RESOURCE_STATE_TRANSITION_MODE_VERIFY, RESOURCE_STATE_SHADER_RESOURCE, STATE_TRANSITION_FLAG_UPDATE_STATE}
		//	//	{m_pinspropertysBuffer, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_SHADER_RESOURCE, STATE_TRANSITION_FLAG_UPDATE_STATE}*/
		//	//};

		//	pCtx->SetPipelineState(_pobs->PSO);
		//	// clang-format on
		//	//pCtx->TransitionResourceStates(_countof(Barriers), Barriers);
		//	pCtx->SetVertexBuffers(0, _countof(pVBs), pVBs, Offsets, RESOURCE_STATE_TRANSITION_MODE_VERIFY, SET_VERTEX_BUFFERS_FLAG_NONE);
		//	pCtx->SetIndexBuffer(m_IndexBuffer, 0, RESOURCE_STATE_TRANSITION_MODE_VERIFY);

		//	pCtx->CommitShaderResources(_pobs->SRBs[0], RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
		//	DrawIndexedIndirectAttribs drawAttrs{ VT_UINT32, DRAW_FLAG_VERIFY_ALL, RESOURCE_STATE_TRANSITION_MODE_TRANSITION };
		//	for (size_t i = 0; i < ArgPos.size(); i++)
		//	{

		//		drawAttrs.DrawArgsOffset = ArgPos[i];
		//		pCtx->DrawIndexedIndirect(drawAttrs, pIndirectArgsBuff);
		//	}

		//}
	};
	export struct StRender :Render
	{
		template<class T>
		using span = Utility::span<T>;
	protected:
		math::uint IndexCount, _vbufnum;
		RefCntAutoPtr<IRenderDevice> m_pDevice;
		RefCntAutoPtr<IDeviceContext> m_pImmediateContext;
		RefCntAutoPtr<IBuffer>                m_VertexBuffer[2];
		RefCntAutoPtr<IBuffer>                m_IndexBuffer;

		RefCntAutoPtr<IPipelineState> _pPSO;
		RefCntAutoPtr<IShaderResourceBinding> m_pSRB;
		std::vector<drawunit> _dus;
		using tnd = std::unique_ptr<GLTF::Node>;
		void AnimNodeFilter(GLTF::Node*& nil, std::vector<GLTF::Node*>& rut) {

			if (nil->_Mesh) {
				rut.emplace_back(nil);
			}
			for (auto& ni : nil->Children)
			{
				AnimNodeFilter(ni, rut);
			}
		}
		//int AniJnum(GLTF::Animation& animation, std::vector<GLTF::Node*>& rut) {

		//	for (auto& channel : animation.Channels)
		//	{
		//		//if(channel.node->_Skin->Joints.)
		//		AnimNodeFilter(channel.node, rut);
		//	}
		//	for (auto& anode:rut)
		//	{

		//	}
		//}
		void AnimNodeFilter(tnd& nil, std::vector<GLTF::Node*>& rut) {

			if (nil->_Skin && nil->_Mesh) {
				rut.emplace_back(nil.get());
			}
			for (auto& ni : nil->Children)
			{
				AnimNodeFilter(ni, rut);
			}
		}
		void AnimNodeFilter(Model::model& mod, const n_Render::modelidn& mdi, std::vector<GLTF::Node*>& rut) {

			auto& nd = mod.Nodes;

			for (size_t i = 0; i < mdi.ndl; i++)
			{
				auto& ni = nd[i + mdi.nds];
				AnimNodeFilter(ni, rut);
			}
		}
	public:
		//POBs<> _pobs[10];

		/*RefCntAutoPtr<IBuffer>                m_pTransAttribsBuffer;
		RefCntAutoPtr<IBuffer>                m_pinspropertysBuffer;
		RefCntAutoPtr<IBuffer>                m_pinsBuffer;
		RefCntAutoPtr<IBuffer>                M_pIndirectArgsBuff;*/
		void init(
			RenderContext& rc,
			IBuffer* pCameraAttribs,
			span< RefCntAutoPtr<IBuffer> >m_VertexBuffer,
			RefCntAutoPtr<IBuffer>                m_IndexBuffer,
			const uint IndexCount,
			const int instancenum = 1
		) {
			m_pDevice = rc.m_pDevice;
			m_pImmediateContext = (rc.m_pImmediateContext);
			//m_pEngineFactory = rc.pShaderSourceFactory;
			m_IndexBuffer = (m_IndexBuffer);
			this->IndexCount = (IndexCount);
			_vbufnum = m_VertexBuffer.size();
			//const auto& RenderTargetFormat=rc.
			for (size_t i = 0; i < m_VertexBuffer.size(); i++)
			{
				this->m_VertexBuffer[i] = m_VertexBuffer[i];
			}
			RefCntAutoPtr<IShaderSourceInputStreamFactory> pShaderSourceFactory = rc.pShaderSourceFactory;
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
			RenderUtil::CreatePipelineStates
			(&_pPSO, m_pDevice, rc.RenderTargetFormat, rc.DepthBufferFormat, pShaderSourceFactory,
				"NoSkin.hlsl", "vert", "frag", Vars, inputDesc);

			Init(m_pDevice, m_pImmediateContext);

			auto& pPSO = _pPSO;

			pPSO->GetStaticVariableByName(SHADER_TYPE_VERTEX, "cbCameraAttribs")->Set(pCameraAttribs);
			//pPSO->CreateShaderResourceBinding(&m_pSRB, true);

			/*if (m_pinsBuffer)
				initGarg(RenderTargetFormat, DepthBufferFormat,pCameraAttribs, pEngineFactory, IndexCount, m_pinsBuffer);*/
		}
		//using namespace Diligent;
		struct renderbatch {
			RefCntAutoPtr<IBuffer> trs, State, joints;
		};
		RefCntAutoPtr<IPipelineState> PSOcmp;
		RefCntAutoPtr<IPipelineState> PSOGra;
		void init(
			RenderContext& rc,
			Model::model& mod,
			const uint IndexCount,
			const int instancenum = 1
		) {
			auto& pCameraAttribs = rc.pCameraAttribs;
			{

				Diligent::ShaderResourceVariableDesc Vars[] =
				{
					//{SHADER_TYPE_COMPUTE, "queueFB", SHADER_RESOURCE_VARIABLE_TYPE_STATIC},
					{ Diligent::SHADER_TYPE_COMPUTE, "Constants",   Diligent::SHADER_RESOURCE_VARIABLE_TYPE_STATIC },
					{ Diligent::SHADER_TYPE_COMPUTE, "VfArg",       Diligent::SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE },
					{ Diligent::SHADER_TYPE_COMPUTE, "audio_bufin", Diligent::SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC },
					{ Diligent::SHADER_TYPE_COMPUTE, "audio_bufout",Diligent::SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE },
					{ Diligent::SHADER_TYPE_COMPUTE, "audio_inst",  Diligent::SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE },
					{ Diligent::SHADER_TYPE_COMPUTE, "RWVfArg",     Diligent::SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE },
					{ Diligent::SHADER_TYPE_COMPUTE, "Vf_Props",    Diligent::SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE },
					{ Diligent::SHADER_TYPE_COMPUTE, "audio_inst_out",Diligent::SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE },
					{ Diligent::SHADER_TYPE_COMPUTE, "cbCameraAttribs", Diligent::SHADER_RESOURCE_VARIABLE_TYPE_STATIC  }
				};

			}
			span<RefCntAutoPtr<IBuffer>>m_VertexBuffer = mod.pVertexBuffer;
			m_IndexBuffer = mod.pIndexBuffer;
			m_pDevice = rc.m_pDevice;
			m_pImmediateContext = (rc.m_pImmediateContext);
			//m_pEngineFactory = rc.pShaderSourceFactory;
			m_IndexBuffer = (m_IndexBuffer);
			this->IndexCount = (IndexCount);
			_vbufnum = m_VertexBuffer.size();
			//const auto& RenderTargetFormat=rc.
			for (size_t i = 0; i < m_VertexBuffer.size(); i++)
			{
				this->m_VertexBuffer[i] = m_VertexBuffer[i];
			}
			RefCntAutoPtr<IShaderSourceInputStreamFactory> pShaderSourceFactory = rc.pShaderSourceFactory;
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
			RenderUtil::CreatePipelineStates
			(&_pPSO, m_pDevice, rc.RenderTargetFormat, rc.DepthBufferFormat, pShaderSourceFactory,
				"NoSkin.hlsl", "vert", "frag", Vars, inputDesc);

			Init(m_pDevice, m_pImmediateContext);

			auto& pPSO = _pPSO;

			pPSO->GetStaticVariableByName(SHADER_TYPE_VERTEX, "cbCameraAttribs")->Set(pCameraAttribs);
			//pPSO->CreateShaderResourceBinding(&m_pSRB, true);

			/*if (m_pinsBuffer)
				initGarg(RenderTargetFormat, DepthBufferFormat,pCameraAttribs, pEngineFactory, IndexCount, m_pinsBuffer);*/
		}
		void addDrawEle(drawunit& du, const int inscap) {
			_pPSO->CreateShaderResourceBinding(&du.m_pSRB, true);
			auto& _SRB = du.m_pSRB;
			auto shv = m_pSRB->GetVariableByName(SHADER_TYPE_PIXEL, "g_ColorMap");
			auto inpro = m_pSRB->GetVariableByName(SHADER_TYPE_VERTEX, "instDatas");
			auto teins = m_pSRB->GetVariableByName(SHADER_TYPE_VERTEX, "objectTransfromBuffer");
			BufferDesc BuffDesc;
			BuffDesc.Name = "transform buffer insRander2";
			BuffDesc.Usage = USAGE_DYNAMIC;
			BuffDesc.BindFlags = BIND_SHADER_RESOURCE;
			BuffDesc.Mode = BUFFER_MODE_STRUCTURED;
			BuffDesc.CPUAccessFlags = CPU_ACCESS_WRITE;
			BuffDesc.ElementByteStride = sizeof(mtransfrom);
			BuffDesc.Size = sizeof(mtransfrom) * inscap;
			const auto shadertp = Diligent::SHADER_TYPE_VERTEX;

			RenderUtil::CreateSetBUf(m_pDevice, m_pImmediateContext, BuffDesc, &du.m_pTransAttribsBuffer,
				_SRB, shadertp, nullptr,
				BIND_SHADER_RESOURCE);
			using tt = sStruct::instData;
			BuffDesc.ElementByteStride = sizeof(tt);
			BuffDesc.Size = sizeof(tt) * inscap;
			RenderUtil::CreateSetBUf(m_pDevice, m_pImmediateContext, BuffDesc, &du.instancebuf,
				_SRB, shadertp, nullptr,
				BIND_SHADER_RESOURCE);
			if (shv)
				shv->Set(du.g_ColorMap ? m_pWhiteTexSRV : du.g_ColorMap->GetDefaultView(TEXTURE_VIEW_SHADER_RESOURCE));
			teins->Set(du.m_pTransAttribsBuffer->GetDefaultView(BUFFER_VIEW_SHADER_RESOURCE));
			inpro->Set(du.instancebuf->GetDefaultView(BUFFER_VIEW_SHADER_RESOURCE));
		}

		//void Render(Utility::span<> drawpara) {
		//	if (instancenum == 0)
		//		return;
		//	auto& pCtx = m_pImmediateContext;

		//	//IBuffer* pVBs[] = { m_VertexBuffer[0],m_VertexBuffer[1] };
		//	//Uint64   Offsets[_countof(pVBs)] = {};

		//	//pCtx->SetPipelineState(_pPSO);
		//	//// clang-format on
		//	//pCtx->TransitionResourceStates(_buftrsdesc.size(), _buftrsdesc.data());
		//	//pCtx->SetVertexBuffers(0, _vbufnum, pVBs, Offsets, RESOURCE_STATE_TRANSITION_MODE_VERIFY, SET_VERTEX_BUFFERS_FLAG_NONE);
		//	//pCtx->SetIndexBuffer(m_IndexBuffer, 0, RESOURCE_STATE_TRANSITION_MODE_VERIFY);
		//	for (size_t i = 0; i < _dus.size(); i++)
		//	{
		//		auto& ele = _dus[i];
		//		if (ele.DRG.NumInstances == 0)
		//			continue;
		//		pCtx->CommitShaderResources(m_pSRB, RESOURCE_STATE_TRANSITION_MODE_VERIFY);
		//		pCtx->DrawIndexed(ele.DRG);
		//	}


		//}
		//template<class ArgStruct = sStruct::VFoutArgs>
		//void IndirectDraw(const nonstd::span<ArgStruct> DrawArgs, RefCntAutoPtr<IBuffer>& pIndirectArgsBuff) {
		//	static std::vector<size_t> ArgPos;
		//	ArgPos.clear();
		//	for (size_t i = 0; i < DrawArgs.size(); i++)
		//	{
		//		if (DrawArgs[i].NumInstances == 0)
		//			continue;
		//		ArgPos.emplace_back(i * sizeof(ArgStruct));
		//	}
		//	if (ArgPos.empty())
		//		return;
		//	auto& pCtx = m_pImmediateContext;

		//	IBuffer* pVBs[] = { m_VertexBuffer };
		//	Uint64   Offsets[_countof(pVBs)] = {};
		//	//StateTransitionDesc Barriers[] =
		//	//{
		//	//	{m_pTransAttribsBuffer,  RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_SHADER_RESOURCE, STATE_TRANSITION_FLAG_UPDATE_STATE},
		//	//	{m_pinspropertysBuffer, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_SHADER_RESOURCE, STATE_TRANSITION_FLAG_UPDATE_STATE},
		//	//	/*{m_VertexBuffer, RESOURCE_STATE_TRANSITION_MODE_VERIFY, RESOURCE_STATE_SHADER_RESOURCE, STATE_TRANSITION_FLAG_UPDATE_STATE}
		//	//	{m_pinspropertysBuffer, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_SHADER_RESOURCE, STATE_TRANSITION_FLAG_UPDATE_STATE}*/
		//	//};

		//	pCtx->SetPipelineState(_pobs->PSO);
		//	// clang-format on
		//	//pCtx->TransitionResourceStates(_countof(Barriers), Barriers);
		//	pCtx->SetVertexBuffers(0, _countof(pVBs), pVBs, Offsets, RESOURCE_STATE_TRANSITION_MODE_VERIFY, SET_VERTEX_BUFFERS_FLAG_NONE);
		//	pCtx->SetIndexBuffer(m_IndexBuffer, 0, RESOURCE_STATE_TRANSITION_MODE_VERIFY);

		//	pCtx->CommitShaderResources(_pobs->SRBs[0], RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
		//	DrawIndexedIndirectAttribs drawAttrs{ VT_UINT32, DRAW_FLAG_VERIFY_ALL, RESOURCE_STATE_TRANSITION_MODE_TRANSITION };
		//	for (size_t i = 0; i < ArgPos.size(); i++)
		//	{

		//		drawAttrs.DrawArgsOffset = ArgPos[i];
		//		pCtx->DrawIndexedIndirect(drawAttrs, pIndirectArgsBuff);
		//	}

		//}
	};
}