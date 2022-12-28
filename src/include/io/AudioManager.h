#pragma once
#include"GProps.h"
#include"Render/RenderUtil.h"
#include"Render/ShaderStructs.h"
#include "MapHelper.hpp"
#include <filesystem>
//#include""
//#undef var
//#include"audio_vorbis.h"
///*#define STB_VORBIS_HEADER_ONLY
//#include "miniaudio/extras/stb_vorbis.c"*/  // Enables Vorbis decoding.
//#define MINIAUDIO_IMPLEMENTATION
//#include"miniaudio/miniaudio.h"
///*#undef  STB_VORBIS_HEADER_ONLY
//#include "miniaudio/extras/stb_vorbis.c" */ // Enables Vorbis decoding.
//#define var auto
#undef R
#undef L
#undef C
//#define DEVICE_SAMPLE_RATE      44100
//#undef L
#define EXPO 
namespace pj2::Audio {
//#define AUDIO
	namespace fs = std::filesystem;
#define SAMPLE_FORMAT   ma_format_f32
#define CHANNEL_COUNT   2
//#define SAMPLE_RATE     44100
#define DEVICE_FORMAT           ma_format_f32
	using IBuffer = Diligent::IBuffer;
	using v4f = math::Vector4f;
	using Char2 = wchar_t;
	using t_Frame = float;
	//using RenderContext = RenderUtil::RenderContext;
	constexpr int abufnm = 4;
	constexpr int maxiumLatency = 2;
	constexpr int  audiobufsize = 4096;
	//using namespace miniaudio;
	//EXPO
		//template<int audiobufsize = 4096>
	struct audioinfo
	{
		ui64 size = -1;
		ui64 Channel = 1;
		ui64 samprate = samplerate;
		ui64 offset = 0;
		//std::vector<math::v4f> data;
	};
	struct AudioManager
	{
	private:

		ReUtS::t_psb1 _pb1;
		ReUtS::t_psb1 _pbfilter;
		sStruct::VFoutArgs voutarg;
	public:
		Diligent::RefCntAutoPtr<IBuffer>                audio_bufin;
		Diligent::RefCntAutoPtr<IBuffer>                audio_bufout;
		Diligent::RefCntAutoPtr<IBuffer>                v_audio_bufout;
		Diligent::RefCntAutoPtr<IBuffer>                audio_inst;
		Diligent::RefCntAutoPtr<IBuffer>                VfArg;


		Diligent::RefCntAutoPtr<IBuffer>                cb_camera;
		Diligent::RefCntAutoPtr<IBuffer>                audio_inst_out;
		//Diligent::RefCntAutoPtr<IBuffer>                audio_inst;
		RenderContext _rc;
		std::unordered_map<std::string, audioinfo> _audiomap;
		std::array<float[audiobufsize], abufnm> audiobuf;
		std::vector<math::v4f> daudios;
		ui64 fram = 1, prevaudiofram = 1;
		Diligent::RefCntAutoPtr<Diligent::IFence> _copyfen;
		//preallocate 20MB buffer
		void LoadAudios(const std::string path, 
			const ui64 totalsize = 1024 * 1024 * 20,
			const ui64 Channel = 1,
			const ui64 samprate = samplerate
		);
		/*void LoadAudios(const fs::path,
			const ui64 totalsize = 1024 * 1024 * 20,
			const ui64 Channel = 1,
			const ui64 samprate = samplerate
		);*/
		void init(const RenderContext& rt, const ui64 maxaudiosource = 4096 * 3);
		inline ui64 getbufpos(const ui64& FrameId) {
			return (FrameId % maxiumLatency) * audiobufsize;
		}
		void Update(int audio_ins);
		//{

		//	using namespace Diligent;
		//	auto&& m_pImmediateContext = _rc.m_pImmediateContext;
		//	DispatchComputeAttribs DispatAttribs(audio_ins, 1, 1);
		//	m_pImmediateContext->SetPipelineState(_pb1.PSO);
		//	m_pImmediateContext->CommitShaderResources(_pb1.GetShaderResourceBinding(), RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
		//	m_pImmediateContext->DispatchCompute(DispatAttribs);
		//	const auto dist = getbufpos(fram);
		//	m_pImmediateContext->CopyBuffer(audio_bufout, 0, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION, v_audio_bufout,
		//		dist, audiobufsize, Diligent::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
		//	m_pImmediateContext->EnqueueSignal(_copyfen, fram);
		//	math::ui64 PrevFrameId = _copyfen->GetCompletedValue();
		//	if (fram - PrevFrameId > maxiumLatency) {
		//		PrevFrameId = fram - maxiumLatency;
		//		m_pImmediateContext->WaitForFence(_copyfen, PrevFrameId, false);
		//	}
		//	{
		//		using ts = v4f;
		//		Diligent::MapHelper<ts> m_MappedData(m_pImmediateContext, v_audio_bufout, MAP_TYPE::MAP_READ, MAP_FLAGS::MAP_FLAG_DO_NOT_WAIT);
		//		std::memcpy(audiobuf[0], &m_MappedData[getbufpos(PrevFrameId)], audiobufsize);
		//		/*for (size_t i = 0; i < audio_ins; i++)
		//		{
		//		assert(m_MappedData[i] == (math::v4f(0, 1, 2, 3) + math::v4f::Ones() * 4 * i) * 500);
		//		}
		//		INFO("sum res={0}", m_MappedData[0].transpose());*/
		//	}
		//	fram++;
		//}

		/*using t_clock = std::chrono::high_resolution_clock;
		t_clock clockprev;*/
		using ma_uint32 = uint32_t;
		ui64 audiol1 = 0, audiol0 = 0;
		void Play_cb(void*& pOutput, ma_uint32 frameCount);
		//static inline void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
		//{
		//	reinterpret_cast<AudioManager*>(pDevice->pUserData)->Play_cb(pDevice, pOutput, frameCount);

		//	/* Unused in this example. */
		//	(void)pDevice;
		//	(void)pInput;
		//}
	};
}
