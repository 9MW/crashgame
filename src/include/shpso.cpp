#include "shpso.h"
#include"GProps.h"
#include"imgui.h"
#include"GProps.h"
namespace pj2::shpso {
	namespace asstFun {
		enum ImDrawFlags_
		{
			ImDrawFlags_None = 0,
			ImDrawFlags_Closed = 1 << 0, // PathStroke(), AddPolyline(): specify that shape should be closed (Important: this is always == 1 for legacy reason)
			ImDrawFlags_RoundCornersTopLeft = 1 << 4, // AddRect(), AddRectFilled(), PathRect(): enable rounding top-left corner only (when rounding > 0.0f, we default to all corners). Was 0x01.
			ImDrawFlags_RoundCornersTopRight = 1 << 5, // AddRect(), AddRectFilled(), PathRect(): enable rounding top-right corner only (when rounding > 0.0f, we default to all corners). Was 0x02.
			ImDrawFlags_RoundCornersBottomLeft = 1 << 6, // AddRect(), AddRectFilled(), PathRect(): enable rounding bottom-left corner only (when rounding > 0.0f, we default to all corners). Was 0x04.
			ImDrawFlags_RoundCornersBottomRight = 1 << 7, // AddRect(), AddRectFilled(), PathRect(): enable rounding bottom-right corner only (when rounding > 0.0f, we default to all corners). Wax 0x08.
			ImDrawFlags_RoundCornersNone = 1 << 8, // AddRect(), AddRectFilled(), PathRect(): disable rounding on all corners (when rounding > 0.0f). This is NOT zero, NOT an implicit flag!
			ImDrawFlags_RoundCornersTop = ImDrawFlags_RoundCornersTopLeft | ImDrawFlags_RoundCornersTopRight,
			ImDrawFlags_RoundCornersBottom = ImDrawFlags_RoundCornersBottomLeft | ImDrawFlags_RoundCornersBottomRight,
			ImDrawFlags_RoundCornersLeft = ImDrawFlags_RoundCornersBottomLeft | ImDrawFlags_RoundCornersTopLeft,
			ImDrawFlags_RoundCornersRight = ImDrawFlags_RoundCornersBottomRight | ImDrawFlags_RoundCornersTopRight,
			ImDrawFlags_RoundCornersAll = ImDrawFlags_RoundCornersTopLeft | ImDrawFlags_RoundCornersTopRight | ImDrawFlags_RoundCornersBottomLeft | ImDrawFlags_RoundCornersBottomRight,
			ImDrawFlags_RoundCornersDefault_ = ImDrawFlags_RoundCornersAll, // Default to ALL corners if none of the _RoundCornersXX flags are specified.
			ImDrawFlags_RoundCornersMask_ = ImDrawFlags_RoundCornersAll | ImDrawFlags_RoundCornersNone
		};
		using namespace Diligent;
		ShaderResourceVariableDesc Vars[] =
		{
			//{SHADER_TYPE_COMPUTE, "queueFB", SHADER_RESOURCE_VARIABLE_TYPE_STATIC},
			{ SHADER_TYPE_COMPUTE, "_tinfo", SHADER_RESOURCE_VARIABLE_TYPE_STATIC },
			{ SHADER_TYPE_COMPUTE, "cbCameraAttribs", SHADER_RESOURCE_VARIABLE_TYPE_STATIC },
			{ SHADER_TYPE_COMPUTE, "instDatas", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE },
			{ SHADER_TYPE_COMPUTE, "sele_out", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE },
			{ SHADER_TYPE_COMPUTE, "cbSelect", SHADER_RESOURCE_VARIABLE_TYPE_STATIC },
			{ SHADER_TYPE_COMPUTE, "sele_filter", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE },
			{ SHADER_TYPE_COMPUTE, "in_sele_out", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE },
		{ SHADER_TYPE_COMPUTE, "instable", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE }
		};
		inline void SetCompbuf(const ShaderResourceVariableDesc& srd,
			IBuffer* ppBuffer, BUFFER_VIEW_TYPE bv,
			RefCntAutoPtr<IShaderResourceBinding>& _SRB) {
			auto pC = _SRB->GetVariableByName(Diligent::SHADER_TYPE_COMPUTE, srd.Name);
			pC->Set((ppBuffer)->GetDefaultView(bv));
		}
		inline void Setstaticbuf(const ShaderResourceVariableDesc& srd,
			IBuffer* ppBuffer, BUFFER_VIEW_TYPE bv,
			RefCntAutoPtr<IPipelineState>& _SRB) {
			auto pC = _SRB->GetStaticVariableByName(Diligent::SHADER_TYPE_COMPUTE, srd.Name);
			pC->Set((ppBuffer)->GetDefaultView(bv));
		}
		shader selects(RefCntAutoPtr<IRenderDevice>& m_pDevice,
			RefCntAutoPtr<IDeviceContext>& m_pImmediateContext,
			RefCntAutoPtr<IShaderSourceInputStreamFactory>& pShaderSourceFactory,
			IBuffer* pCameraAttribs, IBuffer* Vf_Props, IBuffer* sele_out,
			IBuffer* tf,
			IBuffer* csele,
			IBuffer* sele_filter)
		{

			int d = ImDrawFlags_RoundCornersAll;
			shader sdout;
			RefCntAutoPtr<IShaderResourceBinding>&
				SRBrec_ = sdout.SRBrec_, & srbpr_ = sdout.srbpr_,
				& srbray_ = sdout.srbray_;
			pj2::DrawFunc::CreateComputePSO("rectselect", "Util0.hlsl", "rectselect", m_pDevice, pShaderSourceFactory, sdout._pos, Vars);
			pj2::DrawFunc::CreateComputePSO("prepareselect", "Util0.hlsl", "prepareselect", m_pDevice, pShaderSourceFactory, sdout._perparpos, Vars, 1);
			pj2::DrawFunc::CreateComputePSO("rayselect", "Util0.hlsl", "rayselect", m_pDevice, pShaderSourceFactory, sdout._rayselectpeo, Vars);
			sdout._pos->GetStaticVariableByName(SHADER_TYPE_COMPUTE, "cbCameraAttribs")->Set(pCameraAttribs);
			sdout._pos->GetStaticVariableByName(SHADER_TYPE_COMPUTE, "cbSelect")->Set(csele);
			sdout._rayselectpeo->GetStaticVariableByName(SHADER_TYPE_COMPUTE, "cbSelect")->Set(csele);
			asstFun::Setstaticbuf(Vars[0], tf, BUFFER_VIEW_SHADER_RESOURCE, sdout._pos);
			asstFun::Setstaticbuf(Vars[0], tf, BUFFER_VIEW_SHADER_RESOURCE, sdout._rayselectpeo);
			/*sdout._pos->GetStaticVariableByName(SHADER_TYPE_COMPUTE, "_tinfo")->Set(tf->GetDefaultView(BUFFER_VIEW_SHADER_RESOURCE));
			sdout._rayselectpeo->GetStaticVariableByName(SHADER_TYPE_COMPUTE, "_tinfo")->Set(tf);*/
			sdout._pos->CreateShaderResourceBinding(&SRBrec_, true);
			sdout._perparpos->CreateShaderResourceBinding(&srbpr_, true);
			sdout._rayselectpeo->CreateShaderResourceBinding(&srbray_, true);
			BufferData VBData;
			BufferDesc BuffDesc;
			{
				BuffDesc.Mode = BUFFER_MODE_STRUCTURED;
				auto&& SRB_ = SRBrec_;
				//auto pC = SRB_->GetVariableByName(ST, BuffDesc.Name);
				asstFun::SetCompbuf(Vars[2], Vf_Props, BUFFER_VIEW_SHADER_RESOURCE, SRB_);
				asstFun::SetCompbuf(Vars[3], sele_out, BUFFER_VIEW_UNORDERED_ACCESS, SRB_);
				asstFun::SetCompbuf(Vars[5], sele_filter, BUFFER_VIEW_UNORDERED_ACCESS, SRB_);
			}
			{
				BuffDesc.Mode = BUFFER_MODE_STRUCTURED;
				auto&& SRB_ = srbpr_;
				//auto pC = SRB_->GetVariableByName(ST, BuffDesc.Name);
				//asstFun::SetCompbuf(Vars[1], Vf_Props, BUFFER_VIEW_SHADER_RESOURCE, SRB_);
				asstFun::SetCompbuf(Vars[3], sele_out, BUFFER_VIEW_UNORDERED_ACCESS, SRB_);
			}
			{
				BuffDesc.Mode = BUFFER_MODE_STRUCTURED;

				auto&& SRB_ = srbray_;
				//auto pC = SRB_->GetVariableByName(ST, BuffDesc.Name);
				asstFun::SetCompbuf(Vars[2], Vf_Props, BUFFER_VIEW_SHADER_RESOURCE, SRB_);
				auto pso = SRB_->GetVariableByName(SHADER_TYPE_COMPUTE, "sele_out");
				asstFun::SetCompbuf(Vars[3], sele_out, BUFFER_VIEW_UNORDERED_ACCESS, SRB_);
				asstFun::SetCompbuf(Vars[5], sele_filter, BUFFER_VIEW_UNORDERED_ACCESS, SRB_);
			}
			return sdout;
		}
	}

	RefCntAutoPtr<IBuffer>_Selecbuf2;
	//sdocp  : capacity of max selectable unit
	void SelectCS::init(RenderContext& rc, int socap_) {
		constexpr int socap = MaxSelect;
		const auto RenderTargetFormat = rc.RenderTargetFormat;
		const auto DepthBufferFormat = rc.DepthBufferFormat;
		_rt = rc;
		RefCntAutoPtr<IRenderDevice>& m_pDevice = rc.m_pDevice;
		FenceDesc fenceDesc;
		fenceDesc.Name = "select buf cpy fence";
		m_pDevice->CreateFence(fenceDesc, &_cpyfen);
		Diligent::BufferDesc BuffDesc;
		{
			using ts = sStruct::seleout;
			auto&& buf = sele_out;
			BuffDesc.Mode = BUFFER_MODE_STRUCTURED;
			/*VBData.DataSize = fdf.size() * 16;
			VBData.pData = fdf.data();*/
			RenderUtil::md_BufferDesc("sele_out", BuffDesc, socap, sizeof(ts), USAGE_DEFAULT, BIND_UNORDERED_ACCESS | BIND_SHADER_RESOURCE, CPU_ACCESS_NONE);
			rc.m_pDevice->CreateBuffer(BuffDesc, nullptr, &buf);
		}

		{
			using ts = sStruct::instData1;
			auto&& buf = Selefbuf;
			BuffDesc.Mode = BUFFER_MODE_STRUCTURED;
			RenderUtil::md_BufferDesc("Selefbuf", BuffDesc, socap, sizeof(ts), USAGE_DEFAULT, BIND_UNORDERED_ACCESS | BIND_SHADER_RESOURCE, CPU_ACCESS_NONE);
			rc.m_pDevice->CreateBuffer(BuffDesc, nullptr, &buf);
		}
		{
			using ts = sStruct::seleout;
			auto&& buf = stagsele;
			BuffDesc.Mode = BUFFER_MODE_STRUCTURED;
			RenderUtil::md_BufferDesc("cseleout", BuffDesc, socap * (MaxCpyStep), sizeof(ts), USAGE_STAGING, BIND_NONE, CPU_ACCESS_READ);
			rc.m_pDevice->CreateBuffer(BuffDesc, nullptr, &buf);
		}
		initRender(rc);
		ShaderMacroHelper mk;
		if (lastfence != -1) {
			lastfence = -1;
			mk.AddShaderMacro("SCMB", 1);
		}
		_sds = asstFun::selects(rc.m_pDevice, rc.m_pImmediateContext, rc.pShaderSourceFactory,
			rc.pCameraAttribs, Vf_Props, sele_out, tinfo, _Selecbuf, Selefbuf);

		{
			auto& _selecirps = _Circlepsb.PSO;
			_selecirps = nullptr;

			const auto SetCompbuf = [](const ShaderResourceVariableDesc& srd,
				IBuffer* ppBuffer, BUFFER_VIEW_TYPE bv,
				RefCntAutoPtr<IShaderResourceBinding>& _SRB) {
					auto pC = _SRB->GetVariableByName(Diligent::SHADER_TYPE_COMPUTE, srd.Name);
					pC->Set((ppBuffer)->GetDefaultView(bv));
			};
			std::vector<ShaderResourceVariableDesc> Vars =
			{
				//{SHADER_TYPE_COMPUTE, "queueFB", SHADER_RESOURCE_VARIABLE_TYPE_STATIC},
				//{ SHADER_TYPE_COMPUTE, "_tinfo", SHADER_RESOURCE_VARIABLE_TYPE_STATIC },
				{ SHADER_TYPE_VERTEX, "sele_filter", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE },
				//{ SHADER_TYPE_COMPUTE, "sele_out", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE }
			};
			RefCntAutoPtr<IBuffer> bfs[] = { Selefbuf,sele_out };
			RenderUtil::CreatePipelineStates
			(&_selecirps, m_pDevice, RenderTargetFormat, DepthBufferFormat,
				rc.pShaderSourceFactory, "sMesh.hlsl", "vsele", "fsele", Vars, {}, {},
				Diligent::PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP
				, false, false, {}, {}, Diligent::CULL_MODE::CULL_MODE_NONE);
			_selecirps->GetStaticVariableByName(SHADER_TYPE_VERTEX, "cbCameraAttribs")->Set(rc.pCameraAttribs);
			//_PsScreenrect->GetStaticVariableByName(SHADER_TYPE_VERTEX,"cbCameraAttribs")->Set(rc.pCameraAttribs);
			_Circlepsb.CreateShaderResourceBinding();
			//_selecirps->CreateShaderResourceBinding(&_SeleCirRen, true);
			for (size_t i = 0; i < Vars.size(); i++)
			{
				auto& vi = Vars[i];
				_Circlepsb.GetShaderResourceBinding()->GetVariableByName(vi.ShaderStages, vi.Name)->Set(bfs[i]->GetDefaultView(BUFFER_VIEW_SHADER_RESOURCE));
				//_SeleCirRen->GetVariableByName(vi.ShaderStages,vi.Name)->Set(bfs[i]->GetDefaultView(BUFFER_VIEW_SHADER_RESOURCE));
			}
		}

	}
	void SelectCS::initRender(RenderContext& rc) {
		{
			_rt = rc;
			RefCntAutoPtr<IRenderDevice>& m_pDevice = rc.m_pDevice;
			const auto RenderTargetFormat = rc.RenderTargetFormat;
			const auto DepthBufferFormat = rc.DepthBufferFormat;
			std::vector<ShaderResourceVariableDesc> Vars;
			RenderUtil::CreatePipelineStates
			(&_PsScreenrect, m_pDevice, RenderTargetFormat, DepthBufferFormat,
				rc.pShaderSourceFactory
				, "common.hlsl", "vsele", "fsele", {}, {}, {}, PRIMITIVE_TOPOLOGY::PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP,
				{}, {}, {}, {}, Diligent::CULL_MODE::CULL_MODE_NONE);
		}
		Diligent::BufferDesc BuffDesc;
		{
			using ts = sStruct::Selects;
			auto&& buf = _Selecbuf;
			BuffDesc.Mode = BUFFER_MODE_STRUCTURED;
			RenderUtil::md_BufferDesc("cbSelect", BuffDesc, 1, sizeof(ts), USAGE_DYNAMIC, BIND_UNIFORM_BUFFER, CPU_ACCESS_WRITE);
			rc.m_pDevice->CreateBuffer(BuffDesc, nullptr, &buf);
		}
		_PsScreenrect->GetStaticVariableByName(SHADER_TYPE_VERTEX, "cScreenRect")->Set(_Selecbuf);
		_PsScreenrect->CreateShaderResourceBinding(&_SrbScreenrect, true);
	}
	using tsout = sStruct::seleout;
	constexpr auto sele0Array() {
		std::array<tsout, 200> result{};
		for (size_t i = 0; i < result.size(); ++i) {
			result[i].itid = 0;
		}
		return result;
	};
	void SelectCS::prepare() {
		constexpr auto su = sele0Array();
		constexpr int szsu = sizeof(su);
		_rt.m_pImmediateContext->UpdateBuffer(sele_out, 0, szsu, su.data(), RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
	}
	void SelectCS::rectSele(const ui64 objinsepct, const sStruct::Selects& csele, const ui64 sz) {
		DispatchComputeAttribs DispatAttribs(objinsepct, 1, 1);
		cmf(csele);
		_rt.m_pImmediateContext->SetPipelineState(_sds._pos);
		_rt.m_pImmediateContext->CommitShaderResources(_sds.SRBrec_, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

		_rt.m_pImmediateContext->DispatchCompute(DispatAttribs);
		_rt.m_pImmediateContext->CopyBuffer(sele_out, 0,
			Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION,
			stagsele,
			vcpoyoff() * sizeof(t_sinout) * MaxSelect, std::min((sz + 1) * sizeof(t_sinout), MaxSelect * sizeof(t_sinout)), Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
		_rt.m_pImmediateContext->EnqueueSignal(_cpyfen, m_CurrentFenceValue);
		//_rt.m_pImmediateContext->WaitForFence(_cpyfen, m_CurrentFenceValue, false);
		m_CurrentFenceValue++;
	}
	void SelectCS::Raysele(const ui64& objinsepct, const sStruct::Selects& csele, const ui64 sz) {
		cmf(csele);
		DispatchComputeAttribs DispatAttribs(objinsepct, 1, 1);
		_rt.m_pImmediateContext->SetPipelineState(_sds._perparpos);
		_rt.m_pImmediateContext->CommitShaderResources(_sds.srbray_, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
		_rt.m_pImmediateContext->DispatchCompute(DispatAttribs);
		_rt.m_pImmediateContext->CopyBuffer(sele_out, 0,
			Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION,
			stagsele,
			0, sz * sizeof(t_sinout) + 1, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
		_rt.m_pImmediateContext->EnqueueSignal(_cpyfen, m_CurrentFenceValue);
		m_CurrentFenceValue++;
	}
	void SelectCS::RenderScreenRect(const sStruct::Selects& rect) {
		{
			Diligent::MapHelper<sStruct::Selects> mpd(_rt.m_pImmediateContext, _Selecbuf, MAP_WRITE, MAP_FLAG_DISCARD);
			std::memcpy(mpd, &rect, sizeof(sStruct::Selects));
		}
		_rt.m_pImmediateContext->SetPipelineState(_PsScreenrect);
		_rt.m_pImmediateContext->CommitShaderResources(_SrbScreenrect, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
		DrawAttribs DrawAttrs;
		DrawAttrs.Flags = DRAW_FLAG_VERIFY_ALL;
		DrawAttrs.NumVertices = 4;
		DrawAttrs.NumInstances = 1;
		_rt.m_pImmediateContext->Draw(DrawAttrs);
	}
	void SelectCS::RenderSelect(const ui64 sz) {
		_rt.m_pImmediateContext->SetPipelineState(_Circlepsb.PSO);
		_rt.m_pImmediateContext->CommitShaderResources(_Circlepsb.GetShaderResourceBinding(), RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
		DrawAttribs DrawAttrs;
		DrawAttrs.Flags = DRAW_FLAG_VERIFY_ALL;
		DrawAttrs.NumVertices = 4;
		DrawAttrs.NumInstances = sz;
		_rt.m_pImmediateContext->Draw(DrawAttrs);
	}
	void SelectCS::SetFrom(Visible_f& vf) {
		using namespace Diligent;
		auto& SRBrec_ = _sds.SRBrec_, & srbpr_ = _sds.srbpr_,
			& srbray_ = _sds.srbray_;
		auto& _instable = vf._instable;
		auto& seleout = vf.cullout;
		auto ST = SHADER_TYPE_COMPUTE;
		{
			auto&& SRB_ = SRBrec_;
			const auto i0 = 6;
			asstFun::SetCompbuf(asstFun::Vars[i0 + 1], _instable, BUFFER_VIEW_SHADER_RESOURCE, SRB_);
			asstFun::SetCompbuf(asstFun::Vars[i0], seleout, BUFFER_VIEW_UNORDERED_ACCESS, SRB_);
		}
	}
	bool SelectCS::ResultReady() {
		lastfence = _cpyfen->GetCompletedValue();
		return lastfence >= m_CurrentFenceValue - 1;
	}
	sStruct::Selects parasele;
	void Visible_f::init(RenderContext& rc, int mode, int socap, const int maxtinfo) {
		parasele.inum = 0;
		bool ppaudio = true, custsele = 1;;
		auto& m_pDevice = rc.m_pDevice;
		auto& m_pImmediateContext = rc.m_pImmediateContext;
		p_rc = &rc;
		auto& pSwapChain = rc.m_pSwapChain;
		/*this->m_pSwapChain = pSwapChain;
		this->m_pImmediateContext = m_pImmediateContext;*/
		auto& pShaderSourceFactory = rc.pShaderSourceFactory;
		std::vector<StateTransitionDesc> decc_;
		decc_.reserve(32);
		for (auto& ptr : _pMap)
		{
			ptr = nullptr;
		}
		FenceDesc fenceDesc;
		fenceDesc.Name = "cull cpy fence";
		fenceDesc.Type = FENCE_TYPE_GENERAL;
		m_pDevice->CreateFence(fenceDesc, &_cpyfen);
		int maxcullresult = socap / 2;
		_trendertable.resize(maxtinfo);
		_cbfread.resize(maxtinfo);
		BufferDesc BuffDesc;
		ShaderMacroHelper sm;
		if (ppaudio) {
			sm.AddShaderMacro("FiltAudio", 1);
		}
		if (custsele) {
			sm.AddShaderMacro("TSELECT", 1);
		}
		if(!_culltinfo)
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
			using ts = t_sinout;
			auto&& buf = stagsele;
			BuffDesc.Mode = BUFFER_MODE_STRUCTURED;
			RenderUtil::md_BufferDesc("cseleout", BuffDesc, maxcullresult, sizeof(ts), USAGE_STAGING, BIND_NONE, CPU_ACCESS_READ);
			buf.reserve(rc, BuffDesc);
			decc_.emplace_back(buf, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_COPY_DEST, STATE_TRANSITION_FLAG_UPDATE_STATE);
		}
		{
			using ts = t_sinout;
			auto&& buf = seleout;
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
		if(!_SeleVout)
		{
			using ts = sStruct::seleFilter;
			auto&& buf = _SeleVout;
			std::vector<ts> initdata(maxcullresult);
			const auto& dsize = Utility::VecByteSize(initdata);
			std::memset(initdata.data(), 0, dsize);
			RenderUtil::BuildBDesc<ts>(BuffDesc, maxcullresult, "sele_ov",
				Diligent::BUFFER_MODE_STRUCTURED,
				Diligent::USAGE_DEFAULT, BIND_UNORDERED_ACCESS | BIND_SHADER_RESOURCE, Diligent::CPU_ACCESS_NONE);
			buf.reserve(rc, BuffDesc);
			m_pImmediateContext->UpdateBuffer(buf, 0, dsize, initdata.data(), RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
			decc_.emplace_back(buf, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_UNORDERED_ACCESS, STATE_TRANSITION_FLAG_UPDATE_STATE);
		}
		{
			using ts = t_sinout;
			auto&& buf = stagcull;
			BuffDesc.Mode = BUFFER_MODE_STRUCTURED;
			RenderUtil::md_BufferDesc("cseleout", BuffDesc, maxcullresult, sizeof(ts), USAGE_STAGING, BIND_NONE, CPU_ACCESS_READ);
			buf.reserve(rc, BuffDesc);
			decc_.emplace_back(buf, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_COPY_DEST, STATE_TRANSITION_FLAG_UPDATE_STATE);
		}
		{
			using ts = t_sinout;
			auto&& buf = cullout;
			std::vector<ts> initdata(maxcullresult);
			const auto& dsize = Utility::VecByteSize(initdata);
			std::memset(initdata.data(), 0, dsize);
			RenderUtil::BuildBDesc<ts>(BuffDesc, maxcullresult, "seleout",
				Diligent::BUFFER_MODE_STRUCTURED,
				Diligent::USAGE_DEFAULT, BIND_UNORDERED_ACCESS | BIND_SHADER_RESOURCE, Diligent::CPU_ACCESS_NONE);
			buf.reserve(rc, BuffDesc);
			m_pImmediateContext->UpdateBuffer(buf, 0, dsize, initdata.data(), RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
			decc_.emplace_back(buf, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_UNORDERED_ACCESS, STATE_TRANSITION_FLAG_UPDATE_STATE);
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
		if (!_vbIns) {
			{
				using ts = sStruct::instData1;
				auto&& buf = _vbIns;
				RenderUtil::BuildBDesc<ts>(BuffDesc, socap, "instan1Buffer",
					Diligent::BUFFER_MODE_STRUCTURED,
					Diligent::USAGE_DYNAMIC, Diligent::BIND_SHADER_RESOURCE, Diligent::CPU_ACCESS_WRITE);
				buf.reserve(rc, BuffDesc);
				decc_.emplace_back(buf, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_GENERIC_READ, STATE_TRANSITION_FLAG_UPDATE_STATE);
			}
			{
				using ts = math::mtransform;
				auto&& buf = _vbtrans;
				RenderUtil::BuildBDesc<ts>(BuffDesc, socap, "objectTransfromBuffer",
					Diligent::BUFFER_MODE_STRUCTURED,
					Diligent::USAGE_DYNAMIC, Diligent::BIND_SHADER_RESOURCE, Diligent::CPU_ACCESS_WRITE);
				buf.reserve(rc, BuffDesc);
				decc_.emplace_back(_vbtrans, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_GENERIC_READ, STATE_TRANSITION_FLAG_UPDATE_STATE);
			}
			{
				using ts = sStruct::inscmb;
				auto&& buf = Visibleout;
				RenderUtil::BuildBDesc<ts>(BuffDesc, socap, "Visibleout",
					Diligent::BUFFER_MODE_STRUCTURED,
					Diligent::USAGE_DEFAULT, Diligent::BIND_UNORDERED_ACCESS | BIND_SHADER_RESOURCE, Diligent::CPU_ACCESS_NONE);
				buf.reserve(rc, BuffDesc);
				decc_.emplace_back((IBuffer*)buf, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_UNORDERED_ACCESS, STATE_TRANSITION_FLAG_UPDATE_STATE);
			}
		}
		{
			using ts = sStruct::Selects;
			auto&& buf = _cbSelect;
			BuffDesc.Mode = BUFFER_MODE_STRUCTURED;
			RenderUtil::md_BufferDesc("cbSelect", BuffDesc, 1, sizeof(ts)*2, USAGE_DYNAMIC, BIND_UNIFORM_BUFFER, CPU_ACCESS_WRITE);
			buf.reserve(rc, BuffDesc);
			decc_.emplace_back(buf, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_CONSTANT_BUFFER, STATE_TRANSITION_FLAG_UPDATE_STATE);
		}

		p_rc->m_pImmediateContext->TransitionResourceStates(decc_.size(), decc_.data());
		//auto shaderpath = loader::Get(shader);
		ShaderResourceVariableDesc Vars[] =
		{
			//{SHADER_TYPE_COMPUTE, "queueFB", SHADER_RESOURCE_VARIABLE_TYPE_STATIC},
			{ SHADER_TYPE_COMPUTE, "_tinfo", SHADER_RESOURCE_VARIABLE_TYPE_STATIC },
			{ SHADER_TYPE_COMPUTE, "cbCameraAttribs", SHADER_RESOURCE_VARIABLE_TYPE_STATIC },
			{ SHADER_TYPE_COMPUTE, "instable", SHADER_RESOURCE_VARIABLE_TYPE_STATIC },
			{ SHADER_TYPE_COMPUTE, "instDatas", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE },
			{ SHADER_TYPE_COMPUTE, "objectTransfromBuffer", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE },
			{ SHADER_TYPE_COMPUTE, "sele_out", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE },
			{ SHADER_TYPE_COMPUTE, "Visibleout", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE },
			{ SHADER_TYPE_COMPUTE, "cbSelect", SHADER_RESOURCE_VARIABLE_TYPE_STATIC },
			{ Diligent::SHADER_TYPE_COMPUTE, "VfArg",       Diligent::SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE },
			//{ Diligent::SHADER_TYPE_COMPUTE, "audio_bufin", Diligent::SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC },
			//{ Diligent::SHADER_TYPE_COMPUTE, "audio_bufout",Diligent::SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE },
			{ Diligent::SHADER_TYPE_COMPUTE, "in_sele_out",  Diligent::SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE },
		};
		if (mode != 1)
			return;
		IBuffer* vbs[] = {
			_culltinfo,
			rc.pCameraAttribs,
			_instable,
			_vbIns,
			_vbtrans,
			cullout,
			Visibleout,
			_cbSelect,
			a_vfag,
			//a_f,
			seleout
		};
		//using namespace Diligent;
		Diligent::BUFFER_VIEW_TYPE bvs[] = {
			BUFFER_VIEW_SHADER_RESOURCE,
			BUFFER_VIEW_UNDEFINED,
			BUFFER_VIEW_SHADER_RESOURCE,
			BUFFER_VIEW_SHADER_RESOURCE,
			BUFFER_VIEW_SHADER_RESOURCE,
			BUFFER_VIEW_UNORDERED_ACCESS,
			BUFFER_VIEW_UNORDERED_ACCESS,
			BUFFER_VIEW_UNDEFINED,
			BUFFER_VIEW_UNORDERED_ACCESS,
			//BUFFER_VIEW_UNORDERED_ACCESS,
			BUFFER_VIEW_UNORDERED_ACCESS,
		};
		for (auto& itt : Vars)
		{
			itt.Type = SHADER_RESOURCE_VARIABLE_TYPE_STATIC;
		}
		sm.AddShaderMacro("Culling", 1);
		auto& pPSO = _psb.PSO;
		DrawFunc::CreateComputePSO("ScreenCulling", "Util0.hlsl", "ScreenCulling",
			m_pDevice, pShaderSourceFactory, _psb.PSO, Vars, threadgsize, sm);
		RenderUtil::setVbuffer(NULL, pPSO, Vars, bvs, vbs);
		_SrbScreenrect = NULL;
		pPSO->CreateShaderResourceBinding(&this->_SrbScreenrect, true);
	}
	constexpr int rmk = 0xFFFFFF;
	constexpr int remk = 0xFFFFFF+1;
	constexpr int mkcpying =3;
	INT64 Fencesele = -1;
	void Visible_f::prepare(RenderContext& rc, int tsize) {
		tsize = tsize == -1 ? _dyvbc : tsize;
		_rt = rc;
		instnumb = tsize;
		{
			Diligent::MapHelper<sStruct::Selects> sout(_rt.m_pImmediateContext
				, _cbSelect, MAP_WRITE, MAP_FLAG_DISCARD);
			sout[0].inum = instnumb;
			sout[1] = parasele;
		}
		std::memset(_cbfread.data(), 0, Utility::VecByteSize(_cbfread));
		/*lastfence = _cpyfen->GetCompletedValue();
		if (lastfence!=-1&&lastfence == (m_CurrentFenceValue-1))*/
		copy2loacl();
		{
			constexpr auto sele0Array=[]() {
				std::array<tsout, 200> result{};
				for (auto&& ei:result)
				{
					ei.itid = 0;
					ei.tid = 0;
				}
				return result;
			};
			constexpr auto su = sele0Array();// { 0, 0, 0, 0 };
			constexpr int szsu = sizeof(su);
			//memset(su, 0, szsu);
			if(parasele.inum>0)
			_rt.m_pImmediateContext->UpdateBuffer(seleout, 0, szsu, &su, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
			_rt.m_pImmediateContext->UpdateBuffer(cullout, 0, szsu, &su, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
			_rt.m_pImmediateContext->UpdateBuffer(a_vfag, 0, a_vfag->GetDesc().Size, &su, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
		}
		assert(m_CurrentFenceValue - lastfence <= 1);
		_dyvbc = 0;
		auto& pContext = _rt.m_pImmediateContext;
		{
			StateTransitionDesc Barriers[] = {
				{ cullout, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_UNORDERED_ACCESS,STATE_TRANSITION_FLAG_UPDATE_STATE },
				{ seleout, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_UNORDERED_ACCESS,STATE_TRANSITION_FLAG_UPDATE_STATE },
				{ Visibleout, RESOURCE_STATE_UNKNOWN,RESOURCE_STATE_UNORDERED_ACCESS,STATE_TRANSITION_FLAG_UPDATE_STATE },
				{ a_vfag, RESOURCE_STATE_UNKNOWN,RESOURCE_STATE_UNORDERED_ACCESS,STATE_TRANSITION_FLAG_UPDATE_STATE },
			};
			pContext->TransitionResourceStates(_countof(Barriers), Barriers);
		}
		auto& pPSO = _psb.PSO;
		const auto dispx = instnumb / threadgsize + (instnumb % threadgsize) > 0;
		DispatchComputeAttribs DispatAttribs(dispx, 1, 1);
		_rt.m_pImmediateContext->SetPipelineState(pPSO);
		_rt.m_pImmediateContext->CommitShaderResources(_SrbScreenrect, RESOURCE_STATE_TRANSITION_MODE_VERIFY);
		_rt.m_pImmediateContext->DispatchCompute(DispatAttribs);
		{
			StateTransitionDesc Barriers[] = {
				{ cullout, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_COPY_SOURCE,STATE_TRANSITION_FLAG_UPDATE_STATE},
				{ a_vfag, RESOURCE_STATE_UNORDERED_ACCESS, RESOURCE_STATE_SHADER_RESOURCE,STATE_TRANSITION_FLAG_UPDATE_STATE},
				{ Visibleout, RESOURCE_STATE_UNORDERED_ACCESS, RESOURCE_STATE_SHADER_RESOURCE,STATE_TRANSITION_FLAG_UPDATE_STATE }
			};
			pContext->TransitionResourceStates(_countof(Barriers), Barriers);
		}
		const int cpynum = 1024;// std::min(256 * tsize, 2048);
		const int cpybytes = 256 * tsize;
		if (parasele.inum ==rmk||parasele.inum==remk) {
			_rt.m_pImmediateContext->CopyBuffer(this->seleout, 0,
				Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION,
				stagsele,
				0, cpynum * sizeof(t_sinout), Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
			//_rt.m_pImmediateContext->EnqueueSignal(_cpyfen, m_CurrentFenceValue-1);
			Fencesele = m_CurrentFenceValue;
		}
		_rt.m_pImmediateContext->CopyBuffer(this->cullout, 0,
			Diligent::RESOURCE_STATE_TRANSITION_MODE_VERIFY,
			stagcull,
			0, cpynum * sizeof(t_sinout), Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
		_rt.m_pImmediateContext->EnqueueSignal(_cpyfen, m_CurrentFenceValue);
		m_CurrentFenceValue++;
	}

	void Visible_f::rectSele(const sStruct::Selects& csele, const ui64 sz)
	{
		parasele = csele;
		parasele.inum = remk;
	}

	void Visible_f::Raysele(const sStruct::Selects& csele, const ui64 sz)
	{
		parasele = csele;
		parasele.inum = rmk;
	}

	//call once per frame

	void Visible_f::copy2loacl() {
		const auto& fencev = _cpyfen->GetCompletedValue();
		lastfence = fencev;
		auto&& rc_ = this->_rt;
		//max latency kept within 1 frame
		constexpr int maxdiff = 1;
		auto fd = (m_CurrentFenceValue - lastfence);
		if constexpr(0) 
		{
			rc_.m_pImmediateContext->WaitForIdle();//DeviceWaitForFence get incorrect result
			//rc_.m_pImmediateContext->DeviceWaitForFence(_cpyfen, m_CurrentFenceValue - maxdiff);
			lastfence = _cpyfen->GetCompletedValue();
			fd = (m_CurrentFenceValue - lastfence);
		}
		auto cpysele = [](std::vector<t_sinout>& dest, Diligent::MapHelper<t_sinout>& src) {
			auto& selunm = src[0].itid;
			const auto& msele = GloProp::Get().maxSelect;
			assert(src[0].itid < msele);
			dest.resize(std::min(selunm, msele));
			std::memcpy(dest.data(), src+1, Utility::VecByteSize(dest));
		};
		//if (fd == maxdiff)
		{
			auto& rct = *p_rc;
			Diligent::MapHelper<t_sinout> sout(rct.m_pImmediateContext
				, stagcull, MAP_READ, MAP_FLAG_DO_NOT_WAIT);
			std::memcpy(_cbfread.data(), sout, Utility::VecByteSize(_cbfread));
			//if (Fencesele+1==m_CurrentFenceValue)
			{
				Diligent::MapHelper<t_sinout> sout(rct.m_pImmediateContext
					, stagsele, MAP_READ, MAP_FLAG_DO_NOT_WAIT);
				cpysele(_Seleread,sout);
				parasele.inum = 0;
			}
			return;
			/*std::sort(std::execution::par, _cbfread.begin(), _cbfread.end(), [](auto& a, auto b) {
				return a.itid > b.itid;
				});*/
		}
		assert(0);
		//return _cbfread;
	}
}