#include"Render/Renders0.h"
#include<filesystem>
namespace pj2 {
	namespace RenderUtil {
		using Ichar = char;
		void CreateShader(RenderContext& rt,
			const std::string FilePath, const Ichar* ven, const Ichar* pen, Diligent::ShaderMacroHelper Macros) {
			using namespace Diligent;
			auto& m_pDevice = rt.m_pDevice;
			ShaderCreateInfo                               ShaderCI;
			ShaderCI.pShaderSourceStreamFactory = rt.pShaderSourceFactory;
			ShaderCI.SourceLanguage = SHADER_SOURCE_LANGUAGE_HLSL;
			ShaderCI.UseCombinedTextureSamplers = true;

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
			ShaderCI.Desc.Name = stbuffer.data();;
			ShaderCI.EntryPoint = pen;
			ShaderCI.FilePath = FilePath.c_str();
			ShaderCI.Desc.ShaderType = SHADER_TYPE_PIXEL;
			RefCntAutoPtr<IShader> pPS;
			m_pDevice->CreateShader(ShaderCI, &pPS);
		}
	}

	namespace n_Render {
		enum PSOMode
		{
			SToff, STwrite, STMask
		};
		using tnd = GLTF::Node*;
		using Model = ModelImp::model1;
		constexpr int GLTFVtex0 = sizeof(Model::VertexAttribs0);
		constexpr int GLTFVtex1 = sizeof(Model::VertexAttribs1);
		thread_local
			std::vector<tnd> sd(100);
		void SkinRender::LoadModel(Model& mod, RenderContext& rc) {
			using namespace Diligent::GLTF;
			const auto Samplerate = GameProps::Samplerate;
			auto& nd = mod.LinearNodes;
			auto& pDevice = rc.m_pDevice;
			const auto nummodel = _meshtable.size();
			auto& mdin = _meshtable.emplace_back();
			auto& propsize = mdin.propadss;
			propsize = _propsize;
			const auto& idesc = mod.pIndexBuffer->GetDesc();
			_propsize[modeloffset::anim] += mod.Animations.size();
			_propsize[modeloffset::mat] += mod.Materials.size();
			_propsize[modeloffset::etexture] += mod.Textures.size();
			_propsize[modeloffset::vb0] += mod.pVertexBuffer[0]->GetDesc().Size / GLTFVtex0;
			_propsize[modeloffset::vb1] += mod.pVertexBuffer[1]->GetDesc().Size / GLTFVtex1;
			_propsize[modeloffset::Ibuf] += idesc.Size / 4;//assume int as index type
			assert(_maxtexs > _propsize[modeloffset::etexture]);
			auto getsize = [&](const modeloffset propid) {return _propsize[propid] - propsize[propid]; };
			const auto modelId = _meshtable.size() - 1;
			thread_local std::vector<StateTransitionDesc> decc_;
			decc_.clear();
			mdin[0] = _prims.size();
			//mdin[3] = _cprim;
			auto& Vbf0 = mod.pVertexBuffer[0];
			sd.clear();
			for (auto& n : nd)
			{
				auto& p_prim = n->_Mesh;
				if (p_prim) {
					auto& prim = _prims.emplace_back(p_prim->Primitives[0]);
					sd.emplace_back(n);
				}
			}
			//mdin.StartVertexOffset=_mesh1.size()>1?_mesh1.back().StartVertexOffset+
			mdin[1] = _prims.size() - mdin[0];

			for (size_t i = 0; i < _vbufnum; i++)
			{
				auto& mvb = mod.pVertexBuffer[i];
				auto& loaclvb = m_VertexBuffer[i];
				loaclvb.merge(mvb, rc);
				decc_.emplace_back(loaclvb, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_VERTEX_BUFFER, STATE_TRANSITION_FLAG_UPDATE_STATE);
			}
			m_IndexBuffer.merge(mod.pIndexBuffer, rc);
			decc_.emplace_back(m_IndexBuffer, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_INDEX_BUFFER, STATE_TRANSITION_FLAG_UPDATE_STATE);
			const int tnum = mod.Textures.size();
			std::unordered_map<ui64, ui64> timap;
			timap.reserve(mod.Textures.size());
			size_t i = 0;

			for (i = 0; i < mod.Textures.size(); i++)
			{
				int texid = propsize[modeloffset::etexture]+i;
				RefCntAutoPtr<ITexture>& tx = mod.Textures[i];
				Diligent::ITextureView* vp = tx->GetDefaultView(TEXTURE_VIEW_SHADER_RESOURCE);
				_txvs[texid] = (vp);
				timap.emplace(tx->GetUniqueID(), texid);
			}
			_texused += i;
			v_textures->SetArray(_txvs.data(), 0, _txvs.size());


			if (mod.Animations.size() > 0 && !_fxmesh) {
				using t_bonerep = math::Matrix4f;
				thread_local std::vector<t_bonerep> bonerec(1024 * 1024);
				bonerec.clear();
				const int sks = 0;
				std::unique_ptr<Skin>& skin = mod.Skins[sks];
				AnimRec rec;
				rec.modelId = modelId;
				rec.bonenum = skin->Joints.size();
				ui64 boneoffset = 0;
				int incsum = 0;
				for (size_t i = 0; i < mod.Animations.size(); i++)
				{
					Animation& ani = mod.Animations[i];
					incsum += ((ani.End - ani.Start) / Samplerate);
				}
				auto delsize = bonerec.size() + incsum * rec.bonenum;
				bonerec.resize(bonerec.size() + delsize);
				for (size_t i = 0; i < mod.Animations.size(); i++)
				{
					Animation& ani = mod.Animations[i];
					rec.name = ani.Name;
					rec.base = boneoffset+propsize[modeloffset::bone];
					rec.size = 0;
					int inc = ((ani.End - ani.Start) / Samplerate);
					for (size_t isp = 0; isp < inc; isp++)
					{
						mod.UpdateAnimation(i, isp * Samplerate + ani.Start);
						const auto st = bonerec.size();
						assert((boneoffset + rec.size) < st);
						//assum all primitive share same bone
						std::memcpy(&(bonerec[boneoffset + rec.size]), sd[0]->_Mesh->Transforms.jointMatrix, rec.bonenum * sizeof(math::Matrix4f));
						rec.size += rec.bonenum;
					}
					boneoffset += rec.size;
					_AniReco.emplace_back(rec);
				}
				_Animbuf.merge(bonerec, rc);
				decc_.emplace_back(_Animbuf, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_SHADER_RESOURCE, STATE_TRANSITION_FLAG_UPDATE_STATE);
				_propsize[modeloffset::bone] += boneoffset;
			}
			//last texture for default texture
			const auto defid = _txvs.size() - 1;
			std::vector<gPrimitive> gCb(sd.size());
			for (size_t i = 0; i < sd.size(); i++)
			{
				auto& mes = sd[i]->_Mesh;
				auto& modprim = mes->Primitives[0];
				const auto bonenum = mes->Transforms.jointcount;
				//auto& cb0 = rbl._cbufpri;
				using ts = gPrimitive;
				Material& tsrc = mod.Materials[modprim.material];
				gPrimitive& mtdata = gCb[i];
				const auto gettexid = [&](RefCntAutoPtr<ITexture>& const tex)
				{return tex ? timap[tex->GetUniqueID()] : defid; };
				std::memcpy(&mtdata.NodeMatrix, &mes->Transforms.matrix, sizeof(Eigen::Matrix4f));
				mtdata.texid = gettexid(tsrc.pBaseColorTexture);
				mtdata.normaltexid = gettexid(tsrc.pNormalTexture);
				mtdata.bonenumb = bonenum;

			}

			_cbPrim.merge(gCb, rc);
			decc_.emplace_back(_cbPrim, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_CONSTANT_BUFFER, STATE_TRANSITION_FLAG_UPDATE_STATE);
			m_pImmediateContext->TransitionResourceStates(decc_.size(), decc_.data());
			decc_.clear();
			/*	for (size_t i = 0; i < _vbufnum; i++)
				{
					auto& mvb = mod.pVertexBuffer[i];
					auto& loaclvb = m_VertexBuffer[i];
					mvb->Release();
				}
				mod.pIndexBuffer->Release();*/
		}
		int SkinRender::WriteVData(void* pda)
		{
			auto& pCtx = m_pImmediateContext;
			pCtx->UpdateBuffer(_instbuf, 0, sizeof(sStruct::inscmb), pda,
				Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
			return 1;
		}
		int SkinRender::Draw(RenderContext& rt, Diligent::DrawIndexedAttribs& da)
		{
			auto& psb = _oPSOs[PSOMode::STwrite];
			auto& m_pSRB = psb.GetShaderResourceBinding();
			auto& pCtx = rt.m_pImmediateContext;
			pCtx->UpdateBuffer(_indraw, 0, sizeof(IndirArg) * dratt.size(), dratt.data(),
				Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
			IBuffer* pVBs[] = { m_VertexBuffer[0],m_VertexBuffer[1] };
			math::ui64   Offsets[_countof(pVBs)] = {};

			pCtx->SetPipelineState(psb.PSO);
			pCtx->TransitionResourceStates(1, _dbfstd.data());

			pCtx->CommitShaderResources(m_pSRB, RESOURCE_STATE_TRANSITION_MODE_VERIFY);

			pCtx->SetVertexBuffers(0, _vbufnum, pVBs, nullptr, RESOURCE_STATE_TRANSITION_MODE_VERIFY, SET_VERTEX_BUFFERS_FLAG_NONE);
			pCtx->SetIndexBuffer(m_IndexBuffer, 0, RESOURCE_STATE_TRANSITION_MODE_VERIFY);
			pCtx->DrawIndexed(da);
			return 1;
		}
		ui64 SkinRender::LoadModel(RenderContext& rc, Model& model) {

			const auto& check = modmap.try_emplace((ui64)model.pVertexBuffer[0].RawPtr(), _models.size());
			if (!check.second) {
				LOGINFO("model already loaded");
				return check.first->second;
			}
			this->_models.emplace_back(&model);
			auto& modl = model;
			LoadModel(modl, rc);
			return _models.size() - 1;
		}
		template<class T0, class T1>
		auto mksp(T1& t) { return Utility::span<T0>(t); }
		void SkinRender::init(RenderContext& rc, const int mxinstancenum, const int PowSTRIDE, const int vbufnum, const int maxtexs, const int maxanimbuf) {
			_maxtexs = maxtexs;
			_rc = rc;
			std::memset(_propsize.data(), 0, sizeof(_propsize));
			this->_PowSTRIDE = 16;// 32 - _PowSTRIDE;
			_dbfstd[0] = { _instbuf, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_SHADER_RESOURCE, STATE_TRANSITION_FLAG_UPDATE_STATE };
			_dbfstd[1] = { _transbuf, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_SHADER_RESOURCE, STATE_TRANSITION_FLAG_UPDATE_STATE };
			std::vector<StateTransitionDesc> decc_;
			decc_.reserve(32);
			_mxinstancenum = mxinstancenum;
			BufferDesc BuffDesc;
			if (!_Animbuf)
			{
				using ts = float4x4;
				RenderUtil::BuildBDesc<ts>(BuffDesc, maxanimbuf, "animreco",
					Diligent::BUFFER_MODE_STRUCTURED,
					Diligent::USAGE_DEFAULT, Diligent::BIND_SHADER_RESOURCE, Diligent::CPU_ACCESS_NONE);
				_Animbuf.reserve(rc, BuffDesc);
				decc_.emplace_back(_Animbuf, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_GENERIC_READ, STATE_TRANSITION_FLAG_UPDATE_STATE);
			}
			{
				BuffDesc.Name = "cbPrim";
				BuffDesc.BindFlags = Diligent::BIND_UNIFORM_BUFFER;
				BuffDesc.Size = sizeof(gPrimitive) * (maxtexs / 2);
				_cbPrim.reserve(rc, BuffDesc);
				decc_.emplace_back(_cbPrim, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_GENERIC_READ, STATE_TRANSITION_FLAG_UPDATE_STATE);
			}
			{
				auto& buf = this->_indraw;
				BuffDesc.Name = "indirect draw buffer";
				BuffDesc.BindFlags = Diligent::BIND_INDIRECT_DRAW_ARGS;
				BuffDesc.Size = sizeof(IndirArg) * (maxtexs / 2);
				BuffDesc.ElementByteStride = sizeof(IndirArg);
				buf.reserve(rc, BuffDesc);
				decc_.emplace_back(buf, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_GENERIC_READ, STATE_TRANSITION_FLAG_UPDATE_STATE);
			}
			{
				auto& buf = _cboline;
				BuffDesc.Name = "cbOl";
				BuffDesc.BindFlags = Diligent::BIND_UNIFORM_BUFFER;
				BuffDesc.Size = sizeof(sStruct::outlinecb);
				buf.reserve(rc, BuffDesc);
				buf->SetState(RESOURCE_STATE_GENERIC_READ);
				sStruct::outlinecb ouc;
				ouc.scf = 1.2;
				buf.updateat(ouc, _rc, 0);
				decc_.emplace_back(_cboline, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_GENERIC_READ, STATE_TRANSITION_FLAG_UPDATE_STATE);
			}
			/*{
				using ts = t_bfins;
				auto& buf = _instbuf;
				RenderUtil::BuildBDesc<ts>(BuffDesc, mxinstancenum, "skined insbuf",
					Diligent::BUFFER_MODE_STRUCTURED,
					Diligent::USAGE_DYNAMIC, Diligent::BIND_SHADER_RESOURCE);
				buf.reserve(rc, BuffDesc);
				decc_.emplace_back(buf, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_GENERIC_READ, STATE_TRANSITION_FLAG_UPDATE_STATE);

			}
			{
				using ts = math::mtransfrom;
				auto& buf = _transbuf;
				RenderUtil::BuildBDesc<ts>(BuffDesc, mxinstancenum, "buffer mtransfrom",
					Diligent::BUFFER_MODE_STRUCTURED,
					Diligent::USAGE_DYNAMIC, Diligent::BIND_SHADER_RESOURCE);
				buf.reserve(rc, BuffDesc);
				decc_.emplace_back(buf, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_GENERIC_READ, STATE_TRANSITION_FLAG_UPDATE_STATE);

			}*/
			rc.m_pImmediateContext->TransitionResourceStates(decc_.size(), decc_.data());
			_fxmesh = vbufnum == 1;
			this->_vbufnum = vbufnum;
			//_mod = mod;
			m_pDevice = rc.m_pDevice;
			m_pImmediateContext = (rc.m_pImmediateContext);
			if (m_IndexBuffer)
				m_IndexBuffer.reset();

			for (size_t i = 0; i < _vbufnum; i++)
			{
				if (m_VertexBuffer[i])
					m_VertexBuffer[i].reset();
			}
			auto& pCameraAttribs = rc.pCameraAttribs;

			RefCntAutoPtr<IShaderSourceInputStreamFactory> pShaderSourceFactory = rc.pShaderSourceFactory;
			ShaderResourceVariableDesc Vars[] =
			{
				{ SHADER_TYPE_PIXEL, "g_ColorMap", SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC },
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
				{ 4, 1, 4, VT_FLOAT32 },   //float3 Pos     : ATTRIB0;
				{ 5, 1, 4, VT_FLOAT32 },   //float3 Normal  : ATTRIB1;
			};
			ShaderMacroHelper mcr;
			mcr.AddShaderMacro("NUM_PRIM", maxtexs >> 2);//max perimitive permodel based on maxtexs/2
			mcr.AddShaderMacro("NUM_TEXTURES", maxtexs);
			mcr.AddShaderMacro("BOSTRIDE", _PowSTRIDE);
			if (combinedinsbuffer) {
				mcr.AddShaderMacro("SCMB", 1);
			}
			if (_fxmesh) {
				mcr.AddShaderMacro("VTXCOLOR", 1);
				mcr.AddShaderMacro("FIXMESH", 1);
			}
			Utility::span<LayoutElement> lr = inputDesc;
			lr = _fxmesh ? lr.subspan(0, 4) : lr;
			_pPSO = NULL;
			RenderUtil::CreatePipelineStates
			(&_pPSO, m_pDevice, rc.RenderTargetFormat, rc.DepthBufferFormat,
				pShaderSourceFactory,
				"skined.hlsl", "vert", "frag", Vars, mcr, lr);
			{

				RefCntAutoPtr<IShader> pVS;
				RefCntAutoPtr<IShader> pPS;
				const std::string FilePath = "skined.hlsl";
				Diligent::ShaderMacroHelper& Macros = mcr;
				using Ichar = char;
				const Ichar* ven = "vert";
				const Ichar* pen = "frag";
				std::string tail;
				std::array<char, 4096> stbuffer;
				auto CrSh = [&](RenderContext& rt) {
					using namespace Diligent;
					auto& m_pDevice = rt.m_pDevice;
					pVS = NULL;
					pPS = NULL;
					ShaderCreateInfo                               ShaderCI;
					ShaderCI.pShaderSourceStreamFactory = rt.pShaderSourceFactory;
					ShaderCI.SourceLanguage = SHADER_SOURCE_LANGUAGE_HLSL;
					ShaderCI.UseCombinedTextureSamplers = true;
					ShaderCI.Macros = Macros;

					//fixed_iterator<char*> fi(stbuffer.data(), sizeof(stbuffer));
					//stbuffer.reserve(4096);
					std::memset(stbuffer.data(), 0, sizeof(stbuffer));
					fmt::format_to_n(stbuffer.data(), stbuffer.size(), "{0} {1}-{2}", FilePath, ven, tail);
					//Utility::append(stbuffer, FilePath.c_str(), '_'+ven);
					//fmt::format_to(std::back_inserter(stbuffer), "FilePath is {0} name={1}", FilePath,vnm==NULL? ven :vnm);
					ShaderCI.Desc.ShaderType = SHADER_TYPE_VERTEX;
					//stbuffer.push_back('/0');
					ShaderCI.Desc.Name = stbuffer.data();
					ShaderCI.EntryPoint = ven;
					ShaderCI.FilePath = FilePath.c_str();
					m_pDevice->CreateShader(ShaderCI, &pVS);

					std::memset(stbuffer.data(), 0, sizeof(stbuffer));
					fmt::format_to_n(stbuffer.data(), stbuffer.size(), "{0} {1}-{2}", FilePath, pen, tail);
					ShaderCI.Desc.Name = stbuffer.data();;
					ShaderCI.EntryPoint = pen;
					ShaderCI.FilePath = FilePath.c_str();
					ShaderCI.Desc.ShaderType = SHADER_TYPE_PIXEL;
					m_pDevice->CreateShader(ShaderCI, &pPS);
				};
				GraphicsPipelineStateCreateInfo PSOCreateInfo;
				PSOCreateInfo.GraphicsPipeline = _pPSO->GetGraphicsPipelineDesc();
				PSOCreateInfo.PSODesc = _pPSO->GetDesc();
				Diligent::GraphicsPipelineDesc& GraphicsPipeline = PSOCreateInfo.GraphicsPipeline;
				PipelineStateDesc& PSODesc = PSOCreateInfo.PSODesc;

				{
					tail = "mswrite";
					CrSh(rc);
					PSODesc.Name = tail.c_str();// FilePath.c_str();
					PSOCreateInfo.pVS = pVS;
					PSOCreateInfo.pPS = pPS;
					auto& dsd = GraphicsPipeline.DepthStencilDesc;
					auto& pPso = _oPSOs[PSOMode::STwrite].PSO;

					//write
					dsd.StencilEnable = 1;
					dsd.FrontFace.StencilFunc = COMPARISON_FUNC_ALWAYS;
					dsd.FrontFace.StencilPassOp = STENCIL_OP_REPLACE;
					m_pDevice->CreateGraphicsPipelineState(PSOCreateInfo, &pPso);
				}
				{
					tail = "mask";
					Macros.AddShaderMacro("VTXCOLOR", 1);
					Macros.AddShaderMacro("OL", 1);
					CrSh(rc);
					PSODesc.Name = tail.c_str();
					PSOCreateInfo.pVS = pVS;
					PSOCreateInfo.pPS = pPS;
					auto& dsd = GraphicsPipeline.DepthStencilDesc;
					auto& pPso = _oPSOs[PSOMode::STMask].PSO;
					GraphicsPipeline.DepthStencilDesc.DepthEnable = 0;
					dsd.StencilEnable = 1;
					dsd.FrontFace.StencilFunc = COMPARISON_FUNC_NOT_EQUAL;
					dsd.FrontFace.StencilPassOp = STENCIL_OP_KEEP;
					m_pDevice->CreateGraphicsPipelineState(PSOCreateInfo, &pPso);
					pPso->GetStaticVariableByName(SHADER_TYPE_VERTEX, "cbOl")->Set(_cboline);
				}
				for (auto& pb : _oPSOs)
				{
					if (pb.PSO) {
						auto& pPso = pb.PSO;
						auto vtb = pPso->GetStaticVariableByName(Diligent::SHADER_TYPE_VERTEX, "instDatas");
						if (vtb)
							vtb->Set(_instbuf->GetDefaultView(Diligent::BUFFER_VIEW_SHADER_RESOURCE));
						auto ptbuf = pPso->GetStaticVariableByName(Diligent::SHADER_TYPE_VERTEX, "objectTransfromBuffer");
						if (ptbuf)
							ptbuf->Set(_transbuf->GetDefaultView(Diligent::BUFFER_VIEW_SHADER_RESOURCE));
						pPso->GetStaticVariableByName(SHADER_TYPE_VERTEX, "cbCameraAttribs")->Set(pCameraAttribs);
					}
				}
			}
			Diligent::PipelineResourceSignatureDesc sd;
			auto vtb = _pPSO->GetStaticVariableByName(Diligent::SHADER_TYPE_VERTEX, "instDatas");
			IBuffer* vbs[] = {
				_cbPrim,
				_Animbuf
			};
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
			auto spva = mksp<ShaderResourceVariableDesc>(Varsl);
			auto spbvs = mksp<Diligent::BUFFER_VIEW_TYPE>(bvs);
			auto spvbs = mksp<IBuffer*>(vbs);
			if (!_fxmesh) {
				RenderUtil::setVbuffer(NULL, _pPSO, spva, bvs, vbs);
				for (auto& pb : _oPSOs)
				{
					if (pb.PSO) {
						RenderUtil::setVbuffer(NULL, pb.PSO, spva, spbvs, spvbs);
						pb.CreateShaderResourceBinding();
					}
				}
			}
			else
			{
				//RenderUtil::setVbuffer(NULL, _pPSO, spva, bvs, vbs);
				for (auto& pb : _oPSOs)
				{
					if (pb.PSO) {
						//RenderUtil::setVbuffer(NULL, pb.PSO, spva.subspan(0, 1), spbvs.subspan(0, 1), spvbs.subspan(0, 1));
						pb.CreateShaderResourceBinding();
					}
				}
			}
			if (vtb)
				vtb->Set(_instbuf->GetDefaultView(Diligent::BUFFER_VIEW_SHADER_RESOURCE));
			auto ptbuf = _pPSO->GetStaticVariableByName(Diligent::SHADER_TYPE_VERTEX, "objectTransfromBuffer");
			if (ptbuf)
				ptbuf->Set(_transbuf->GetDefaultView(Diligent::BUFFER_VIEW_SHADER_RESOURCE));
			Init(m_pDevice, m_pImmediateContext);
			_pPSO->GetStaticVariableByName(SHADER_TYPE_VERTEX, "cbCameraAttribs")->Set(pCameraAttribs);


			//set default texture 
			_txvs.resize(maxtexs);
			for (auto& tv : _txvs)
			{
				tv = m_pDefaultNormalMapSRV;
			}
			this->init();
		}

		void SkinRender::Render(Utility::span<srDrawArg> inss, const int& NumIndices_) {

			auto& pCtx = m_pImmediateContext;

			auto& psb = _oPSOs[PSOMode::STwrite];
			auto& m_pSRB = psb.GetShaderResourceBinding();
			auto& pPSO = psb.PSO;
			pCtx->SetStencilRef(0xff);
			/*auto& cullres = scrf._cbfread;
			if (cullres.size() == 0)
			return;*/

			IBuffer* pVBs[] = { m_VertexBuffer[0],m_VertexBuffer[1] };
			math::ui64   Offsets[_countof(pVBs)] = {};
			//_insFilter.clear();
			pCtx->SetPipelineState(pPSO);
			pCtx->TransitionResourceStates(1, _dbfstd.data());
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
			int k = 0, insc = 0;

			for (auto& ins : inss)//for (size_t i = 0; i < _mesh1.size(); i++)
			{
				const auto& drg = ins;// s[i];
				insc = drg.FirstInstanceLocation;
				const auto& NumInstances = drg.NumInstances;
				if (NumInstances == 0) [[unlikely]]
					continue;
				auto& ele = attribs;
				ele.NumInstances = NumInstances;
				//auto& prmc = _mesh1[i];
				//for (size_t i2 = 0; i2 < prmc[1]; i2++, k++) 
				{
					ele.FirstInstanceLocation = ((k << _PowSTRIDE)) | insc;//inc up to 1<<_PowSTRIDE
					//auto& prim = _prims[i2 + prmc[0]];
					ele.NumIndices = NumIndices_;
					ele.FirstIndexLocation = 0;
					pCtx->DrawIndexed(ele);
				}
			}
			_instancenum = 0;
		}

		void SkinRender::Render(Utility::span<srDrawArg> inss) {
			auto&& [num, bvtx, modid, baseis] = inss[0];
			auto& prmc = _meshtable[modid];

			IndirArg dratt[16];
			size_t i2 = 0, k = 0;
			//k is  for index per primitive material
			for (; i2 < prmc[1]; i2++, k++) {
				auto& attr = dratt[k];
				attr.BaseVertex = bvtx;
				const UINT32 primid = i2 + prmc[0];
				attr.NumInstances = num;
				CalFirstInstanceLocation(attr.FirstInstanceLocation, primid, baseis);
				auto& prim = _prims[primid];
				attr.NumIndices = prim.IndexCount;
				attr.FirstIndexLocation = prim.FirstIndex;
			}
			auto& psb = _oPSOs[PSOMode::STwrite];
			auto& m_pSRB = psb.GetShaderResourceBinding();
			auto& pCtx = m_pImmediateContext;
			pCtx->UpdateBuffer(_indraw, 0, sizeof(IndirArg) * k, dratt,
				Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
			IBuffer* pVBs[] = { m_VertexBuffer[0],m_VertexBuffer[1] };
			math::ui64   Offsets[_countof(pVBs)] = {};

			std::array< StateTransitionDesc, 3> dbfstd;
			int pi = this->combinedinsbuffer ? 1 : 2;
			std::memcpy(dbfstd.data(), _dbfstd.data(), pi * sizeof(StateTransitionDesc));
			dbfstd[pi] = { _indraw, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_INDIRECT_ARGUMENT, STATE_TRANSITION_FLAG_UPDATE_STATE };

			pi++;
			pCtx->SetPipelineState(psb.PSO);
			pCtx->TransitionResourceStates(pi, dbfstd.data());

			pCtx->CommitShaderResources(m_pSRB, RESOURCE_STATE_TRANSITION_MODE_VERIFY);
			DrawIndexedIndirectAttribs iattr(VT_UINT32, _indraw, DRAW_FLAG_VERIFY_ALL, k);
			iattr.DrawArgsStride = sizeof(IndirArg);
			// clang-format on
			pCtx->SetVertexBuffers(0, _vbufnum, pVBs, nullptr, RESOURCE_STATE_TRANSITION_MODE_VERIFY, SET_VERTEX_BUFFERS_FLAG_NONE);
			pCtx->SetIndexBuffer(m_IndexBuffer, 0, RESOURCE_STATE_TRANSITION_MODE_VERIFY);
			pCtx->DrawIndexedIndirect(iattr);
		}

		void SkinRender::RenderOl(Utility::span<srDrawArg> inss) {
			if (inss.size() == 0) { return; }
			SkinRender& sr = *this;
			auto& pCtx = sr.m_pImmediateContext;
			auto& m_VertexBuffer = sr.m_VertexBuffer;
			auto& m_IndexBuffer = sr.m_IndexBuffer;
			auto& _mesh1 = sr._meshtable;
			auto& _prims = sr._prims;
			auto& psb = sr._oPSOs[PSOMode::STMask];
			auto& m_pSRB = psb.GetShaderResourceBinding();
			pCtx->SetStencilRef(0xff);
			IBuffer* pVBs[] = { m_VertexBuffer[0],m_VertexBuffer[1] };
			math::ui64   Offsets[_countof(pVBs)] = {};
			//_insFilter.clear();
			pCtx->SetPipelineState(psb.PSO);
			//pCtx->TransitionResourceStates(this->combinedinsbuffer ? 1 : 2, _dbfstd.data());
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
			pCtx->SetVertexBuffers(0, _vbufnum, pVBs, Offsets, RESOURCE_STATE_TRANSITION_MODE_VERIFY, SET_VERTEX_BUFFERS_FLAG_NONE);
			pCtx->SetIndexBuffer(m_IndexBuffer, 0, RESOURCE_STATE_TRANSITION_MODE_VERIFY);

			pCtx->CommitShaderResources(m_pSRB, RESOURCE_STATE_TRANSITION_MODE_VERIFY);
			/*auto& rb = _RBs[0];
			pCtx->CommitShaderResources(rb.m_pSRB, RESOURCE_STATE_TRANSITION_MODE_VERIFY);*/
			DrawIndexedAttribs attribs(0, VT_UINT32, DRAW_FLAG_VERIFY_ALL);
			attribs.BaseVertex = 0;
			attribs.Flags |= DRAW_FLAG_DYNAMIC_RESOURCE_BUFFERS_INTACT;
			// clang-format on
			//const uint inststri=
			int k = 0, insc = 0;

			for (auto& ins : inss)
			{
				const auto& NumInstances = ins.NumInstances;
				const auto& FirstInstanceLocation = ins.FirstInstanceLocation;
				if (NumInstances == 0)
					continue;
				auto& ele = attribs;
				ele.NumInstances = NumInstances;
				auto& prmc = _mesh1[ins.modiid];
				for (size_t i2 = 0; i2 < prmc[1]; i2++, k++) {
					sr.CalFirstInstanceLocation(ele.FirstInstanceLocation, k, FirstInstanceLocation);
					auto& prim = _prims[i2 + prmc[0]];
					ele.NumIndices = prim.IndexCount;
					ele.FirstIndexLocation = prim.FirstIndex;
					pCtx->DrawIndexed(ele);
				}
				insc += NumInstances;
			}
			/*for (size_t i = 0; i < _mesh1.size(); i++)
			{
			const auto& NumInstances = inss[i].NumInstances;
			const auto& FirstInstanceLocation = inss[i].FirstInstanceLocation;
			if (NumInstances == 0)
			continue;
			auto& ele = attribs;
			ele.NumInstances = NumInstances;
			auto& prmc = _mesh1[i];
			for (size_t i2 = 0; i2 < prmc[1]; i2++, k++) {
			CalFirstInstanceLocation(ele.FirstInstanceLocation, k, FirstInstanceLocation);
			auto& prim = _prims[i2 + prmc[0]];
			ele.NumIndices = prim.IndexCount;
			ele.FirstIndexLocation = prim.FirstIndex;
			pCtx->DrawIndexed(ele);
			}
			insc += NumInstances;
			}*/
		}

		//model id get from loadmodel,instance count,FirstInstanceLocation,

		void SkinRender::push(const rbs& pa) {
			auto&& [modid, num, baseis, p] = pa;
			auto& prmc = _meshtable[modid];
			//k is  for index per primitive material
			for (size_t i2 = 0, k = 0; i2 < prmc[1]; i2++, k++) {
				auto& attr = dratt.emplace_back();
				const UINT32 primid = i2 + prmc[0];
				attr.NumInstances = num;
				CalFirstInstanceLocation(attr.FirstInstanceLocation, primid, baseis);
				int prim4id = attr.FirstInstanceLocation >> this->_PowSTRIDE;
				auto& prim = _prims[primid];
				attr.NumIndices = prim.IndexCount;
				attr.FirstIndexLocation = prmc.propadss[modeloffset::Ibuf] + prim.FirstIndex;
				attr.BaseVertex = prmc.propadss[modeloffset::vb0];
			}

		}
		void SkinRender::Render() {
			if (dratt.size() == 0)
				return;
			auto& psb = _oPSOs[PSOMode::STwrite];
			auto& m_pSRB = psb.GetShaderResourceBinding();
			auto& pCtx = m_pImmediateContext;
			pCtx->UpdateBuffer(_indraw, 0, sizeof(IndirArg) * dratt.size(), dratt.data(),
				Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
			IBuffer* pVBs[] = { m_VertexBuffer[0],m_VertexBuffer[1] };
			math::ui64   Offsets[_countof(pVBs)] = {};

			std::array< StateTransitionDesc, 3> dbfstd;
			int pi = this->combinedinsbuffer ? 1 : 2;
			std::memcpy(dbfstd.data(), _dbfstd.data(), pi * sizeof(StateTransitionDesc));
			dbfstd[pi] = { _indraw, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_INDIRECT_ARGUMENT, STATE_TRANSITION_FLAG_UPDATE_STATE };

			pi++;
			pCtx->SetPipelineState(psb.PSO);
			pCtx->TransitionResourceStates(pi, dbfstd.data());

			pCtx->CommitShaderResources(m_pSRB, RESOURCE_STATE_TRANSITION_MODE_VERIFY);
			DrawIndexedIndirectAttribs iattr(VT_UINT32, _indraw, DRAW_FLAG_VERIFY_ALL, dratt.size());
			iattr.DrawArgsStride = sizeof(IndirArg);
			// clang-format on
			pCtx->SetVertexBuffers(0, _vbufnum, pVBs, nullptr, RESOURCE_STATE_TRANSITION_MODE_VERIFY, SET_VERTEX_BUFFERS_FLAG_NONE);
			pCtx->SetIndexBuffer(m_IndexBuffer, 0, RESOURCE_STATE_TRANSITION_MODE_VERIFY);
			pCtx->DrawIndexedIndirect(iattr);
			dratt.clear();
		}

		void SkinRender::init() {
			//_pPSO->CreateShaderResourceBinding(&m_pSRB, true);
			auto& psb = _oPSOs[PSOMode::STwrite];
			auto& m_pSRB = psb.GetShaderResourceBinding();
			if (!_fxmesh) {
				if (auto p = m_pSRB->GetVariableByName(SHADER_TYPE_PIXEL, "g_ColorMap"))
					v_textures = p;
			}
			/*	else
				{
					_pPSO->CreateShaderResourceBinding(&m_pSRB, true);
				}*/
		}
		void dyvbf::merge(RefCntAutoPtr<IBuffer>& vdatain, RenderContext& rt, int expandfactor) {
			auto VBDesc = vdatain->GetDesc();
			auto sz = VBDesc.Size;
			auto& pDevice = rt.m_pDevice;
			if (sz + _size < cap) {
				const auto& vd = m_pObject->GetDesc();
				assert(VBDesc.BindFlags == vd.BindFlags && VBDesc.Mode == vd.Mode);
				rt.m_pImmediateContext->CopyBuffer(vdatain, 0, RESOURCE_STATE_TRANSITION_MODE_TRANSITION,
					m_pObject, _size, sz, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
			}
			else
			{
				{
					cap = (sz + _size) * expandfactor;
					VBDesc.Size = cap;
					VBDesc.Usage = USAGE_DEFAULT;
					IBuffer* pn = NULL;
					pDevice->CreateBuffer(VBDesc, NULL, &pn);
					if (m_pObject)
					{
						rt.m_pImmediateContext->CopyBuffer(m_pObject, 0, RESOURCE_STATE_TRANSITION_MODE_TRANSITION,
							pn, 0, _size, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
						m_pObject->Release();
					}
					m_pObject = pn;
					rt.m_pImmediateContext->CopyBuffer(vdatain, 0, RESOURCE_STATE_TRANSITION_MODE_TRANSITION,
						m_pObject, _size, sz, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
				}
			}
			_size += sz;
		}
		void OlskRender::init(SkinRender& fr) {
			p_sr = &fr;
		}
		void FixRender::init(RenderContext& rc, const int mxinstancenum, const int maxtexs) {
			_maxtexs = maxtexs;
			this->_rc = rc;
			this->_fxmesh = 1;
			this->combinedinsbuffer = maxtexs == 256 ? combinedinsbuffer : false;
			const int vbufnum = 1;
			std::memset(_propsize.data(), 0, sizeof(_propsize));
			this->_PowSTRIDE = PowSTRIDE;
			_dbfstd[0] = { _instbuf, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_SHADER_RESOURCE, STATE_TRANSITION_FLAG_UPDATE_STATE };
			_dbfstd[1] = { _transbuf, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_SHADER_RESOURCE, STATE_TRANSITION_FLAG_UPDATE_STATE };
			std::vector<StateTransitionDesc> decc_;
			decc_.reserve(32);
			_mxinstancenum = mxinstancenum;
			BufferDesc BuffDesc;
			{
				BuffDesc.Name = "cbPrim";
				BuffDesc.BindFlags = Diligent::BIND_UNIFORM_BUFFER;
				BuffDesc.Size = sizeof(gPrimitive) * (maxtexs / 2);
				_cbPrim.reserve(rc, BuffDesc);
				decc_.emplace_back(_cbPrim, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_GENERIC_READ, STATE_TRANSITION_FLAG_UPDATE_STATE);
			}
			{
				using ts = math::mtransform;
				BuffDesc.Name = "_cbPrtrs";
				BuffDesc.BindFlags = Diligent::BIND_UNIFORM_BUFFER;
				BuffDesc.Size = sizeof(ts);
				_cbPrtrs.reserve(rc, BuffDesc);
				decc_.emplace_back(_cbPrtrs, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_GENERIC_READ, STATE_TRANSITION_FLAG_UPDATE_STATE);
			}
			/*{
			using ts = t_bfins;
			auto& buf = _instbuf;
			RenderUtil::BuildBDesc<ts>(BuffDesc, mxinstancenum, "skined insbuf",
			Diligent::BUFFER_MODE_STRUCTURED,
			Diligent::USAGE_DYNAMIC, Diligent::BIND_SHADER_RESOURCE);
			buf.reserve(rc, BuffDesc);
			decc_.emplace_back(buf, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_GENERIC_READ, STATE_TRANSITION_FLAG_UPDATE_STATE);

			}
			{
			using ts = math::mtransfrom;
			auto& buf = _transbuf;
			RenderUtil::BuildBDesc<ts>(BuffDesc, mxinstancenum, "buffer mtransfrom",
			Diligent::BUFFER_MODE_STRUCTURED,
			Diligent::USAGE_DYNAMIC, Diligent::BIND_SHADER_RESOURCE);
			buf.reserve(rc, BuffDesc);
			decc_.emplace_back(buf, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_GENERIC_READ, STATE_TRANSITION_FLAG_UPDATE_STATE);

			}*/
			rc.m_pImmediateContext->TransitionResourceStates(decc_.size(), decc_.data());
			_fxmesh = vbufnum == 1;
			this->_vbufnum = vbufnum;
			//_mod = mod;
			m_pDevice = rc.m_pDevice;
			m_pImmediateContext = (rc.m_pImmediateContext);
			if (m_IndexBuffer)
				m_IndexBuffer.reset();

			for (size_t i = 0; i < _vbufnum; i++)
			{
				if (m_VertexBuffer[i])
					m_VertexBuffer[i].reset();
			}
			auto& pCameraAttribs = rc.pCameraAttribs;

			RefCntAutoPtr<IShaderSourceInputStreamFactory> pShaderSourceFactory = rc.pShaderSourceFactory;
			ShaderResourceVariableDesc Vars[] =
			{
				{ SHADER_TYPE_PIXEL, "g_ColorMap", SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC },
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
				{ 4, 1, 4, VT_FLOAT32 },   //float3 Pos     : ATTRIB0;
				{ 5, 1, 4, VT_FLOAT32 },   //float3 Normal  : ATTRIB1;
			};
			ShaderMacroHelper mcr;
			constexpr int isst = 1 << 17;
			constexpr int is8st = 1 << (32 - 17);
			constexpr int isste = (isst + 545) >> 17;
			constexpr int is80s = (isst + 545) % (1 << 17);

			mcr.AddShaderMacro("NUM_PRIM", maxtexs >> 2);//max perimitive permodel based on maxtexs/2
			mcr.AddShaderMacro("NUM_TEXTURES", maxtexs);
			mcr.AddShaderMacro("BOSTRIDE", _PowSTRIDE);
			mcr.AddShaderMacro("PRETRANSFORM", 1);
			if (combinedinsbuffer) {
				mcr.AddShaderMacro("SCMB", 1);
			}
			if (_fxmesh) {
				mcr.AddShaderMacro("VTXCOLOR", 1);
				mcr.AddShaderMacro("FIXMESH", 1);
			}
			Utility::span<LayoutElement> lr = inputDesc;
			lr = _fxmesh ? lr.subspan(0, 4) : lr;
			_pPSO = NULL;
			RenderUtil::CreatePipelineStates
			(&_pPSO, m_pDevice, rc.RenderTargetFormat, rc.DepthBufferFormat,
				pShaderSourceFactory,
				"skined.hlsl", "vert", "frag", Vars, mcr, lr);
			{

				RefCntAutoPtr<IShader> pVS;
				RefCntAutoPtr<IShader> pPS;
				const std::string FilePath = "skined.hlsl";
				Diligent::ShaderMacroHelper& Macros = mcr;
				using Ichar = char;
				const Ichar* ven = "vert";
				const Ichar* pen = "frag";
				std::string tail;
				auto CrSh = [&](RenderContext& rt) {
					using namespace Diligent;
					auto& m_pDevice = rt.m_pDevice;
					pVS = NULL;
					pPS = NULL;
					ShaderCreateInfo                               ShaderCI;
					ShaderCI.pShaderSourceStreamFactory = rt.pShaderSourceFactory;
					ShaderCI.SourceLanguage = SHADER_SOURCE_LANGUAGE_HLSL;
					ShaderCI.UseCombinedTextureSamplers = true;
					ShaderCI.Macros = Macros;

					std::array<char, 4096> stbuffer;
					//fixed_iterator<char*> fi(stbuffer.data(), sizeof(stbuffer));
					//stbuffer.reserve(4096);
					std::memset(stbuffer.data(), 0, sizeof(stbuffer));
					fmt::format_to_n(stbuffer.data(), stbuffer.size(), "{0} {1}-{2}", FilePath, ven, tail);
					//Utility::append(stbuffer, FilePath.c_str(), '_'+ven);
					//fmt::format_to(std::back_inserter(stbuffer), "FilePath is {0} name={1}", FilePath,vnm==NULL? ven :vnm);
					ShaderCI.Desc.ShaderType = SHADER_TYPE_VERTEX;
					//stbuffer.push_back('/0');
					ShaderCI.Desc.Name = stbuffer.data();
					ShaderCI.EntryPoint = ven;
					ShaderCI.FilePath = FilePath.c_str();
					m_pDevice->CreateShader(ShaderCI, &pVS);

					std::memset(stbuffer.data(), 0, sizeof(stbuffer));
					fmt::format_to_n(stbuffer.data(), stbuffer.size(), "{0} {1}-{2}", FilePath, pen, tail);
					ShaderCI.Desc.Name = stbuffer.data();;
					ShaderCI.EntryPoint = pen;
					ShaderCI.FilePath = FilePath.c_str();
					ShaderCI.Desc.ShaderType = SHADER_TYPE_PIXEL;
					m_pDevice->CreateShader(ShaderCI, &pPS);
				};
				GraphicsPipelineStateCreateInfo PSOCreateInfo;
				PSOCreateInfo.GraphicsPipeline = _pPSO->GetGraphicsPipelineDesc();
				PSOCreateInfo.PSODesc = _pPSO->GetDesc();
				Diligent::GraphicsPipelineDesc& GraphicsPipeline = PSOCreateInfo.GraphicsPipeline;
				PipelineStateDesc& PSODesc = PSOCreateInfo.PSODesc;

				{
					tail = "mswrite";
					CrSh(rc);
					PSODesc.Name = FilePath.c_str();
					PSOCreateInfo.pVS = pVS;
					PSOCreateInfo.pPS = pPS;
					auto& dsd = GraphicsPipeline.DepthStencilDesc;
					auto& pPso = _oPSOs[PSOMode::STwrite].PSO;

					//write
					dsd.StencilEnable = 1;
					dsd.FrontFace.StencilFunc = COMPARISON_FUNC_ALWAYS;
					dsd.FrontFace.StencilPassOp = STENCIL_OP_REPLACE;
					m_pDevice->CreateGraphicsPipelineState(PSOCreateInfo, &pPso);
				}
				{
					tail = "mask";
					Macros.AddShaderMacro("VTXCOLOR", 1);
					Macros.AddShaderMacro("OL", 1);
					CrSh(rc);
					PSOCreateInfo.pVS = pVS;
					PSOCreateInfo.pPS = pPS;
					auto& dsd = GraphicsPipeline.DepthStencilDesc;
					auto& pPso = _oPSOs[PSOMode::STMask].PSO;
					GraphicsPipeline.DepthStencilDesc.DepthEnable = 0;
					dsd.StencilEnable = 1;
					dsd.FrontFace.StencilFunc = COMPARISON_FUNC_NOT_EQUAL;
					dsd.FrontFace.StencilPassOp = STENCIL_OP_KEEP;
					m_pDevice->CreateGraphicsPipelineState(PSOCreateInfo, &pPso);
					pPso->GetStaticVariableByName(SHADER_TYPE_VERTEX, "cbOl")->Set(_cboline);
				}
				for (auto& pb : _oPSOs)
				{
					if (pb.PSO) {
						auto& pPso = pb.PSO;

						auto& m_pSRB = pb.GetShaderResourceBinding();
						auto vtb = pPso->GetStaticVariableByName(Diligent::SHADER_TYPE_VERTEX, "instDatas");
						if (vtb)
							vtb->Set(_instbuf->GetDefaultView(Diligent::BUFFER_VIEW_SHADER_RESOURCE));
						auto ptbuf = pPso->GetStaticVariableByName(Diligent::SHADER_TYPE_VERTEX, "objectTransfromBuffer");
						if (ptbuf)
							ptbuf->Set(_transbuf->GetDefaultView(Diligent::BUFFER_VIEW_SHADER_RESOURCE));
						pPso->GetStaticVariableByName(SHADER_TYPE_VERTEX, "cbCameraAttribs")->Set(pCameraAttribs);
						pPso->GetStaticVariableByName(SHADER_TYPE_VERTEX, "cbpretrs")->Set(_cbPrtrs);
					}
				}
			}
			Diligent::PipelineResourceSignatureDesc sd;
			auto vtb = _pPSO->GetStaticVariableByName(Diligent::SHADER_TYPE_VERTEX, "instDatas");
			IBuffer* vbs[] = {
				_cbPrim,
				_Animbuf
			};
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
			auto spva = mksp<ShaderResourceVariableDesc>(Varsl);
			auto spbvs = mksp<Diligent::BUFFER_VIEW_TYPE>(bvs);
			auto spvbs = mksp<IBuffer*>(vbs);
			if (!_fxmesh) {
				RenderUtil::setVbuffer(NULL, _pPSO, spva, bvs, vbs);
				for (auto& pb : _oPSOs)
				{
					if (pb.PSO) {
						RenderUtil::setVbuffer(NULL, pb.PSO, spva, spbvs, spvbs);
						pb.CreateShaderResourceBinding();
					}
				}
			}
			else
			{
				//RenderUtil::setVbuffer(NULL, _pPSO, spva, bvs, vbs);
				for (auto& pb : _oPSOs)
				{
					if (pb.PSO) {
						//RenderUtil::setVbuffer(NULL, pb.PSO, spva.subspan(0, 1), spbvs.subspan(0, 1), spvbs.subspan(0, 1));
						pb.CreateShaderResourceBinding();
					}
				}
			}
			if (vtb)
				vtb->Set(_instbuf->GetDefaultView(Diligent::BUFFER_VIEW_SHADER_RESOURCE));
			auto ptbuf = _pPSO->GetStaticVariableByName(Diligent::SHADER_TYPE_VERTEX, "objectTransfromBuffer");
			if (ptbuf)
				ptbuf->Set(_transbuf->GetDefaultView(Diligent::BUFFER_VIEW_SHADER_RESOURCE));
			Init(m_pDevice, m_pImmediateContext);
			_pPSO->GetStaticVariableByName(SHADER_TYPE_VERTEX, "cbCameraAttribs")->Set(pCameraAttribs);
			_pPSO->GetStaticVariableByName(SHADER_TYPE_VERTEX, "cbpretrs")->Set(_cbPrtrs);
			/*if(!_fxmesh)
			_pPSO->GetStaticVariableByName(SHADER_TYPE_VERTEX, "animreco")->
			Set(_Animbuf->GetDefaultView(Diligent::BUFFER_VIEW_SHADER_RESOURCE));*/
			_txvs.resize(maxtexs);
			for (auto& tv : _txvs)
			{
				tv = m_pDefaultNormalMapSRV;
			}
			SkinRender::init();
			UpdatePreTrans({ 0,0,0 });
		}
	}
}

