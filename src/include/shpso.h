#pragma once
#include"Render/RenderUtil.h"
#include<execution>

#include"GraphicsUtilities.h"
#include"Math1.h"
#include"Render/ShaderStructs.h"
#include "MapHelper.hpp"
#include"Common/span.hpp"
#include"GProps.h"
//#include"taskflow/taskflow.hpp"
namespace pj2::shpso {
	using namespace Diligent;
	using Uint64_t = math::ui64;
	struct shader
	{
		RefCntAutoPtr<IPipelineState> _pos, _perparpos, _rayselectpeo;
		RefCntAutoPtr<IShaderResourceBinding> SRBrec_, srbpr_, srbray_;
	};
	struct Visible_f
	{
		using vbuffer = n_Render::dyvbf;
	protected:
		RenderUtil::PSB<1> _psb;
		//RefCntAutoPtr<IPipelineState> _selecirps;
		//RefCntAutoPtr<IShaderResourceBinding> _SeleCirRen;
		RenderContext _rt;
		shader _sds;
		RefCntAutoPtr<Diligent::IFence> _cpyfen;
		math::ui64 m_CurrentFenceValue = 1;
		vbuffer _cbSelect;
		//RefCntAutoPtr<IBuffer>_cScreenrect;
		RefCntAutoPtr<IPipelineState> _PsScreenrect;
		RefCntAutoPtr<IShaderResourceBinding> _SrbScreenrect;
		static constexpr int MaxCpyStep = sStruct::MaxCpyStep + 1;
		/*shader selects(RefCntAutoPtr<IRenderDevice>& m_pDevice,
			RefCntAutoPtr<IDeviceContext>& m_pImmediateContext,
			RefCntAutoPtr<IShaderSourceInputStreamFactory>& pShaderSourceFactory,
			IBuffer* pCameraAttribs,
			IBuffer* Vf_Props,
			IBuffer* sele_out,
			IBuffer* tf, const int maxcap = 0);*/
		ui64 cpybk = 0;
		inline int vcpoyoff() {
			assert((cpybk + 1 - lastfence) < MaxCpyStep);
			return cpybk++ % MaxCpyStep;
		}
		std::array<void*, 2> _pMap;

		vbuffer seleout;
		vbuffer stagsele;
	public:
		using t_sinout = sStruct::seleout;
		ui64 lastfence = 0, instnumb = 0, _dyvbc = 0;
		static constexpr ui64 threadgsize = 64;
		//select circle render buffer
		vbuffer _vbtrans;
		vbuffer _vbIns;
		RefCntAutoPtr<IBuffer> a_vfag;
		vbuffer Visibleout,_SeleVout;
		vbuffer _culltinfo;
		vbuffer _instable;
		vbuffer Vf_Props;
		vbuffer cullout;
		vbuffer stagcull;
		RenderContext* p_rc;
		std::vector<sStruct::bufferinfo> _trendertable;
		void init(RenderContext& rc,int mode=1, int socap = 16384, const int maxtinfo = 512);
		void appendtable(Utility::span<sStruct::bufferinfo> binf, Utility::span<sStruct::tinfo> tinf) {
			_instable.merge(binf, *p_rc);
			_culltinfo.merge(tinf, *p_rc);
		}
		inline auto& GetVBT() { return _vbtrans; }
		inline auto& GetVBI() { return _vbIns; }
		void begainWrite() {
			_dyvbc = 0;
			IBuffer* const vbs[] = {
					 _vbIns,
					 _vbtrans
			};
			using namespace Diligent;
			auto& pContext = p_rc->m_pImmediateContext;
			for (size_t i = 0; i < _pMap.size(); i++)
			{
				auto& ptr = _pMap[i];
				pContext->MapBuffer(vbs[i], MAP_WRITE, MAP_FLAG_DISCARD, (PVoid&)ptr);
			}
			_vbfinfo.clear();
		}

		inline void FinishWrite() {
			IBuffer* const vbs[] = {
					 _vbIns,
					 _vbtrans
			};
			using namespace Diligent;
			auto& pContext = p_rc->m_pImmediateContext;
			for (size_t i = 0; i < _pMap.size(); i++)
			{
				auto& ptr = _pMap[i];
				ptr = 0;
				pContext->UnmapBuffer(vbs[i], MAP_WRITE);
			}
			pContext->UpdateBuffer(_instable, 0, Utility::VecByteSize(_trendertable),_trendertable.data(), RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
			StateTransitionDesc barr[] = {
				{_vbtrans, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_GENERIC_READ, STATE_TRANSITION_FLAG_UPDATE_STATE},
				{_vbIns, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_GENERIC_READ, STATE_TRANSITION_FLAG_UPDATE_STATE},
				{_instable, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_GENERIC_READ, STATE_TRANSITION_FLAG_UPDATE_STATE}
			};
			p_rc->m_pImmediateContext->TransitionResourceStates(_countof(barr), barr);
		}
		std::vector<sStruct::bufferinfo> _vbfinfo;
		std::vector<t_sinout> _cbfread;
		std::vector<t_sinout> _Seleread;
		auto write(Utility::span<math::mtransform> trs, Utility::span<sStruct::instData1> ins,const int& tid) {

			auto& bi = _vbfinfo.emplace_back();
			bi.org = _dyvbc;
			bi.count = trs.size();
			{
				auto ptr = (char*)_pMap[0];
				auto& container = ins;
				constexpr auto seg = sizeof(container[0]);
				std::memcpy(ptr + _dyvbc * seg, container.data(), container.size_bytes());
			}
			{
				auto ptr = (char*)_pMap[1];
				auto& container = trs;
				constexpr auto seg = sizeof(container[0]);
				std::memcpy(ptr + _dyvbc * seg, container.data(), container.size_bytes());
			}
			_trendertable[tid].org = _dyvbc;
			_dyvbc += bi.count;
			return bi.org;
		}
		inline int BaseInst(const int& tid) {
			return _trendertable[tid].org;
		}
		void prepare(RenderContext& rc, int tsize = -1);
		void rectSele(const sStruct::Selects& csele, const ui64 sz = 1024);
		void Raysele( const sStruct::Selects& csele, const ui64 sz = 1024);
		void RenderScreenRect(const sStruct::Selects& rect);
		void RenderSelect(const ui64 sz);
		bool ResultReady() {
			lastfence = _cpyfen->GetCompletedValue();
			return lastfence >= m_CurrentFenceValue - 1;
		}
		int Culllet() { return _dyvbc; }
		inline bool TaskRunning() {
			return lastfence + 1 < m_CurrentFenceValue;
		}
		
		inline auto& GetResult() {
			assert((m_CurrentFenceValue - lastfence )== 1);
			return _cbfread;
		}
		//call once per frame
		inline void copy2loacl();
		template<class F>
		inline void GetNewResult(F&& f, const ui64 maxdiff = 2) {
			if (!TaskRunning()) {
				return;
				//f(nullptr);
			}
			const auto fencev = _cpyfen->GetCompletedValue();
			if (lastfence == fencev)
				return;
			lastfence = fencev;// _cpyfen->GetCompletedValue();
			if ((m_CurrentFenceValue - lastfence) > maxdiff) {
				_rt.m_pImmediateContext->DeviceWaitForFence(_cpyfen, std::max(m_CurrentFenceValue - maxdiff, (Diligent::Uint64)0));
			}

			Diligent::MapHelper<sStruct::seleout> sout(_rt.m_pImmediateContext
				, stagcull, MAP_READ, MAP_FLAG_DO_NOT_WAIT);
			f(sout);
		}
	};
	struct SelectCS
	{
	private:
		RenderUtil::PSB<1> _Circlepsb;
		//RefCntAutoPtr<IPipelineState> _selecirps;
		//RefCntAutoPtr<IShaderResourceBinding> _SeleCirRen;
		RenderContext _rt;
		shader _sds;
		RefCntAutoPtr<Diligent::IFence> _cpyfen;
		math::ui64 m_CurrentFenceValue = 1;
		//select circle render buffer
		RefCntAutoPtr<IBuffer>Selefbuf;

		RefCntAutoPtr<IBuffer>_Selecbuf;
		//RefCntAutoPtr<IBuffer>_cScreenrect;
		RefCntAutoPtr<IPipelineState> _PsScreenrect;
		RefCntAutoPtr<IShaderResourceBinding> _SrbScreenrect;
		static constexpr int MaxCpyStep = sStruct::MaxCpyStep + 1;
		/*shader selects(RefCntAutoPtr<IRenderDevice>& m_pDevice,
			RefCntAutoPtr<IDeviceContext>& m_pImmediateContext,
			RefCntAutoPtr<IShaderSourceInputStreamFactory>& pShaderSourceFactory,
			IBuffer* pCameraAttribs,
			IBuffer* Vf_Props,
			IBuffer* sele_out,
			IBuffer* tf, const int maxcap = 0);*/
		inline void cmf(const sStruct::Selects& csele) {
			Diligent::MapHelper<sStruct::Selects> mpd(_rt.m_pImmediateContext, _Selecbuf, MAP_WRITE, MAP_FLAG_DISCARD);
			std::memcpy(mpd, &csele, sizeof(csele));
		}
		ui64 cpybk = 0;
		inline int vcpoyoff() {
			assert((cpybk + 1 - lastfence) < MaxCpyStep);
			return cpybk++ % MaxCpyStep;
		}
	public:
		using t_sinout = sStruct::seleout;
		ui64 lastfence = -1;
		RefCntAutoPtr<IBuffer>sele_out;
		RefCntAutoPtr<IBuffer>tinfo;
		RefCntAutoPtr<IBuffer>Vf_Props;
		RefCntAutoPtr<IBuffer>stagsele;
		std::vector<t_sinout> _lastSele;
		void init(RenderContext& rc, int socap = 16384);
		void initRender(RenderContext& rc);
		void prepare();
		RefCntAutoPtr<IBuffer>& GetSIVBf() { return Selefbuf; }
		void rectSele(const ui64 objinsepct, const sStruct::Selects& csele, const ui64 sz = 1024);
		void Raysele(const ui64& objinsepct, const sStruct::Selects& csele, const ui64 sz = 1024);
		void RenderScreenRect(const sStruct::Selects& rect);
		void RenderSelect(const ui64 sz);
		void SetFrom(Visible_f& vf);
		bool ResultReady();
		inline bool TaskRunning() {
			return lastfence + 1 < m_CurrentFenceValue;
		}
		inline bool Read() {
			constexpr int maxdiff = 1;
			const auto fencev = _cpyfen->GetCompletedValue();
			if (lastfence == fencev)
				return false;
			if ((m_CurrentFenceValue - lastfence) > maxdiff) {
				_rt.m_pImmediateContext->DeviceWaitForFence(_cpyfen, std::max(m_CurrentFenceValue - maxdiff, (Diligent::Uint64)0));
			}
			Diligent::MapHelper<sStruct::seleout> sout(_rt.m_pImmediateContext
				, stagsele, MAP_READ, MAP_FLAG_DO_NOT_WAIT);
			t_sinout* pd = sout;
			_lastSele.resize(pd[0].itid);
			std::memcpy(_lastSele.data(), pd + 1, Utility::VecByteSize(_lastSele));
			return true;
		}
		template<class F>
		inline void GetNewResult(F&& f, const ui64 maxdiff = 2) {
			if (!TaskRunning()) {
				return;
				//f(nullptr);
			}
			const auto fencev = _cpyfen->GetCompletedValue();
			if (lastfence == fencev)
				return;
			lastfence = fencev;// _cpyfen->GetCompletedValue();
			if ((m_CurrentFenceValue - lastfence) > maxdiff) {
				_rt.m_pImmediateContext->DeviceWaitForFence(_cpyfen, std::max(m_CurrentFenceValue - maxdiff, (Diligent::Uint64)0));
			}

			Diligent::MapHelper<sStruct::seleout> sout(_rt.m_pImmediateContext
				, stagsele, MAP_READ, MAP_FLAG_DO_NOT_WAIT);
			f(sout);
		}
	};
}
namespace pj2::computelib {
	using SelectCS = shpso::SelectCS;
}