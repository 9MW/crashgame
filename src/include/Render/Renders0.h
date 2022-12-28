#pragma once
#include"io/loader.h"
#include"Render/RenderUtil.h"
#include"ShaderStructs.h"
#include"parallel_hashmap/phmap.h"
#include"Gameprops0.h"
#include"Common/ecs.hpp"
#include <Render/RenderCommon.h>
#include"shpso.h"
#include"AssetLoader/GlTFLD.h"
#define IMPMARK assert(false);
namespace pj2::n_Render {
	using namespace Diligent;

	enum modeloffset
	{
		etexture, mat, anim, Ibuf, vb0, vb1, bone, count
	};
	constexpr int PowSTRIDE = 32 - 8;
	//struct SkinRender: Render{
	struct SkinRender :BatchRenderb
	{
		friend struct OlskRender;
		using mtransfrom = math::mtransform;
		template<class T>
		using span = Utility::span<T>;
		struct rb {
			RefCntAutoPtr<IShaderResourceBinding> m_pSRB;
			RefCntAutoPtr<IBuffer>                _instbuf;
			RefCntAutoPtr<IBuffer>                _transbuf;
			RefCntAutoPtr<IBuffer>                _cbufpri;
		};
		const std::array<int, modeloffset::count>& PropsSum()const{return _propsize;}
	protected:
		using Model = ModelImp::model1;
		math::uint  _vbufnum, _mxinstancenum = 0;
		RefCntAutoPtr<IRenderDevice> m_pDevice;
		RefCntAutoPtr<IDeviceContext> m_pImmediateContext;
		dyvbf                m_VertexBuffer[2];
		dyvbf                m_IndexBuffer;
		dyvbf                 _cbPrim,_cboline,_indraw;
		dyvbf                _Animbuf;

		RefCntAutoPtr<IShaderResourceBinding> m_pSRB;
		std::vector<RefCntAutoPtr<IShaderResourceBinding>> m_pSRBs;
		RefCntAutoPtr<IPipelineState> _pPSO;// , _fixPSO;
		std::vector<rb> _rbs;
		//std::vector<drawunit> _dus;
		using tnd = std::unique_ptr<GLTF::Node>;
		static inline int _TexPerModel = -1;
		RenderContext _rc;
		void init();
		RefCntAutoPtr<IShaderResourceVariable> v_textures;
		std::vector<IDeviceObject*> _txvs;
		std::unordered_map<ui64, ui64> modmap;
		std::array<int, modeloffset::count> _propsize;
		void AnimNodeFilter(Model& mod, std::vector<GLTF::Node*>& rut) {
			using namespace GLTF;
			auto& nd = mod.LinearNodes;

			for (size_t i = 0; i < nd.size(); i++)
			{
				Node*& ni = nd[i];
				if (ni->_Mesh)rut.emplace_back(ni);
			}
		}
		void LoadModel(Model& modelprocess, RenderContext& rc);
		int _instancenum = 0;

		std::array<RenderUtil::PSB<1>, 10> _oPSOs;
		using int2 = math::int2;
	public:
		template<modeloffset prop>
		int2 propbegleh(int modid) {
			int2 vr;
			auto& mdin = _meshtable[modid];
			auto& propsize = mdin.propadss;
			vr[0] = propsize[prop];
			_meshtable[modid];
			vr[1]= modid <= (_meshtable.size() - 1 )?
				_propsize[prop]:
				_meshtable[modid+1].propadss[prop]-
				mdin.propadss[prop];
			return vr;
		}
		int WriteVData(void* pda)override;
		int Draw(RenderContext& rt, Diligent::DrawIndexedAttribs& da)override;
		using t_bfins=  sStruct::instData1;
		dyvbf                _transbuf,_instbuf;
		struct Primitive {
			using GP = Diligent::GLTF::Primitive;
			using Uint32 = math::uint;
			Uint32    FirstIndex;
			Uint32    IndexCount;
			Uint32    vertexcount0;
			Uint32    TexStart;
			Primitive(GP& gpri) {
				std::memcpy(this, &gpri, sizeof(*this));
			}
		};
		struct DrawArg {
			Uint32    FirstInsd;
			Uint32    InsdCount;
		};

		ui64 LoadModel(RenderContext& rc, Model& oumodel);
		template<modeloffset mof>
		inline ui64 MPSRC(const auto&  modelid) {
			return std::get<(ui64)mof>(_meshtable[modelid].propadss);
		}

		struct vPrimProo {
			RefCntAutoPtr<Diligent::IShaderResourceBinding> srb;
			RefCntAutoPtr<IBuffer>                _instbuf;
			RefCntAutoPtr<IBuffer>                _transbuf;
			RefCntAutoPtr<IBuffer>                _cbufpri;
		};
		struct modelreco
		{

			math::v4f bbox, offset;
			math::int4 pse;
			//uint bonenumb, StartVertexOffset,
			//	texstart, texsize, texsps, texspl,
			//	NumVertex, StartMaterialOffset, numMt,
			//	anis, anil;//_AniReco start and count
			std::array<int, modeloffset::count> propadss;
			modelreco() {
				std::memset(this, 0, sizeof(*this));
			}
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
		std::vector<modelreco>_meshtable;
		std::vector<rb>_RBs;
		std::vector<AnimRec> _AniReco;
		std::vector<Model*> _models;
		
		//std::vector<n_Render::modelidn> _modeloff;
		math::ui64 modelnum = 0, _vertexs = 0, _texused = 0,_cprim=0;
		int _texnum = 2;// mb.texstart + mb.texsize;
		bool combinedinsbuffer = true;
		inline auto& GLTFModel(const math::ui64& i) {
			return *_models[i];
		}
		inline void updatePrimTab(){}
		
		inline ui64 Ldgomatry(
			 RefCntAutoPtr<IBuffer>& m_VertexBuffer_,
			 RefCntAutoPtr<IBuffer>& m_IndexBuffer_,
			const uint IndexCount,Diligent::ITextureView* tv=NULL) {
			_vbufnum = 1;
			this->m_VertexBuffer[0] = m_VertexBuffer_;
			this->m_IndexBuffer = m_IndexBuffer_;
			Diligent::IDeviceObject* tg[] = {(Diligent::IDeviceObject*)m_pWhiteTexSRV};
			if(v_textures)
			v_textures->SetArray(tg, 1, _txvs.size());
			return 0;
		}
		SkinRender(){}

		//2^12 primitive max 2^12=4096
		int _PowSTRIDE=32-12, _totalprimitive, _maxtexs;
		bool _fxmesh;
		void init(
			RenderContext& rc,
			const int mxinstancenum = 5000,
			const int _PowSTRIDE = 17,
			const int vbufnum = 2,
			const int maxtexs = 64,
			const int maxanimbuf = 1024 * 1024 * 2
		);
		//using namespace Diligent;
		struct renderbatch {
			RefCntAutoPtr<IBuffer> trs, State, joints;
		};
		std::array< StateTransitionDesc, 2> _dbfstd;
		std::vector<int> _insFilter;
		void WriteVbufferAtZero(RenderContext& rct,std::vector<math::mtransform>& trs,std::vector<sStruct::instData1>& insds) {
			_insFilter.push_back(trs.size());
			_instancenum += trs.size(); 
			assert(trs.size() == insds.size()&& _instancenum < _mxinstancenum );
			{
				using ts = t_bfins;
				auto& vdata = insds;
				auto& vbuf = _instbuf;
				Diligent::MapHelper<ts> ind(rct.m_pImmediateContext
					, vbuf, MAP_WRITE, MAP_FLAG_DO_NOT_WAIT);
				std::memcpy(ind, vdata.data(), vdata.size() * sizeof(ts));
			}
			{
				using ts = mtransfrom;
				auto& vdata = trs;
				auto& vbuf = _transbuf;
				Diligent::MapHelper<ts> ind(rct.m_pImmediateContext
					, vbuf, MAP_WRITE, MAP_FLAG_DO_NOT_WAIT);
				std::memcpy(ind, vdata.data(), vdata.size() * sizeof(ts));
			}
		}
		const auto& GetBBox(const int& modid) {
			auto& md = GLTFModel(modid);
			return md.dimensions.max-md.dimensions.min;
		}
		struct IndirArg
		{
			Uint32 NumIndices;
			Uint32 NumInstances;
			Uint32 FirstIndexLocation;
			Uint32 BaseVertex;
			Uint32 FirstInstanceLocation;
			int64_t peed;
			/*IndirArg(DrawIndexedAttribs& kl) {

			}*/
		};
		struct srDrawArg
		{
			int NumInstances=0, FirstInstanceLocation=0,modiid,p;
		};
		void Render(Utility::span<srDrawArg> inss, const int& NumIndices_);
		inline void CalFirstInstanceLocation(uint& iout,const uint& primid,const uint& baseinst) {
			iout = (primid << _PowSTRIDE) | baseinst;
		}
		void Render(Utility::span<srDrawArg>  inss);
		void RenderOl(Utility::span<srDrawArg>  inss);
		void Render(Utility::span<int> inss) {
			assert(inss.size() == _meshtable.size());
			auto& pCtx = m_pImmediateContext;

			IBuffer* pVBs[] = { m_VertexBuffer[0],m_VertexBuffer[1] };
			math::ui64   Offsets[_countof(pVBs)] = {};
			//_insFilter.clear();
			pCtx->SetPipelineState(_pPSO);
			pCtx->TransitionResourceStates(_dbfstd.size(), _dbfstd.data());
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

			for (size_t i = 0; i < _meshtable.size(); i++)
			{
				const auto& NumInstances = inss[i];
				if (NumInstances == 0) [[unlikely]]
					continue;
				auto& ele = attribs;
				ele.NumInstances = NumInstances;
				auto& prmc = _meshtable[i];
				for (size_t i2 = 0; i2 < prmc[1]; i2++, k++) {
					ele.FirstInstanceLocation =   ((_PowSTRIDE << k)) | insc;//inc up to 1<<32-_PowSTRIDE
					auto& prim = _prims[i2 + prmc[0]];
					ele.NumIndices = prim.IndexCount;
					ele.FirstIndexLocation = prim.FirstIndex;
					pCtx->DrawIndexed(ele);
				}
				insc += NumInstances;
			}
			_instancenum = 0;
		}
		using rbs = std::array<UINT32, 4>;
		std::vector<rbs> _renderlist;
		std::vector<IndirArg>dratt;
		//model id get from loadmodel,instance count,FirstInstanceLocation,
		void push(const rbs& pa);
		void Render();
		void dRender(Utility::span<int> inss) {
			assert(inss.size() == _meshtable.size());
			auto& pCtx = m_pImmediateContext;
			IBuffer* pVBs[] = { m_VertexBuffer[0],m_VertexBuffer[1] };
			math::ui64   Offsets[_countof(pVBs)] = {};
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
			DrawIndexedAttribs attribs(0, VT_UINT32, DRAW_FLAG_VERIFY_ALL);
			for (size_t i = 0; i < inss.size(); i++)
			{
				pCtx->CommitShaderResources(m_pSRBs[i], RESOURCE_STATE_TRANSITION_MODE_VERIFY);
			}
			/*auto& rb = _RBs[0];
			pCtx->CommitShaderResources(rb.m_pSRB, RESOURCE_STATE_TRANSITION_MODE_VERIFY);*/
			attribs.BaseVertex = 0;
			attribs.Flags |= DRAW_FLAG_DYNAMIC_RESOURCE_BUFFERS_INTACT;
			// clang-format on
			pCtx->SetVertexBuffers(0, _vbufnum, pVBs, nullptr, RESOURCE_STATE_TRANSITION_MODE_VERIFY, SET_VERTEX_BUFFERS_FLAG_NONE);
			pCtx->SetIndexBuffer(m_IndexBuffer, 0, RESOURCE_STATE_TRANSITION_MODE_VERIFY);
			//const uint inststri=
			int k = 0;
			for (size_t i = 0; i < _meshtable.size(); i++)
			{
				const auto& NumInstances = inss[i];
				if (NumInstances == 0) [[unlikely]]
					continue;
				auto& ele = attribs;
				ele.NumInstances = NumInstances;
				auto& prmc = _meshtable[i];
				//encode primitive space ,max primitive limte up to 1000
				for (size_t i2 = 0; i2 < prmc[1]; i2++, k++) {
					ele.FirstInstanceLocation = k * (1 << _PowSTRIDE);
					auto& prim = _prims[i2 + prmc[0]];
					ele.NumIndices = prim.IndexCount;
					ele.FirstIndexLocation = prim.FirstIndex;
					pCtx->DrawIndexed(ele);
				}
				//_insFilter.emplace_back(i);
			}
		}
		//void Render(RenderAttr& Ra) {
		//	/*if (_instancenum == 0)
		//		return;*/
		//	auto& pCtx = m_pImmediateContext;
		//	auto& m_VertexBuffer = Ra.m_VertexBuffer;
		//	auto& m_IndexBuffer = Ra.m_IndexBuffer;
		//	auto& eles = Ra.prims;
		//	IBuffer* pVBs[] = { m_VertexBuffer[0],m_VertexBuffer[1] };
		//	math::ui64   Offsets[_countof(pVBs)] = {};

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
		//	math::ui64   Offsets[_countof(pVBs)] = {};
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
	struct FixRender :SkinRender
	{
		dyvbf                _cbPrtrs;
		void UpdatePreTrans(const mtransfrom& trs) {
			_cbPrtrs.updateat(trs, (this->_rc), 0);
			StateTransitionDesc d={_cbPrtrs, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_CONSTANT_BUFFER, STATE_TRANSITION_FLAG_UPDATE_STATE };
			_rc.m_pImmediateContext->TransitionResourceStates(1, &d);
		}
		void init(RenderContext& rc, const int mxinstancenum, const int maxtexs = 2);

	};
	struct OlskRender:BatchRenderb {
		friend struct SkinRender;
		math::uint  _vbufnum, _mxinstancenum = 0;
		RefCntAutoPtr<IRenderDevice> m_pDevice;
		RefCntAutoPtr<IDeviceContext> m_pImmediateContext;


		RefCntAutoPtr<IShaderResourceBinding> m_pSRB;
		std::vector<RefCntAutoPtr<IShaderResourceBinding>> m_pSRBs;
		RefCntAutoPtr<IPipelineState> _pPSO;// , _fixPSO;
		using t_bfins = sStruct::instData1;
		dyvbf                _transbuf, _instbuf;
		using Primitive = SkinRender::Primitive;
		using srDrawArg = SkinRender::srDrawArg;
		using DrawArg = SkinRender::DrawArg;
		using vPrimProo = SkinRender::vPrimProo;
		using vPrimProo = SkinRender::vPrimProo;
		struct modelreco
		{
			math::int2 pse;
			//uint bonenumb, StartVertexOffset,
			//	texstart, texsize, texsps, texspl,
			//	NumVertex, StartMaterialOffset, numMt,
			//	anis, anil;//_AniReco start and count
			std::array<int, modeloffset::count> propadss;
			modelreco() {
				std::memset(this, 0, sizeof(*this));
			}
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
		//std::vector<n_Render::modelidn> _modeloff;
		math::ui64 modelnum = 0, _vertexs = 0, _texused = 0;
		int _texnum = 2;// mb.texstart + mb.texsize;
		SkinRender* p_sr=NULL;
		void init(SkinRender& fr);
		void Render(Utility::span<srDrawArg> inss) {}

	};
	struct renderbatch {
		list<n_Render::IndirectArgs> drawarg;
		list<RefCntAutoPtr<IBuffer>> instancebuf;
		DrawIndexedAttribs DRG;
		RefCntAutoPtr<IPipelineState> _pPSO;
		RefCntAutoPtr<IShaderResourceBinding> m_pSRB;
		RefCntAutoPtr<IBuffer>                m_VertexBuffer[2];
		RefCntAutoPtr<IBuffer>                m_IndexBuffer;
		RefCntAutoPtr<IBuffer>                M_pIndirectArgsBuff;
		int vertexsolts = 1;
		inline int WriteVbf(const int bfid, const ECS::Conany_thsafe& indata, RenderContext& rct) {
			auto& pCtx = rct.m_pImmediateContext;
			void* m_pMappedData;
			auto& tgbf = instancebuf[bfid];
			pCtx->MapBuffer(tgbf, MAP_WRITE, MAP_FLAG_DISCARD, m_pMappedData);
			indata.copyto(m_pMappedData);
			pCtx->UnmapBuffer(tgbf, MAP_WRITE);
			return 1;
		}
		inline int WriteVbf(const int bfid, const ui64 size, const void*& indata, RenderContext& rct) {
			auto& pCtx = rct.m_pImmediateContext;
			void* m_pMappedData;
			auto& tgbf = instancebuf[bfid];
			pCtx->MapBuffer(tgbf, MAP_WRITE, MAP_FLAG_DISCARD, m_pMappedData);
			std::memcpy(m_pMappedData, indata, size);
			pCtx->UnmapBuffer(tgbf, MAP_WRITE);
			return 1;
		}
	};
	struct rendermanager
	{
		list<renderbatch> batchs;
		list<DrawIndexedAttribs> drg;
		void Render(RenderContext& rct_, int id) {
			//auto& pCtx = rct_.m_pImmediateContext;
			//renderbatch& rb = batchs[id];
			//IBuffer* pVBs[] = { rb.m_VertexBuffer[0],rb.m_VertexBuffer[1] };
			//math::ui64   Offsets[_countof(pVBs)] = {};
			//pCtx->SetVertexBuffers(0,rb.vertexsolts, pVBs, Offsets, RESOURCE_STATE_TRANSITION_MODE_VERIFY, SET_VERTEX_BUFFERS_FLAG_NONE);
			//pCtx->SetIndexBuffer(rb.m_IndexBuffer, 0, RESOURCE_STATE_TRANSITION_MODE_VERIFY);
			//rct_.m_pImmediateContext->SetPipelineState(batchs[id]._pPSO);
			//pCtx->CommitShaderResources(rb.m_pSRB, RESOURCE_STATE_TRANSITION_MODE_VERIFY); DrawIndexedIndirectAttribs drawAttrs{ VT_UINT32, DRAW_FLAG_VERIFY_ALL, RESOURCE_STATE_TRANSITION_MODE_TRANSITION };
			//
			////auto& pIndirectArgsBuff = M_pIndirectArgsBuff;// CreateIndirectDrawArgsBuffer(nonstd::span((uint*)DrawArgs.data(), DrawArgs.size() * sizeof(IndirectArgs) / sizeof(uint)));
			//for (size_t i = 0; i < batchs.size(); i++)
			//{
			//	//drawAttrs.DrawArgsOffset = i * sizeof(IndirectArgs);
			//	pCtx->DrawIndexed(drg[i]);
			//}
		}
	};
}